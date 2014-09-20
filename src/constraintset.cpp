/*
 * constraintset.cpp
 *
 *  Created on: Sep 1, 2013
 *      Author: stolee
 */

#include <stdlib.h>
#include <stdio.h>
#include "constraintset.hpp"

void initConstraintSet(constraintset* c)
{
	c->num_vars = 0;

	c->size_variable = 10000000;
	c->num_variable = 0;
	c->variable_coeff = (int*) malloc(c->size_variable * sizeof(int));
	c->variable_index = (int*) malloc(c->size_variable * sizeof(int));
	c->variable_numerators = (int*) malloc(c->size_variable * sizeof(int));
	c->variable_denominators = (int*) malloc(c->size_variable * sizeof(int));

	c->num_constraints = 0;
	c->size_constraints = 500000;
	c->constraint_size = (int*) malloc(c->size_constraints * sizeof(int));
	c->constraint_start = (int*) malloc(c->size_constraints * sizeof(int));
	c->constraint_rhs = (int*) malloc(c->size_constraints * sizeof(int));
	c->constraint_has_opt = (bool*) malloc(c->size_constraints * sizeof(bool));
	c->constraint_opt_coeff = (int*) malloc(c->size_constraints * sizeof(int));
}

void freeConstraintSet(constraintset* c)
{
	if ( c == 0 )
	{
		return;
	}

	free(c->variable_denominators);
	free(c->variable_numerators);
	free(c->variable_coeff);
	free(c->variable_index);
	free(c->constraint_has_opt);
	free(c->constraint_opt_coeff);
	free(c->constraint_size);
	free(c->constraint_start);
	free(c->constraint_rhs);
}

void addConstraint(constraintset* c, int c_size, int* variable_indices, int* variable_coeffs, int rhs)
{
	while ( c->num_variable + c_size >= c->size_variable )
	{
		(c->size_variable) += 500000;
		c->variable_coeff = (int*) realloc(c->variable_coeff, c->size_variable * sizeof(int));
		c->variable_index = (int*) realloc(c->variable_index, c->size_variable * sizeof(int));
		c->variable_numerators = (int*) realloc(c->variable_numerators, c->size_variable * sizeof(int));
		c->variable_denominators = (int*) realloc(c->variable_denominators, c->size_variable * sizeof(int));
	}

	while ( c->num_constraints >= c->size_constraints )
	{
		(c->size_constraints) += 500000;
		c->constraint_has_opt = (bool*) realloc(c->constraint_has_opt, c->size_constraints * sizeof(bool));
		c->constraint_opt_coeff = (int*) realloc(c->constraint_opt_coeff, c->size_constraints * sizeof(int));
		c->constraint_start = (int*) realloc(c->constraint_start, c->size_constraints * sizeof(int));
		c->constraint_size = (int*) realloc(c->constraint_size, c->size_constraints * sizeof(int));
		c->constraint_rhs = (int*) realloc(c->constraint_rhs, c->size_constraints * sizeof(int));
	}

	c->constraint_start[c->num_constraints] = c->num_variable;
	c->constraint_size[c->num_constraints] = 0;

	c->constraint_rhs[c->num_constraints] = rhs;

	c->constraint_has_opt[c->num_constraints] = false; // to start

	int start = c->num_variable;
	int cur_index = c->num_variable;
	for ( int i = 0; i < c_size; i++ )
	{
		if ( variable_indices[i] < 0 )
		{
			// TODO: if we worry about cluster sums, then we may need a coefficient here!
			c->constraint_has_opt[c->num_constraints] = true;
			c->constraint_opt_coeff[c->num_constraints] = variable_coeffs[i];
			continue;
		}

		// look for this next variable somewhere in the current list (and modify the coefficient only)
		bool found = false;
		for ( int j = start; !found && j < cur_index; j++ )
		{
			if ( c->variable_index[j] == variable_indices[i] )
			{
				c->variable_coeff[j] += variable_coeffs[i];
				found = true;
			}
		}

		if ( !found )
		{
			c->variable_index[cur_index] = variable_indices[i];

			if ( variable_indices[i] >= c->num_vars )
			{
				c->num_vars = variable_indices[i] + 1;
			}

			c->variable_coeff[cur_index] = variable_coeffs[i];

			cur_index++;
		}
	}
	c->constraint_size[c->num_constraints] = cur_index - c->constraint_start[c->num_constraints];
	c->num_variable = cur_index;

	(c->num_constraints)++;
}

