/*
 * LinearProgramCPLEX.hpp
 *
 *  Created on: May 26, 2015
 *      Author: stolee
 */

#ifndef LINEARPROGRAMCPLEX_HPP_
#define LINEARPROGRAMCPLEX_HPP_

#include "fraction.hpp"
#include "LinearConstraint.hpp"
#include "Trie.hpp"

extern "C"
{
#include "ilcplex/cplex.h"
}

namespace adage {
/**
 * The LinearProgram class is a wrapper interface for linear programs using the structures of ADAGE.
 *
 * The default class simply writes the constraints to STDOUT. Others will actually implement the LP.
 *
 * It can test for feasability/optimality as constratints/variables are added.
 *
 * It can return the values of the variables in the solution space.
 */
class LinearProgramCPLEX : public LinearProgram
{
protected:	
	/**
	 * The LP instance in terms of CPLEX
	 */
	CPXLPptr lpx;
	CPXENVptr env;

	// Store full objects, in order to use for rounding!
	LinearConstraint** constraint_list;
	int num_linearconstraints;
	int size_constraint_list;
	char* constraint_active;

	bool has_gap_columns;
	int num_remaining_variables;
	int num_remaining_constraints;

	fraction max_constraint_error_with_w;
	fraction max_constraint_error_without_w;

	int num_primes;
	int* primes;
	int* powers;
	bool use_simplex;

	/**
	 * As we remove variables during the rounding process, we will need to 
	 * translate the updated column indices to match our variable indices.
	 * 
	 * Hence, variable_reverse_lookup will list 0...n for our first call, but 
	 * then will shrink as we remove variables.
	 */
	int* variable_reverse_lookup; // CPX variable to our variable.
	int* variable_forward_lookup; // our variable to CPX variable. -1 for rounded
	int* constraint_forward_lookup;

public:
	LinearProgramCPLEX();
	virtual ~LinearProgramCPLEX();

	/**
	 * The default cost is to maximize 'w' but if there is a constraint 'w == opt' and we add "slack"
	 * variables to make only important constraints sharp, then we will want the slack variables to be 
	 * maximized. This must be done by the client.
	 */
	virtual void setCost(LinearConstraint* constraint, bool maximize=true);

	/**
	 * If solve() is called, it will run the LP and wait for completion.
	 *
	 * Then isFeasible and optimum will return the results. Thus the client can stop
	 * if an infeasible situation occurs, or if the optimum is below the goal value.
	 */
	virtual void solve();
	virtual void addConstraint(LinearConstraint* constraint);
	virtual void setFractions(int num_vars, int* vars, fraction* fractions, bool add_gaps );
	virtual void importArguments(int argc, char** argv);
};

}	

#endif /*  */
