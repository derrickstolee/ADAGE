/*
 * test_HexagonalGrid.cpp
 *
 *  Created on: Apr 17, 2014
 *      Author: stolee
 */

#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include "Grid.hpp"
#include "Configuration.hpp"
#include "HexagonalGrid.hpp"
 #include <stdlib.h>

using namespace adage::grids;

int main(int argc, char** argv)
{
	Grid* grid = new HexagonalGrid(20);

	int r = -1;

	for ( int i = 0; i < argc; i++ )
	{
		if ( strcmp(argv[i],"-r") == 0 )
		{
			r = atoi(argv[i+1]);
		}
	}

	int N = grid->getMaxVertexIndex();
	int F = grid->getMaxFacialIndex();

	if ( r < 0 )
	{
		grid->printDualAdjLists();
	}
	else
	{
		int ballsize = 0;
		int* ball = 0;
		ball = grid->getVertexBall(0, r, ballsize);

		Configuration* conf = new Configuration(grid);

		for ( int i = 0; i < ballsize; i++ )
		{
			conf->addVertexToShape(ball[i]);

			for ( int j = 0; j < grid->getVertexDegree(ball[i]); j++ )
			{
				int f = grid->neighborVF(ball[i],j);

				if ( conf->isFaceInShape(f) == false )
				{
					conf->addFaceToShape(f);

					for ( int k = 0; k < grid->getFaceDegree(f); k++ )
					{
						int u = grid->neighborFV(f,k);

						if ( conf->isVertexInShape(u) == false )
						{
							conf->addVertexToShape(u);
						}
					}
				}
			}
		}

		conf->print();

		delete conf;
		free(ball);
	}

	delete grid;

	return 0;
}
