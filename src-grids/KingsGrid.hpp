/*
 * KingsGrid.hpp
 *
 *  Created on: Aug 12, 2014
 *      Author: stolee
 */

#ifndef KINGSGRID_HPP_
#define KINGSGRID_HPP_

#define KINGS_GRID 4

#include "Grid.hpp"

namespace adage
{
namespace grids
{
/**
 * The King's grid has vertices of degree 8, and is NOT planar! can we use square faces with multi-edges?
 */
class KingsGrid : public Grid
{
protected:
	int reserveNewVertex();
	int reserveNewFace();

public:
	KingsGrid(int diameter);
	virtual ~KingsGrid();
};
}
}
#endif /* KINGSGRID_HPP_ */