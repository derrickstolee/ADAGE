/*
 * ElongatedSquareGrid.hpp
 *
 *  Created on: Jul 3, 2015
 *      Author: stolee
 */

#ifndef ELONGATEDSQUAREGRID_HPP_
#define ELONGATEDSQUAREGRID_HPP_

#define SQUARE_GRID 2

#include "Grid.hpp"

namespace adage
{
namespace grids
{
/**
 * The square grid has vertices of degree 4 and faces of length 4.
 */
class ElongatedSquareGrid : public Grid
{
protected:
	int reserveNewVertex();
	int reserveNewFace(int len);

public:
	ElongatedSquareGrid(int diameter);
	virtual ~ElongatedSquareGrid();
};
}
}
#endif /* ELONGATEDSQUAREGRID_HPP_ */