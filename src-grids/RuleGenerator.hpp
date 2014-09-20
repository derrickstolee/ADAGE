/*
 * RuleGenerator.hpp
 *
 *  Created on: Apr 17, 2014
 *      Author: stolee
 */

#ifndef RULEGENERATOR_HPP_
#define RULEGENERATOR_HPP_

#include "Grid.hpp"
#include "Configuration.hpp"
#include "Rule.hpp"
#include "Trie.hpp"

#include <stdio.h>

namespace adage
{
namespace grids
{
class RuleGenerator
{
protected:
	Grid* grid;
	Configuration* conf; // Grow this!
	Configuration* cur_base; // Use this to figure out what to decide!

	/**
	 * The base configurations are the shapes that are specified to start.
	 *
	 * When generating, the first choice is which of these to use.
	 *
	 * These can differ due to the following factors:
	 * 1. Different kind of center (vertex/face)
	 * 2. Not isomorphic (without flips)
	 */
	int size_base_confs;
	int num_base_confs;
	int* to_colors;
	int* num_from;
	int** from_colors;
	Configuration** base_confs;
	void addBaseConfiguration(Configuration* c, int to_color, int num_from, int* from);
	bool hasIsomorphicConfiguration(Configuration* c);
	void initBaseConfigurations();
	void freeBaseConfigurations();

	// List of Rules.
	int cur_num_vars;
	int size_rules;
	int num_rules;
	Rule** rules;
	bool write_as_inserted;
	int current_rule_type;
	int current_to_color;
	int current_num_from;
	int* current_from_colors; // Not actually colors, but indices of faces!
	bool restrict_center_face;
	bool set_face_sizes;
	int restrict_center_size;
	Configuration* current_base_conf;
	void addRule(Configuration* c);
	void initRuleList();
	void freeRuleList();

	Trie* canonical_trie;



	// This recursive algorithm simply colors each vertex that it comes across.
	void recurseGenerateRules();

public:
	/**
	 * We need to supply some details.
	 *
	 * 1. The Grid we are working in.
	 * 2. A Configuration to use that includes the correct propagation methods.
	 * 3. Parameters for the types of rules:
	 *   a. Should we generate vertex to face rules? (these will then use all faces incident to the vertices, and make them full.)
	 *   b. Should we generate face to face rules? If so, how many pulling? how many in shape?
	 *
	 * For example, we could generate what happens when a face has 4 incident faces specified, but only pull from the middle 2.
	 * For example, we could generate what happens when a face has 3 incident faces specified, but only pull from the middle one.
	 */
	RuleGenerator(Grid* grid, Configuration* conf, bool write=false);
	virtual ~RuleGenerator();

	/**
	 * Generate rules to pull/push from a center vertex to its adjacent faces,
	 *   where the ball of diameter "vert_diameter" is specified exactly.
	 *
	 * for all vertex orbits:
	 *    Create all adjacent faces and add vertices within the given distance (and within these faces?)
	 *    Make the center vertex an element.
	 *    Recursively check all arrangements of elements around the center.
	 */
	int generateVertexToFaceRules(int vert_diameter, bool restrict_to_faces, bool use_face_sizes=false);

	/**
	 * Generate rules to pull/push from a center face, where there are "facial_degree" adjacent faces.
	 * The middle "pull_degree" of them actually create the rules.
	 *
	 * TODO: Set a maximum size of the pulling face!
	 */
	int generateFaceToFaceRules(int facial_degree, int pull_degree, int max_pull_face_size=-1, int full_face_degree=-1);
	int generateFaceToFaceRules(Configuration* base_configuration, int to_color, int num_from, int* from_colors, int max_pull_face_size=-1);
	int generateVertexToVertexRules(Configuration* base_configuration, int to_color, int num_from, int* from_colors);
	int generateFaceToVertexRules(Configuration* base_configuration, int to_color, int num_from, int* from_colors);
	int generateVertexToFaceRules(Configuration* base_configuration, int to_color, int num_from, int* from_colors);

	void setFaceSizes(bool val);

	void writeAllRules(FILE* file);
};
}
}
#endif /* RULEGENERATOR_HPP_ */
