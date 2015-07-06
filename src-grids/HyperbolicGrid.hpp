/*
 * HyperbolicGrid.hpp
 *
 *  Created on: Jul 1, 2015
 *      Author: stolee
 */

#ifndef HYPERBOLICGRID_HPP_
#define HYPERBOLICGRID_HPP_

#include "Grid.hpp"

#define HYPERBOLIC_GRID 10

namespace adage
{
namespace grids
{
/**
 * A hyperbolic grid takes a vertex degree and a face length and creates a "plane" grid.
 */
class HyperbolicGrid: public Grid
{
protected:
	int vertex_degree;
	int face_len;
	int reserveNewVertex();
	int reserveNewFace();

public:
	HyperbolicGrid(int deg, int len, int radius);
	virtual ~HyperbolicGrid();
};
}
}
#endif /* HYPERBOLICGRID_HPP_ */
