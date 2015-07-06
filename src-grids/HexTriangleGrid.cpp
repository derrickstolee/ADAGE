/*
 * HexTriangleGrid.cpp
 *
 *  Created on: Mar 20, 2015
 *      Author: stolee
 */

#include "HexTriangleGrid.hpp"
#include "TriangularGrid.hpp"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

using namespace adage::grids;

#define IN_BOUNDS_OR_NULL(x,y,max,array)  ( x < 0 || y < 0 || y>= max || x>= max) ? -1 : array[x][y]


HexTriangleGrid::HexTriangleGrid(int max_magnitude)
{
	
}

HexTriangleGrid::~HexagonalGrid()
{
}

