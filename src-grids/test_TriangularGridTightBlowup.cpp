/*
 * test_HexagonalGrid.cpp
 *
 *  Created on: Apr 17, 2014
 *      Author: stolee
 */

#include <stdio.h>
#include "Grid.hpp"
#include "TriangularGrid.hpp"

using namespace adage::grids;

int main(void)
{
	Grid* grid = new TriangularGrid(20);

	int N = grid->getMaxVertexIndex();
	int F = grid->getMaxFacialIndex();

	Grid* blowup = grid->getTightBlowup();
	blowup->printDualAdjLists();
		fprintf(stderr, "minface: %d\n", blowup->getMaxVertexIndex());


	delete grid;
	delete blowup;

	return 0;
}
