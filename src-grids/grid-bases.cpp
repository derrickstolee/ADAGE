/*
 * grid-rules.cpp
 *
 *  Created on: Jul 2, 2015
 *      Author: stolee
 */

#include "Grid.hpp"
#include "Rule.hpp"
#include "Configuration.hpp"
#include "HexagonalGrid.hpp"
#include "HexagonalGridNoRotate.hpp"
#include "SquareGrid.hpp"
#include "TriangularGrid.hpp"
#include "PentagonalGrid.hpp"
#include "KingsGrid.hpp"
#include "LinearProgram.hpp"
#include "IdentifyingCodeConfiguration.hpp"

#include "ConstraintGenerator.hpp"
#include "SharpnessGenerator.hpp"
#include "LinearProgram.hpp"

#include "Trie.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace adage;
using namespace adage::grids;

/**
 * grid-rules.exe [grid details] 
 *
 * Generate the rules to fill a rule file for a given grid.
 */
int main(int argc, char** argv)
{
	Grid* grid = new HexagonalGrid(20);
	char* gridname = (char*)malloc(100);

	for ( int i = 1; i < argc; i++ )
	{
		if ( strcmp(argv[i],"--hexagonal")== 0)
		{
			delete grid;
			grid = new HexagonalGrid(20);
			strcpy(gridname, argv[i]+2);
		}
		if ( strcmp(argv[i],"--hexnorot")== 0)
		{
			delete grid;
			grid = new HexagonalGridNoRotate(20);
			strcpy(gridname, argv[i]+2);
		}
		if ( strcmp(argv[i],"--square")== 0)
		{
			delete grid;
			grid = new SquareGrid(20);
			strcpy(gridname, argv[i]+2);
		}
		if ( strcmp(argv[i],"--triangular")== 0)
		{
			delete grid;
			grid = new TriangularGrid(20);
			strcpy(gridname, argv[i]+2);
		}
		if ( strcmp(argv[i],"--pentagonal")== 0)
		{
			delete grid;
			grid = new PentagonalGrid(20);
			strcpy(gridname, argv[i]+2);
		}
		
		if ( strcmp(argv[i],"--tblowup")== 0)
		{
			Grid* oldgrid = grid;
			grid = oldgrid->getTightBlowup();
			delete oldgrid;

			strcat(gridname, "t");
		}
		if ( strcmp(argv[i],"--blowup")== 0)
		{
			Grid* oldgrid = grid;
			grid = oldgrid->getBlowup();
			delete oldgrid;

			strcat(gridname, "b");
		}
		if ( strcmp(argv[i],"--dual")== 0)
		{
			Grid* oldgrid = grid;
			grid = oldgrid->getDual();
			delete oldgrid;

			strcat(gridname, "d");
		}
		if ( strcmp(argv[i],"--snub")== 0)
		{
			Grid* oldgrid = grid;
			grid = oldgrid->getSnub();
			delete oldgrid;

			strcat(gridname, "s");
		}
	}

	for ( int r = 1; r <= 4; r++ )
	{
		int ball_size = 0;
		int* ball = grid->getVertexBall(0, r, ball_size);

		Configuration* c = new Configuration(grid);

		for ( int i = 0; i < ball_size; i++ )
		{
			c->addVertexToShape(ball[i]);

			for ( int j = 0; j < grid->getVertexDegree(ball[i]); j++ )
			{
				int f = grid->neighborVF(ball[i],j);

				if ( c->isFaceInShape(f) == false )
				{
					c->addFaceToShape(f);
				}
			}
		}

		char* buffer = (char*)malloc(50);
		sprintf(buffer, "bases-%s-%d.txt", gridname, r);

		FILE* fout = fopen(buffer, "w");

		c->write(fout);

		fclose(fout);
	}


	delete grid;
	return 0;
}

