/*******************************************************************************
 * This file is part of SWIFT.
 * Copyright (c) 2013 Pedro Gonnet (pedro.gonnet@durham.ac.uk)
 *               2016 Matthieu Schaller (matthieu.schaller@durham.ac.uk)
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
#ifndef SWIFT_MULTIPOLE_H
#define SWIFT_MULTIPOLE_H

/* Config parameters. */
#include "../config.h"

/* Some standard headers. */
#include <math.h>
#include <string.h>

/* Includes. */
//#include "active.h"
#include "align.h"
#include "const.h"
#include "error.h"
#include "gravity_derivatives.h"
#include "inline.h"
#include "kernel_gravity.h"
#include "minmax.h"
#include "part.h"
#include "vector_power.h"

#define multipole_align 128

struct grav_tensor {

  /* 0th order terms */
  float F_000;

#if SELF_GRAVITY_MULTIPOLE_ORDER > 0

  /* 1st order terms */
  float F_100, F_010, F_001;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 1

  /* 2nd order terms */
  float F_200, F_020, F_002;
  float F_110, F_101, F_011;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 2

  /* 3rd order terms */
  float F_300, F_030, F_003;
  float F_210, F_201;
  float F_120, F_021;
  float F_102, F_012;
  float F_111;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 3

  /* 4th order terms */
  float F_400, F_040, F_004;
  float F_310, F_301;
  float F_130, F_031;
  float F_103, F_013;
  float F_220, F_202, F_022;
  float F_211, F_121, F_112;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 4
#error "Missing implementation for order >4"
#endif
#ifdef SWIFT_DEBUG_CHECKS

  /* Total number of gpart this field tensor interacted with */
  long long num_interacted;

#endif
};

struct multipole {

  /* Bulk velocity */
  float vel[3];

  /* 0th order terms */
  float M_000;

#if SELF_GRAVITY_MULTIPOLE_ORDER > 0

  /* 1st order terms */
  float M_100, M_010, M_001;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 1

  /* 2nd order terms */
  float M_200, M_020, M_002;
  float M_110, M_101, M_011;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 2

  /* 3rd order terms */
  float M_300, M_030, M_003;
  float M_210, M_201;
  float M_120, M_021;
  float M_102, M_012;
  float M_111;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 3

  /* 4th order terms */
  float M_400, M_040, M_004;
  float M_310, M_301;
  float M_130, M_031;
  float M_103, M_013;
  float M_220, M_202, M_022;
  float M_211, M_121, M_112;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 4
#error "Missing implementation for order >4"
#endif

#ifdef SWIFT_DEBUG_CHECKS

  /* Total number of gpart in this multipole */
  long long num_gpart;

#endif
};

/**
 * @brief The multipole expansion of a mass distribution.
 */
struct gravity_tensors {

  /*! Linking pointer for "memory management". */
  struct gravity_tensors *next;

  /*! Centre of mass of the matter dsitribution */
  double CoM[3];

  /*! The actual content */
  struct {

    /*! Multipole mass */
    struct multipole m_pole;

    /*! Field tensor for the potential */
    struct grav_tensor pot;
  };
} SWIFT_STRUCT_ALIGN;

/**
 * @brief Reset the data of a #multipole.
 *
 * @param m The #multipole.
 */
INLINE static void gravity_reset(struct gravity_tensors *m) {

  /* Just bzero the struct. */
  bzero(m, sizeof(struct gravity_tensors));
}

/**
 * @brief Drifts a #multipole forward in time.
 *
 * @param m The #multipole.
 * @param dt The drift time-step.
 */
INLINE static void gravity_drift(struct gravity_tensors *m, double dt) {

  /* Move the whole thing according to bulk motion */
  m->CoM[0] += m->m_pole.vel[0] * dt;
  m->CoM[1] += m->m_pole.vel[1] * dt;
  m->CoM[2] += m->m_pole.vel[2] * dt;
}

INLINE static void gravity_field_tensors_init(struct grav_tensor *l) {

  bzero(l, sizeof(struct grav_tensor));
}

/**
 * @brief Adds field tensrs to other ones (i.e. does la += lb).
 *
 * @param la The gravity tensors to add to.
 * @param lb The gravity tensors to add.
 */
INLINE static void gravity_field_tensors_add(struct grav_tensor *la,
                                             const struct grav_tensor *lb) {
#ifdef SWIFT_DEBUG_CHECKS
  if (lb->num_interacted == 0) error("Adding tensors that did not interact");
  la->num_interacted += lb->num_interacted;
#endif

  /* Add 0th order terms */
  la->F_000 += lb->F_000;

#if SELF_GRAVITY_MULTIPOLE_ORDER > 0
  /* Add 1st order terms */
  la->F_100 += lb->F_100;
  la->F_010 += lb->F_010;
  la->F_001 += lb->F_001;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 1
  /* Add 2nd order terms */
  la->F_200 += lb->F_200;
  la->F_020 += lb->F_020;
  la->F_002 += lb->F_002;
  la->F_110 += lb->F_110;
  la->F_101 += lb->F_101;
  la->F_011 += lb->F_011;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 2
  /* Add 3rd order terms */
  la->F_300 += lb->F_300;
  la->F_030 += lb->F_030;
  la->F_003 += lb->F_003;
  la->F_210 += lb->F_210;
  la->F_201 += lb->F_201;
  la->F_120 += lb->F_120;
  la->F_021 += lb->F_021;
  la->F_102 += lb->F_102;
  la->F_012 += lb->F_012;
  la->F_111 += lb->F_111;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 3
  /* Add 4th order terms */
  la->F_400 += lb->F_400;
  la->F_040 += lb->F_040;
  la->F_004 += lb->F_004;
  la->F_310 += lb->F_310;
  la->F_301 += lb->F_301;
  la->F_130 += lb->F_130;
  la->F_031 += lb->F_031;
  la->F_103 += lb->F_103;
  la->F_013 += lb->F_013;
  la->F_220 += lb->F_220;
  la->F_202 += lb->F_202;
  la->F_022 += lb->F_022;
  la->F_211 += lb->F_211;
  la->F_121 += lb->F_121;
  la->F_112 += lb->F_112;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 4
#error "Missing implementation for order >4"
#endif
}

/**
 * @brief Prints the content of a #grav_tensor to stdout.
 *
 * Note: Uses directly printf(), not a call to message().
 *
 * @param l The #grav_tensor to print.
 */
INLINE static void gravity_field_tensors_print(const struct grav_tensor *l) {

  printf("-------------------------\n");
  printf("F_000= %12.5e\n", l->F_000);
#if SELF_GRAVITY_MULTIPOLE_ORDER > 0
  printf("-------------------------\n");
  printf("F_100= %12.5e F_010= %12.5e F_001= %12.5e\n", l->F_100, l->F_010,
         l->F_001);
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 1
  printf("-------------------------\n");
  printf("F_200= %12.5e F_020= %12.5e F_002= %12.5e\n", l->F_200, l->F_020,
         l->F_002);
  printf("F_110= %12.5e F_101= %12.5e F_011= %12.5e\n", l->F_110, l->F_101,
         l->F_011);
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 2
  printf("-------------------------\n");
  printf("F_300= %12.5e F_030= %12.5e F_003= %12.5e\n", l->F_300, l->F_030,
         l->F_003);
  printf("F_210= %12.5e F_201= %12.5e F_120= %12.5e\n", l->F_210, l->F_201,
         l->F_120);
  printf("F_021= %12.5e F_102= %12.5e F_012= %12.5e\n", l->F_021, l->F_102,
         l->F_012);
  printf("F_111= %12.5e\n", l->F_111);
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 3
  printf("-------------------------\n");
  printf("F_400= %12.5e F_040= %12.5e F_004= %12.5e\n", l->F_400, l->F_040,
         l->F_004);
  printf("F_310= %12.5e F_301= %12.5e F_130= %12.5e\n", l->F_310, l->F_301,
         l->F_130);
  printf("F_031= %12.5e F_103= %12.5e F_013= %12.5e\n", l->F_031, l->F_103,
         l->F_013);
  printf("F_220= %12.5e F_202= %12.5e F_022= %12.5e\n", l->F_220, l->F_202,
         l->F_022);
  printf("F_211= %12.5e F_121= %12.5e F_112= %12.5e\n", l->F_211, l->F_121,
         l->F_112);
#endif
  printf("-------------------------\n");
#if SELF_GRAVITY_MULTIPOLE_ORDER > 4
#error "Missing implementation for order >4"
#endif
}

/**
 * @brief Prints the content of a #multipole to stdout.
 *
 * Note: Uses directly printf(), not a call to message().
 *
 * @param m The #multipole to print.
 */
INLINE static void gravity_multipole_print(const struct multipole *m) {

  printf("Vel= [%12.5e %12.5e %12.5e]\n", m->vel[0], m->vel[1], m->vel[2]);
  printf("-------------------------\n");
  printf("M_000= %12.5e\n", m->M_000);
#if SELF_GRAVITY_MULTIPOLE_ORDER > 0
  printf("-------------------------\n");
  printf("M_100= %12.5e M_010= %12.5e M_001= %12.5e\n", m->M_100, m->M_010,
         m->M_001);
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 1
  printf("-------------------------\n");
  printf("M_200= %12.5e M_020= %12.5e M_002= %12.5e\n", m->M_200, m->M_020,
         m->M_002);
  printf("M_110= %12.5e M_101= %12.5e M_011= %12.5e\n", m->M_110, m->M_101,
         m->M_011);
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 2
  printf("-------------------------\n");
  printf("M_300= %12.5e M_030= %12.5e M_003= %12.5e\n", m->M_300, m->M_030,
         m->M_003);
  printf("M_210= %12.5e M_201= %12.5e M_120= %12.5e\n", m->M_210, m->M_201,
         m->M_120);
  printf("M_021= %12.5e M_102= %12.5e M_012= %12.5e\n", m->M_021, m->M_102,
         m->M_012);
  printf("M_111= %12.5e\n", m->M_111);
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 3
  printf("-------------------------\n");
  printf("M_400= %12.5e M_040= %12.5e M_004= %12.5e\n", m->M_400, m->M_040,
         m->M_004);
  printf("M_310= %12.5e M_301= %12.5e M_130= %12.5e\n", m->M_310, m->M_301,
         m->M_130);
  printf("M_031= %12.5e M_103= %12.5e M_013= %12.5e\n", m->M_031, m->M_103,
         m->M_013);
  printf("M_220= %12.5e M_202= %12.5e M_022= %12.5e\n", m->M_220, m->M_202,
         m->M_022);
  printf("M_211= %12.5e M_121= %12.5e M_112= %12.5e\n", m->M_211, m->M_121,
         m->M_112);
#endif
  printf("-------------------------\n");
#if SELF_GRAVITY_MULTIPOLE_ORDER > 4
#error "Missing implementation for order >4"
#endif
}

/**
 * @brief Adds a #multipole to another one (i.e. does ma += mb).
 *
 * @param ma The multipole to add to.
 * @param mb The multipole to add.
 */
INLINE static void gravity_multipole_add(struct multipole *ma,
                                         const struct multipole *mb) {

  const float M_000 = ma->M_000 + mb->M_000;
  const float inv_M_000 = 1.f / M_000;

  /* Add the bulk velocities */
  ma->vel[0] = (ma->vel[0] * ma->M_000 + mb->vel[0] * mb->M_000) * inv_M_000;
  ma->vel[1] = (ma->vel[1] * ma->M_000 + mb->vel[1] * mb->M_000) * inv_M_000;
  ma->vel[2] = (ma->vel[2] * ma->M_000 + mb->vel[2] * mb->M_000) * inv_M_000;

  /* Add 0th order terms */
  ma->M_000 = M_000;

#if SELF_GRAVITY_MULTIPOLE_ORDER > 0
  /* Add 1st order terms */
  ma->M_100 += mb->M_100;
  ma->M_010 += mb->M_010;
  ma->M_001 += mb->M_001;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 1
  /* Add 2nd order terms */
  ma->M_200 += mb->M_200;
  ma->M_020 += mb->M_020;
  ma->M_002 += mb->M_002;
  ma->M_110 += mb->M_110;
  ma->M_101 += mb->M_101;
  ma->M_011 += mb->M_011;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 2
  /* Add 3rd order terms */
  ma->M_300 += mb->M_300;
  ma->M_030 += mb->M_030;
  ma->M_003 += mb->M_003;
  ma->M_210 += mb->M_210;
  ma->M_201 += mb->M_201;
  ma->M_120 += mb->M_120;
  ma->M_021 += mb->M_021;
  ma->M_102 += mb->M_102;
  ma->M_012 += mb->M_012;
  ma->M_111 += mb->M_111;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 3
  /* Add 4th order terms */
  ma->M_400 += mb->M_400;
  ma->M_040 += mb->M_040;
  ma->M_004 += mb->M_004;
  ma->M_310 += mb->M_310;
  ma->M_301 += mb->M_301;
  ma->M_130 += mb->M_130;
  ma->M_031 += mb->M_031;
  ma->M_103 += mb->M_103;
  ma->M_013 += mb->M_013;
  ma->M_220 += mb->M_220;
  ma->M_202 += mb->M_202;
  ma->M_022 += mb->M_022;
  ma->M_211 += mb->M_211;
  ma->M_121 += mb->M_121;
  ma->M_112 += mb->M_112;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 4
#error "Missing implementation for order >4"
#endif

  // MATTHIEU
  ma->M_100 = 0.f;
  ma->M_010 = 0.f;
  ma->M_001 = 0.f;

#ifdef SWIFT_DEBUG_CHECKS
  ma->num_gpart += mb->num_gpart;
#endif
}

/**
 * @brief Verifies whether two #multipole's are equal or not.
 *
 * @param ga The first #multipole.
 * @param gb The second #multipole.
 * @param tolerance The maximal allowed relative difference for the fields.
 * @return 1 if the multipoles are equal, 0 otherwise
 */
INLINE static int gravity_multipole_equal(const struct gravity_tensors *ga,
                                          const struct gravity_tensors *gb,
                                          double tolerance) {

  /* Check CoM */
  if (fabs(ga->CoM[0] - gb->CoM[0]) / fabs(ga->CoM[0] + gb->CoM[0]) >
      tolerance) {
    message("CoM[0] different");
    return 0;
  }
  if (fabs(ga->CoM[1] - gb->CoM[1]) / fabs(ga->CoM[1] + gb->CoM[1]) >
      tolerance) {
    message("CoM[1] different");
    return 0;
  }
  if (fabs(ga->CoM[2] - gb->CoM[2]) / fabs(ga->CoM[2] + gb->CoM[2]) >
      tolerance) {
    message("CoM[2] different");
    return 0;
  }

  /* Helper pointers */
  const struct multipole *ma = &ga->m_pole;
  const struct multipole *mb = &gb->m_pole;

  const double v2 = ma->vel[0] * ma->vel[0] + ma->vel[1] * ma->vel[1] +
                    ma->vel[2] * ma->vel[2];

  /* Check bulk velocity (if non-zero and component > 1% of norm)*/
  if (fabsf(ma->vel[0] + mb->vel[0]) > 1e-10 &&
      (ma->vel[0] * ma->vel[0]) > 0.0001 * v2 &&
      fabsf(ma->vel[0] - mb->vel[0]) / fabsf(ma->vel[0] + mb->vel[0]) >
          tolerance) {
    message("v[0] different");
    return 0;
  }
  if (fabsf(ma->vel[1] + mb->vel[1]) > 1e-10 &&
      (ma->vel[1] * ma->vel[1]) > 0.0001 * v2 &&
      fabsf(ma->vel[1] - mb->vel[1]) / fabsf(ma->vel[1] + mb->vel[1]) >
          tolerance) {
    message("v[1] different");
    return 0;
  }
  if (fabsf(ma->vel[2] + mb->vel[2]) > 1e-10 &&
      (ma->vel[2] * ma->vel[2]) > 0.0001 * v2 &&
      fabsf(ma->vel[2] - mb->vel[2]) / fabsf(ma->vel[2] + mb->vel[2]) >
          tolerance) {
    message("v[2] different");
    return 0;
  }

  /* Check 0th order terms */
  if (fabsf(ma->M_000 - mb->M_000) / fabsf(ma->M_000 + mb->M_000) > tolerance) {
    message("M_000 term different");
    return 0;
  }

#if SELF_GRAVITY_MULTIPOLE_ORDER > 0
  /* Check 1st order terms */
  if (fabsf(ma->M_100 + mb->M_100) > 1e-6 * ma->M_000 &&
      fabsf(ma->M_100 - mb->M_100) / fabsf(ma->M_100 + mb->M_100) > tolerance) {
    message("M_100 term different");
    return 0;
  }
  if (fabsf(ma->M_010 + mb->M_010) > 1e-6 * ma->M_000 &&
      fabsf(ma->M_010 - mb->M_010) / fabsf(ma->M_010 + mb->M_010) > tolerance) {
    message("M_010 term different");
    return 0;
  }
  if (fabsf(ma->M_001 + mb->M_001) > 1e-6 * ma->M_000 &&
      fabsf(ma->M_001 - mb->M_001) / fabsf(ma->M_001 + mb->M_001) > tolerance) {
    message("M_001 term different");
    return 0;
  }
#endif

#if SELF_GRAVITY_MULTIPOLE_ORDER > 1
  /* Check 2nd order terms */
  if (fabsf(ma->M_200 + mb->M_200) > 1e-5 * ma->M_000 &&
      fabsf(ma->M_200 - mb->M_200) / fabsf(ma->M_200 + mb->M_200) > tolerance) {
    message("M_200 term different");
    return 0;
  }
  if (fabsf(ma->M_020 + mb->M_020) > 1e-5 * ma->M_000 &&
      fabsf(ma->M_020 - mb->M_020) / fabsf(ma->M_020 + mb->M_020) > tolerance) {
    message("M_020 term different");
    return 0;
  }
  if (fabsf(ma->M_002 + mb->M_002) > 1e-5 * ma->M_000 &&
      fabsf(ma->M_002 - mb->M_002) / fabsf(ma->M_002 + mb->M_002) > tolerance) {
    message("M_002 term different");
    return 0;
  }
  if (fabsf(ma->M_110 + mb->M_110) > 1e-5 * ma->M_000 &&
      fabsf(ma->M_110 - mb->M_110) / fabsf(ma->M_110 + mb->M_110) > tolerance) {
    message("M_110 term different");
    return 0;
  }
  if (fabsf(ma->M_101 + mb->M_101) > 1e-5 * ma->M_000 &&
      fabsf(ma->M_101 - mb->M_101) / fabsf(ma->M_101 + mb->M_101) > tolerance) {
    message("M_101 term different");
    return 0;
  }
  if (fabsf(ma->M_011 + mb->M_011) > 1e-5 * ma->M_000 &&
      fabsf(ma->M_011 - mb->M_011) / fabsf(ma->M_011 + mb->M_011) > tolerance) {
    message("M_011 term different");
    return 0;
  }
#endif

  tolerance *= 10.;

#if SELF_GRAVITY_MULTIPOLE_ORDER > 2
  /* Check 3rd order terms */
  if (fabsf(ma->M_300 + mb->M_300) > 1e-5 * ma->M_000 &&
      fabsf(ma->M_300 - mb->M_300) / fabsf(ma->M_300 + mb->M_300) > tolerance) {
    message("M_300 term different");
    return 0;
  }
  if (fabsf(ma->M_030 + mb->M_030) > 1e-5 * ma->M_000 &&
      fabsf(ma->M_030 - mb->M_030) / fabsf(ma->M_030 + mb->M_030) > tolerance) {
    message("M_030 term different");
    return 0;
  }
  if (fabsf(ma->M_003 + mb->M_003) > 1e-5 * ma->M_000 &&
      fabsf(ma->M_003 - mb->M_003) / fabsf(ma->M_003 + mb->M_003) > tolerance) {
    message("M_003 term different");
    return 0;
  }
  if (fabsf(ma->M_210 + mb->M_210) > 1e-5 * ma->M_000 &&
      fabsf(ma->M_210 - mb->M_210) / fabsf(ma->M_210 + mb->M_210) > tolerance) {
    message("M_210 term different");
    return 0;
  }
  if (fabsf(ma->M_201 + mb->M_201) > 1e-5 * ma->M_000 &&
      fabsf(ma->M_201 - mb->M_201) / fabsf(ma->M_201 + mb->M_201) > tolerance) {
    message("M_201 term different");
    return 0;
  }
  if (fabsf(ma->M_120 + mb->M_120) > 1e-5 * ma->M_000 &&
      fabsf(ma->M_120 - mb->M_120) / fabsf(ma->M_120 + mb->M_120) > tolerance) {
    message("M_120 term different");
    return 0;
  }
  if (fabsf(ma->M_021 + mb->M_021) > 1e-5 * ma->M_000 &&
      fabsf(ma->M_021 - mb->M_021) / fabsf(ma->M_021 + mb->M_021) > tolerance) {
    message("M_021 term different");
    return 0;
  }
  if (fabsf(ma->M_102 + mb->M_102) > 1e-5 * ma->M_000 &&
      fabsf(ma->M_102 - mb->M_102) / fabsf(ma->M_102 + mb->M_102) > tolerance) {
    message("M_102 term different");
    return 0;
  }
  if (fabsf(ma->M_012 + mb->M_012) > 1e-5 * ma->M_000 &&
      fabsf(ma->M_012 - mb->M_012) / fabsf(ma->M_012 + mb->M_012) > tolerance) {
    message("M_012 term different");
    return 0;
  }
  if (fabsf(ma->M_111 + mb->M_111) > 1e-5 * ma->M_000 &&
      fabsf(ma->M_111 - mb->M_111) / fabsf(ma->M_111 + mb->M_111) > tolerance) {
    message("M_111 term different");
    return 0;
  }
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 3
  /* Check 4th order terms */
  if (fabsf(ma->M_400 + mb->M_400) > 1e-5 * ma->M_000 &&
      fabsf(ma->M_400 - mb->M_400) / fabsf(ma->M_400 + mb->M_400) > tolerance) {
    message("M_400 term different");
    return 0;
  }
  if (fabsf(ma->M_040 + mb->M_040) > 1e-5 * ma->M_000 &&
      fabsf(ma->M_040 - mb->M_040) / fabsf(ma->M_040 + mb->M_040) > tolerance) {
    message("M_040 term different");
    return 0;
  }
  if (fabsf(ma->M_004 + mb->M_004) > 1e-5 * ma->M_000 &&
      fabsf(ma->M_004 - mb->M_004) / fabsf(ma->M_004 + mb->M_004) > tolerance) {
    message("M_003 term different");
    return 0;
  }
  if (fabsf(ma->M_310 + mb->M_310) > 1e-5 * ma->M_000 &&
      fabsf(ma->M_310 - mb->M_310) / fabsf(ma->M_310 + mb->M_310) > tolerance) {
    message("M_310 term different");
    return 0;
  }
  if (fabsf(ma->M_301 + mb->M_301) > 1e-5 * ma->M_000 &&
      fabsf(ma->M_301 - mb->M_301) / fabsf(ma->M_301 + mb->M_301) > tolerance) {
    message("M_301 term different");
    return 0;
  }
  if (fabsf(ma->M_130 + mb->M_130) > 1e-5 * ma->M_000 &&
      fabsf(ma->M_130 - mb->M_130) / fabsf(ma->M_130 + mb->M_130) > tolerance) {
    message("M_130 term different");
    return 0;
  }
  if (fabsf(ma->M_031 + mb->M_031) > 1e-5 * ma->M_000 &&
      fabsf(ma->M_031 - mb->M_031) / fabsf(ma->M_031 + mb->M_031) > tolerance) {
    message("M_031 term different");
    return 0;
  }
  if (fabsf(ma->M_103 + mb->M_103) > 1e-5 * ma->M_000 &&
      fabsf(ma->M_103 - mb->M_103) / fabsf(ma->M_103 + mb->M_103) > tolerance) {
    message("M_103 term different");
    return 0;
  }
  if (fabsf(ma->M_013 + mb->M_013) > 1e-5 * ma->M_000 &&
      fabsf(ma->M_013 - mb->M_013) / fabsf(ma->M_013 + mb->M_013) > tolerance) {
    message("M_013 term different");
    return 0;
  }
  if (fabsf(ma->M_220 + mb->M_220) > 1e-5 * ma->M_000 &&
      fabsf(ma->M_220 - mb->M_220) / fabsf(ma->M_220 + mb->M_220) > tolerance) {
    message("M_220 term different");
    return 0;
  }
  if (fabsf(ma->M_202 + mb->M_202) > 1e-5 * ma->M_000 &&
      fabsf(ma->M_202 - mb->M_202) / fabsf(ma->M_202 + mb->M_202) > tolerance) {
    message("M_202 term different");
    return 0;
  }
  if (fabsf(ma->M_022 + mb->M_022) > 1e-5 * ma->M_000 &&
      fabsf(ma->M_022 - mb->M_022) / fabsf(ma->M_022 + mb->M_022) > tolerance) {
    message("M_022 term different");
    return 0;
  }
  if (fabsf(ma->M_211 + mb->M_211) > 1e-5 * ma->M_000 &&
      fabsf(ma->M_211 - mb->M_211) / fabsf(ma->M_211 + mb->M_211) > tolerance) {
    message("M_211 term different");
    return 0;
  }
  if (fabsf(ma->M_121 + mb->M_121) > 1e-5 * ma->M_000 &&
      fabsf(ma->M_121 - mb->M_121) / fabsf(ma->M_121 + mb->M_121) > tolerance) {
    message("M_121 term different");
    return 0;
  }
  if (fabsf(ma->M_112 + mb->M_112) > 1e-5 * ma->M_000 &&
      fabsf(ma->M_112 - mb->M_112) / fabsf(ma->M_112 + mb->M_112) > tolerance) {
    message("M_112 term different");
    return 0;
  }
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 4
#error "Missing implementation for order >4"
#endif

  /* All is good */
  return 1;
}

/**
 * @brief Constructs the #multipole of a bunch of particles around their
 * centre of mass.
 *
 * Corresponds to equation (28c).
 *
 * @param m The #multipole (content will  be overwritten).
 * @param gparts The #gpart.
 * @param gcount The number of particles.
 */
INLINE static void gravity_P2M(struct gravity_tensors *m,
                               const struct gpart *gparts, int gcount) {

  /* Temporary variables */
  double mass = 0.0;
  double com[3] = {0.0, 0.0, 0.0};
  float vel[3] = {0.f, 0.f, 0.f};

  /* Collect the particle data. */
  for (int k = 0; k < gcount; k++) {
    const float m = gparts[k].mass;

    mass += m;
    com[0] += gparts[k].x[0] * m;
    com[1] += gparts[k].x[1] * m;
    com[2] += gparts[k].x[2] * m;
    vel[0] += gparts[k].v_full[0] * m;
    vel[1] += gparts[k].v_full[1] * m;
    vel[2] += gparts[k].v_full[2] * m;
  }

  /* Final operation on CoM */
    
  const double imass = 1.0 / mass;
  com[0] *= imass;
  com[1] *= imass;
  com[2] *= imass;
  vel[0] *= imass;
  vel[1] *= imass;
  vel[2] *= imass;

/* Prepare some local counters */
#if SELF_GRAVITY_MULTIPOLE_ORDER > 0
  float M_100 = 0.f, M_010 = 0.f, M_001 = 0.f;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 1
  float M_200 = 0.f, M_020 = 0.f, M_002 = 0.f;
  float M_110 = 0.f, M_101 = 0.f, M_011 = 0.f;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 2
  float M_300 = 0.f, M_030 = 0.f, M_003 = 0.f;
  float M_210 = 0.f, M_201 = 0.f, M_120 = 0.f;
  float M_021 = 0.f, M_102 = 0.f, M_012 = 0.f;
  float M_111 = 0.f;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 3
  float M_400 = 0.f, M_040 = 0.f, M_004 = 0.f;
  float M_310 = 0.f, M_301 = 0.f, M_130 = 0.f;
  float M_031 = 0.f, M_103 = 0.f, M_013 = 0.f;
  float M_220 = 0.f, M_202 = 0.f, M_022 = 0.f;
  float M_211 = 0.f, M_121 = 0.f, M_112 = 0.f;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 4
#error "Missing implementation for order >4"
#endif

  /* Construce the higher order terms */
  for (int k = 0; k < gcount; k++) {
#if SELF_GRAVITY_MULTIPOLE_ORDER > 0
    const float m = gparts[k].mass;
    const double dx[3] = {gparts[k].x[0] - com[0], gparts[k].x[1] - com[1],
                          gparts[k].x[2] - com[2]};

    M_100 += -m * X_100(dx);
    M_010 += -m * X_010(dx);
    M_001 += -m * X_001(dx);
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 1
    M_200 += m * X_200(dx);
    M_020 += m * X_020(dx);
    M_002 += m * X_002(dx);
    M_110 += m * X_110(dx);
    M_101 += m * X_101(dx);
    M_011 += m * X_011(dx);
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 2
    M_300 += -m * X_300(dx);
    M_030 += -m * X_030(dx);
    M_003 += -m * X_003(dx);
    M_210 += -m * X_210(dx);
    M_201 += -m * X_201(dx);
    M_120 += -m * X_120(dx);
    M_021 += -m * X_021(dx);
    M_102 += -m * X_102(dx);
    M_012 += -m * X_012(dx);
    M_111 += -m * X_111(dx);
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 3
    M_400 += m * X_400(dx);
    M_040 += m * X_040(dx);
    M_004 += m * X_004(dx);
    M_310 += m * X_310(dx);
    M_301 += m * X_301(dx);
    M_130 += m * X_130(dx);
    M_031 += m * X_031(dx);
    M_103 += m * X_103(dx);
    M_013 += m * X_013(dx);
    M_220 += m * X_220(dx);
    M_202 += m * X_202(dx);
    M_022 += m * X_022(dx);
    M_211 += m * X_211(dx);
    M_121 += m * X_121(dx);
    M_112 += m * X_112(dx);
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 4
#error "Missing implementation for order >4"
#endif
  }

#if SELF_GRAVITY_MULTIPOLE_ORDER > 0
  M_100 = M_010 = M_001 = 0.f; /* Matthieu */
#endif

  /* Store the data on the multipole. */
  m->m_pole.M_000 = mass;
  m->CoM[0] = com[0];
  m->CoM[1] = com[1];
  m->CoM[2] = com[2];
  m->m_pole.vel[0] = vel[0];
  m->m_pole.vel[1] = vel[1];
  m->m_pole.vel[2] = vel[2];
#if SELF_GRAVITY_MULTIPOLE_ORDER > 0
  m->m_pole.M_100 = M_100;
  m->m_pole.M_010 = M_010;
  m->m_pole.M_001 = M_001;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 1
  m->m_pole.M_200 = M_200;
  m->m_pole.M_020 = M_020;
  m->m_pole.M_002 = M_002;
  m->m_pole.M_110 = M_110;
  m->m_pole.M_101 = M_101;
  m->m_pole.M_011 = M_011;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 2
  m->m_pole.M_300 = M_300;
  m->m_pole.M_030 = M_030;
  m->m_pole.M_003 = M_003;
  m->m_pole.M_210 = M_210;
  m->m_pole.M_201 = M_201;
  m->m_pole.M_120 = M_120;
  m->m_pole.M_021 = M_021;
  m->m_pole.M_102 = M_102;
  m->m_pole.M_012 = M_012;
  m->m_pole.M_111 = M_111;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 3
  m->m_pole.M_400 = M_400;
  m->m_pole.M_040 = M_040;
  m->m_pole.M_004 = M_004;
  m->m_pole.M_310 = M_310;
  m->m_pole.M_301 = M_301;
  m->m_pole.M_130 = M_130;
  m->m_pole.M_031 = M_031;
  m->m_pole.M_103 = M_103;
  m->m_pole.M_013 = M_013;
  m->m_pole.M_220 = M_220;
  m->m_pole.M_202 = M_202;
  m->m_pole.M_022 = M_022;
  m->m_pole.M_211 = M_211;
  m->m_pole.M_121 = M_121;
  m->m_pole.M_112 = M_112;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 4
#error "Missing implementation for order >4"
#endif

#ifdef SWIFT_DEBUG_CHECKS
  m->m_pole.num_gpart = gcount;
#endif
}

/**
 * @brief Creates a copy of #multipole shifted to a new location.
 *
 * Corresponds to equation (28d).
 *
 * @param m_a The #multipole copy (content will  be overwritten).
 * @param m_b The #multipole to shift.
 * @param pos_a The position to which m_b will be shifted.
 * @param pos_b The current postion of the multipole to shift.
 * @param periodic Is the calculation periodic ?
 */
INLINE static void gravity_M2M(struct multipole *m_a,
                               const struct multipole *m_b,
                               const double pos_a[3], const double pos_b[3],
                               int periodic) {
  /* Shift bulk velocity */
  m_a->vel[0] = m_b->vel[0];
  m_a->vel[1] = m_b->vel[1];
  m_a->vel[2] = m_b->vel[2];

  /* Shift 0th order term */
  m_a->M_000 = m_b->M_000;

#if SELF_GRAVITY_MULTIPOLE_ORDER > 0
  const double dx[3] = {pos_a[0] - pos_b[0], pos_a[1] - pos_b[1],
                        pos_a[2] - pos_b[2]};

  /* Shift 1st order term */
  m_a->M_100 = m_b->M_100 + X_100(dx) * m_b->M_000;
  m_a->M_010 = m_b->M_010 + X_010(dx) * m_b->M_000;
  m_a->M_001 = m_b->M_001 + X_001(dx) * m_b->M_000;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 1

  /* Shift 2nd order term */
  m_a->M_200 = m_b->M_200 + X_100(dx) * m_b->M_100 + X_200(dx) * m_b->M_000;
  m_a->M_020 = m_b->M_020 + X_010(dx) * m_b->M_010 + X_020(dx) * m_b->M_000;
  m_a->M_002 = m_b->M_002 + X_001(dx) * m_b->M_001 + X_002(dx) * m_b->M_000;
  m_a->M_110 = m_b->M_110 + X_100(dx) * m_b->M_010 + X_010(dx) * m_b->M_100 +
               X_110(dx) * m_b->M_000;
  m_a->M_101 = m_b->M_101 + X_100(dx) * m_b->M_001 + X_001(dx) * m_b->M_100 +
               X_101(dx) * m_b->M_000;
  m_a->M_011 = m_b->M_011 + X_010(dx) * m_b->M_001 + X_001(dx) * m_b->M_010 +
               X_011(dx) * m_b->M_000;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 2

  /* Shift 3rd order term */
  m_a->M_300 = m_b->M_300 + X_100(dx) * m_b->M_200 + X_200(dx) * m_b->M_100 +
               X_300(dx) * m_b->M_000;
  m_a->M_030 = m_b->M_030 + X_010(dx) * m_b->M_020 + X_020(dx) * m_b->M_010 +
               X_030(dx) * m_b->M_000;
  m_a->M_003 = m_b->M_003 + X_001(dx) * m_b->M_002 + X_002(dx) * m_b->M_001 +
               X_003(dx) * m_b->M_000;
  m_a->M_210 = m_b->M_210 + X_100(dx) * m_b->M_110 + X_010(dx) * m_b->M_200 +
               X_200(dx) * m_b->M_010 + X_110(dx) * m_b->M_100 +
               X_210(dx) * m_b->M_000;
  m_a->M_201 = m_b->M_201 + X_100(dx) * m_b->M_101 + X_001(dx) * m_b->M_200 +
               X_200(dx) * m_b->M_001 + X_101(dx) * m_b->M_100 +
               X_201(dx) * m_b->M_000;
  m_a->M_120 = m_b->M_120 + X_010(dx) * m_b->M_110 + X_100(dx) * m_b->M_020 +
               X_020(dx) * m_b->M_100 + X_110(dx) * m_b->M_010 +
               X_120(dx) * m_b->M_000;
  m_a->M_021 = m_b->M_021 + X_010(dx) * m_b->M_011 + X_001(dx) * m_b->M_020 +
               X_020(dx) * m_b->M_001 + X_011(dx) * m_b->M_010 +
               X_021(dx) * m_b->M_000;
  m_a->M_102 = m_b->M_102 + X_001(dx) * m_b->M_101 + X_100(dx) * m_b->M_002 +
               X_002(dx) * m_b->M_100 + X_101(dx) * m_b->M_001 +
               X_102(dx) * m_b->M_000;
  m_a->M_012 = m_b->M_012 + X_001(dx) * m_b->M_011 + X_010(dx) * m_b->M_002 +
               X_002(dx) * m_b->M_010 + X_011(dx) * m_b->M_001 +
               X_012(dx) * m_b->M_000;
  m_a->M_111 = m_b->M_111 + X_100(dx) * m_b->M_011 + X_010(dx) * m_b->M_101 +
               X_001(dx) * m_b->M_110 + X_110(dx) * m_b->M_001 +
               X_101(dx) * m_b->M_010 + X_011(dx) * m_b->M_100 +
               X_111(dx) * m_b->M_000;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 3
#error "Missing implementation for order >3"
#endif

#ifdef SWIFT_DEBUG_CHECKS
  m_a->num_gpart = m_b->num_gpart;
#endif
}

/**
 * @brief Compute the field tensors due to a multipole.
 *
 * Corresponds to equation (28b).
 *
 * @param l_b The field tensor to compute.
 * @param m_a The multipole creating the field.
 * @param pos_b The position of the field tensor.
 * @param pos_a The position of the multipole.
 * @param periodic Is the calculation periodic ?
 */
INLINE static void gravity_M2L(struct grav_tensor *l_b,
                               const struct multipole *m_a,
                               const double pos_b[3], const double pos_a[3],
                               int periodic) {

  double dx, dy, dz;
  if (periodic) {
    dx = box_wrap(pos_b[0] - pos_a[0], 0., 1.);
    dy = box_wrap(pos_b[1] - pos_a[1], 0., 1.);
    dz = box_wrap(pos_b[2] - pos_a[2], 0., 1.);
  } else {
    dx = pos_b[0] - pos_a[0];
    dy = pos_b[1] - pos_a[1];
    dz = pos_b[2] - pos_a[2];
  }
  const double r2 = dx * dx + dy * dy + dz * dz;

  const double r_inv = 1. / sqrt(r2);

  /*  0th order term */
  l_b->F_000 += m_a->M_000 * D_000(dx, dy, dz, r_inv);

#if SELF_GRAVITY_MULTIPOLE_ORDER > 0
  /*  1st order multipole term (addition to rank 0)*/
  l_b->F_000 += m_a->M_100 * D_100(dx, dy, dz, r_inv) +
                m_a->M_010 * D_010(dx, dy, dz, r_inv) +
                m_a->M_001 * D_001(dx, dy, dz, r_inv);

  /*  1st order multipole term (addition to rank 1)*/
  l_b->F_100 += m_a->M_000 * D_100(dx, dy, dz, r_inv);
  l_b->F_010 += m_a->M_000 * D_010(dx, dy, dz, r_inv);
  l_b->F_001 += m_a->M_000 * D_001(dx, dy, dz, r_inv);
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 1

  /*  2nd order multipole term (addition to rank 0)*/
  l_b->F_000 += m_a->M_200 * D_200(dx, dy, dz, r_inv) +
                m_a->M_020 * D_020(dx, dy, dz, r_inv) +
                m_a->M_002 * D_002(dx, dy, dz, r_inv);
  l_b->F_000 += m_a->M_110 * D_110(dx, dy, dz, r_inv) +
                m_a->M_101 * D_101(dx, dy, dz, r_inv) +
                m_a->M_011 * D_011(dx, dy, dz, r_inv);

  /*  2nd order multipole term (addition to rank 1)*/
  l_b->F_100 += m_a->M_100 * D_200(dx, dy, dz, r_inv) +
                m_a->M_010 * D_110(dx, dy, dz, r_inv) +
                m_a->M_001 * D_101(dx, dy, dz, r_inv);
  l_b->F_010 += m_a->M_100 * D_110(dx, dy, dz, r_inv) +
                m_a->M_010 * D_020(dx, dy, dz, r_inv) +
                m_a->M_001 * D_011(dx, dy, dz, r_inv);
  l_b->F_001 += m_a->M_100 * D_101(dx, dy, dz, r_inv) +
                m_a->M_010 * D_011(dx, dy, dz, r_inv) +
                m_a->M_001 * D_002(dx, dy, dz, r_inv);

  /*  2nd order multipole term (addition to rank 2)*/
  l_b->F_200 += m_a->M_000 * D_200(dx, dy, dz, r_inv);
  l_b->F_020 += m_a->M_000 * D_020(dx, dy, dz, r_inv);
  l_b->F_002 += m_a->M_000 * D_002(dx, dy, dz, r_inv);
  l_b->F_110 += m_a->M_000 * D_110(dx, dy, dz, r_inv);
  l_b->F_101 += m_a->M_000 * D_101(dx, dy, dz, r_inv);
  l_b->F_011 += m_a->M_000 * D_011(dx, dy, dz, r_inv);
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 2

  /*  3rd order multipole term (addition to rank 0)*/
  l_b->F_000 += m_a->M_300 * D_300(dx, dy, dz, r_inv) +
                m_a->M_030 * D_030(dx, dy, dz, r_inv) +
                m_a->M_003 * D_003(dx, dy, dz, r_inv);
  l_b->F_000 += m_a->M_210 * D_210(dx, dy, dz, r_inv) +
                m_a->M_201 * D_201(dx, dy, dz, r_inv) +
                m_a->M_120 * D_120(dx, dy, dz, r_inv);
  l_b->F_000 += m_a->M_021 * D_021(dx, dy, dz, r_inv) +
                m_a->M_102 * D_102(dx, dy, dz, r_inv) +
                m_a->M_012 * D_012(dx, dy, dz, r_inv);
  l_b->F_000 += m_a->M_111 * D_111(dx, dy, dz, r_inv);

  /*  3rd order multipole term (addition to rank 1)*/
  l_b->F_100 += m_a->M_200 * D_300(dx, dy, dz, r_inv) +
                m_a->M_020 * D_120(dx, dy, dz, r_inv) +
                m_a->M_002 * D_102(dx, dy, dz, r_inv);
  l_b->F_100 += m_a->M_110 * D_210(dx, dy, dz, r_inv) +
                m_a->M_101 * D_201(dx, dy, dz, r_inv) +
                m_a->M_011 * D_111(dx, dy, dz, r_inv);
  l_b->F_010 += m_a->M_200 * D_210(dx, dy, dz, r_inv) +
                m_a->M_020 * D_030(dx, dy, dz, r_inv) +
                m_a->M_002 * D_012(dx, dy, dz, r_inv);
  l_b->F_010 += m_a->M_110 * D_120(dx, dy, dz, r_inv) +
                m_a->M_101 * D_111(dx, dy, dz, r_inv) +
                m_a->M_011 * D_021(dx, dy, dz, r_inv);
  l_b->F_001 += m_a->M_200 * D_201(dx, dy, dz, r_inv) +
                m_a->M_020 * D_021(dx, dy, dz, r_inv) +
                m_a->M_002 * D_003(dx, dy, dz, r_inv);
  l_b->F_001 += m_a->M_110 * D_111(dx, dy, dz, r_inv) +
                m_a->M_101 * D_102(dx, dy, dz, r_inv) +
                m_a->M_011 * D_012(dx, dy, dz, r_inv);

  /*  3rd order multipole term (addition to rank 2)*/
  l_b->F_200 += m_a->M_100 * D_300(dx, dy, dz, r_inv) +
                m_a->M_010 * D_210(dx, dy, dz, r_inv) +
                m_a->M_001 * D_201(dx, dy, dz, r_inv);
  l_b->F_020 += m_a->M_100 * D_120(dx, dy, dz, r_inv) +
                m_a->M_010 * D_030(dx, dy, dz, r_inv) +
                m_a->M_001 * D_021(dx, dy, dz, r_inv);
  l_b->F_002 += m_a->M_100 * D_102(dx, dy, dz, r_inv) +
                m_a->M_010 * D_012(dx, dy, dz, r_inv) +
                m_a->M_001 * D_003(dx, dy, dz, r_inv);
  l_b->F_110 += m_a->M_100 * D_210(dx, dy, dz, r_inv) +
                m_a->M_010 * D_120(dx, dy, dz, r_inv) +
                m_a->M_001 * D_111(dx, dy, dz, r_inv);
  l_b->F_101 += m_a->M_100 * D_201(dx, dy, dz, r_inv) +
                m_a->M_010 * D_111(dx, dy, dz, r_inv) +
                m_a->M_001 * D_102(dx, dy, dz, r_inv);
  l_b->F_011 += m_a->M_100 * D_111(dx, dy, dz, r_inv) +
                m_a->M_010 * D_021(dx, dy, dz, r_inv) +
                m_a->M_001 * D_012(dx, dy, dz, r_inv);

  /*  3rd order multipole term (addition to rank 2)*/
  l_b->F_300 += m_a->M_000 * D_300(dx, dy, dz, r_inv);
  l_b->F_030 += m_a->M_000 * D_030(dx, dy, dz, r_inv);
  l_b->F_003 += m_a->M_000 * D_003(dx, dy, dz, r_inv);
  l_b->F_210 += m_a->M_000 * D_210(dx, dy, dz, r_inv);
  l_b->F_201 += m_a->M_000 * D_201(dx, dy, dz, r_inv);
  l_b->F_120 += m_a->M_000 * D_120(dx, dy, dz, r_inv);
  l_b->F_021 += m_a->M_000 * D_021(dx, dy, dz, r_inv);
  l_b->F_102 += m_a->M_000 * D_102(dx, dy, dz, r_inv);
  l_b->F_012 += m_a->M_000 * D_012(dx, dy, dz, r_inv);
  l_b->F_111 += m_a->M_000 * D_111(dx, dy, dz, r_inv);
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 3
#error "Missing implementation for order >3"
#endif

#ifdef SWIFT_DEBUG_CHECKS

  l_b->num_interacted += m_a->num_gpart;
#endif
}

/**
 * @brief Creates a copy of #grav_tensor shifted to a new location.
 *
 * Corresponds to equation (28e).
 *
 * @param la The #grav_tensor copy (content will  be overwritten).
 * @param lb The #grav_tensor to shift.
 * @param pos_a The position to which m_b will be shifted.
 * @param pos_b The current postion of the multipole to shift.
 * @param periodic Is the calculation periodic ?
 */
INLINE static void gravity_L2L(struct grav_tensor *la,
                               const struct grav_tensor *lb,
                               const double pos_a[3], const double pos_b[3],
                               int periodic) {

  /* Initialise everything to zero */
  gravity_field_tensors_init(la);

#ifdef SWIFT_DEBUG_CHECKS
  if (lb->num_interacted == 0) error("Shifting tensors that did not interact");
  la->num_interacted = lb->num_interacted;
#endif

  /* Distance to shift by */
  const double dx[3] = {pos_a[0] - pos_b[0], pos_a[1] - pos_b[1],
                        pos_a[2] - pos_b[2]};

  /* Shift 0th order term */
  la->F_000 += X_000(dx) * lb->F_000;

#if SELF_GRAVITY_MULTIPOLE_ORDER > 0
  /* Shift 1st order multipole term (addition to rank 0)*/
  la->F_000 +=
      X_100(dx) * lb->F_100 + X_010(dx) * lb->F_010 + X_001(dx) * lb->F_001;

  /* Shift 1st order multipole term (addition to rank 1)*/
  la->F_100 += X_000(dx) * lb->F_100;
  la->F_010 += X_000(dx) * lb->F_010;
  la->F_001 += X_000(dx) * lb->F_001;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 1

  /* Shift 2nd order multipole term (addition to rank 0)*/
  la->F_000 +=
      X_200(dx) * lb->F_200 + X_020(dx) * lb->F_020 + X_002(dx) * lb->F_002;
  la->F_000 +=
      X_110(dx) * lb->F_110 + X_101(dx) * lb->F_101 + X_011(dx) * lb->F_011;

  /* Shift 2nd order multipole term (addition to rank 1)*/
  la->F_100 +=
      X_100(dx) * lb->F_200 + X_010(dx) * lb->F_110 + X_001(dx) * lb->F_101;
  la->F_010 +=
      X_100(dx) * lb->F_110 + X_010(dx) * lb->F_020 + X_001(dx) * lb->F_011;
  la->F_001 +=
      X_100(dx) * lb->F_101 + X_010(dx) * lb->F_011 + X_001(dx) * lb->F_002;

  /* Shift 2nd order multipole term (addition to rank 2)*/
  la->F_200 += X_000(dx) * lb->F_200;
  la->F_020 += X_000(dx) * lb->F_020;
  la->F_002 += X_000(dx) * lb->F_002;
  la->F_110 += X_000(dx) * lb->F_110;
  la->F_101 += X_000(dx) * lb->F_101;
  la->F_011 += X_000(dx) * lb->F_011;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 2

  /* Shift 3rd order multipole term (addition to rank 0)*/
  la->F_000 +=
      X_300(dx) * lb->F_300 + X_030(dx) * lb->F_030 + X_003(dx) * lb->F_003;
  la->F_000 +=
      X_210(dx) * lb->F_210 + X_201(dx) * lb->F_201 + X_120(dx) * lb->F_120;
  la->F_000 +=
      X_021(dx) * lb->F_021 + X_102(dx) * lb->F_102 + X_012(dx) * lb->F_012;
  la->F_000 += X_111(dx) * lb->F_111;

  /* Shift 3rd order multipole term (addition to rank 1)*/
  la->F_100 +=
      X_200(dx) * lb->F_300 + X_020(dx) * lb->F_120 + X_002(dx) * lb->F_102;
  la->F_100 +=
      X_110(dx) * lb->F_210 + X_101(dx) * lb->F_201 + X_011(dx) * lb->F_111;
  la->F_010 +=
      X_200(dx) * lb->F_210 + X_020(dx) * lb->F_030 + X_002(dx) * lb->F_012;
  la->F_010 +=
      X_110(dx) * lb->F_120 + X_101(dx) * lb->F_111 + X_011(dx) * lb->F_021;
  la->F_001 +=
      X_200(dx) * lb->F_201 + X_020(dx) * lb->F_021 + X_002(dx) * lb->F_003;
  la->F_001 +=
      X_110(dx) * lb->F_111 + X_101(dx) * lb->F_102 + X_011(dx) * lb->F_012;

  /* Shift 3rd order multipole term (addition to rank 2)*/
  la->F_200 +=
      X_100(dx) * lb->F_300 + X_010(dx) * lb->F_210 + X_001(dx) * lb->F_201;
  la->F_020 +=
      X_100(dx) * lb->F_120 + X_010(dx) * lb->F_030 + X_001(dx) * lb->F_021;
  la->F_002 +=
      X_100(dx) * lb->F_102 + X_010(dx) * lb->F_012 + X_001(dx) * lb->F_003;
  la->F_110 +=
      X_100(dx) * lb->F_210 + X_010(dx) * lb->F_120 + X_001(dx) * lb->F_111;
  la->F_101 +=
      X_100(dx) * lb->F_201 + X_010(dx) * lb->F_111 + X_001(dx) * lb->F_102;
  la->F_011 +=
      X_100(dx) * lb->F_111 + X_010(dx) * lb->F_021 + X_001(dx) * lb->F_012;

  /* Shift 3rd order multipole term (addition to rank 2)*/
  la->F_300 += X_000(dx) * lb->F_300;
  la->F_030 += X_000(dx) * lb->F_030;
  la->F_003 += X_000(dx) * lb->F_003;
  la->F_210 += X_000(dx) * lb->F_210;
  la->F_201 += X_000(dx) * lb->F_201;
  la->F_120 += X_000(dx) * lb->F_120;
  la->F_021 += X_000(dx) * lb->F_021;
  la->F_102 += X_000(dx) * lb->F_102;
  la->F_012 += X_000(dx) * lb->F_012;
  la->F_111 += X_000(dx) * lb->F_111;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 3
#error "Missing implementation for order >3"
#endif
}

/**
 * @brief Applies the  #grav_tensor to a  #gpart.
 *
 * Corresponds to equation (28a).
 *
 * @param lb The gravity field tensor to apply.
 * @param loc The position of the gravity field tensor.
 * @param gp The #gpart to update.
 */
INLINE static void gravity_L2P(const struct grav_tensor *lb,
                               const double loc[3], struct gpart *gp) {

#ifdef SWIFT_DEBUG_CHECKS
  if (lb->num_interacted == 0) error("Interacting with empty field tensor");
  gp->num_interacted += lb->num_interacted;
#endif

#if SELF_GRAVITY_MULTIPOLE_ORDER > 0
  /* Distance to the multipole */
  const double dx[3] = {gp->x[0] - loc[0], gp->x[1] - loc[1],
                        gp->x[2] - loc[2]};

  /* 0th order interaction */
  gp->a_grav[0] += X_000(dx) * lb->F_100;
  gp->a_grav[1] += X_000(dx) * lb->F_010;
  gp->a_grav[2] += X_000(dx) * lb->F_001;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 1

  /* 1st order interaction */
  gp->a_grav[0] +=
      X_100(dx) * lb->F_200 + X_010(dx) * lb->F_110 + X_001(dx) * lb->F_101;
  gp->a_grav[1] +=
      X_100(dx) * lb->F_110 + X_010(dx) * lb->F_020 + X_001(dx) * lb->F_011;
  gp->a_grav[2] +=
      X_100(dx) * lb->F_101 + X_010(dx) * lb->F_011 + X_001(dx) * lb->F_002;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 2

  /* 2nd order interaction */
  gp->a_grav[0] +=
      X_200(dx) * lb->F_300 + X_020(dx) * lb->F_120 + X_002(dx) * lb->F_102;
  gp->a_grav[0] +=
      X_110(dx) * lb->F_210 + X_101(dx) * lb->F_201 + X_011(dx) * lb->F_111;
  gp->a_grav[1] +=
      X_200(dx) * lb->F_210 + X_020(dx) * lb->F_030 + X_002(dx) * lb->F_012;
  gp->a_grav[1] +=
      X_110(dx) * lb->F_120 + X_101(dx) * lb->F_111 + X_011(dx) * lb->F_021;
  gp->a_grav[2] +=
      X_200(dx) * lb->F_201 + X_020(dx) * lb->F_021 + X_002(dx) * lb->F_003;
  gp->a_grav[2] +=
      X_110(dx) * lb->F_111 + X_101(dx) * lb->F_102 + X_011(dx) * lb->F_012;
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 3
#error "Missing implementation for order >3"
#endif
#if SELF_GRAVITY_MULTIPOLE_ORDER > 4
#error "Missing implementation for order >4"
#endif
}

#endif /* SWIFT_MULTIPOLE_H */
