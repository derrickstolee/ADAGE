/**
 * LinearConstraint.hpp
 *
 * A linear constraint is a list of monomials and their coefficients.
 *
 * The monomials are sorted in a priority queue and modified by 
 * a RuleShape in order to add or remove variables from the list.
 */
#ifndef LINEARCONSTRAINT_HPP__
#define LINEARCONSTRAINT_HPP__

#define INEQUALITY_GEQ 1
#define INEQUALITY_EQ 2
#define INEQUALITY_LEQ 3

namespace adage {

/**
 * A monomial is a combination of a coefficient times a variable
 * corresponding to some kernelization.
 *
 * If two monomials have the same varname, then they come from the 
 * same rule and they have the same number of keys.
 *
 * The kernelization is modeled by the keys, in order.
 *
 * The monomials form a doubly-linked list, so we can keep a priority queue.
 */
class Monomial
{
protected:
	int coefficient;
	char* varname;
	int num_keys;
	int* keys; 
	Monomial* next;
	Monomial* prev;

public:
	Monomial();
	Monomial(int coefficient, char* varname, int num_keys, int* keys);
	virtual ~Monomial();

	/**
	 * This insert method may add to the linked list!
	 */
	Monomial* insert(int coefficient, char* varname, int num_keys, int* keys);
	char* getString(); // write the entire list!
	char* getVariableName();
	int getCoefficient();
	Monomial* getNext();
	char* getBasicString();
	int getNumKeys();
	int* getKeys();
};

class LinearConstraint 
{
protected:
	Monomial* monomial_list;
	int num_monomials;

	int w_coeff;
	int rhs;
	int inequality_mode;

public:
	LinearConstraint(int w_coeff, int rhs, int inequality_mode);
	virtual ~LinearConstraint();

	LinearConstraint* duplicate();
	void insertMonomial(int coefficient, char* varname, int num_keys, int* keys);
	char* getString();

	int getInequality();
	int getNumMonomials();
	Monomial* getMonomial(int i);
	int getRHS();
	int getWCoeff();
};

		
}



#endif 
