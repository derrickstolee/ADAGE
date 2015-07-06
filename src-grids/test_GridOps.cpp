/*
 * test_HexagonalGrid.cpp
 *
 *  Created on: Apr 17, 2014
 *      Author: stolee
 */

#include <stdio.h>
#include <string.h>
#include "Grid.hpp"
#include "HexagonalGrid.hpp"
#include "HexagonalGridNoRotate.hpp"
#include "TriangularGrid.hpp"
#include "SquareGrid.hpp"
#include "PentagonalGrid.hpp"

using namespace adage::grids;

int main(int argc, char** argv)
{
	Grid* grid = new HexagonalGrid(20);

	for ( int i = 1; i < argc; i++ )
	{
		if ( strcmp(argv[i],"--hexagonal")== 0)
		{
			delete grid;
			grid = new HexagonalGrid(20);
		}
		if ( strcmp(argv[i],"--hexnorot")== 0)
		{
			delete grid;
			grid = new HexagonalGridNoRotate(20);
		}
		if ( strcmp(argv[i],"--square")== 0)
		{
			delete grid;
			grid = new SquareGrid(20);
		}
		if ( strcmp(argv[i],"--triangular")== 0)
		{
			delete grid;
			grid = new TriangularGrid(20);
		}
		if ( strcmp(argv[i],"--pentagonal")== 0)
		{
			delete grid;
			grid = new PentagonalGrid(20);
		}
		
		if ( strcmp(argv[i],"--tblowup")== 0)
		{
			Grid* oldgrid = grid;
			grid = oldgrid->getTightBlowup();
			delete oldgrid;
		}
		if ( strcmp(argv[i],"--blowup")== 0)
		{
			Grid* oldgrid = grid;
			grid = oldgrid->getBlowup();
			delete oldgrid;
		}
		if ( strcmp(argv[i],"--dual")== 0)
		{
			Grid* oldgrid = grid;
			grid = oldgrid->getDual();
			delete oldgrid;
		}
		if ( strcmp(argv[i],"--snub")== 0)
		{
			Grid* oldgrid = grid;
			grid = oldgrid->getSnub();
			delete oldgrid;
		}
	}

	grid->printDualAdjLists();

	fprintf(stderr, "minface: %d\n", grid->getMaxVertexIndex());

	delete grid;

	return 0;
}
