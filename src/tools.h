/*******************************************************************************
 * This file is part of SWIFT.
 * Copyright (c) 2012 Pedro Gonnet (pedro.gonnet@durham.ac.uk),
 *                    Matthieu Schaller (matthieu.schaller@durham.ac.uk)
 * Copyright (c) 2015 Peter W. Draper (p.w.draper@durham.ac.uk)
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

#ifndef SWIFT_TOOL_H
#define SWIFT_TOOL_H

/* Config parameters. */
#include "../config.h"

/* Includes. */
#include "cell.h"
#include "part.h"
#include "runner.h"

void factor(int value, int *f1, int *f2);
void density_dump(int N);
void pairs_single_grav(double *dim, long long int pid,
                       struct gpart *restrict gparts, const struct part *parts,
                       int N, int periodic);
void pairs_single_density(double *dim, long long int pid,
                          struct part *restrict parts, int N, int periodic);

void pairs_all_density(struct runner *r, struct cell *ci, struct cell *cj);
void self_all_density(struct runner *r, struct cell *ci);

void pairs_n2(double *dim, struct part *restrict parts, int N, int periodic);

double random_uniform(double a, double b);
void shuffle_particles(struct part *parts, const int count);
void generate_random_positions(struct cell *c);

#endif /* SWIFT_TOOL_H */
