/*
 * Grid.hpp
 *
 *  Created on: Apr 16, 2014
 *      Author: stolee
 */

#ifndef GRID_HPP_
#define GRID_HPP_

namespace adage
{
namespace grids
{
class Grid
{
protected:
	int max_vertex_index;
	int max_facial_index;

	int num_vertex_orbits;
	int num_facial_orbits;

	// Grid Dual stored like a sparsegraph
	int f_len;
	int max_f_value;
	int* f; // Position of the out-edges in the arrays below
	int* fd; // Facial degree
	int fe_len; // length of the f*e arrays
	int* fve; // Edges from faces to vertices
	int* ffe; // Edges from faces to faces
	int* fbe; // Edges from faces to both
	int* fo; // Facial orbit

	int v_len;
	int max_v_value;
	int* v;
	int* vd;
	int ve_len;
	int* vve;
	int* vfe;
	int* vbe;
	int* vo; // Vertex orbit

	void lengthenVArrays(int length, int e_length);
	void lengthenFArrays(int length, int e_length);

	/**
	 * The constructor is protected in order to make this
	 * an essentially abstract class, but also can be instantiated
	 * on its own (as in the blowup and power methods).
	 */
	Grid();

public:
	virtual ~Grid();

	virtual int getVertexDegree(int vertex);
	virtual int getFaceDegree(int face);

	virtual int getNumVertexOrbits();
	virtual int getNumFacialOrbits();

	virtual int getVertexOrbitRepresentative(int orbit);
	virtual int getFacialOrbitRepresentative(int orbit);

	virtual int getOrbitOfVertex(int vertex);
	virtual int getOrbitOfFace(int face);

	int getMaxVertexIndex();
	int getMaxFacialIndex();

	/**
	 * Get the ith neighbor of v, modulo the degree of v!
	 *
	 * V : vertex
	 * F : face
	 * B : both (evens are opposite type, odds are same type)
	 * This allows the edge between vertices 0&1 to border faces 0&1 and these are all near each other.
	 */
	virtual int neighborVV(int v, int i);
	virtual int neighborVF(int v, int i);
	virtual int neighborVB(int v, int i);

	virtual int neighborFV(int v, int i);
	virtual int neighborFF(int v, int i);
	virtual int neighborFB(int v, int i);

	/**
	 * The transform?? methods fill two permutations for how to transform vertices and faces
	 * according to the given "rooted" objects.
	 */
	virtual void transformVV(int*& fperm, int*& vperm, int from_v1, int from_v2, int to_v1, int to_v2, bool flip = false);
	virtual void transformVF(int*& fperm, int*& vperm, int from_v1, int from_f2, int to_v1, int to_f2, bool flip = false);
	virtual void transformFV(int*& fperm, int*& vperm, int from_f1, int from_v2, int to_f1, int to_v2, bool flip = false);
	virtual void transformFF(int*& fperm, int*& vperm, int from_f1, int from_f2, int to_f1, int to_f2, bool flip = false);

	/**
	 * Given a vertex, determine the ball of a given radius.
	 */
	virtual int* getVertexBall(int vertex, int radius, int& ball_size);

	/**
	 * For future implementation:
	 */
	/**
	 * Replace each vertex with a face whose length is the degree of the original vertex.
	 */
	//Grid* getBlowup();
	Grid* getDual();
	void printDualAdjLists();
};

}
}

#endif /* GRID_HPP_ */
