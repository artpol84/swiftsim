/*******************************************************************************
 * This file is part of SWIFT.
 * Copyright (c) 2018 Matthieu Schaller (matthieu.schaller@durham.ac.uk)
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
#ifndef SWIFT_EAGLE_BH_IACT_H
#define SWIFT_EAGLE_BH_IACT_H

/* Local includes */
#include "hydro.h"
#include "random.h"
#include "space.h"

/**
 * @brief Density interaction between two particles (non-symmetric).
 *
 * @param r2 Comoving square distance between the two particles.
 * @param dx Comoving vector separating both particles (pi - pj).
 * @param hi Comoving smoothing-length of particle i.
 * @param hj Comoving smoothing-length of particle j.
 * @param bi First particle (black hole).
 * @param pj Second particle (gas, not updated).
 * @param xpj The extended data of the second particle (not updated).
 * @param cosmo The cosmological model.
 * @param ti_current Current integer time value (for random numbers).
 */
__attribute__((always_inline)) INLINE static void runner_iact_nonsym_bh_density(
    const float r2, const float *dx, const float hi, const float hj,
    struct bpart *restrict bi, const struct part *restrict pj,
    const struct xpart *restrict xpj, const struct cosmology *cosmo,
    const integertime_t ti_current) {

  float wi, wi_dx;

  /* Get r and 1/r. */
  const float r_inv = 1.0f / sqrtf(r2);
  const float r = r2 * r_inv;

  /* Compute the kernel function */
  const float hi_inv = 1.0f / hi;
  const float ui = r * hi_inv;
  kernel_deval(ui, &wi, &wi_dx);

  /* Compute contribution to the number of neighbours */
  bi->density.wcount += wi;
  bi->density.wcount_dh -= (hydro_dimension * wi + ui * wi_dx);

  /* Contribution to the number of neighbours */
  bi->num_ngbs += 1;

  /* Neighbour gas mass */
  const float mj = hydro_get_mass(pj);

  /* Contribution to the BH gas density */
  bi->rho_gas += mj * wi;

  /* Contribution to the total neighbour mass */
  bi->ngb_mass += mj;

  /* Neighbour sounds speed */
  const float cj = hydro_get_comoving_soundspeed(pj);

  /* Contribution to the smoothed sound speed */
  bi->sound_speed_gas += mj * cj * wi;

  /* Neighbour peculiar drifted velocity */
  const float vj[3] = {pj->v[0], pj->v[1], pj->v[2]};

  /* Contribution to the smoothed velocity */
  bi->velocity_gas[0] += mj * vj[0] * wi;
  bi->velocity_gas[1] += mj * vj[1] * wi;
  bi->velocity_gas[2] += mj * vj[2] * wi;

  /* Contribution to the circular valocity */
  const float dv[3] = {bi->v[0] - vj[0], bi->v[1] - vj[1], bi->v[2] - vj[2]};
  bi->circular_velocity_gas[0] += mj * wi * (dx[1] * dv[2] - dx[2] * dv[1]);
  bi->circular_velocity_gas[1] += mj * wi * (dx[2] * dv[0] - dx[0] * dv[2]);
  bi->circular_velocity_gas[2] += mj * wi * (dx[0] * dv[1] - dx[1] * dv[0]);

#ifdef DEBUG_INTERACTIONS_BH
  /* Update ngb counters */
  if (si->num_ngb_density < MAX_NUM_OF_NEIGHBOURS_BH)
    bi->ids_ngbs_density[si->num_ngb_density] = pj->id;

  /* Update ngb counters */
  ++si->num_ngb_density;
#endif
}

/**
 * @brief Swallowing interaction between two particles (non-symmetric).
 *
 * Function used to flag the gas particles that will be swallowed
 * by the black hole particle.
 *
 * @param r2 Comoving square distance between the two particles.
 * @param dx Comoving vector separating both particles (pi - pj).
 * @param hi Comoving smoothing-length of particle i.
 * @param hj Comoving smoothing-length of particle j.
 * @param bi First particle (black hole).
 * @param pj Second particle (gas)
 * @param xpj The extended data of the second particle.
 * @param cosmo The cosmological model.
 * @param ti_current Current integer time value (for random numbers).
 */
__attribute__((always_inline)) INLINE static void runner_iact_nonsym_bh_swallow(
    const float r2, const float *dx, const float hi, const float hj,
    struct bpart *restrict bi, struct part *restrict pj,
    struct xpart *restrict xpj, const struct cosmology *cosmo,
    const integertime_t ti_current) {

  float wi;

  /* Get r and 1/r. */
  const float r_inv = 1.0f / sqrtf(r2);
  const float r = r2 * r_inv;

  /* Compute the kernel function */
  const float hi_inv = 1.0f / hi;
  const float hi_inv_dim = pow_dimension(hi_inv);
  const float ui = r * hi_inv;
  kernel_eval(ui, &wi);

  /* Is the BH hungry? */
  if (bi->subgrid_mass > bi->mass) {

    /* Probability to swallow this particle
     * Recall that in SWIFT the SPH kernel is recovered by computing
     * kernel_eval() and muliplying by (1/h^d) */
    const float prob =
        (bi->subgrid_mass - bi->mass) * hi_inv_dim * wi / bi->rho_gas;

    /* Draw a random number (Note mixing both IDs) */
    const float rand = random_unit_interval(bi->id + pj->id, ti_current,
                                            random_number_BH_swallow);

    /* Are we lucky? */
    if (rand < prob) {

      /* This particle is swallowed by the BH with the largest ID of all the
       * candidates wanting to swallow it */
      if (pj->black_holes_data.swallow_id < bi->id) {

        message("BH %lld wants to swallow gas particle %lld", bi->id, pj->id);

        pj->black_holes_data.swallow_id = bi->id;

      } else {

        message(
            "BH %lld wants to swallow gas particle %lld BUT CANNOT (old "
            "swallow id=%lld)",
            bi->id, pj->id, pj->black_holes_data.swallow_id);
      }
    }
  }
}

/**
 * @brief Feedback interaction between two particles (non-symmetric).
 *
 * @param r2 Comoving square distance between the two particles.
 * @param dx Comoving vector separating both particles (pi - pj).
 * @param hi Comoving smoothing-length of particle i.
 * @param hj Comoving smoothing-length of particle j.
 * @param bi First particle (black hole).
 * @param pj Second particle (gas)
 * @param xpj The extended data of the second particle.
 * @param cosmo The cosmological model.
 * @param ti_current Current integer time value (for random numbers).
 */
__attribute__((always_inline)) INLINE static void
runner_iact_nonsym_bh_feedback(const float r2, const float *dx, const float hi,
                               const float hj, struct bpart *restrict bi,
                               struct part *restrict pj,
                               struct xpart *restrict xpj,
                               const struct cosmology *cosmo,
                               const integertime_t ti_current) {

  /* Get the heating probability */
  const float prob = bi->to_distribute.AGN_heating_probability;

  /* Are we doing some feedback? */
  if (prob > 0.f) {

    /* Draw a random number (Note mixing both IDs) */
    const float rand = random_unit_interval(bi->id + pj->id, ti_current,
                                            random_number_BH_feedback);

    /* Are we lucky? */
    if (rand < prob) {

      /* Compute new energy of this particle */
      const double u_init = hydro_get_physical_internal_energy(pj, xpj, cosmo);
      const float delta_u = bi->to_distribute.AGN_delta_u;
      const double u_new = u_init + delta_u;

      hydro_set_physical_internal_energy(pj, xpj, cosmo, u_new);
      hydro_set_drifted_physical_internal_energy(pj, cosmo, u_new);

      /* Impose maximal viscosity */
      hydro_diffusive_feedback_reset(pj);

      /* message( */
      /*     "We did some AGN heating! id %llu BH id %llu probability " */
      /*     " %.5e  random_num %.5e du %.5e du/ini %.5e", */
      /*     pj->id, bi->id, prob, rand, delta_u, delta_u / u_init); */
    }
  }

#ifdef DEBUG_INTERACTIONS_BH
  /* Update ngb counters */
  if (si->num_ngb_force < MAX_NUM_OF_NEIGHBOURS_BH)
    bi->ids_ngbs_force[si->num_ngb_force] = pj->id;

  /* Update ngb counters */
  ++si->num_ngb_force;
#endif
}

#endif /* SWIFT_EAGLE_BH_IACT_H */
