/*
 * ADAGELinearProgram.cpp
 *
 *  Created on: Aug 30, 2013
 *      Author: stolee
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ADAGELinearProgram.hpp"

ADAGELinearProgram::~ADAGELinearProgram()
{

}

/**
 * the given constraintset already has variable values.
 * Now, find fractions that correspond to those values.
 */
void ADAGELinearProgram::convertToFractions(constraintset* constraints)
{
	if ( constraints->variable_numerators == 0 )
	{
		constraints->variable_numerators = (int*) malloc(constraints->num_vars * sizeof(int));
		constraints->variable_denominators = (int*) malloc(constraints->num_vars * sizeof(int));
	}

	for ( int i = 0; i < constraints->num_vars; i++ )
	{
		double frac = constraints->variable_values[i];
		double closest_dist = 100.0;

		int best_b = 1;
		int best_a = 2;

		int max_num = 1000;
		int max_denom = 1000;

		for ( double b = 1.0; b <= max_denom; b += 1.0 )
		{
			// floor..
			double a = (double) (floor(frac * b));

			double aoverb = a / b;
			double diff = aoverb - frac;
			if ( diff < 0 )
			{
				diff = -diff;
			}

			if ( diff < closest_dist )
			{
				closest_dist = diff;
				best_b = (int) b;
				best_a = (int) a;
			}

			a = a + 1.0;
			aoverb = a / b;
			diff = (aoverb - frac);

			if ( diff < 0 )
			{
				diff = -diff;
			}

			if ( diff < closest_dist )
			{
				closest_dist = diff;
				best_b = (int) b;
				best_a = (int) a;
			}
		}

		constraints->variable_numerators[i] = best_a;
		constraints->variable_denominators[i] = best_b;

//		printf("%d/%d == %lf\n", best_a, best_b, frac);
	}
}
