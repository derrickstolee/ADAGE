/*
 * RuleVerifier.hpp
 *
 *  Created on: Apr 18, 2014
 *      Author: dstolee
 */

#ifndef RULEVERIFIER_HPP_
#define RULEVERIFIER_HPP_

#include <stdio.h>
#include "Grid.hpp"
#include "Configuration.hpp"
#include "Rule.hpp"
#include "SearchManager.hpp"
#include <stack>
#include "fraction.hpp"

namespace adage
{
namespace grids
{
/**
 * The RuleVerifier takes a list of discharging rules WITH VALUES
 * and then tests if there is a configuration that violates those values.
 *
 * We need a target value and also need to keep track of "max_value" or "min_value".
 *
 * Two modes: target is a lower bound on resulting charge, or upper bound on resulting charge.
 *
 * Uses "fraction" to perform exact arithmetic.
 */
class RuleVerifier: public SearchManager
{
protected:
	Grid* grid;

	fraction target;
	bool target_is_lower_bound;
	fraction cur_min_charge;
	fraction cur_max_charge;
	// TODO: Snapshot/rollback these charges

	// This is the configuration to fill with things.
	Configuration* conf;

	int size_rules;
	int num_rules;
	Rule** rules;

	/**
	 * Variations store all possible rules and ways to transform them about the "center".
	 *
	 * We use configurations to store the transformed versions, but track which variable
	 */
	int size_rule_variations;
	int num_rule_variations;
	Configuration** variation_conf; // The transformed configuration
	Rule** variation_rule; // Points to the rule inside of the "rules" array
	bool* variation_is_on_center; // if true, then use all variables on this variation
	int* variation_center_color; // Otherwise, which "from color" is this center on?
	int* variation_variable_index; // Also when not on center, which variable to use?
	int* variation_variable_coeff; // Also when not on center, which coefficient to use?

	// Permanent
	int max_vertex_index;
	int* num_variations_using_vertex; // Never changes after initialization
	int* first_variation_using_vertex; // stores where in "variations_using_vertex" to start the list.

	// Subject to snapshot/rollback
	int* num_undetermined_variations_using_vertex; // decrease as variations are determined... helps pick which vertex to select next
	int* variations_using_vertex;  // Which variations are used for that vertex. As determined, set to -1.

	void removeVariationAtVertex(int vertex, int variation, int variation_using_index);
	std::stack<int> num_undetermined_i; // for a decrement.
	std::stack<unsigned int> num_undetermined_size;

	std::stack<int> variations_using_i; // for an assignment to -1
	std::stack<int> variations_using_value; // need to store the old value.
	std::stack<unsigned int> variations_using_size;

	void writeConstraint();

	/**
	 * Given the current configuration, select the best vertex that remains undetermined and
	 * get its index. Will be used to select as element or as nonelement.
	 */
	int getBestUndeterminedVertex();

public:
	RuleVerifier(Grid* grid);
	virtual ~RuleVerifier();

	/**
	 * Read all rules from the given file.
	 *
	 * This file should include values for all of the rules.
	 */
	void loadRules(FILE* f);

	void clear();
	void snapshot();
	void rollback();

	/**
	 * pushNext -- deepen the search to the next child
	 * 	of the current node.
	 *
	 * @return the label for the new node. -1 if none.
	 */
	virtual LONG_T pushNext();

	/**
	 * pushTo -- deepen the search to the specified child
	 * 	of the current node.
	 *
	 * @param child the specified label for the new node
	 * @return the label for the new node. -1 if none, or failed.
	 */
	virtual LONG_T pushTo(LONG_T child);

	/**
	 * pop -- remove the current node and move up the tree.
	 *
	 * @return the label of the node after the pop.
	 * 		This return value is used for validation purposes
	 * 		to check proper implementation of push*() and pop().
	 */
	virtual LONG_T pop();

	/**
	 * prune -- Perform a check to see if this node should be pruned.
	 *
	 * @return 0 if no prune should occur, 1 if prune should occur.
	 */
	virtual int prune();

	/**
	 * isSolution -- Perform a check to see if a solution exists
	 * 		at this point.
	 *
	 * @return 0 if no solution is found, 1 if a solution is found.
	 */
	virtual int isSolution();

};

}
}

#endif /* RULEVERIFIER_HPP_ */
