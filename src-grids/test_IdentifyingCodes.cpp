/*
 * test_IdentifyingCodes.cpp
 *
 *  Created on: Apr 17, 2014
 *      Author: stolee
 */


#include "Grid.hpp"
#include "HexagonalGrid.hpp"
#include "Configuration.hpp"
#include "IdentifyingCodeConfiguration.hpp"

using namespace adage::grids;

bool test_ConstructorDestructor();
bool test_Propagate();
bool test_Snapshots();
bool test_Build();

Grid* grid;

int main(void)
{
	grid = new HexagonalGrid(12);

	int passes = 0;
	test_ConstructorDestructor() ? passes++ : printf("constructor failed!\n");

	test_Propagate() ? passes++ : printf("propagate failed!\n");

	test_Snapshots() ? passes++ : printf("Snapshots failed!\n");

	printf("Build Test:\n");
	test_Build() ? passes++ : printf("Snapshots failed!\n");

	printf("%d tests passed.\n", passes);

	delete grid;

	return 0;
	return 0;
}



bool test_ConstructorDestructor()
{
	Configuration* conf = new IdentifyingCodeConfiguration(grid);

	delete conf;

	return true;
}

bool test_Propagate()
{
	bool result = true;
	Configuration* conf = new IdentifyingCodeConfiguration(grid);

	conf->addFaceToShape(0);
	conf->print();

	for ( int i = 0; i < grid->getFaceDegree(0); i++ )
	{
		try{
			conf->addNonElement(grid->neighborFV(0,i));
			conf->print();
		} catch( ... )
		{
			result = false;
		}
	}

	conf->computeSymmetry();
	conf->print();

	delete conf;

	return result;

}

bool test_Snapshots()
{
bool result = true;
	Configuration* conf = new IdentifyingCodeConfiguration(grid);

	conf->addFaceToShape(0);
	conf->print();

	for ( int i = 0; i < grid->getFaceDegree(0); i++ )
	{
		try{
			conf->snapshot();
			conf->addNonElement(grid->neighborFV(0,i));
			conf->print();
		} catch( ... )
		{
			result = false;
		}
	}

	for (int i = grid->getFaceDegree(0)-1; i >= 0; i-- )
	{
		printf("Rollback\n");
		conf->rollback();
		conf->print();

		if ( true )
		{
			printf("Insert %d\n", grid->neighborFV(0,i));
			conf->addNonElement(grid->neighborFV(0,i));
			conf->print();
		}
	}

	delete conf;

	return result;
}


bool test_Build()
{
	bool result = true;
	Configuration* conf = new IdentifyingCodeConfiguration(grid);

	int ball_size = 0;
	int* ball = grid->getVertexBall(0, 10, ball_size);

	for ( int i = 0; i < ball_size; i++ )
	{
		if ( conf->isVertexInShape(ball[i]) == false )
		{
			conf->addNonElement(ball[i]);
		}
	}

	conf->print();

	delete conf;

	return result;
}


