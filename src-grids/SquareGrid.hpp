/*
 * SquareGrid.hpp
 *
 *  Created on: Aug 12, 2014
 *      Author: stolee
 */

#ifndef SQUAREGRID_HPP_
#define SQUAREGRID_HPP_

#define SQUARE_GRID 2

#include "Grid.hpp"

namespace adage
{
namespace grids
{
/**
 * The square grid has vertices of degree 4 and faces of length 4.
 */
class SquareGrid : public Grid
{
protected:
	int reserveNewVertex();
	int reserveNewFace();

public:
	SquareGrid(int diameter);
	virtual ~SquareGrid();
};
}
}
#endif /* SQUAREGRID_HPP_ */