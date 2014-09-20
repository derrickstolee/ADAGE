/*
 * test_GridTransform.cpp
 *
 *  Created on: May 1, 2014
 *      Author: stolee
 */

#include <stdio.h>
#include "Grid.hpp"
#include "HexagonalGrid.hpp"

using namespace adage::grids;

void testTransformVV(Grid* grid);
void testTransformVVFlip(Grid* grid);
void testTransformVF(Grid* grid);
void testTransformFV(Grid* grid);
void testTransformFF(Grid* grid);

int main(void)
{
	Grid* grid = new HexagonalGrid(20);

	int N = grid->getMaxVertexIndex();
	int F = grid->getMaxFacialIndex();

	// testTransformVV(grid);
	// testTransformVVFlip(grid);

//	testTransformVF(grid);
//
//	testTransformFV(grid);
//
//	testTransformFF(grid);

	// This is the first set of possible automorphisms!

	int* vperm = 0;
	int* fperm = 0;

	{
		printf("vautoms = [];\n");
		{
			int f1 = 0;
			int f2 = 6;
			int g1 = 349-313;
			int g2 = 379-313;
			bool flip = false;

			grid->transformFF(fperm, vperm, f1, f2, g1, g2, flip);

			printf("vperm = [");
			for ( int i = 0; i < grid->getMaxVertexIndex(); i++ )
			{
				printf(" %3d", vperm[i]);
				if ( i < grid->getMaxVertexIndex( )-1)
				{
					printf(",");
				}
			}
			printf(" ];\nvautoms.append(vperm);\n\n");
		}

		
		{
			int f1 = 0;
			int f2 = 3;
			int g1 = 475-313;
			int g2 = 429-313;
			bool flip = false;

			grid->transformFF(fperm, vperm, f1, f2, g1, g2, flip);

			printf("vperm = [");
			for ( int i = 0; i < grid->getMaxVertexIndex(); i++ )
			{
				printf(" %3d", vperm[i]);
				if ( i < grid->getMaxVertexIndex( )-1)
				{
					printf(",");
				}
			}
			printf(" ];\nvautoms.append(vperm);\n\n");
		}
		
		{
			int f1 = 0;
			int f2 = 7;
			int g1 = 439-313;
			int g2 = 397-313;
			bool flip = false;

			grid->transformFF(fperm, vperm, f1, f2, g1, g2, flip);

			printf("vperm = [");
			for ( int i = 0; i < grid->getMaxVertexIndex(); i++ )
			{
				printf(" %3d", vperm[i]);
				if ( i < grid->getMaxVertexIndex( )-1)
				{
					printf(",");
				}
			}
			printf(" ];\nvautoms.append(vperm);\n\n");
		}
		printf("vautoms1 = vautoms;\n\n\nvautoms = [];\n");
	}	


	{
		printf("vautoms = [];\n");
		{
			int f1 = 0;
			int f2 = 2;
			int g1 = 349-313;
			int g2 = 379-313;
			bool flip = false;

			grid->transformFF(fperm, vperm, f1, f2, g1, g2, flip);

			printf("vperm = [");
			for ( int i = 0; i < grid->getMaxVertexIndex(); i++ )
			{
				printf(" %3d", vperm[i]);
				if ( i < grid->getMaxVertexIndex( )-1)
				{
					printf(",");
				}
			}
			printf(" ];\nvautoms.append(vperm);\n\n");
		}

		
		{
			int f1 = 0;
			int f2 = 3;
			int g1 = 475-313;
			int g2 = 429-313;
			bool flip = false;

			grid->transformFF(fperm, vperm, f1, f2, g1, g2, flip);

			printf("vperm = [");
			for ( int i = 0; i < grid->getMaxVertexIndex(); i++ )
			{
				printf(" %3d", vperm[i]);
				if ( i < grid->getMaxVertexIndex( )-1)
				{
					printf(",");
				}
			}
			printf(" ];\nvautoms.append(vperm);\n\n");
		}
		
		{
			int f1 = 0;
			int f2 = 7;
			int g1 = 439-313;
			int g2 = 397-313;
			bool flip = false;

			grid->transformFF(fperm, vperm, f1, f2, g1, g2, flip);

			printf("vperm = [");
			for ( int i = 0; i < grid->getMaxVertexIndex(); i++ )
			{
				printf(" %3d", vperm[i]);
				if ( i < grid->getMaxVertexIndex( )-1)
				{
					printf(",");
				}
			}
			printf(" ];\nvautoms.append(vperm);\n\n");
		}
		printf("vautoms2 = vautoms;\n\n\nvautoms = [];\n");
	}	


	{
		printf("vautoms = [];\n");
		{
			int f1 = 0;
			int f2 = 6;
			int g1 = 349-313;
			int g2 = 379-313;
			bool flip = false;

			grid->transformFF(fperm, vperm, f1, f2, g1, g2, flip);

			printf("vperm = [");
			for ( int i = 0; i < grid->getMaxVertexIndex(); i++ )
			{
				printf(" %3d", vperm[i]);
				if ( i < grid->getMaxVertexIndex( )-1)
				{
					printf(",");
				}
			}
			printf(" ];\nvautoms.append(vperm);\n\n");
		}

		
		{
			int f1 = 0;
			int f2 = 3;
			int g1 = 475-313;
			int g2 = 429-313;
			bool flip = false;

			grid->transformFF(fperm, vperm, f1, f2, g1, g2, flip);

			printf("vperm = [");
			for ( int i = 0; i < grid->getMaxVertexIndex(); i++ )
			{
				printf(" %3d", vperm[i]);
				if ( i < grid->getMaxVertexIndex( )-1)
				{
					printf(",");
				}
			}
			printf(" ];\nvautoms.append(vperm);\n\n");
		}
		
		{
			int f1 = 0;
			int f2 = 6;
			int g1 = 439-313;
			int g2 = 397-313;
			bool flip = false;

			grid->transformFF(fperm, vperm, f1, f2, g1, g2, flip);

			printf("vperm = [");
			for ( int i = 0; i < grid->getMaxVertexIndex(); i++ )
			{
				printf(" %3d", vperm[i]);
				if ( i < grid->getMaxVertexIndex( )-1)
				{
					printf(",");
				}
			}
			printf(" ];\nvautoms.append(vperm);\n\n");
		}
		printf("vautoms3 = vautoms;\n\n\nvautoms = [];\n");
	}	


	{
		printf("vautoms = [];\n");
		{
			int f1 = 0;
			int f2 = 6;
			int g1 = 349-313;
			int g2 = 379-313;
			bool flip = false;

			grid->transformFF(fperm, vperm, f1, f2, g1, g2, flip);

			printf("vperm = [");
			for ( int i = 0; i < grid->getMaxVertexIndex(); i++ )
			{
				printf(" %3d", vperm[i]);
				if ( i < grid->getMaxVertexIndex( )-1)
				{
					printf(",");
				}
			}
			printf(" ];\nvautoms.append(vperm);\n\n");
		}

		
		{
			int f1 = 0;
			int f2 = 3;
			int g1 = 475-313;
			int g2 = 429-313;
			bool flip = false;

			grid->transformFF(fperm, vperm, f1, f2, g1, g2, flip);

			printf("vperm = [");
			for ( int i = 0; i < grid->getMaxVertexIndex(); i++ )
			{
				printf(" %3d", vperm[i]);
				if ( i < grid->getMaxVertexIndex( )-1)
				{
					printf(",");
				}
			}
			printf(" ];\nvautoms.append(vperm);\n\n");
		}
		
		{
			int f1 = 0;
			int f2 = 2;
			int g1 = 439-313;
			int g2 = 397-313;
			bool flip = false;

			grid->transformFF(fperm, vperm, f1, f2, g1, g2, flip);

			printf("vperm = [");
			for ( int i = 0; i < grid->getMaxVertexIndex(); i++ )
			{
				printf(" %3d", vperm[i]);
				if ( i < grid->getMaxVertexIndex( )-1)
				{
					printf(",");
				}
			}
			printf(" ];\nvautoms.append(vperm);\n\n");
		}
		printf("vautoms4 = vautoms;\n\n\nvautoms = [];\n");
	}	



	{
		printf("vautoms = [];\n");
		{
			int f1 = 0;
			int f2 = 6;
			int g1 = 349-313;
			int g2 = 379-313;
			bool flip = false;

			grid->transformFF(fperm, vperm, f1, f2, g1, g2, flip);

			printf("vperm = [");
			for ( int i = 0; i < grid->getMaxVertexIndex(); i++ )
			{
				printf(" %3d", vperm[i]);
				if ( i < grid->getMaxVertexIndex( )-1)
				{
					printf(",");
				}
			}
			printf(" ];\nvautoms.append(vperm);\n\n");
		}

		
		{
			int f1 = 0;
			int f2 = 1;
			int g1 = 475-313;
			int g2 = 429-313;
			bool flip = false;

			grid->transformFF(fperm, vperm, f1, f2, g1, g2, flip);

			printf("vperm = [");
			for ( int i = 0; i < grid->getMaxVertexIndex(); i++ )
			{
				printf(" %3d", vperm[i]);
				if ( i < grid->getMaxVertexIndex( )-1)
				{
					printf(",");
				}
			}
			printf(" ];\nvautoms.append(vperm);\n\n");
		}
		
		{
			int f1 = 0;
			int f2 = 7;
			int g1 = 439-313;
			int g2 = 397-313;
			bool flip = false;

			grid->transformFF(fperm, vperm, f1, f2, g1, g2, flip);

			printf("vperm = [");
			for ( int i = 0; i < grid->getMaxVertexIndex(); i++ )
			{
				printf(" %3d", vperm[i]);
				if ( i < grid->getMaxVertexIndex( )-1)
				{
					printf(",");
				}
			}
			printf(" ];\nvautoms.append(vperm);\n\n");
		}
		printf("vautoms5 = vautoms;\n\n\nvautoms = [];\n");
	}	
	// TODO: Actually verify!


	delete grid;

	return 0;
}

void testTransformVV(Grid* grid)
{
	printf("Testing Transform VV:\n");

	int* vperm = 0;
	int* fperm = 0;

	int v1 = 0;
	int v2 = 4;
	int u1 = 72;
	int u2 = 98;
	bool flip = false;

	grid->transformVV(fperm, vperm, v1, v2, u1, u2, flip);

	printf("vperm = [");
	for ( int i = 0; i < grid->getMaxVertexIndex(); i++ )
	{
		printf(" %3d", vperm[i]);
		if ( i < grid->getMaxVertexIndex( )-1)
		{
			printf(",");
		}
	}
	printf(" ];\nfperm = [");
	for ( int i = 0; i < grid->getMaxFacialIndex(); i++ )
	{
		printf(" %3d", fperm[i]);
		if ( i < grid->getMaxFacialIndex()-1)
		{
			printf(",");
		}
	}
	printf("];\n\n");

	// TODO: Actually verify!
}


void testTransformVVFlip(Grid* grid)
{
	printf("Testing Transform VV Flip:\n");

	int* vperm = 0;
	int* fperm = 0;

	int v1 = 0;
	int v2 = 4;
	int u1 = 72;
	int u2 = 98;
	bool flip = true;

	grid->transformVV(fperm, vperm, v1, v2, u1, u2, flip);

	printf("vperm = [");
	for ( int i = 0; i < grid->getMaxVertexIndex(); i++ )
	{
		printf(" %3d", vperm[i]);
		if ( i < grid->getMaxVertexIndex( )-1)
		{
			printf(",");
		}
	}
	printf(" ];\nfperm = [");
	for ( int i = 0; i < grid->getMaxFacialIndex(); i++ )
	{
		printf(" %3d", fperm[i]);
		if ( i < grid->getMaxFacialIndex()-1)
		{
			printf(",");
		}
	}
	printf("];\n\n");

	// TODO: Actually verify!
}

void testTransformVF(Grid* grid)
{
	printf("Testing Transform VF:\n");
	int* vperm = 0;
	int* fperm = 0;

	int v1 = 0;
	int f2 = 0;
	int u1 = 0;
	int g2 = 0;
	bool flip = false;

	grid->transformVF(fperm, vperm, v1, f2, u1, g2, flip);

	printf("vperm = [");
	for ( int i = 0; i < grid->getMaxVertexIndex(); i++ )
	{
		printf(" %3d", vperm[i]);
		if ( i < grid->getMaxVertexIndex( )-1)
		{
			printf(",");
		}
	}
	printf(" ];\nfperm = [");
	for ( int i = 0; i < grid->getMaxFacialIndex(); i++ )
	{
		printf(" %3d", fperm[i]);
		if ( i < grid->getMaxFacialIndex()-1)
		{
			printf(",");
		}
	}
	printf("];\n\n");

	// TODO: Actually verify!
}

void testTransformFV(Grid* grid)
{
	printf("Testing Transform FV:\n");
	int* vperm = 0;
	int* fperm = 0;

	int f1 = 0;
	int v2 = 0;
	int g1 = 0;
	int u2 = 0;
	bool flip = false;

	grid->transformFV(fperm, vperm, f1, v2, g1, u2, flip);


	printf("vperm = [");
	for ( int i = 0; i < grid->getMaxVertexIndex(); i++ )
	{
		printf(" %3d", vperm[i]);
		if ( i < grid->getMaxVertexIndex( )-1)
		{
			printf(",");
		}
	}
	printf(" ];\nfperm = [");
	for ( int i = 0; i < grid->getMaxFacialIndex(); i++ )
	{
		printf(" %3d", fperm[i]);
		if ( i < grid->getMaxFacialIndex()-1)
		{
			printf(",");
		}
	}
	printf("];\n\n");

	// TODO: Actually verify!
}

void testTransformFF(Grid* grid)
{
	printf("Testing Transform FF:\n");
	int* vperm = 0;
	int* fperm = 0;

	int f1 = 0;
	int f2 = 0;
	int g1 = 0;
	int g2 = 0;
	bool flip = false;

	grid->transformFF(fperm, vperm, f1, f2, g1, g2, flip);


	printf("vperm = [");
	for ( int i = 0; i < grid->getMaxVertexIndex(); i++ )
	{
		printf(" %3d", vperm[i]);
		if ( i < grid->getMaxVertexIndex( )-1)
		{
			printf(",");
		}
	}
	printf(" ];\nfperm = [");
	for ( int i = 0; i < grid->getMaxFacialIndex(); i++ )
	{
		printf(" %3d", fperm[i]);
		if ( i < grid->getMaxFacialIndex()-1)
		{
			printf(",");
		}
	}
	printf("];\n\n");

	// TODO: Actually verify!

}
