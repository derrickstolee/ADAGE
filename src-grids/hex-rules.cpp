/*
 * hex-rules.cpp
 *
 *  Created on: Apr 17, 2014
 *      Author: stolee
 */

#include "Grid.hpp"
#include "HexagonalGrid.hpp"
#include "SquareGrid.hpp"
#include "TriangularGrid.hpp"
#include "KingsGrid.hpp"
#include "Configuration.hpp"
#include "IdentifyingCodeConfiguration.hpp"
#include "RuleGenerator.hpp"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

using namespace adage::grids;

/**
 * hex-rules.exe [vertexdiameter] [facedegree] [--pull #] [--restrict]
 *
 * Generate a series of rules for IdentifyingCodes on the Hexagonal Grid.
 *
 * Vertex-to-Face Rules: Use a diameter ball around an element to send charge.
 * 		If --restrict is on, then restrict to vertices within the adjacent faces. (matters for diameter > 2)
 *
 * Face-to-Face Rules: Use a center face with "facedegree" adjacent faces, specify the vertices.
 * 		If "--pull #" is used, then only pull from the center # of adjacent faces.
 * 		Example: 3 --pull 1 pulls from face in middle of set of three.
 * 		Example: 4 --pull 2 pulls from middle two faces of four.
 *
 *
 * TODO: Set a maximum pull size on the faces! (So only pull if a <=3-face, for instance.)
 *
 */

int main(int argc, char** argv)
{
	int grid_type = HEXAGONAL_GRID;
	int vertexdiameter = -1;
	int facedegree = -1;
	int pulldegree = 2;
	int detaildegree = -1;
	int max_pull_size = 3;
	bool restrict_to_diameter = false;
	bool use_face_sizes = false;
	bool test_v2f = false;
	bool test_f2f = false;
	bool write = false;
	bool triface = false;
	bool yface = false;
	bool yfaceplus = false;
	bool ysize = false;
	bool threefacevert = false;
	bool vertjump = false;
	bool c1 = false;
	bool c2 = false;
	bool c3 = false;
	int mode = MODE_IDENTIFYING;

	for ( int i = 1; i < argc; i++ )
	{
		if ( strcmp(argv[i],"--restrict")== 0)
		{
			restrict_to_diameter = true;
		}
		if ( strcmp(argv[i],"--facesizes")== 0)
		{
			use_face_sizes = true;
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
		if ( strcmp(argv[i],"--triface")== 0)
		{
			triface = true;
		}
		if ( strcmp(argv[i],"--yface")== 0)
		{
			yface = true;
		}
		if ( strcmp(argv[i],"--yface+")== 0)
		{
			yfaceplus = true;
		}
		if ( strcmp(argv[i],"--3face")== 0)
		{
			threefacevert = true;
		}
		if ( strcmp(argv[i],"--ysize")== 0)
		{
			ysize = true;
		}
		if ( strcmp(argv[i],"--vjump")== 0)
		{
			vertjump = true;
		}
		if ( strcmp(argv[i],"--v2fonly")== 0)
		{
			test_f2f = false;
		}
		if ( strcmp(argv[i],"--f2fonly")== 0)
		{
			test_v2f = false;
		}
		if ( strcmp(argv[i],"--write")== 0)
		{
			write = true;
		}
		if ( argv[i][0] == '-' && argv[i][1] == 'c' )
		{
			// continue testing things for c1, c2, c3...
			for ( int j = 2; argv[i][j] != 0; j++ )
			{
				if ( argv[i][j] == '1' )
				{
					c1 = true;
				}
				if ( argv[i][j] == '2' )
				{
					c2 = true;
				}
				if ( argv[i][j] == '3' )
				{
					c3 = true;
				}
			}
		}
		if ( i < argc - 1 && strcmp(argv[i], "--pull") == 0)
		{
			pulldegree = atoi(argv[i+1]);
		}
		if ( i < argc - 1 && strcmp(argv[i], "--vertex") == 0)
		{
			vertexdiameter = atoi(argv[i+1]);
		}
		if ( i < argc - 1 && strcmp(argv[i], "--face") == 0)
		{
			facedegree = atoi(argv[i+1]);
		}
		if ( i < argc - 1 && strcmp(argv[i], "--detail") == 0)
		{
			detaildegree = atoi(argv[i+1]);
		}
		if ( i < argc - 1 && strcmp(argv[i], "--pullsize") == 0)
		{
			max_pull_size = atoi(argv[i+1]);
		}
	}

	Grid* grid = 0;

	switch ( grid_type )
	{
		case HEXAGONAL_GRID:
			grid =  new HexagonalGrid(12);
			break;
	}

	Configuration* conf= new IdentifyingCodeConfiguration(grid, mode);

	RuleGenerator* rulegen = new RuleGenerator(grid, conf, write);

	if ( test_f2f || facedegree > 0 )
	{
		rulegen->generateFaceToFaceRules(facedegree, pulldegree, max_pull_size, detaildegree);
	}


	if ( test_v2f || vertexdiameter > 0 )
	{
		rulegen->generateVertexToFaceRules(vertexdiameter, restrict_to_diameter, use_face_sizes);
	}

	int num_from = 3;
	int*  from_colors = 0;

	if ( threefacevert )
	{
		// Given all info for the three incident faces,
		// decide how to send/receive charge among incident faces...
		Configuration* c = new IdentifyingCodeConfiguration(grid);

		if ( grid_type == HEXAGONAL_GRID )
		{
			c->addVertexToShape(0);
			
			c->addVertexToShape(1);
			c->addVertexToShape(2);
			c->addVertexToShape(4);

			c->addVertexToShape(5);
			c->addVertexToShape(14);
			c->addVertexToShape(6);
			c->addVertexToShape(10);
			c->addVertexToShape(22);
			c->addVertexToShape(12);
			c->addVertexToShape(11);
			c->addVertexToShape(3);
			c->addVertexToShape(7);

			c->addFaceToShape(0);
			c->addFaceToShape(2);
			c->addFaceToShape(6);

			// c->addVertexToShape(7);
			// c->addVertexToShape(11);
			// c->addVertexToShape(3);
			// c->addVertexToShape(16);
			// c->addVertexToShape(20);

			from_colors = (int*)malloc(3*sizeof(int));
			from_colors[0] = 0;
			from_colors[1] = 2;
			from_colors[2] = 6;

			c->setCenter(0, true);
			c->computeSymmetry();
		}
		// TODO: Other Grids!

		rulegen->generateFaceToVertexRules(c, 0, num_from, from_colors);
		free(from_colors);

		delete c;
	}

	if ( vertjump )
	{
		// Jump over a neighbor to that face
		// to send or receive charge!
		Configuration* c = new IdentifyingCodeConfiguration(grid);


		if ( grid_type == HEXAGONAL_GRID )
		{
			c->addVertexToShape(0);
			
			c->addVertexToShape(1);
			c->addVertexToShape(2);
			c->addVertexToShape(4);

			c->addVertexToShape(5);
			c->addVertexToShape(14);
			c->addVertexToShape(6);
			c->addVertexToShape(10);
			c->addVertexToShape(22);
			c->addVertexToShape(12);
			c->addVertexToShape(11);
			c->addVertexToShape(3);
			c->addVertexToShape(7);

			c->addFaceToShape(6);

			// c->addVertexToShape(7);
			// c->addVertexToShape(11);
			// c->addVertexToShape(3);
			// c->addVertexToShape(16);
			// c->addVertexToShape(20);

			num_from = 1;
			int* from_colors = (int*)malloc(num_from*sizeof(int));
			from_colors[0] = 6;

			// the center is the vertex 1
			c->setCenter(1, true);
			c->computeSymmetry();
		}

		rulegen->generateFaceToVertexRules(c, 0, num_from, from_colors);
		free(from_colors);

		delete c;
	}

	if ( triface)
	{
		Configuration* c = new IdentifyingCodeConfiguration(grid);

		if ( grid_type == HEXAGONAL_GRID )
		{
			c->addFaceToShape(0);
			c->addFaceToShape(2);
			c->addFaceToShape(3);
			c->addFaceToShape(8);

			for ( int i = 0; i < c->getNumFacesInShape(); i++ )
			{
				int f = c->getFaceFromShape(i);

				for ( int j = 0; j < grid->getFaceDegree(f); j++ )
				{
					int v = grid->neighborFV(f,j);

					if ( c->isVertexInShape(v) == false )
					{
						c->addVertexToShape(v);
					}
				}
			}

			num_from = 3;
			int* from_colors = (int*)malloc(num_from*sizeof(int));
			from_colors[0] = 2;
			from_colors[1] = 3;
			from_colors[2] = 8;

			c->setCenter(0, false);
			c->computeSymmetry();
		}

		rulegen->generateFaceToFaceRules(c, 0, 3, from_colors, max_pull_size);
		free(from_colors);

		delete c;
	}

	if ( yface)
	{
		// Y-face rules are similar to triface rules, but instead is
		// based on the sizes of the surrounding faces.
		// This will create a lot of repeats in the constraint list, helping
		// the size of the LP.
		Configuration* c = new IdentifyingCodeConfiguration(grid);


		if ( grid_type == HEXAGONAL_GRID )
		{
			c->addFaceToShape(0);
			c->addFaceToShape(2);
			c->addFaceToShape(3);
			c->addFaceToShape(8);

			int f = 0;

			for ( int j = 0; j < grid->getFaceDegree(f); j++ )
			{
				int v = grid->neighborFV(f,j);

				if ( c->isVertexInShape(v) == false )
				{
					c->addVertexToShape(v);
				}

				// // and the neighbors! (?)
				// for ( int k = 0; k < grid->getVertexDegree(v); k++ )
				// {
				// 	int u = grid->neighborVV(v, k);

				// 	if ( c->isVertexInShape(u) == false )
				// 	{
				// 		c->addVertexToShape(u);
				// 	}
				// }
			}

			num_from = 3;
			int* from_colors = (int*)malloc(num_from*sizeof(int));
			from_colors[0] = 2;
			from_colors[1] = 3;
			from_colors[2] = 8;

			c->setCenter(0, false);
			c->computeSymmetry();
		}

		// DON'T use face sizes!
		rulegen->setFaceSizes(false);
		rulegen->generateFaceToFaceRules(c, 0, 3, from_colors, max_pull_size);
		free(from_colors);

		delete c;
	}
	if ( yfaceplus)
	{
		// Y-face rules are similar to triface rules, but instead is
		// based on the sizes of the surrounding faces.
		// This will create a lot of repeats in the constraint list, helping
		// the size of the LP.
		Configuration* c = new IdentifyingCodeConfiguration(grid);


		if ( grid_type == HEXAGONAL_GRID )
		{
			c->addFaceToShape(0);
			c->addFaceToShape(2);
			c->addFaceToShape(3);
			c->addFaceToShape(8);

			int f = 0;

			for ( int j = 0; j < grid->getFaceDegree(f); j++ )
			{
				int v = grid->neighborFV(f,j);

				if ( c->isVertexInShape(v) == false )
				{
					c->addVertexToShape(v);
				}

				// and the neighbors! (?)
				for ( int k = 0; k < grid->getVertexDegree(v); k++ )
				{
					int u = grid->neighborVV(v, k);

					if ( c->isVertexInShape(u) == false )
					{
						c->addVertexToShape(u);
					}
				}
			}

			num_from = 3;
			int* from_colors = (int*)malloc(num_from*sizeof(int));
			from_colors[0] = 2;
			from_colors[1] = 3;
			from_colors[2] = 8;

			c->setCenter(0, false);
			c->computeSymmetry();
		}	

		// DON'T use face sizes!
		rulegen->setFaceSizes(false);
		rulegen->generateFaceToFaceRules(c, 0, 3, from_colors, max_pull_size);
		free(from_colors);

		delete c;
	}


	if ( ysize )
	{
		// Y-face rules are similar to triface rules, but instead is
		// based on the sizes of the surrounding faces.
		// This will create a lot of repeats in the constraint list, helping
		// the size of the LP.
		Configuration* c = new IdentifyingCodeConfiguration(grid);


		if ( grid_type == HEXAGONAL_GRID )
		{
			c->addFaceToShape(0);
			c->addFaceToShape(2);
			c->addFaceToShape(3);
			c->addFaceToShape(8);

			num_from = 3;
			int* from_colors = (int*)malloc(num_from*sizeof(int));
			from_colors[0] = 2;
			from_colors[1] = 3;
			from_colors[2] = 8;

			c->setCenter(0, false);
			c->computeSymmetry();
		}

		rulegen->generateFaceToFaceRules(c, 0, 3, from_colors, max_pull_size);
		free(from_colors);

		delete c;
	}

	if ( c1 )
	{
		// Between two adjacent vertices, knowing everything about 
		// the two faces sharing that edge.
		Configuration* c = new IdentifyingCodeConfiguration(grid);


		if ( grid_type == HEXAGONAL_GRID )
		{
			c->addVertexToShape(0);
			c->addVertexToShape(2);
			c->addVertexToShape(10);
			c->addVertexToShape(22);
			c->addVertexToShape(12);
			c->addVertexToShape(4);

			c->addVertexToShape(6);
			c->addVertexToShape(14);
			c->addVertexToShape(5);
			c->addVertexToShape(1);

			// c->addVertexToShape(7);
			// c->addVertexToShape(11);
			// c->addVertexToShape(3);
			// c->addVertexToShape(16);
			// c->addVertexToShape(20);

			num_from = 1;
			int* from_colors = (int*)malloc(num_from*sizeof(int));
			from_colors[0] = 2;
			// from_colors[1] = 2;
			// from_colors[2] = 4;

			c->setCenter(0, true);
			c->computeSymmetry();
		}

		rulegen->generateVertexToVertexRules(c, 0, num_from, from_colors);
		free(from_colors);

		delete c;
	}

	if ( c2 )
	{
		// Between two vertices at distance 2, knowing everything about their neighborhoods
		// and the face they share.
		Configuration* c = new IdentifyingCodeConfiguration(grid);


		if ( grid_type == HEXAGONAL_GRID )
		{
			c->addVertexToShape(0);
			c->addVertexToShape(1);
			c->addVertexToShape(2);
			c->addVertexToShape(4);

			c->addVertexToShape(10);
			c->addVertexToShape(22);
			c->addVertexToShape(20);

			c->addVertexToShape(12);
			c->addVertexToShape(6);

			// c->addVertexToShape(20);
			// c->addVertexToShape(23);
			// c->addVertexToShape(13);

			num_from = 1;
			int* from_colors = (int*)malloc(num_from*sizeof(int));
			from_colors[0] = 10;

			c->setCenter(0, true);
			c->computeSymmetry();
		}	

		rulegen->generateVertexToVertexRules(c, 0, num_from, from_colors);
		free(from_colors);

		delete c;
	}

	if ( c3 )
	{
		// Between two vertices at distance 3, knowing everything about their neighborhoods
		// and the face they share.
		Configuration* c = new IdentifyingCodeConfiguration(grid);


		if ( grid_type == HEXAGONAL_GRID )
		{
			c->addVertexToShape(0);
			c->addVertexToShape(1);
			c->addVertexToShape(2);
			c->addVertexToShape(10);
			c->addVertexToShape(22);
			c->addVertexToShape(12);
			c->addVertexToShape(4);
			c->addVertexToShape(38);
			c->addVertexToShape(6);
			c->addVertexToShape(20);
			c->addVertexToShape(11);
			c->addVertexToShape(24);

			num_from = 1;
			int* from_colors = (int*)malloc(num_from*sizeof(int));
			from_colors[0] = 22;

			c->setCenter(0, true);
			c->computeSymmetry();
		}

		rulegen->generateVertexToVertexRules(c, 0, num_from, from_colors);
		free(from_colors);

		delete c;
	}

	if ( !write )
	{
		rulegen->writeAllRules(stdout);
	}


	delete rulegen;
	delete conf;
	delete grid;

	return 0;
}
