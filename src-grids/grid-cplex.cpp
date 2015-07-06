/*
 * grid-cplex.cpp
 *
 *  Created on: May 26, 2015
 *      Author: stolee
 */

#include "Grid.hpp"
#include "Rule.hpp"
#include "Configuration.hpp"
#include "HexagonalGrid.hpp"
#include "HexagonalGridNoRotate.hpp"
#include "SquareGrid.hpp"
#include "TriangularGrid.hpp"
#include "KingsGrid.hpp"
#include "PentagonalGrid.hpp"
#include "LinearProgram.hpp"
#include "IdentifyingCodeConfiguration.hpp"

#include "ConstraintGenerator.hpp"
#include "SharpnessGenerator.hpp"
#include "LinearProgramCPLEX.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace adage;
using namespace adage::grids;

/**
 * hex-constraints.exe [TreeSearch arguments] --rules [file]
 *
 * Load the rules from the given file (possibly with values?) and use that to generate
 * all of the constraints for the linear program.
 */
int main(int argc, char** argv)
{
	bool do_sharpness = false;
	int mode = MODE_IDENTIFYING;

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

	Configuration* conf = new IdentifyingCodeConfiguration(grid, mode);

	LinearProgram* lp = new LinearProgramCPLEX();
	lp->importArguments(argc, argv);

	ConstraintGenerator* generator = new ConstraintGenerator(grid, conf, lp);

	generator->importArguments(argc, argv);

	generator->loadEmptyJob();
	generator->doSearch();
	
	printf("Solving normal LP:\n");
	lp->solve();

	if ( lp->isFeasible() )
	{
		printf("LP is feasible.\n");

		double dopt = lp->getOptimum();
		fraction opt = lp->getExactOptimum();

		printf("Optimum Value: %lf == %lld / %lld.\n", dopt, opt.a, opt.b);
	}
	else
	{
		printf("LP is infeasible!\n");
	}

	delete generator;

	if ( do_sharpness ) 
	{
		printf("\nSearching for a sharpness example.\n");
		SharpnessGenerator* sg = new SharpnessGenerator(grid, conf, lp);

		sg->importArguments(argc, argv);
		sg->loadEmptyJob();
		sg->doSearch();
	}

	// delete lp;

	delete conf;
	delete grid;
	return 0;
}

