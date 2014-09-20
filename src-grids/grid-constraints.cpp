/*
 * hex-constraints.cpp
 *
 *  Created on: Apr 17, 2014
 *      Author: stolee
 */

#include "Grid.hpp"
#include "Rule.hpp"
#include "Configuration.hpp"
#include "HexagonalGrid.hpp"
#include "SquareGrid.hpp"
#include "TriangularGrid.hpp"
#include "KingsGrid.hpp"
#include "IdentifyingCodeConfiguration.hpp"

#include "ConstraintGenerator.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace adage::grids;

/**
 * hex-constraints.exe [TreeSearch arguments] --rules [file]
 *
 * Load the rules from the given file (possibly with values?) and use that to generate
 * all of the constraints for the linear program.
 */
int main(int argc, char** argv)
{
	// TODO: Make grid and conf be changeable by arguments?
	int grid_type = HEXAGONAL_GRID;
	int mode = MODE_IDENTIFYING;

	for ( int i = 1; i < argc; i++ )
	{
		if ( strcmp(argv[i],"--hexagonal")== 0)
		{
			grid_type = HEXAGONAL_GRID;
		}
		if ( strcmp(argv[i],"--square")== 0)
		{
			grid_type = SQUARE_GRID;
		}if ( strcmp(argv[i],"--triangular")== 0)
		{
			grid_type = TRIANGULAR_GRID;
		}
		
		if ( strcmp(argv[i],"--identifying")== 0)
		{
			mode = MODE_IDENTIFYING;
		}
		if ( strcmp(argv[i],"--dominating")== 0)
		{
			mode = MODE_DOMINATING;
		}
		if ( strcmp(argv[i],"--locating")== 0)
		{
			mode = MODE_LOCATING_DOMINATING;
		}
		if ( strcmp(argv[i],"--strongidentifying")== 0)
		{
			mode = MODE_STRONG_IDENTIFYING;
		}
		if ( strcmp(argv[i],"--openneighborhood")== 0)
		{
			mode = MODE_OPEN_NEIGHBORHOOD;
		}
		if ( strcmp(argv[i],"--neighbor")== 0)
		{
			mode = MODE_NEIGHBOR_IDENTIFYING;
		}
	}

	Grid* grid = 0;

	switch ( grid_type )
	{
		case HEXAGONAL_GRID:
			grid =  new HexagonalGrid(12);
			break;
		case SQUARE_GRID:
			grid =  new SquareGrid(12);
			break;
		case TRIANGULAR_GRID:
			grid =  new TriangularGrid(12);
			break;
	}

	Configuration* conf = new IdentifyingCodeConfiguration(grid, mode);

	ConstraintGenerator* generator = new ConstraintGenerator(grid, conf);

	generator->importArguments(argc, argv);

	while ( generator->readJob(stdin) > 0 )
	{
		generator->doSearch();
	}

	delete generator;
	delete conf;
	delete grid;

	return 0;
}

