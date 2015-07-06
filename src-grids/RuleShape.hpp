/**
 * RuleShape.hpp
 *
 * A RuleShape contains a Configuration that describes how a rule could
 * pull or push charge. It is not directly associated with a specific rule, 
 * but instead is a template for a rule.
 * 
 * When generating constraints, this rule determines which variable should be used
 * specifically between two chargeable objects, depending on the situation of the 
 * given Configuration.
 */
#ifndef RULESHAPE_HPP__
#define RULESHAPE_HPP__

#include "Grid.hpp"
#include "Configuration.hpp"
#include "LinearConstraint.hpp"
#include "Rule.hpp"


#define KEY_TYPE long long int

#define GEQ 0
#define LEQ 1
#define EQ 2

namespace adage {

namespace grids {



class RuleShape
{
protected:
	char* rule_name;
	char* var_name;

	/**
     * The center_id and center_is_face should agree with rule_type, either pulling or pushing.
     */ 
	int rule_type;
	int center_id;
	bool center_is_face;
	bool center_is_puller;
	Configuration* shape_conf;



	/**
	 * There are also a list of chargeable objects!
	 *
	 * These can send charge to the center!
	 */
	int size_chargeable;
	int num_chargeable;
	int* chargeable;

	/**
	 * Now, for the variable function!
	 *
	 * In the duplication, these keys are translated with the permutation!
	 */
	int center_is_which_chargeable;
	int num_kernel_keys; // num total terms
	int* num_kernel_vertices; // num terms in one key
	int** kernel_vertex_indices; // the vertex indices in the key
	int** kernel_vertex_coefficients; // the coefficients in the key

	/**
	 * These KERNELs are to stop checking other areas of the configuration
	 * usually because these areas are "dense enough" and discharging can be
	 * done in a "stupid" way.
	 */
	int size_kernels;
	int num_kernels;
	int* kernel_size;	
	int* kernel_bound;
	char* kernel_bound_type; // GEQ, EQ, LEQ
	int** kernel_indices;
	int** kernel_coeffs;

	RuleShape();

public:
	/**
	 * Build a rule shape from an existing one.
	 */
	RuleShape(RuleShape* oldshape, int* fperm, int* vperm, int constraint_center, bool center_is_puller);

	virtual ~RuleShape();

	int getType();
	int getCenter();
	bool isCenterFace();
	Configuration* getShape();

	int getNumChargeable();
	int getChargeable(int i);
	void addChargeable(int i);

	int centerIsWhichChargeable();

	/**
	 * How many kernels is this vertex in?
	 */
	int getKernelCount(int vertex);

	/**
	 * Is this rule complete for the given conf as per kernels?
	 */
	bool isComplete(Configuration* conf);


	/**
	 * modifyCoefficients modifies a list of coefficients depending on the given configuration.
	 *
	 */
	void modifyCoefficients(Configuration* conf, LinearConstraint* constraint);
	void getVariableName(Configuration* conf, int chargeable, char*& name, int& name_len);


	void print();
	void write(FILE* file);

	/** 
	 * getRuleName is used to connect command-line tools with
	 * the rules in a rule file!
	 */
	char* getRuleName();

	/**
	 * Given a rule shape, read it from a file!
	 */
	static RuleShape* read(Grid* grid, FILE* file);
};

}

}

#endif
