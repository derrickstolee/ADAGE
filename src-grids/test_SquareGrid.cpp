/*
 * test_HexagonalGrid.cpp
 *
 *  Created on: Apr 17, 2014
 *      Author: stolee
 */

#include <stdio.h>
#include "Grid.hpp"
#include "SquareGrid.hpp"

using namespace adage::grids;

int main(void)
{
	Grid* grid = new SquareGrid(12);

	int N = grid->getMaxVertexIndex();
	int F = grid->getMaxFacialIndex();

	grid->printDualAdjLists();

	delete grid;

	return 0;
}
