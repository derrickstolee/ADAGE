/*
 * TriangularGrid.hpp
 *
 *  Created on: Aug 12, 2014
 *      Author: stolee
 */

#ifndef TRIANGULARGRID_HPP_
#define TRIANGULARGRID_HPP_

#define TRIANGULAR_GRID 3

#include "Grid.hpp"

namespace adage
{
namespace grids
{
/**
 * The square grid has vertices of degree 6 and faces of length 3.
 */
class TriangularGrid : public Grid
{
protected:
	int reserveNewVertex();
	int reserveNewFace();

public:
	TriangularGrid(int diameter);
	virtual ~TriangularGrid();
};
}
}
#endif /* TRIANGULARGRID_HPP_ */