/*
 * Rule.hpp
 *
 *  Created on: Apr 17, 2014
 *      Author: stolee
 */

#ifndef RULE_HPP_
#define RULE_HPP_

#include <stdio.h>
#include "fraction.hpp"
#include "Grid.hpp"
#include "Configuration.hpp"

#define VARIABLE_TYPE_V2V 1
#define VARIABLE_TYPE_V2F 2
#define VARIABLE_TYPE_F2V 3
#define VARIABLE_TYPE_F2F 4
#define VARIABLE_VALUE_UNDEF -100000;


namespace adage
{
namespace grids
{
class RuleShape;

class Rule
{
protected:
	RuleShape* shape; // The configuration of the BASE SHAPE of the rule...
	Configuration* conf;

	/**
	 * Variables
	 * From/To Color/Mult/Type/value
	 */
	int num_variables;
	int rule_type;
	bool values_are_set;
	int* variable_index;
	int* variable_from_color;
	int* variable_to_color;
	int* variable_from_mult;
	int* variable_to_mult;
	int* variable_type;
	fraction* variable_value;

	Rule();

public:
	/**
	 * Create a rule from a configuration.
	 *
	 * Will use "duplicate" to make a deep copy.
	 *
	 * The first constructor goes from all colors to the center color.
	 * The second constructor allows the user to specify which colors can actually send charge.
	 */
	Rule(const Configuration* conf, int rule_type, int base_index);
	Rule(const Configuration* conf, int rule_type, int to_color, int* from_colors, int num_from, int base_index);
	virtual ~Rule();

	void setShape(RuleShape* shape_conf);
	void setShapeConfiguration(Configuration* conf);
	RuleShape* getShape();

	int getNumVariables();
	int getVariableIndex(int i);
	int getType();

	/**
	 * Get Access to the configuration, with symmetry calculated.
	 */
	Configuration* getConfiguration();

	/**
	 * Get everything about a single variable, and quickly.
	 */
	int getVariableInfo(int i, int& v_index, int& type, int& from_color, int& to_color, int& from_mult, int& to_mult, fraction& value);

	/**
	 * For FileIO
	 *
	 * The configuration does not need to be of a certain type, since the specific implementations
	 * are only used for generation steps, not for storage.
	 */
	void print() const;
	void write(FILE* file) const;
	static Rule* read(Grid* g, FILE * file); // Returns 0 if EOF.
};
}
}

#endif /* RULE_HPP_ */
