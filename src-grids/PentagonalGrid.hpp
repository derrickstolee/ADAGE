/*
 * PentagonalGrid.hpp
 *
 *  Created on: Jul 3, 2015
 *      Author: stolee
 */

#ifndef PENTAGONALGRID_HPP_
#define PENTAGONALGRID_HPP_

#define PENTAGONAL_GRID 7

#include "Grid.hpp"

namespace adage
{
namespace grids
{
/**
 * The square grid has vertices of degree 4 and faces of length 4.
 */
class PentagonalGrid : public Grid
{
protected:
	int cur_vve_index;
	int reserveNewVertex(int deg);
	int reserveNewFace();

public:
	PentagonalGrid(int diameter);
	virtual ~PentagonalGrid();
};
}
}
#endif /* PENTAGONALGRID_HPP_ */