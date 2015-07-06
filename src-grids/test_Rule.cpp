/*
 * test_Rule.cpp
 *
 *  Created on: Apr 17, 2014
 *      Author: stolee
 */


#include <stdlib.h>
#include <stdio.h>
#include "Grid.hpp"
#include "HexagonalGrid.hpp"
#include "Configuration.hpp"
#include "Rule.hpp"

using namespace adage::grids;

int cur_variable_index = 0;
bool test_ConstructorDestructor();
bool test_Vertex2Face();
bool test_Face2Face();
bool test_WriteRead();

Grid* grid;

int main(void)
{
	grid = new HexagonalGrid(10);

	int passes = 0;
	test_ConstructorDestructor() ? passes++ : printf("constructor failed!\n");
	test_Vertex2Face() ? passes++ : printf("vertex-to-face failed!\n");
	test_Face2Face() ? passes++ : printf("face-to-face failed!\n");
	test_WriteRead() ? passes++ : printf("Write/Read failed!\n");

	printf("%d tests passed.\n", passes);

	delete grid;

	return 0;
}


bool test_ConstructorDestructor()
{
	printf("\n\n\n");
	Configuration* conf = new Configuration(grid);


	conf->addFaceToShape(0);

	for ( int i = 0; i < grid->getFaceDegree(0); i++ )
	{
		conf->addNonElement(grid->neighborFV(0,i));
	}

	Rule* rule = new Rule(conf, VARIABLE_TYPE_V2V,  cur_variable_index);


	delete rule;
	delete conf;

	return true;
}

bool test_Vertex2Face()
{
	printf("\n\n\n");
	Configuration* conf = new Configuration(grid);

	conf->addElement(0);

	for ( int i = 0; i < grid->getVertexDegree(0); i++ )
	{
		conf->addNonElement(grid->neighborVV(0,i));
		conf->addFaceToShape(grid->neighborVF(0,i));
	}

	conf->computeSymmetry();
	conf->print();

	Rule* rule = new Rule(conf, VARIABLE_TYPE_V2F,  cur_variable_index);

	cur_variable_index += rule->getNumVariables();
	rule->write(stdout);

	delete rule;
	delete conf;

	return true;
}

bool test_Face2Face()
{
	printf("\n\n\n");
	Configuration* conf = new Configuration(grid);

	conf->addFaceToShape(0);

	int* faces = (int*)malloc(4 * sizeof(int));
	int num_from = 2;

	for ( int i = 0; i < 4; i++ )
	{
		int f = grid->neighborFF(0,i);
		conf->addFaceToShape(f);

		if ( i > 0 && i <= num_from )
		{
			faces[i-1] = f;
		}
	}

	// Set face 0 as center
	conf->setCenter( 0, false );

	conf->computeSymmetry();


	conf->print();

	Rule* rule = new Rule(conf, VARIABLE_TYPE_F2F, 0, faces, num_from,  cur_variable_index);

	cur_variable_index += rule->getNumVariables();
	rule->write(stdout);

	delete rule;
	delete conf;
	free(faces);

	return true;
}

bool test_WriteRead()
{
	printf("\n\n\n");
	Configuration* conf = new Configuration(grid);

	conf->addFaceToShape(0);

	int* faces = (int*)malloc(4 * sizeof(int));
	int num_from = 2;

	for ( int i = 0; i < 4; i++ )
	{
		int f = grid->neighborFF(0,i);
		conf->addFaceToShape(f);

		if ( i > 0 && i <= num_from )
		{
			faces[i-1] = f;
		}

		for ( int j = 0 ;j < grid->getFaceDegree(f); j++ )
		{
			int v = grid->neighborFV(f,j);
			if ( conf->isVertexInShape(v) == false )
			{
				conf->addVertexToShape(v);
			}
		}
	}


	// Set face 0 as center
	conf->setCenter( 0, false );

	conf->computeSymmetry();

	// conf->print();

	Rule* rule = new Rule(conf, VARIABLE_TYPE_F2F, 0, faces, num_from, cur_variable_index);

	cur_variable_index += rule->getNumVariables();

	printf("Rule:\n");
	rule->print();
	printf("\n");

	FILE* out = fopen("test_rule.txt", "w");
	rule->write(out);
	fclose(out);

	FILE* in = fopen("test_rule.txt", "r");
	Rule* rule2 = Rule::read(grid, in);
	fclose(in);


	printf("Rule2:\n");
	rule2->print();
	// TODO: Compare the rules?

	delete rule2;
	delete rule;
	delete conf;
	free(faces);

	return true;
}

