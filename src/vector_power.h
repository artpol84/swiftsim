/*******************************************************************************
 * This file is part of SWIFT.
 * Copyright (c) 2016 Matthieu Schaller (matthieu.schaller@durham.ac.uk)
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
#ifndef SWIFT_VECTOR_POWER_H
#define SWIFT_VECTOR_POWER_H

/**
 * @file vector_power.h
 * @brief Powers of 3D vectors to a multi-index with factorial.
 *
 * These expressions are to be used in 3D Taylor series.
 *
 * We use the notation of Dehnen, Computational Astrophysics and Cosmology,
 * 1, 1, pp. 24 (2014), arXiv:1405.2255.
 *
 * We compute \f$ \frac{1}{\vec{m}!}\vec{v}^{\vec{m}} \f$ for all relevant m.
 */

/* Config parameters. */
#include "../config.h"

/* Local headers. */
#include "inline.h"

/***************************/
/* 0th order vector powers */
/***************************/

/**
 * @brief \f$ \frac{1}{(0,0,0)!}\vec{v}^{(0,0,0)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_000(const float v[3]) {

  return 1.;
}

/***************************/
/* 1st order vector powers */
/***************************/

/**
 * @brief \f$ \frac{1}{(1,0,0)!}\vec{v}^{(1,0,0)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_100(const float v[3]) {

  return v[0];
}

/**
 * @brief \f$ \frac{1}{(0,1,0)!}\vec{v}^{(0,1,0)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_010(const float v[3]) {

  return v[1];
}

/**
 * @brief \f$ \frac{1}{(0,0,1)!}\vec{v}^{(0,0,1)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_001(const float v[3]) {

  return v[2];
}

/***************************/
/* 2nd order vector powers */
/***************************/

/**
 * @brief \f$ \frac{1}{(2,0,0)!}\vec{v}^{(2,0,0)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_200(const float v[3]) {

  return 0.5f * v[0] * v[0];
}

/**
 * @brief \f$ \frac{1}{(0,2,0)!}\vec{v}^{(0,2,0)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_020(const float v[3]) {

  return 0.5f * v[1] * v[1];
}

/**
 * @brief \f$ \frac{1}{(0,0,2)!}\vec{v}^{(0,0,2)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_002(const float v[3]) {

  return 0.5f * v[2] * v[2];
}

/**
 * @brief \f$ \frac{1}{(1,1,0)!}\vec{v}^{(1,1,0)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_110(const float v[3]) {

  return v[0] * v[1];
}

/**
 * @brief \f$ \frac{1}{(1,0,1)!}\vec{v}^{(1,0,1)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_101(const float v[3]) {

  return v[0] * v[2];
}

/**
 * @brief \f$ \frac{1}{(0,1,1)!}\vec{v}^{(0,1,1)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_011(const float v[3]) {

  return v[1] * v[2];
}

/***************************/
/* 3rd order vector powers */
/***************************/

/**
 * @brief \f$ \frac{1}{(3,0,0)!}\vec{v}^{(3,0,0)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_300(const float v[3]) {

  return 0.1666666666666667f * v[0] * v[0] * v[0];
}

/**
 * @brief \f$ \frac{1}{(0,3,0)!}\vec{v}^{(0,3,0)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_030(const float v[3]) {

  return 0.1666666666666667f * v[1] * v[1] * v[1];
}

/**
 * @brief \f$ \frac{1}{(0,0,3)!}\vec{v}^{(0,0,3)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_003(const float v[3]) {

  return 0.1666666666666667f * v[2] * v[2] * v[2];
}

/**
 * @brief \f$ \frac{1}{(2,1,0)!}\vec{v}^{(2,1,0)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_210(const float v[3]) {

  return 0.5f * v[0] * v[0] * v[1];
}

/**
 * @brief \f$ \frac{1}{(2,0,1)!}\vec{v}^{(2,0,1)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_201(const float v[3]) {

  return 0.5f * v[0] * v[0] * v[2];
}

/**
 * @brief \f$ \frac{1}{(1,2,0)!}\vec{v}^{(1,2,0)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_120(const float v[3]) {

  return 0.5f * v[0] * v[1] * v[1];
}

/**
 * @brief \f$ \frac{1}{(0,2,1)!}\vec{v}^{(0,2,1)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_021(const float v[3]) {

  return 0.5f * v[1] * v[1] * v[2];
}

/**
 * @brief \f$ \frac{1}{(1,0,2)!}\vec{v}^{(1,0,2)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_102(const float v[3]) {

  return 0.5f * v[0] * v[2] * v[2];
}

/**
 * @brief \f$ \frac{1}{(0,1,2)!}\vec{v}^{(0,1,2)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_012(const float v[3]) {

  return 0.5 * v[1] * v[2] * v[2];
}

/**
 * @brief \f$ \frac{1}{(1,1,1)!}\vec{v}^{(1,1,1)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_111(const float v[3]) {

  return v[0] * v[1] * v[2];
}

/***************************/
/* 4th order vector powers */
/***************************/

/**
 * @brief \f$ \frac{1}{(4,0,0)!}\vec{v}^{(4,0,0)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_400(const float v[3]) {

  const float vv = v[0] * v[0];
  return 0.041666666666666667f * vv * vv;
}

/**
 * @brief \f$ \frac{1}{(0,4,0)!}\vec{v}^{(0,4,0)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_040(const float v[3]) {

  const float vv = v[1] * v[1];
  return 0.041666666666666667f * vv * vv;
}

/**
 * @brief \f$ \frac{1}{(0,0,4)!}\vec{v}^{(0,0,4)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_004(const float v[3]) {

  const float vv = v[2] * v[2];
  return 0.041666666666666667f * vv * vv;
}

/**
 * @brief \f$ \frac{1}{(3,1,0)!}\vec{v}^{(3,1,0)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_310(const float v[3]) {

  return 0.1666666666666667f * v[0] * v[0] * v[0] * v[1];
}

/**
 * @brief \f$ \frac{1}{(3,0,1)!}\vec{v}^{(3,0,1)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_301(const float v[3]) {

  return 0.1666666666666667f * v[0] * v[0] * v[0] * v[2];
}

/**
 * @brief \f$ \frac{1}{(1,3,0)!}\vec{v}^{(1,3,0)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_130(const float v[3]) {

  return 0.1666666666666667f * v[0] * v[1] * v[1] * v[1];
}

/**
 * @brief \f$ \frac{1}{(0,3,1)!}\vec{v}^{(0,3,1)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_031(const float v[3]) {

  return 0.1666666666666667f * v[1] * v[1] * v[1] * v[2];
}

/**
 * @brief \f$ \frac{1}{(1,0,3)!}\vec{v}^{(1,0,3)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_103(const float v[3]) {

  return 0.1666666666666667f * v[0] * v[2] * v[2] * v[2];
}

/**
 * @brief \f$ \frac{1}{(0,1,3)!}\vec{v}^{(0,1,3)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_013(const float v[3]) {

  return 0.1666666666666667 * v[1] * v[2] * v[2] * v[2];
}

/**
 * @brief \f$ \frac{1}{(2,2,0)!}\vec{v}^{(2,2,0)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_220(const float v[3]) {

  return 0.25f * v[0] * v[0] * v[1] * v[1];
}

/**
 * @brief \f$ \frac{1}{(2,0,2)!}\vec{v}^{(2,0,2)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_202(const float v[3]) {

  return 0.25f * v[0] * v[0] * v[2] * v[2];
}

/**
 * @brief \f$ \frac{1}{(0,2,2)!}\vec{v}^{(0,2,2)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_022(const float v[3]) {

  return 0.25f * v[1] * v[1] * v[2] * v[2];
}

/**
 * @brief \f$ \frac{1}{(2,1,1)!}\vec{v}^{(2,1,1)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_211(const float v[3]) {

  return 0.5f * v[0] * v[0] * v[1] * v[2];
}

/**
 * @brief \f$ \frac{1}{(1,2,1)!}\vec{v}^{(1,2,1)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_121(const float v[3]) {

  return 0.5f * v[0] * v[1] * v[1] * v[2];
}

/**
 * @brief \f$ \frac{1}{(1,1,2)!}\vec{v}^{(1,1,2)} \f$.
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_112(const float v[3]) {

  return 0.5f * v[0] * v[1] * v[2] * v[2];
}

/***************************/
/* 5th order vector powers */
/***************************/

/**
 * @brief Compute \f$ \frac{1}{(0,0,5)!}\vec{v}^{(0,0,5)} \f$.
 *
 * Note \f$ \vec{v}^{(0,0,5)} = v_z^5 \f$
 * and \f$ \frac{1}{(0,0,5)!} = 1/(0!*0!*5!) = 1/120 = 8.333333e-03 \f$
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_005(const float v[3]) {

  const float vv = v[2] * v[2];
  const float vvvv = vv * vv;
  return 8.333333333333333e-03f * vvvv * v[2];
}

/**
 * @brief Compute \f$ \frac{1}{(0,1,4)!}\vec{v}^{(0,1,4)} \f$.
 *
 * Note \f$ \vec{v}^{(0,1,4)} = v_y^1 v_z^4 \f$
 * and \f$ \frac{1}{(0,1,4)!} = 1/(0!*1!*4!) = 1/24 = 4.166667e-02 \f$
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_014(const float v[3]) {

  const float vv = v[2] * v[2];
  const float vvvv = vv * vv;
  return 4.166666666666666e-02f * v[1] * vvvv;
}

/**
 * @brief Compute \f$ \frac{1}{(0,2,3)!}\vec{v}^{(0,2,3)} \f$.
 *
 * Note \f$ \vec{v}^{(0,2,3)} = v_y^2 v_z^3 \f$
 * and \f$ \frac{1}{(0,2,3)!} = 1/(0!*2!*3!) = 1/12 = 8.333333e-02 \f$
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_023(const float v[3]) {

  return 8.333333333333333e-02f * v[1] * v[1] * v[2] * v[2] * v[2];
}

/**
 * @brief Compute \f$ \frac{1}{(0,3,2)!}\vec{v}^{(0,3,2)} \f$.
 *
 * Note \f$ \vec{v}^{(0,3,2)} = v_y^3 v_z^2 \f$
 * and \f$ \frac{1}{(0,3,2)!} = 1/(0!*3!*2!) = 1/12 = 8.333333e-02 \f$
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_032(const float v[3]) {

  return 8.333333333333333e-02f * v[1] * v[1] * v[1] * v[2] * v[2];
}

/**
 * @brief Compute \f$ \frac{1}{(0,4,1)!}\vec{v}^{(0,4,1)} \f$.
 *
 * Note \f$ \vec{v}^{(0,4,1)} = v_y^4 v_z^1 \f$
 * and \f$ \frac{1}{(0,4,1)!} = 1/(0!*4!*1!) = 1/24 = 4.166667e-02 \f$
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_041(const float v[3]) {

  const float vv = v[1] * v[1];
  const float vvvv = vv * vv;
  return 4.166666666666666e-02f * vvvv * v[2];
}

/**
 * @brief Compute \f$ \frac{1}{(0,5,0)!}\vec{v}^{(0,5,0)} \f$.
 *
 * Note \f$ \vec{v}^{(0,5,0)} = v_y^5 \f$
 * and \f$ \frac{1}{(0,5,0)!} = 1/(0!*5!*0!) = 1/120 = 8.333333e-03 \f$
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_050(const float v[3]) {

  const float vv = v[1] * v[1];
  const float vvvv = vv * vv;
  return 8.333333333333333e-03f * vvvv * v[1];
}

/**
 * @brief Compute \f$ \frac{1}{(1,0,4)!}\vec{v}^{(1,0,4)} \f$.
 *
 * Note \f$ \vec{v}^{(1,0,4)} = v_x^1 v_z^4 \f$
 * and \f$ \frac{1}{(1,0,4)!} = 1/(1!*0!*4!) = 1/24 = 4.166667e-02 \f$
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_104(const float v[3]) {

  const float vv = v[2] * v[2];
  const float vvvv = vv * vv;
  return 4.166666666666666e-02f * v[0] * vvvv;
}

/**
 * @brief Compute \f$ \frac{1}{(1,1,3)!}\vec{v}^{(1,1,3)} \f$.
 *
 * Note \f$ \vec{v}^{(1,1,3)} = v_x^1 v_y^1 v_z^3 \f$
 * and \f$ \frac{1}{(1,1,3)!} = 1/(1!*1!*3!) = 1/6 = 1.666667e-01 \f$
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_113(const float v[3]) {

  return 1.666666666666667e-01f * v[0] * v[1] * v[2] * v[2] * v[2];
}

/**
 * @brief Compute \f$ \frac{1}{(1,2,2)!}\vec{v}^{(1,2,2)} \f$.
 *
 * Note \f$ \vec{v}^{(1,2,2)} = v_x^1 v_y^2 v_z^2 \f$
 * and \f$ \frac{1}{(1,2,2)!} = 1/(1!*2!*2!) = 1/4 = 2.500000e-01 \f$
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_122(const float v[3]) {

  return 2.500000000000000e-01f * v[0] * v[1] * v[1] * v[2] * v[2];
}

/**
 * @brief Compute \f$ \frac{1}{(1,3,1)!}\vec{v}^{(1,3,1)} \f$.
 *
 * Note \f$ \vec{v}^{(1,3,1)} = v_x^1 v_y^3 v_z^1 \f$
 * and \f$ \frac{1}{(1,3,1)!} = 1/(1!*3!*1!) = 1/6 = 1.666667e-01 \f$
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_131(const float v[3]) {

  return 1.666666666666667e-01f * v[0] * v[1] * v[1] * v[1] * v[2];
}

/**
 * @brief Compute \f$ \frac{1}{(1,4,0)!}\vec{v}^{(1,4,0)} \f$.
 *
 * Note \f$ \vec{v}^{(1,4,0)} = v_x^1 v_y^4 \f$
 * and \f$ \frac{1}{(1,4,0)!} = 1/(1!*4!*0!) = 1/24 = 4.166667e-02 \f$
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_140(const float v[3]) {

  const float vv = v[1] * v[1];
  const float vvvv = vv * vv;
  return 4.166666666666666e-02f * v[0] * vvvv;
}

/**
 * @brief Compute \f$ \frac{1}{(2,0,3)!}\vec{v}^{(2,0,3)} \f$.
 *
 * Note \f$ \vec{v}^{(2,0,3)} = v_x^2 v_z^3 \f$
 * and \f$ \frac{1}{(2,0,3)!} = 1/(2!*0!*3!) = 1/12 = 8.333333e-02 \f$
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_203(const float v[3]) {

  return 8.333333333333333e-02f * v[0] * v[0] * v[2] * v[2] * v[2];
}

/**
 * @brief Compute \f$ \frac{1}{(2,1,2)!}\vec{v}^{(2,1,2)} \f$.
 *
 * Note \f$ \vec{v}^{(2,1,2)} = v_x^2 v_y^1 v_z^2 \f$
 * and \f$ \frac{1}{(2,1,2)!} = 1/(2!*1!*2!) = 1/4 = 2.500000e-01 \f$
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_212(const float v[3]) {

  return 2.500000000000000e-01f * v[0] * v[0] * v[1] * v[2] * v[2];
}

/**
 * @brief Compute \f$ \frac{1}{(2,2,1)!}\vec{v}^{(2,2,1)} \f$.
 *
 * Note \f$ \vec{v}^{(2,2,1)} = v_x^2 v_y^2 v_z^1 \f$
 * and \f$ \frac{1}{(2,2,1)!} = 1/(2!*2!*1!) = 1/4 = 2.500000e-01 \f$
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_221(const float v[3]) {

  return 2.500000000000000e-01f * v[0] * v[0] * v[1] * v[1] * v[2];
}

/**
 * @brief Compute \f$ \frac{1}{(2,3,0)!}\vec{v}^{(2,3,0)} \f$.
 *
 * Note \f$ \vec{v}^{(2,3,0)} = v_x^2 v_y^3 \f$
 * and \f$ \frac{1}{(2,3,0)!} = 1/(2!*3!*0!) = 1/12 = 8.333333e-02 \f$
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_230(const float v[3]) {

  return 8.333333333333333e-02f * v[0] * v[0] * v[1] * v[1] * v[1];
}

/**
 * @brief Compute \f$ \frac{1}{(3,0,2)!}\vec{v}^{(3,0,2)} \f$.
 *
 * Note \f$ \vec{v}^{(3,0,2)} = v_x^3 v_z^2 \f$
 * and \f$ \frac{1}{(3,0,2)!} = 1/(3!*0!*2!) = 1/12 = 8.333333e-02 \f$
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_302(const float v[3]) {

  return 8.333333333333333e-02f * v[0] * v[0] * v[0] * v[2] * v[2];
}

/**
 * @brief Compute \f$ \frac{1}{(3,1,1)!}\vec{v}^{(3,1,1)} \f$.
 *
 * Note \f$ \vec{v}^{(3,1,1)} = v_x^3 v_y^1 v_z^1 \f$
 * and \f$ \frac{1}{(3,1,1)!} = 1/(3!*1!*1!) = 1/6 = 1.666667e-01 \f$
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_311(const float v[3]) {

  return 1.666666666666667e-01f * v[0] * v[0] * v[0] * v[1] * v[2];
}

/**
 * @brief Compute \f$ \frac{1}{(3,2,0)!}\vec{v}^{(3,2,0)} \f$.
 *
 * Note \f$ \vec{v}^{(3,2,0)} = v_x^3 v_y^2 \f$
 * and \f$ \frac{1}{(3,2,0)!} = 1/(3!*2!*0!) = 1/12 = 8.333333e-02 \f$
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_320(const float v[3]) {

  return 8.333333333333333e-02f * v[0] * v[0] * v[0] * v[1] * v[1];
}

/**
 * @brief Compute \f$ \frac{1}{(4,0,1)!}\vec{v}^{(4,0,1)} \f$.
 *
 * Note \f$ \vec{v}^{(4,0,1)} = v_x^4 v_z^1 \f$
 * and \f$ \frac{1}{(4,0,1)!} = 1/(4!*0!*1!) = 1/24 = 4.166667e-02 \f$
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_401(const float v[3]) {

  const float vv = v[0] * v[0];
  const float vvvv = vv * vv;
  return 4.166666666666666e-02f * vvvv * v[2];
}

/**
 * @brief Compute \f$ \frac{1}{(4,1,0)!}\vec{v}^{(4,1,0)} \f$.
 *
 * Note \f$ \vec{v}^{(4,1,0)} = v_x^4 v_y^1 \f$
 * and \f$ \frac{1}{(4,1,0)!} = 1/(4!*1!*0!) = 1/24 = 4.166667e-02 \f$
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_410(const float v[3]) {

  const float vv = v[0] * v[0];
  const float vvvv = vv * vv;
  return 4.166666666666666e-02f * vvvv * v[1];
}

/**
 * @brief Compute \f$ \frac{1}{(5,0,0)!}\vec{v}^{(5,0,0)} \f$.
 *
 * Note \f$ \vec{v}^{(5,0,0)} = v_x^5 \f$
 * and \f$ \frac{1}{(5,0,0)!} = 1/(5!*0!*0!) = 1/120 = 8.333333e-03 \f$
 *
 * @param v vector (\f$ v \f$).
 */
__attribute__((always_inline)) INLINE static float X_500(const float v[3]) {

  const float vv = v[0] * v[0];
  const float vvvv = vv * vv;
  return 8.333333333333333e-03f * vvvv * v[0];
}

#endif /* SWIFT_VECTOR_POWER_H */
