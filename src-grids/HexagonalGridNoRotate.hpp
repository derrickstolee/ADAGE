/*
 * HexagonalGridNoRotate.hpp
 *
 *  Created on: Dec 10, 2014
 *      Author: stolee
 */

#ifndef HEXAGONALGRIDNOROTATE_HPP_
#define HEXAGONALGRIDNOROTATE_HPP_

#include "Grid.hpp"

#define HEXAGONAL_GRID_NO_ROTATE 5

namespace adage
{
namespace grids
{
/**
 * The hexagonal grid has vertices of degree 3 and faces of length 6.
 *
 * This version allows translation but NOT rotation!
 *
 * So: it has TWO orbits of vertices and ONE orbit of faces!
 */
class HexagonalGridNoRotate: public Grid
{
protected:
	int reserveNewVertex();
	int reserveNewFace();

	typedef Grid super;

public:
	HexagonalGridNoRotate(int diameter);
	virtual ~HexagonalGridNoRotate();


	/**
	 * The transform?? methods fill two permutations for how to transform vertices and faces
	 * according to the given "rooted" objects.
	 *
	 * We make sure that the transformations do not rotate the plane!
	 */
	virtual bool transformVV(int*& fperm, int*& vperm, int from_v1, int from_v2, int to_v1, int to_v2, bool flip = false);
	virtual bool transformVF(int*& fperm, int*& vperm, int from_v1, int from_f2, int to_v1, int to_f2, bool flip = false);
	virtual bool transformFV(int*& fperm, int*& vperm, int from_f1, int from_v2, int to_f1, int to_v2, bool flip = false);
	virtual bool transformFF(int*& fperm, int*& vperm, int from_f1, int from_f2, int to_f1, int to_f2, bool flip = false);
};
}
}
#endif /* HEXAGONALGRID_HPP_ */
