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
	Grid* grid = new HexagonalGrid(5);
	int mode = MODE_IDENTIFYING;
	char* rulefile = 0;

	for ( int i = 1; i < argc; i++ )
	{
		if ( i < argc - 1 && strcmp(argv[i],"--rules") == 0 )
		{
			rulefile = argv[i+1];
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

	if ( rulefile == 0 )
	{
		printf("Usage: hex-constraints.exe [TreeSearch arguments] --rules rulefile");
	}

	Configuration* conf = new IdentifyingCodeConfiguration(grid, mode);
	ConstraintGenerator* generator = new ConstraintGenerator(grid, conf);

	FILE* file = fopen(rulefile, "r");
	generator->loadRules(file);
	fclose(file);

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

