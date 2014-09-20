/*
 * HexagonalGrid.hpp
 *
 *  Created on: Apr 17, 2014
 *      Author: stolee
 */

#ifndef HEXAGONALGRID_HPP_
#define HEXAGONALGRID_HPP_

#include "Grid.hpp"

#define HEXAGONAL_GRID 1

namespace adage
{
namespace grids
{
/**
 * The hexagonal grid has vertices of degree 3 and faces of length 6.
 */
class HexagonalGrid: public Grid
{
protected:
	int reserveNewVertex();
	int reserveNewFace();

public:
	HexagonalGrid(int diameter);
	virtual ~HexagonalGrid();
};
}
}
#endif /* HEXAGONALGRID_HPP_ */
