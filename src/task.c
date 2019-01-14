/*******************************************************************************
 * This file is part of SWIFT.
 * Copyright (c) 2012 Pedro Gonnet (pedro.gonnet@durham.ac.uk)
 *                    Matthieu Schaller (matthieu.schaller@durham.ac.uk)
 *               2015 Peter W. Draper (p.w.draper@durham.ac.uk)
 *               2016 John A. Regan (john.a.regan@durham.ac.uk)
 *                    Tom Theuns (tom.theuns@durham.ac.uk)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

/* Config parameters. */
#include "../config.h"

/* Some standard headers. */
#include <float.h>
#include <limits.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* MPI headers. */
#ifdef WITH_MPI
#include <mpi.h>
#endif

/* This object's header. */
#include "task.h"

/* Local headers. */
#include "atomic.h"
#include "error.h"
#include "inline.h"
#include "lock.h"

/* Task type names. */
const char *taskID_names[task_type_count] = {"none",
                                             "sort",
                                             "self",
                                             "pair",
                                             "sub_self",
                                             "sub_pair",
                                             "init_grav",
                                             "init_grav_out",
                                             "ghost_in",
                                             "ghost",
                                             "ghost_out",
                                             "extra_ghost",
                                             "drift_part",
                                             "drift_gpart",
                                             "drift_gpart_out",
                                             "end_force",
                                             "kick1",
                                             "kick2",
                                             "timestep",
                                             "timestep_limiter",
                                             "send",
                                             "recv",
                                             "grav_long_range",
                                             "grav_mm",
                                             "grav_down_in",
                                             "grav_down",
                                             "grav_mesh",
                                             "cooling",
                                             "star_formation",
                                             "logger",
                                             "stars_ghost_in",
                                             "stars_ghost",
                                             "stars_ghost_out",
                                             "stars_sort"};

/* Sub-task type names. */
const char *subtaskID_names[task_subtype_count] = {
    "none",    "density",       "gradient",      "force",
    "limiter", "grav",          "external_grav", "tend",
    "xv",      "rho",           "gpart",         "multipole",
    "spart",   "stars_density", "stars_feedback"};

#ifdef WITH_MPI
/* MPI communicators for the subtypes. */
MPI_Comm subtaskMPI_comms[task_subtype_count];
#endif

/**
 * @brief Computes the overlap between the parts array of two given cells.
 *
 * @param TYPE is the type of parts (e.g. #part, #gpart, #spart)
 * @param ARRAY is the array of this specific type.
 * @param COUNT is the number of elements in the array.
 */
#define TASK_CELL_OVERLAP(TYPE, ARRAY, COUNT)                               \
  __attribute__((always_inline))                                            \
      INLINE static size_t task_cell_overlap_##TYPE(                        \
          const struct cell *restrict ci, const struct cell *restrict cj) { \
                                                                            \
    if (ci == NULL || cj == NULL) return 0;                                 \
                                                                            \
    if (ci->ARRAY <= cj->ARRAY &&                                           \
        ci->ARRAY + ci->COUNT >= cj->ARRAY + cj->COUNT) {                   \
      return cj->COUNT;                                                     \
    } else if (cj->ARRAY <= ci->ARRAY &&                                    \
               cj->ARRAY + cj->COUNT >= ci->ARRAY + ci->COUNT) {            \
      return ci->COUNT;                                                     \
    }                                                                       \
                                                                            \
    return 0;                                                               \
  }

TASK_CELL_OVERLAP(part, hydro.parts, hydro.count);
TASK_CELL_OVERLAP(gpart, grav.parts, grav.count);
TASK_CELL_OVERLAP(spart, stars.parts, stars.count);

/**
 * @brief Returns the #task_actions for a given task.
 *
 * @param t The #task.
 */
__attribute__((always_inline)) INLINE static enum task_actions task_acts_on(
    const struct task *t) {

  switch (t->type) {

    case task_type_none:
      return task_action_none;
      break;

    case task_type_drift_part:
    case task_type_sort:
    case task_type_ghost:
    case task_type_extra_ghost:
    case task_type_timestep_limiter:
    case task_type_cooling:
      return task_action_part;
      break;

    case task_type_star_formation:
      return task_action_all;

    case task_type_stars_ghost:
    case task_type_stars_sort:
      return task_action_spart;
      break;

    case task_type_self:
    case task_type_pair:
    case task_type_sub_self:
    case task_type_sub_pair:
      switch (t->subtype) {

        case task_subtype_density:
        case task_subtype_gradient:
        case task_subtype_force:
        case task_subtype_limiter:
          return task_action_part;
          break;

        case task_subtype_stars_density:
        case task_subtype_stars_feedback:
          return task_action_all;
          break;

        case task_subtype_grav:
        case task_subtype_external_grav:
          return task_action_gpart;
          break;

        default:
          error("Unknow task_action for task");
          return task_action_none;
          break;
      }
      break;

    case task_type_end_force:
    case task_type_kick1:
    case task_type_kick2:
    case task_type_logger:
    case task_type_timestep:
    case task_type_send:
    case task_type_recv:
      if (t->ci->hydro.count > 0 && t->ci->grav.count > 0)
        return task_action_all;
      else if (t->ci->hydro.count > 0)
        return task_action_part;
      else if (t->ci->grav.count > 0)
        return task_action_gpart;
      else
        error("Task without particles");
      break;

    case task_type_init_grav:
    case task_type_grav_mm:
    case task_type_grav_long_range:
      return task_action_multipole;
      break;

    case task_type_drift_gpart:
    case task_type_grav_down:
    case task_type_grav_mesh:
      return task_action_gpart;
      break;

    default:
      error("Unknown task_action for task");
      return task_action_none;
      break;
  }

  /* Silence compiler warnings */
  error("Unknown task_action for task");
  return task_action_none;
}

/**
 * @brief Compute the Jaccard similarity of the data used by two
 *        different tasks.
 *
 * @param ta The first #task.
 * @param tb The second #task.
 */
float task_overlap(const struct task *restrict ta,
                   const struct task *restrict tb) {

  if (ta == NULL || tb == NULL) return 0.f;

  const enum task_actions ta_act = task_acts_on(ta);
  const enum task_actions tb_act = task_acts_on(tb);

  /* First check if any of the two tasks are of a type that don't
     use cells. */
  if (ta_act == task_action_none || tb_act == task_action_none) return 0.f;

  const int ta_part = (ta_act == task_action_part || ta_act == task_action_all);
  const int ta_gpart =
      (ta_act == task_action_gpart || ta_act == task_action_all);
  const int ta_spart =
      (ta_act == task_action_spart || ta_act == task_action_all);
  const int tb_part = (tb_act == task_action_part || tb_act == task_action_all);
  const int tb_gpart =
      (tb_act == task_action_gpart || tb_act == task_action_all);
  const int tb_spart =
      (tb_act == task_action_spart || tb_act == task_action_all);

  /* In the case where both tasks act on parts */
  if (ta_part && tb_part) {

    /* Compute the union of the cell data. */
    size_t size_union = 0;
    if (ta->ci != NULL) size_union += ta->ci->hydro.count;
    if (ta->cj != NULL) size_union += ta->cj->hydro.count;
    if (tb->ci != NULL) size_union += tb->ci->hydro.count;
    if (tb->cj != NULL) size_union += tb->cj->hydro.count;

    /* Compute the intersection of the cell data. */
    const size_t size_intersect = task_cell_overlap_part(ta->ci, tb->ci) +
                                  task_cell_overlap_part(ta->ci, tb->cj) +
                                  task_cell_overlap_part(ta->cj, tb->ci) +
                                  task_cell_overlap_part(ta->cj, tb->cj);

    return ((float)size_intersect) / (size_union - size_intersect);
  }

  /* In the case where both tasks act on gparts */
  else if (ta_gpart && tb_gpart) {

    /* Compute the union of the cell data. */
    size_t size_union = 0;
    if (ta->ci != NULL) size_union += ta->ci->grav.count;
    if (ta->cj != NULL) size_union += ta->cj->grav.count;
    if (tb->ci != NULL) size_union += tb->ci->grav.count;
    if (tb->cj != NULL) size_union += tb->cj->grav.count;

    /* Compute the intersection of the cell data. */
    const size_t size_intersect = task_cell_overlap_gpart(ta->ci, tb->ci) +
                                  task_cell_overlap_gpart(ta->ci, tb->cj) +
                                  task_cell_overlap_gpart(ta->cj, tb->ci) +
                                  task_cell_overlap_gpart(ta->cj, tb->cj);

    return ((float)size_intersect) / (size_union - size_intersect);
  }

  /* In the case where both tasks act on sparts */
  else if (ta_spart && tb_spart) {

    /* Compute the union of the cell data. */
    size_t size_union = 0;
    if (ta->ci != NULL) size_union += ta->ci->stars.count;
    if (ta->cj != NULL) size_union += ta->cj->stars.count;
    if (tb->ci != NULL) size_union += tb->ci->stars.count;
    if (tb->cj != NULL) size_union += tb->cj->stars.count;

    /* Compute the intersection of the cell data. */
    const size_t size_intersect = task_cell_overlap_spart(ta->ci, tb->ci) +
                                  task_cell_overlap_spart(ta->ci, tb->cj) +
                                  task_cell_overlap_spart(ta->cj, tb->ci) +
                                  task_cell_overlap_spart(ta->cj, tb->cj);

    return ((float)size_intersect) / (size_union - size_intersect);
  }

  /* Else, no overlap */
  return 0.f;
}

/**
 * @brief Unlock the cell held by this task.
 *
 * @param t The #task.
 */
void task_unlock(struct task *t) {

  const enum task_types type = t->type;
  const enum task_subtypes subtype = t->subtype;
  struct cell *ci = t->ci, *cj = t->cj;

  /* Act based on task type. */
  switch (type) {

    case task_type_end_force:
    case task_type_kick1:
    case task_type_kick2:
    case task_type_logger:
    case task_type_timestep:
      cell_unlocktree(ci);
      cell_gunlocktree(ci);
      break;

    case task_type_drift_part:
    case task_type_sort:
    case task_type_ghost:
    case task_type_timestep_limiter:
      cell_unlocktree(ci);
      break;

    case task_type_drift_gpart:
    case task_type_grav_mesh:
      cell_gunlocktree(ci);
      break;

    case task_type_stars_sort:
      cell_sunlocktree(ci);
      break;

    case task_type_self:
    case task_type_sub_self:
      if (subtype == task_subtype_grav) {
        cell_gunlocktree(ci);
        cell_munlocktree(ci);
      } else if (subtype == task_subtype_stars_density) {
        cell_sunlocktree(ci);
      } else if (subtype == task_subtype_stars_feedback) {
        cell_sunlocktree(ci);
        cell_unlocktree(ci);
      } else {
        cell_unlocktree(ci);
      }
      break;

    case task_type_pair:
    case task_type_sub_pair:
      if (subtype == task_subtype_grav) {
        cell_gunlocktree(ci);
        cell_gunlocktree(cj);
        cell_munlocktree(ci);
        cell_munlocktree(cj);
      } else if (subtype == task_subtype_stars_density) {
        cell_sunlocktree(ci);
        cell_sunlocktree(cj);
      } else if (subtype == task_subtype_stars_feedback) {
        cell_sunlocktree(ci);
        cell_sunlocktree(cj);
        cell_unlocktree(ci);
        cell_unlocktree(cj);
      } else {
        cell_unlocktree(ci);
        cell_unlocktree(cj);
      }
      break;

    case task_type_grav_down:
      cell_gunlocktree(ci);
      cell_munlocktree(ci);
      break;

    case task_type_grav_long_range:
      cell_munlocktree(ci);
      break;

    case task_type_grav_mm:
      cell_munlocktree(ci);
      cell_munlocktree(cj);
      break;

    case task_type_star_formation:
      cell_unlocktree(ci);
      cell_sunlocktree(ci);
      cell_gunlocktree(ci);
      break;

    default:
      break;
  }
}

/**
 * @brief Try to lock the cells associated with this task.
 *
 * @param t the #task.
 */
int task_lock(struct task *t) {

  const enum task_types type = t->type;
  const enum task_subtypes subtype = t->subtype;
  struct cell *ci = t->ci, *cj = t->cj;
#ifdef WITH_MPI
  int res = 0, err = 0;
  MPI_Status stat;
#endif

  switch (type) {

    /* Communication task? */
    case task_type_recv:
    case task_type_send:
#ifdef WITH_MPI
      /* Check the status of the MPI request. */
      if ((err = MPI_Test(&t->req, &res, &stat)) != MPI_SUCCESS) {
        char buff[MPI_MAX_ERROR_STRING];
        int len;
        MPI_Error_string(err, buff, &len);
        error(
            "Failed to test request on send/recv task (type=%s/%s tag=%lld, "
            "%s).",
            taskID_names[t->type], subtaskID_names[t->subtype], t->flags, buff);
      }
      return res;
#else
      error("SWIFT was not compiled with MPI support.");
#endif
      break;

    case task_type_end_force:
    case task_type_kick1:
    case task_type_kick2:
    case task_type_logger:
    case task_type_timestep:
      if (atomic_read(&ci->hydro.hold) || ci->grav.phold) return 0;
      if (cell_locktree(ci) != 0) return 0;
      if (cell_glocktree(ci) != 0) {
        cell_unlocktree(ci);
        return 0;
      }
      break;

    case task_type_drift_part:
    case task_type_sort:
    case task_type_ghost:
    case task_type_timestep_limiter:
      if (atomic_read(&ci->hydro.hold)) return 0;
      if (cell_locktree(ci) != 0) return 0;
      break;

    case task_type_stars_sort:
      if (ci->stars.hold) return 0;
      if (cell_slocktree(ci) != 0) return 0;
      break;

    case task_type_drift_gpart:
    case task_type_grav_mesh:
      if (ci->grav.phold) return 0;
      if (cell_glocktree(ci) != 0) return 0;
      break;

    case task_type_self:
    case task_type_sub_self:
      if (subtype == task_subtype_grav) {
        /* Lock the gparts and the m-pole */
        if (ci->grav.phold || ci->grav.mhold) return 0;
        if (cell_glocktree(ci) != 0)
          return 0;
        else if (cell_mlocktree(ci) != 0) {
          cell_gunlocktree(ci);
          return 0;
        }
      } else if (subtype == task_subtype_stars_density) {
        if (ci->stars.hold) return 0;
        if (cell_slocktree(ci) != 0) return 0;
      } else if (subtype == task_subtype_stars_feedback) {
        if (ci->stars.hold) return 0;
        if (atomic_read(&ci->hydro.hold)) return 0;
        if (cell_slocktree(ci) != 0) return 0;
        if (cell_locktree(ci) != 0) {
          cell_sunlocktree(ci);
          return 0;
        }
      } else { /* subtype == hydro */
        if (atomic_read(&ci->hydro.hold)) return 0;
        if (cell_locktree(ci) != 0) return 0;
      }
      break;

    case task_type_pair:
    case task_type_sub_pair:
      if (subtype == task_subtype_grav) {
        /* Lock the gparts and the m-pole in both cells */
        if (ci->grav.phold || cj->grav.phold) return 0;
        if (cell_glocktree(ci) != 0) return 0;
        if (cell_glocktree(cj) != 0) {
          cell_gunlocktree(ci);
          return 0;
        } else if (cell_mlocktree(ci) != 0) {
          cell_gunlocktree(ci);
          cell_gunlocktree(cj);
          return 0;
        } else if (cell_mlocktree(cj) != 0) {
          cell_gunlocktree(ci);
          cell_gunlocktree(cj);
          cell_munlocktree(ci);
          return 0;
        }
      } else if (subtype == task_subtype_stars_density) {
        if (ci->stars.hold || cj->stars.hold) return 0;
        if (cell_slocktree(ci) != 0) return 0;
        if (cell_slocktree(cj) != 0) {
          cell_sunlocktree(ci);
          return 0;
        }
      } else if (subtype == task_subtype_stars_feedback) {
        /* Lock the stars and the gas particles in both cells */
        if (ci->stars.hold || cj->stars.hold) return 0;
        if (atomic_read(&ci->hydro.hold) || atomic_read(&cj->hydro.hold)) return 0;
        if (cell_slocktree(ci) != 0) return 0;
        if (cell_slocktree(cj) != 0) {
          cell_sunlocktree(ci);
          return 0;
        }
        if (cell_locktree(ci) != 0) {
          cell_sunlocktree(ci);
          cell_sunlocktree(cj);
          return 0;
        }
        if (cell_locktree(cj) != 0) {
          cell_sunlocktree(ci);
          cell_sunlocktree(cj);
          cell_unlocktree(ci);
          return 0;
        }
      } else { /* subtype == hydro */
        /* Lock the parts in both cells */
        if (atomic_read(&ci->hydro.hold) || atomic_read(&cj->hydro.hold)) return 0;
        if (cell_locktree(ci) != 0) return 0;
        if (cell_locktree(cj) != 0) {
          cell_unlocktree(ci);
          return 0;
        }
      }
      break;

    case task_type_grav_down:
      /* Lock the gparts and the m-poles */
      if (ci->grav.phold || ci->grav.mhold) return 0;
      if (cell_glocktree(ci) != 0)
        return 0;
      else if (cell_mlocktree(ci) != 0) {
        cell_gunlocktree(ci);
        return 0;
      }
      break;

    case task_type_grav_long_range:
      /* Lock the m-poles */
      if (ci->grav.mhold) return 0;
      if (cell_mlocktree(ci) != 0) return 0;
      break;

    case task_type_grav_mm:
      /* Lock both m-poles */
      if (ci->grav.mhold || cj->grav.mhold) return 0;
      if (cell_mlocktree(ci) != 0) return 0;
      if (cell_mlocktree(cj) != 0) {
        cell_munlocktree(ci);
        return 0;
      }
      break;

    case task_type_star_formation:
      /* Lock the gas, gravity and star particles */
      if (atomic_read(&ci->hydro.hold) || ci->stars.hold || ci->grav.phold) return 0;
      if (cell_locktree(ci) != 0) return 0;
      if (cell_slocktree(ci) != 0) {
        cell_unlocktree(ci);
        return 0;
      }
      if (cell_glocktree(ci) != 0) {
        cell_unlocktree(ci);
        cell_sunlocktree(ci);
        return 0;
      }

    default:
      break;
  }

  /* If we made it this far, we've got a lock. */
  return 1;
}

/**
 * @brief Print basic information about a task.
 *
 * @param t The #task.
 */
void task_print(const struct task *t) {

  message("Type:'%s' sub_type:'%s' wait=%d nr_unlocks=%d skip=%d",
          taskID_names[t->type], subtaskID_names[t->subtype], t->wait,
          t->nr_unlock_tasks, t->skip);
}

/**
 * @brief Get the group name of a task.
 *
 * This is used to group tasks with similar actions in the task dependency
 * graph.
 *
 * @param type The #task type.
 * @param subtype The #task subtype.
 * @param cluster (return) The group name (should be allocated)
 */
void task_get_group_name(int type, int subtype, char *cluster) {

  if (type == task_type_grav_long_range || type == task_type_grav_mm ||
      type == task_type_grav_mesh) {

    strcpy(cluster, "Gravity");
    return;
  }

  switch (subtype) {
    case task_subtype_density:
      strcpy(cluster, "Density");
      break;
    case task_subtype_gradient:
      strcpy(cluster, "Gradient");
      break;
    case task_subtype_force:
      strcpy(cluster, "Force");
      break;
    case task_subtype_grav:
      strcpy(cluster, "Gravity");
      break;
    case task_subtype_limiter:
      strcpy(cluster, "Timestep_limiter");
      break;
    case task_subtype_stars_density:
      strcpy(cluster, "Stars");
      break;
    default:
      strcpy(cluster, "None");
      break;
  }
}

/**
 * @brief Generate the full name of a #task.
 *
 * @param type The #task type.
 * @param subtype The #task type.
 * @param name (return) The formatted string
 */
void task_get_full_name(int type, int subtype, char *name) {

#ifdef SWIFT_DEBUG_CHECKS
  /* Check input */
  if (type >= task_type_count) error("Unknown task type %i", type);

  if (subtype >= task_subtype_count)
    error("Unknown task subtype %i with type %s", subtype, taskID_names[type]);
#endif

  /* Full task name */
  if (subtype == task_subtype_none)
    sprintf(name, "%s", taskID_names[type]);
  else
    sprintf(name, "%s_%s", taskID_names[type], subtaskID_names[subtype]);
}

#ifdef WITH_MPI
/**
 * @brief Create global communicators for each of the subtasks.
 */
void task_create_mpi_comms(void) {
  for (int i = 0; i < task_subtype_count; i++) {
    MPI_Comm_dup(MPI_COMM_WORLD, &subtaskMPI_comms[i]);
  }
}
#endif
