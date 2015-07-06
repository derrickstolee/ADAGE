/*
 * LinearProgram.hpp
 *
 *  Created on: May 26, 2015
 *      Author: stolee
 */

#ifndef LINEARPROGRAM_HPP_
#define LINEARPROGRAM_HPP_

#include "fraction.hpp"
#include "LinearConstraint.hpp"
#include "Trie.hpp"

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
class LinearProgram
{
protected:
	int num_constraints;
	Trie<int>* constraint_trie;
	Trie<int>* constraint_trie_backup;
	int max_constraint_trie_size;

	int size_solution;
	double* solution;
	fraction* exact_solution;
	bool is_feasible;
	double optimum;
	fraction exact_optimum;

	double round_to_zero;
	bool do_rounding;
	bool do_constraint_checking;
	bool write_constraints;

	/**
	 * Variable Indices by Key
	 */
	Trie<int>* variables;
	int num_nongap_variables;
	int num_variables;
	int size_variables;
	char** variable_list;

public:
	LinearProgram();
	virtual ~LinearProgram();

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
	virtual bool isFeasible();
	virtual double getOptimum();
	virtual fraction getExactOptimum();

	virtual void addConstraint(LinearConstraint* constraint);

	virtual double getApproxValue(int var);
	virtual double getApproxValue(char* key);
	virtual fraction getValue(int var);
	virtual fraction getValue(char* key);
	virtual int getIndex(char* key);
	virtual char* getKey(int var);

	int getNumConstraints();
	virtual int getNumVariables();
	virtual void setOptimum(fraction f);
	virtual void importArguments(int argc, char** argv);
	virtual void readValues(char* filename);
};

}	

#endif /* ADAGELINEARPROGRAM_HPP_ */
