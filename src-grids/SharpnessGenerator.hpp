/*
 * SharpnessGenerator.hpp
 *
 *  Created on: Jan 15, 2015
 *      Author: stolee
 *
 * An adaptation of ConstraintGenerator.hpp
 */

#ifndef SharpnessGenerator_HPP_
#define SharpnessGenerator_HPP_

#include <stdio.h>
#include "Grid.hpp"
#include "Configuration.hpp"
#include "Rule.hpp"
#include "RuleShape.hpp"
#include "SearchManager.hpp"
#include "LinearProgram.hpp"
#include "Trie.hpp"
#include <stack>


namespace adage
{
namespace grids
{

/**
 * The SharpnessGenerator class generates the linear constraints for the discharging rules
 * to be properly satisfied.
 *
 * For now, we are only focusing on preserving charge at the faces, as vertices add and remove
 * charge from the faces, and faces send charge among each other.
 *
 * Since only Face2Face and Vertex2Face rules have been implemented, there will be a separate
 * list of constraints for charge at each vertex, specifically one per each V2F rule.
 * 
 * TODO: Make this work with multiple face-orbits! Currently, it centers around face 0, whatever its orbit.
 * It SHOULD be that the first level of the search selects the orbit of the center face and then the 
 * variations are built around it!
 */
class SharpnessGenerator : public SearchManager
{
protected:
	Grid* grid;
	adage::LinearProgram* lp;
	fraction w;

	/**
	 * As we read the rules, we will fill full_conf with faces and vertices 
	 * that are used by all of the variations.
	 * 
	 * We will use conf to recursively build all possible combinations of
	 * elements/nonelements within full_conf. The Configuration of conf will
	 * determine how things are propagated.
	 *
	 * Due to propagation, we will need to be careful about marking vertices as
	 * elements or nonelements! Thus, blank_conf is a regular Configuration with no 
	 * propagation! After a change is made on conf, we will see how it differs from
	 * blank_conf in order to track which rules are allowed or eliminated.
	 *
	 * We will select vertices from full_conf based on the number of active rules used 
	 * by that vertex. Thus, if all rules using a particular vertex are eliminated,
	 * then we will not expand our configuration to those vertices.
	 */
	Configuration* conf;
	Configuration* blank_conf;
	Configuration* cur_base;
	void initConfigurations();
	void freeConfigurations();

	int* fperm;
	int* vperm;

	int size_shapes;
	int num_shapes;
	RuleShape** rule_shapes;
	void initRuleShapes();
	void freeRuleShapes();
	void addRuleShape(RuleShape* rule_shape);


	// These are selected as the first branch of the search tree!
	int num_constraint_bases;
	int cur_constraint_base;
	Configuration** constraint_bases;

	bool center_is_face;
	int center_id;


	/**
	 * The variations are set after the center type and center orbit are selected!
	 */
	int size_variations;
	int num_variations;
	RuleShape** shape_variations;
	Configuration* full_conf; // all vertices used by the shape variations
	void initVariations();
	void freeVariations();
	void addVariation(RuleShape* variation);
	void determineVariations(); // Fills in these details
	int getNextUndeterminedVertex();

	/**
	 * write the current configuration, as we have found a sharp example!
	 */
	bool writeConfiguration();

	bool vertices_distribute_evenly;

	// List of generators for translation/rotation of the object
	int from_v;
	int from_v_n;
	int max_to_v;
	int num_generators;
	int size_generators;
	int* generators;
	void initGenerators();
	void freeGenerators();


public:
	SharpnessGenerator(Grid* grid, Configuration* conf, adage::LinearProgram* lp); // TODO: rules, variables, values
	virtual ~SharpnessGenerator();

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

	virtual void importArguments(int argc, char** argv);


	// Use existing configuration to find a way to cover the plane!
	virtual bool findCover();
};
}
}


#endif /* SharpnessGenerator_HPP_ */
