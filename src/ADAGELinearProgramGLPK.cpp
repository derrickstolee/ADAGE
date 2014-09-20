/*
 * ADAGELinearProgramGLPK.cpp
 *
 *  Created on: Aug 30, 2013
 *      Author: stolee
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// #include "point.hpp"
// #include "PointSet.hpp"
// #include "GridConfiguration.hpp"
// #include "Grid.hpp"
// #include "DischargingRule.hpp"
// #include "GridProblem.hpp"
#include "ADAGELinearProgram.hpp"

extern "C"
{
#include <glpk.h>
}

ADAGELinearProgram::ADAGELinearProgram()
{
	this->lp = 0;
	this->num_lp_constraints = 0;
	this->parm = 0;
}

double ADAGELinearProgram::solveLP(constraintset* constraints, bool minimize_density, bool do_solve)
{
//	glp_term_hook(NULL, NULL);
//	glp_term_out(0);

	this->lp = glp_create_prob();

	this->parm = (glp_smcp*) malloc(sizeof(glp_smcp));
	glp_init_smcp(this->parm);
	this->parm->msg_lev = GLP_MSG_ALL; //GLP_MSG_ERR
	// this->parm->meth = GLP_DUALP; // USE PRIMAL_DUAL ALGORITHM!
	// this->parm->meth = GLP_DUAL; // USE DUAL SIMPLEX!
	this->parm->meth = GLP_PRIMAL; // USE PRIMAL SIMPLEX!
	this->parm->presolve = GLP_OFF; // use presolver?

	// fill in the LP: variables, objective, and constraints
	/**
	 * We have variables alpha[*] and w.
	 */
	int n = constraints->num_vars + 1;

	glp_add_cols(this->lp, n);

	int* row_indices = (int*) malloc(2 * (n + 1) * sizeof(int));
	double* row_values = (double*) malloc(2 * (n + 1) * sizeof(double));

	for ( int i = 1; i < n; i++ ) // GLPK is 1-indexed
	{
		// free variables!
		glp_set_col_bnds(this->lp, i, GLP_FR, 0.0, 100000.0);
	}
	glp_set_col_bnds(this->lp, n, GLP_LO, 0.0, 100000.0);

	this->num_lp_constraints = 0;

	// first, all of the positive discharging rules where the pull point is NOT an element
	int cur_num_rows = 0;
	int cur_col_index = 1;

	// Encoding ALL constraints!
	for ( int i = 0; i < constraints->num_constraints; i++ )
	{
		// fill in the values for this constraint!
		int start = constraints->constraint_start[i];
		for ( int j = 0; j < constraints->constraint_size[i]; j++ )
		{
			int index = constraints->variable_index[start + j] + 1;
			double coeff = constraints->variable_coeff[start + j];

			bool found = false;
			for ( int k = 0; !found && k < cur_col_index; k++ )
			{
				if ( row_indices[k] == index )
				{
					found = true;
					row_values[k] = row_values[k] + coeff;
				}
			}

			if ( !found )
			{
				row_indices[cur_col_index] = index; // GLPK is 1-indexed
				row_values[cur_col_index] = coeff;
				cur_col_index++;
			}
		}

		if ( constraints->constraint_has_opt[i] )
		{
			// add in the last variable!
			row_indices[cur_col_index] = n; // GLPK is 1-indexed
			row_values[cur_col_index] = constraints->constraint_opt_coeff[i];
			cur_col_index++;
		}

		glp_add_rows(this->lp, 1);
		(this->num_lp_constraints)++;
		cur_num_rows++; // GLPK is 1-indexed

//		printf("Adding row %4d with %4d columns: ", cur_num_rows, cur_col_index - 1);
//		for ( int k = 1; k < cur_col_index; k++ )
//		{
//			printf("v[%3d]=%3lf ", row_indices[k], row_values[k]);
//		}
//		printf("\n");
//		fflush(stdout);

		glp_set_mat_row(this->lp, cur_num_rows, cur_col_index - 1, row_indices, row_values);

		if ( minimize_density )
		{
			// if we wish to maximize w (as a lower bound on density), we maximize w and spread charge such that it is >= ... .
			glp_set_row_bnds(this->lp, cur_num_rows, GLP_LO, constraints->constraint_rhs[i], 1000.0);
		}
		else
		{
			// if we wish to minimize 1/w (as an upper bound on density), we maximize w and spread charge such that it is <= ... .
			glp_set_row_bnds(this->lp, cur_num_rows, GLP_UP, -10000.0, constraints->constraint_rhs[i]);
		}

		cur_col_index = 1;
	}

	free(row_indices);
	row_indices = 0;
	free(row_values);
	row_values = 0;


	glp_std_basis(this->lp);

	for ( int i = 1; i < n; i++ )
	{
		glp_set_obj_coef(this->lp, i, 0.0);
	}
	glp_set_obj_coef(this->lp, n, 1.0);

	if ( minimize_density )
	{
		// if we wish to maximize 1/w, we maximize w and spread charge such that it is >= ... .
		glp_set_obj_dir(this->lp, GLP_MAX);
	}
	else
	{
		// if we wish to minimize 1/w, we minimize w and spread charge such that it is <= ... .
		glp_set_obj_dir(this->lp, GLP_MIN);
	}

	glp_write_lp(this->lp, 0, "adage.lp");

	if ( !do_solve )
	{
		return 0.0;
	}

	int result = glp_simplex(this->lp, this->parm);

	// or glp_simplex?

	if ( result == GLP_EOBJUL )
	{
		printf("GLP_EOBJUL\n\n");
		return -1;
	}

	if ( result == GLP_EBADB )
	{
		printf("GLP_EBADB\n\n");
		return -1;
	}
	if ( result == GLP_ESING )
	{
		printf("GLP_ESING\n\n");
		return -1;
	}
	if ( result == GLP_ECOND )
	{
		printf("GLP_ECOND\n\n");
		return -1;
	}
	if ( result == GLP_EFAIL )
	{
		printf("GLP_EFAIL\n\n");
		return -1;
	}
	if ( result == GLP_EITLIM )
	{
		printf("GLP_EITLIM\n\n");
		return -1;
	}
	if ( result == GLP_ENOPFS )
	{
		printf("GLP_ENOPFS\n\n");
		return -1;
	}
	if ( result == GLP_ENODFS )
	{
		printf("GLP_ENODFS\n\n");
		return -1;
	}

	if ( result != 0 )
	{
		printf("--The Solver had an error! %d\n", result);
		return -1.0;
	}

	result = glp_get_status(this->lp);
	if ( result == GLP_INFEAS || result == GLP_NOFEAS )
	{
		printf("-- infeasible!\n");
		return -2.0;
	}

	double w = glp_get_obj_val(this->lp);

	constraints->variable_values = (double*) malloc(n * sizeof(double));
	for ( int i = 0; i < n; i++ )
	{
		constraints->variable_values[i] = glp_get_col_prim(this->lp, i + 1);
	}

	/* free the LP */
	if ( this->lp != 0 )
	{
		glp_delete_prob(this->lp);
		this->lp = 0;

		free(this->parm);
		this->parm = 0;
	}

	return w;
}
