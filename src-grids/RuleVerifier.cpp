/*
 * RuleVerifier.cpp
 *
 *  Created on: Apr 17, 2014
 *      Author: stolee
 */

#include "RuleVerifier.hpp"


using namespace adage::grids;

void RuleVerifier::removeVariationAtVertex(int vertex, int variation, int variation_using_index)
{
}

void RuleVerifier::writeConstraint()
{
}

/**
 * Given the current configuration, select the best vertex that remains undetermined and
 * get its index. Will be used to select as element or as nonelement.
 */
int RuleVerifier::getBestUndeterminedVertex()
{
	return 0;
}

RuleVerifier::RuleVerifier(Grid* grid)
{
}

RuleVerifier::~RuleVerifier()
{
}

/**
 * Read all rules from the given file.
 */
void RuleVerifier::loadRules(FILE* f)
{
}

void RuleVerifier::clear()
{
}
void RuleVerifier::snapshot()
{
}
void RuleVerifier::rollback()
{
}

/**
 * pushNext -- deepen the search to the next child
 * 	of the current node.
 *
 * @return the label for the new node. -1 if none.
 */
LONG_T RuleVerifier::pushNext()
{

	return -1;
}

/**
 * pushTo -- deepen the search to the specified child
 * 	of the current node.
 *
 * @param child the specified label for the new node
 * @return the label for the new node. -1 if none, or failed.
 */
LONG_T RuleVerifier::pushTo(LONG_T child)
{

	return child;
}

/**
 * pop -- remove the current node and move up the tree.
 *
 * @return the label of the node after the pop.
 * 		This return value is used for validation purposes
 * 		to check proper implementation of push*() and pop().
 */
LONG_T RuleVerifier::pop()
{

	return 0;
}

/**
 * prune -- Perform a check to see if this node should be pruned.
 *
 * @return 0 if no prune should occur, 1 if prune should occur.
 */
int RuleVerifier::prune()
{
	return 0;
}

/**
 * isSolution -- Perform a check to see if a solution exists
 * 		at this point.
 *
 * @return 0 if no solution is found, 1 if a solution is found.
 */
int RuleVerifier::isSolution()
{

	return 0;
}
