/*
 * IdentifyingCodeConfiguration.hpp
 *
 *  Created on: Apr 17, 2014
 *      Author: stolee
 */

#ifndef IDENTIFYINGCODECONFIGURATION_HPP_
#define IDENTIFYINGCODECONFIGURATION_HPP_

/**
 * A dominating set D has N[v] \cap D \neq \varnothing
 */
#define MODE_DOMINATING 0

/**
 * An identifying code C has N[v] \cap C \neq \varnothing
 * and N[v] \cap C \neq N[u] \cap C for all u, v in V(G)
 */
#define MODE_IDENTIFYING 1
 
/**
 * A strong identifying code C has N(v) \cap C \neq \varnothing
 * and for all u \neq v, we have that { N(v) \cap C, N[v]\cap C} \cap { N(u) \cap C, N[u] \cap C } = \varnothing.
 * THIS IS A VERY STRANGE CONDITION!
 */
#define MODE_STRONG_IDENTIFYING 2

/**
 * A locating-dominating code C has N[v] \cap C \neq \varnothing
 * and N[v] \cap C \neq N[u] \cap C for all u, v \in V(G) \setminus C.
 */
#define MODE_LOCATING_DOMINATING 3

/**
 * An open-neighborhood identifying code has N(v) \cap C \neq \varnothing
 * and N(v) \cap C \neq N(u) \cap C for all u,v \in V(G).
 */
#define MODE_OPEN_NEIGHBORHOOD 4

/**
 * A neighbor-identifying code has
 * N[v] \cap C \neq \varnothing
 * and N[v] \cap C \neq N[u] \cap C for all uv \in E(G).
 */
#define MODE_NEIGHBOR_IDENTIFYING 5

#include "Configuration.hpp"
#include <stack>

namespace adage
{
namespace grids
{


class IdentifyingCodeConfiguration: public Configuration
{
protected:
	/**
	 * as "addElement" or "addNonElement" is called,
	 * this boolean might be updated to reflect an 
	 * impossibility.
	 */
	bool is_inconsistent; 
	int mode;

	/**
	 * For each vertex, we list the possible
	 * conflict vertices. These conflicts are
	 * removed as elements are added to 
	 * the symmetric difference.
	 */
	int num_vertices;
	int* conflicts;
	int* vertex_conflict_start;
	int* vertex_conflict_size;
	void initConflicts();
	void freeConflicts();
	void snapshotConflicts();
	void rollbackConflicts();
	void removeConflict(int i);
	std::stack<int> snapshot_conflicts_i;
	std::stack<int> snapshot_conflicts_vals;
	std::stack<unsigned int> snapshot_conflicts_size;
	std::stack<bool> snapshot_is_inconsistent;



	int* ball_list;
	int* vertex_ball_start;
	int* vertex_ball_size;
	void initBalls();
	void freeBalls();

	bool isReducible();

public:
	IdentifyingCodeConfiguration(Grid* grid, int mode=MODE_IDENTIFYING);
	virtual ~IdentifyingCodeConfiguration();

	/** Public Interface for Accessing Things */
	virtual void snapshot();
	virtual void rollback();

	/**
	 * These methods are overridden simply to update the 
	 * conflict values. Not all are helpful, though!
	 */
	// No use for this method... yet
	//virtual void addVertexToShape(int i);

	// If added to the symmetric difference of a constraint,
	// then that constraint is removed!
	virtual void addElement(int i);
	virtual void addElement(int i, bool check);

	// Checks to see if a ball becomes fixed as empty!
	virtual void addNonElement(int i);
	virtual void addNonElement(int i, bool check);


	/**
	 * Create a new configuration using the same sub-class. Necessary for some features of
	 * configurations. This mode will simply create a copy into a regular Configuration.
	 * This may be enough for most applications.
	 */
	virtual Configuration* duplicate() const;
	virtual Configuration* duplicate(int* fperm, int* vperm) const;
	virtual Configuration* getEmptyConfiguration() const;

	/**
	 * Extensions will use the propagate method to expand the configuration to things that are "known"
	 * based on the given information. Returns false if and only if something is wrong!
	 *
	 * Requires knowing the grid for neighborhoods and such.
	 */
	virtual bool propagate();
};
}
}
#endif /* IDENTIFYINGCODECONFIGURATION_HPP_ */
