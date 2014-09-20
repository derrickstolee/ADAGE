/*
 * constraintset.hpp
 *
 *  Created on: Aug 26, 2013
 *      Author: dstolee
 */

#ifndef CONSTRAINTSET_HPP_
#define CONSTRAINTSET_HPP_

typedef struct constraint_struct
{
	int num_constraints;
	int size_constraints;
	int* constraint_start;
	int* constraint_size;
	int* constraint_rhs;
	bool* constraint_has_opt; // does this constraint have a "+w"?
	int* constraint_opt_coeff; // does this constraint have a "+w"?
	int opt_coeff;

	int num_variable;
	int size_variable;
	int* variable_index;
	int* variable_coeff;


	int num_vars;
	double* variable_values;
	int* variable_numerators;
	int* variable_denominators;;
} constraintset;


void initConstraintSet(constraintset* c);
void freeConstraintSet(constraintset* c);
void addConstraint(constraintset* c, int c_size, int* variable_indices, int* variable_coeffs, int rhs);

#endif /* CONSTRAINTSET_HPP_ */
