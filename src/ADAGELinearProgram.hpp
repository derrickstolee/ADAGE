/*
 * ADAGELinearProgram.hpp
 *
 *  Created on: Aug 30, 2013
 *      Author: stolee
 */

#ifndef ADAGELINEARPROGRAM_HPP_
#define ADAGELINEARPROGRAM_HPP_

#ifdef CPLEX
extern "C"
{
#include "ilcplex/cplex.h"
}
#endif

#ifdef GLPK
extern "C"
{
#include <glpk.h>
}
#endif

#include "constraintset.hpp"

/**
 * The ADAGELinearProgram class takes a constraint structure and uses it to build an LP.
 *
 * It then returns the values of the variables for the best LP.
 */
class ADAGELinearProgram
{
protected:

#ifdef CPLEX
	/**
	 * The LP instance in terms of CPLEX
	 */
	CPXLPptr lpx;
	CPXENVptr env;
#endif

#ifdef GLPK
	/**
	 * The LP instance in terms of GLPK
	 */
	glp_prob *lp;
	glp_smcp* parm;
#endif
	int num_lp_constraints;

public:
	ADAGELinearProgram();
	virtual ~ADAGELinearProgram();

	double solveLP(constraintset* constraints, bool maximize_density, bool do_solve=true);

	/**
	 * the given constraintset already has variable values.
	 * Now, find fractions that correspond to those values.
	 */
	void convertToFractions(constraintset* constraints);
};


#endif /* ADAGELINEARPROGRAM_HPP_ */
