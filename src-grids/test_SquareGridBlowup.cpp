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
	Grid* grid = new SquareGrid(20);

	Grid* blowup = grid->getBlowup();
	blowup->printDualAdjLists();

	fprintf(stderr, "minface: %d\n", blowup->getMaxVertexIndex());

	int* vperm = 0;
	int* fperm = 0;

	bool result = blowup->transformVV(fperm, vperm, 0, 1, 1, 2, false);

	if ( result )
	{
		printf("rotation worked.\n");
	}

	delete grid;
	delete blowup;

	return 0;
}
