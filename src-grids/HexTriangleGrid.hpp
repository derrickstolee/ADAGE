/*
 * HexTriangleGrid.hpp
 *
 * This grid has two types of faces: triangles and hexagons.
 * It has ONE kind of vertex: 4-vertex with two incident faces of each type
 *
 *  Created on: Mar 20, 2015
 *      Author: stolee
 */

#ifndef HEXTRIANGLEGRID_HPP_
#define HEXTRIANGLEGRID_HPP_

#include "Grid.hpp"

#define HEXTRIANGLE_GRID 10

namespace adage
{
namespace grids
{
class HexTriangleGrid: public Grid
{
protected:
	int reserveNewVertex();
	int reserveNewFace();

public:
	HexTriangleGrid(int diameter);
	virtual ~HexTriangleGrid();
};
}
}
#endif /* HEXTRIANGLEGRID_HPP_ */
