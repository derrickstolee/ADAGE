/*
 * Configuration.hpp
 *
 *  Created on: Apr 16, 2014
 *      Author: stolee
 */

#ifndef CONFIGURATION_HPP_
#define CONFIGURATION_HPP_

#include "Grid.hpp"
#include <stdio.h>
#include <stack>

namespace adage
{
namespace grids
{
class Configuration
{
protected:
	Grid* grid;

	int vertices_in_shape;
	int size_shape_vertices;
	int* shape_vertices;
	void initShapeVertices();
	void freeShapeVertices();
	void snapshotShapeVertices();
	void rollbackShapeVertices();
	std::stack<int> snapshot_num_vertices_in_shape;

	int size_elements;
	int num_elements;
	int* elements;
	int size_nonelements;
	int num_nonelements;
	int* nonelements;
	void initElements();
	void freeElements();
	void snapshotElements();
	void rollbackElements();
	std::stack<int> snapshot_num_elements;
	std::stack<int> snapshot_num_nonelements;

	int max_v_index;
	int bitmask_length;
	unsigned long long int num_bits_per;
	unsigned long long int* vertex_in_shape;
	unsigned long long int* vertex_is_element;
	unsigned long long int* vertex_is_nonelement;
	void initBitmasks();
	void freeBitmasks();
	void snapshotBitmasks();
	void rollbackBitmasks();
	bool isBitOn(unsigned long long int* array, int i);
	void turnBitOn(unsigned long long int* array, int i);
	void turnBitOff(unsigned long long int* array, int i);
	void setVertexInShapeBitmask(int i);
	void setIsElementBitmask(int i);
	void setIsNonElementBitmask(int i);
	std::stack<int> snapshot_bitmask_shape_i;
	std::stack<int> snapshot_bitmask_shape_size;
	std::stack<int> snapshot_bitmask_element_i;
	std::stack<int> snapshot_bitmask_element_size;
	std::stack<int> snapshot_bitmask_nonelement_i;
	std::stack<int> snapshot_bitmask_nonelement_size;

	// Something is specified
	bool has_center;
	bool center_is_vertex;
	int center_index; // HOW TO SET?

	int faces_length;
	int faces_in_shape;
	int* face_index; // Which faces are in the shape? (iterable)
	int* face_sizes; // If not -1, then this is a specified size! (iterable)

	int* face_to_index; // Which faces are in the shape? (random-access)
	int* face_elts; // How many elements in this face? (random-access)
	int* face_nonelts; // How many nonelements in this face? (random-access)
	int* face_undets; // How many undetermined in this face? (random-access)
	void initFaces();
	void freeFaces();
	void snapshotFaces();
	void rollbackFaces();
	void addElementToFace(int f);
	void addNonElementToFace(int f);
	std::stack<int> snapshot_face_in_shape;
	std::stack<int> snapshot_face_elt_i;
	std::stack<unsigned int> snapshot_face_elt_size;
	std::stack<int> snapshot_face_nonelt_i;
	std::stack<unsigned int> snapshot_face_nonelt_size;
	std::stack<int> snapshot_face_size_i;
	std::stack<unsigned int> snapshot_face_size_size;

	/**
	 * We can compute the symmetry of this object, subject to certain symmetry requirements.
	 */
	int num_vertex_colors;
	int num_face_colors;
	int* vertex_colors;
	int* face_colors;
	char* canonical_string;
	void initColors();
	void freeColors();

	// TODO: Store a list of automorphisms?

	virtual void initAll();
	virtual void freeAll();

public:
	Configuration(Grid* grid);
	virtual ~Configuration();

	Grid* getGrid();

	/** Public Interface for Accessing Things */
	virtual void snapshot();
	virtual void rollback();

	bool isVertexInShape(int i);
	bool isElement(int i);
	bool isNonElement(int i);

	int getNumVerticesInShape();
	int getVertexFromShape(int i);
	int getNumFacesInShape();
	int getFaceFromShape(int i);

	virtual void addVertexToShape(int i);
	virtual void addElement(int i);
	virtual void addNonElement(int i);

	bool isFaceInShape(int f);
	int getFaceSize(int f);
	void setFaceSize(int f, int size);
	int getNumElementsInFace(int f);
	int getNumNonElementsInFace(int f);

	void addFaceToShape(int f);

	void setCenter(int index, bool is_vertex);
	bool hasCenter() const;
	bool isCenterVertex() const;
	int getCenter() const;

	/**
	 * Coloring things.
	 *
	 * If there are automorphisms (with flips, perhaps) then color each orbit of vertices
	 * and faces. Necessary for defining symmetry-aware rules.
	 *
	 * Also, need to allow for symmetry under "fixing" a face/vertex...(uses center!)
	 */
	void computeSymmetry(bool allow_flip = false);

	/**
	 * Canonical Strings:
	 *
	 * Given a vertex and one of its neighbors, we create a "rotation string" by using BFS to visit all vertex neighbors until every
	 * vertex in the shape is visited, and then using BFS to visit all facial neighbors until every face in the shape is visited.
	 * The BFS will respect the rotational order of the vertices. The root vertex starts its neighbor selection by the given neighbor.
	 * The rest of the vertices and faces will use their BFS tree parent and follow the rotational order from that.
	 *
	 * If the configuration has a center, the string starts with the letter C.
	 * The string starts with the orbit of the vertex or face to start.
	 * Then we have the letter V if it starts at a vertex, or the letter F if it starts at a face.
	 * Then we have the orbit of the vertex where the rotation starts, followed by the letter V.
	 *
	 * For the vertices that we visit, we use the following split:
	 * 3 : Not in the shape.
	 * 2 : In the shape, but undetermined.
	 * 1 : An element.
	 * 0 : A nonelement.
	 *
	 * For the faces that we visit, we use the following split:
	 * 1 : In the shape.
	 * 0 : Not in the shape.
	 *
	 * The canonical string is the lex-minimum string of this type, respecting the center, if necessary.
	 *
	 * Thus, the configuration can be reconstructed from the canonical string using the Grid, if necessary.
	 *
	 * If vorder&forder are given as nonzero, then they point to integer pointers that can be realloced to the correct length.
	 * They will then have the order of the vertices and faces as visited, which can be used to assign colors.
	 */
	char* getRotationStringVV(int start_v, int first_neighbor_v, int** vorder=0, int** forder=0);
	char* getRotationStringFV(int start_f, int first_neighbor_v, int** vorder=0, int** forder=0);
	char* getCanonicalString();

	/**
	 * Once the canonical string is determined, we will know how the rotational order is fixed.
	 * The orbits of vertices and faces are assigned colors in order of their lowest-indexed representatives,
	 * with respect to the canonical order.
	 */
	int getNumVertexColors();
	int getNumFaceColors();
	int getColorOfVertex(int i); // given an index, get the color
	int getColorOfFace(int f); // given an index, get the color
	void getVertexColorClass(int color, int*& class_list, int& class_size);
	void getFaceColorClass(int color, int*& class_list, int& class_size);
	int getVertexColorClassSize(int color);
	int getFaceColorClassSize(int color);

	/**
	 * Given a configuration with some common vertices, test
	 * if their intersection has common agreements with
	 *
	 */
	bool matches(Configuration* conf);

	/**
	 * equals: Are these configurations THE SAME?
	 *
	 * This is used for automorphism calculations!
	 */
	bool equals(Configuration* conf, bool verbose = false);

	/**
	 * Create a new configuration using the same sub-class. Necessary for some features of
	 * configurations. This mode will simply create a copy into a regular Configuration.
	 * This may be enough for most applications.
	 */
	virtual Configuration* duplicate() const;
	virtual Configuration* duplicate(int* fperm, int* vperm) const;

	/**
	 * Extensions will use the propagate method to expand the configuration to things that are "known"
	 * based on the given information. Returns false if and only if something is wrong!
	 *
	 * Requires knowing the grid for neighborhoods and such.
	 */
	virtual bool propagate();

	virtual void print() const;
	virtual void write(FILE* out) const;
	static Configuration* read(Grid* g, FILE* in);
};
}
}
#endif /* CONFIGURATION_HPP_ */
