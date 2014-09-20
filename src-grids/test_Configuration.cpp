/*
 * test_Configuration.cpp
 *
 *  Created on: Apr 17, 2014
 *      Author: stolee
 */

#include <stdlib.h>
#include <stdio.h>
#include "Grid.hpp"
#include "HexagonalGrid.hpp"
#include "Configuration.hpp"

using namespace adage::grids;

bool test_ConstructorDestructor();
bool test_AddingFaces();
bool test_AddingVertices();
bool test_AddingElements();
bool test_AddingNonElements();
bool test_Centers();
bool test_Matches();
bool test_Equals();
bool test_Duplicate();
bool test_Colors();
bool test_Snapshots();
bool test_Snapshots2();
bool test_WriteRead();

Grid* grid;

int main(void)
{
	grid = new HexagonalGrid(10);

	int passes = 0;
	test_ConstructorDestructor() ? passes++ : printf("constructor failed!\n");
	test_AddingFaces() ? passes++ : printf("Adding faces failed!\n");
	test_AddingVertices() ? passes++ : printf("Adding vertices failed!\n");
	test_AddingElements() ? passes++ : printf("Adding elements failed!\n");
	test_AddingNonElements() ? passes++ : printf("Adding nonelements failed!\n");
	test_Centers() ? passes++ : printf("Centers failed!\n");
	test_Matches() ? passes++ : printf("Matches failed!\n");
	test_Equals() ? passes++ : printf("Equals failed!\n");

	test_Duplicate() ? passes++ : printf("Duplicate failed!\n");

	test_Colors() ? passes++ : printf("Colors failed!\n");

	test_Snapshots() ? passes++ : printf("Snapshots failed!\n");
	test_Snapshots2() ? passes++ : printf("Snapshots2 failed!\n");

	test_WriteRead() ? passes++ : printf("Write/Read failed!\n");

	printf("%d tests passed.\n", passes);

	delete grid;

	return 0;
}

bool test_ConstructorDestructor()
{
	Configuration* conf = new Configuration(grid);

	delete conf;

	return true;
}

bool test_AddingFaces()
{
	bool result = true;

	Configuration* conf = new Configuration(grid);

	conf->addFaceToShape(0);
	conf->addFaceToShape(1);
	conf->addFaceToShape(5);
	conf->addFaceToShape(6);
	conf->addFaceToShape(10);

	if ( conf->isFaceInShape(0) == false )
	{
		result = false;
	}
	if ( conf->isFaceInShape(1) == false )
	{
		result = false;
	}

	if ( conf->isFaceInShape(2) == true )
	{
		result = false;
	}
	if ( conf->isFaceInShape(5) == false )
	{
		result = false;
	}
	if ( conf->isFaceInShape(6) == false )
	{
		result = false;
	}

	if ( conf->isFaceInShape(7) == true )
	{
		result = false;
	}

	if ( conf->isFaceInShape(10) == false )
	{
		result = false;
	}

	if ( conf->isFaceInShape(11) == true )
	{
		result = false;
	}

	if ( conf->isFaceInShape(-1) == true )
	{
		result = false;
	}

	delete conf;

	return result;
}

bool test_AddingVertices()
{
	bool result = true;

	Configuration* conf = new Configuration(grid);

	conf->addVertexToShape(0);
	conf->addVertexToShape(1);
	conf->addVertexToShape(2);
	conf->addVertexToShape(5);
	conf->addVertexToShape(6);
	conf->addVertexToShape(10);
	conf->addVertexToShape(-1);

	if ( conf->isVertexInShape(0) == false )
	{
		result = false;
	}

	if ( conf->isVertexInShape(1) == false )
	{
		result = false;
	}

	if ( conf->isVertexInShape(2) == false )
	{
		result = false;
	}

	if ( conf->isVertexInShape(3) == true )
	{
		result = false;
	}

	if ( conf->isVertexInShape(5) == false )
	{
		result = false;
	}

	if ( conf->isVertexInShape(6) == false )
	{
		result = false;
	}
	if ( conf->isVertexInShape(7) == true )
	{
		result = false;
	}

	if ( conf->isVertexInShape(10) == false )
	{
		result = false;
	}

	if ( conf->isVertexInShape(-1) == true )
	{
		result = false;
	}

	delete conf;
	return result;
}

bool test_AddingElements()
{
	bool result = true;

	Configuration* conf = new Configuration(grid);

	conf->addElement(0);
	conf->addElement(1);
	conf->addElement(2);
	conf->addElement(5);
	conf->addElement(6);
	conf->addElement(10);
	conf->addElement(-1);

	if ( conf->isElement(0) == false )
	{
		result = false;
	}

	if ( conf->isElement(1) == false )
	{
		result = false;
	}

	if ( conf->isElement(2) == false )
	{
		result = false;
	}

	if ( conf->isElement(3) == true )
	{
		result = false;
	}

	if ( conf->isElement(5) == false )
	{
		result = false;
	}

	if ( conf->isElement(6) == false )
	{
		result = false;
	}
	if ( conf->isElement(7) == true )
	{
		result = false;
	}

	if ( conf->isElement(10) == false )
	{
		result = false;
	}

	if ( conf->isElement(-1) == true )
	{
		result = false;
	}

	delete conf;
	return result;
}

bool test_AddingNonElements()
{
	bool result = true;

	Configuration* conf = new Configuration(grid);

	conf->addNonElement(0);
	conf->addNonElement(1);
	conf->addNonElement(2);
	conf->addNonElement(5);
	conf->addNonElement(6);
	conf->addNonElement(10);
	conf->addNonElement(-1);

	if ( conf->isNonElement(0) == false )
	{
		result = false;
	}

	if ( conf->isNonElement(1) == false )
	{
		result = false;
	}

	if ( conf->isNonElement(2) == false )
	{
		result = false;
	}

	if ( conf->isNonElement(3) == true )
	{
		result = false;
	}

	if ( conf->isNonElement(5) == false )
	{
		result = false;
	}

	if ( conf->isNonElement(6) == false )
	{
		result = false;
	}
	if ( conf->isNonElement(7) == true )
	{
		result = false;
	}

	if ( conf->isNonElement(10) == false )
	{
		result = false;
	}

	if ( conf->isNonElement(-1) == true )
	{
		result = false;
	}

	delete conf;
	return result;
}

bool test_Centers()
{
	bool result = true;

	Configuration* conf = new Configuration(grid);

	conf->addVertexToShape(0);
	conf->addVertexToShape(1);
	conf->addVertexToShape(2);
	conf->addVertexToShape(3);

	conf->addFaceToShape(0);
	conf->addFaceToShape(1);
	conf->addFaceToShape(2);
	conf->addFaceToShape(3);

	if ( conf->hasCenter() )
	{
		result = false;
	}

	conf->setCenter(1, true);

	if ( !conf->hasCenter() )
	{
		result = false;
	}

	if ( conf->getCenter() != 1 )
	{
		result = false;
	}

	if ( conf->isCenterVertex() == false )
	{
		result = false;
	}

	conf->setCenter(2, false);

	if ( !conf->hasCenter() )
	{
		result = false;
	}

	if ( conf->getCenter() != 2 )
	{
		result = false;
	}

	if ( conf->isCenterVertex() == true )
	{
		result = false;
	}

	delete conf;

	return result;
}

bool test_Matches()
{
	bool result = true;

	Configuration* conf1 = new Configuration(grid);
	Configuration* conf2 = new Configuration(grid);

	conf1->addVertexToShape(0);
	conf1->addVertexToShape(1);
	conf1->addVertexToShape(2);
	conf1->addVertexToShape(4);
	conf1->addVertexToShape(5);
	conf1->addVertexToShape(7);

	conf2->addVertexToShape(0);
	conf2->addVertexToShape(1);
	conf2->addVertexToShape(3);
	conf2->addVertexToShape(4);
	conf2->addVertexToShape(6);
	conf2->addVertexToShape(7);

	conf1->addElement(1);
	conf1->addElement(4);
	conf1->addNonElement(7);

	conf2->addElement(1);
	conf2->addElement(4);
	conf2->addNonElement(7);

	if ( conf1->matches(conf1) == false )
	{
		result = false;
	}
	if ( conf1->matches(conf2) == false )
	{
		result = false;
	}
	if ( conf2->matches(conf2) == false )
	{
		result = false;
	}
	if ( conf2->matches(conf1) == false )
	{
		result = false;
	}

	conf1->addNonElement(0);
	conf2->addElement(0);

	if ( conf1->matches(conf2) )
	{
		result = false;
	}

	delete conf1;
	delete conf2;

	return result;
}

bool test_Equals()
{
	bool result = true;

	Configuration* conf1 = new Configuration(grid);
	Configuration* conf2 = new Configuration(grid);

	conf1->addVertexToShape(0);
	conf1->addVertexToShape(1);
	conf1->addVertexToShape(2);
	conf1->addVertexToShape(4);

	conf2->addVertexToShape(0);
	conf2->addVertexToShape(2);
	conf2->addVertexToShape(1);
	conf2->addVertexToShape(4);

	conf1->addElement(4);

	conf2->addElement(4);

	if ( conf1->equals(conf1) == false )
	{
		result = false;
	}
	if ( conf1->equals(conf2) == false )
	{
		result = false;
	}
	if ( conf2->equals(conf2) == false )
	{
		result = false;
	}
	if ( conf2->equals(conf1) == false )
	{
		result = false;
	}

	conf1->addNonElement(0);
	conf2->addElement(0);

	if ( conf1->equals(conf2) )
	{
		result = false;
	}
	if ( conf2->equals(conf1) )
	{
		result = false;
	}

	delete conf1;
	delete conf2;

	return result;
}
bool test_Duplicate()
{
	bool result = true;

	Configuration* conf1 = new Configuration(grid);

	conf1->addVertexToShape(0);
	conf1->addVertexToShape(1);
	conf1->addVertexToShape(2);
	conf1->addVertexToShape(4);

	conf1->addElement(4);

	Configuration* conf2 = conf1->duplicate();

	if ( conf1->equals(conf1) == false )
	{
		result = false;
	}
	if ( conf1->equals(conf2) == false )
	{
		result = false;
	}
	if ( conf2->equals(conf2) == false )
	{
		result = false;
	}
	if ( conf2->equals(conf1) == false )
	{
		result = false;
	}

	int* perm = (int*) malloc(1000 * sizeof(int));
	for ( int i = 0; i < 1000; i++ )
	{
		perm[i] = i;
	}

	perm[1] = 2;
	perm[2] = 1;

	delete conf2;
	conf2 = conf1->duplicate(perm, perm);
	free(perm);

	if ( conf1->equals(conf1) == false )
	{
		result = false;
	}
	if ( conf1->equals(conf2) == false )
	{
		result = false;
	}
	if ( conf2->equals(conf2) == false )
	{
		result = false;
	}
	if ( conf2->equals(conf1) == false )
	{
		result = false;
	}

	delete conf1;
	delete conf2;

	return result;
}

bool test_Colors()
{
	bool result = true;
	int conf_i = 0;

	Configuration* conf = new Configuration(grid);

	conf->addVertexToShape(0);
	conf->addVertexToShape(1);
	conf->addVertexToShape(2);
	conf->addVertexToShape(4);

	conf->computeSymmetry(false);

	if ( conf->getNumVertexColors() != 2 )
	{
		printf("testColors : conf %d - Num Colors  = %d != 2\n", conf_i, conf->getNumVertexColors());
		result = false;
	}

	if ( conf->getColorOfVertex(0) != 0 )
	{
		printf("testColors : conf %d - color of vertex %d  = %d != 0\n", conf_i, 0, conf->getColorOfVertex(0));
		result = false;
	}
	if ( conf->getColorOfVertex(1) != 1 )
	{
		printf("testColors : conf %d - color of vertex %d  = %d != 1\n", conf_i, 1, conf->getColorOfVertex(1));
		result = false;
	}
	if ( conf->getColorOfVertex(2) != 1 )
	{
		printf("testColors : conf %d - color of vertex %d  = %d != 1\n", conf_i, 2, conf->getColorOfVertex(2));
		result = false;
	}
	if ( conf->getColorOfVertex(4) != 1 )
	{
		printf("testColors : conf %d - color of vertex %d  = %d != 1\n", conf_i, 4, conf->getColorOfVertex(4));
		result = false;
	}
	if ( !result )
	{
		conf->print();
		delete conf;
		return false;
	}

	delete conf;

	conf_i++;
	conf = new Configuration(grid);

	conf->addVertexToShape(0);
	conf->addVertexToShape(1);
	conf->addVertexToShape(2);
	conf->addVertexToShape(4);
	conf->addElement(4);

	conf->computeSymmetry(false);

	if ( conf->getNumVertexColors() != 4 )
	{
		printf("testColors : conf %d - Num Colors  = %d != 4\n", conf_i, conf->getNumVertexColors());
		result = false;
	}

	if ( conf->getColorOfVertex(0) != 0 )
	{
		printf("testColors : conf %d - color of vertex %d  = %d != 0\n", conf_i, 0, conf->getColorOfVertex(0));
		result = false;
	}
	if ( conf->getColorOfVertex(1) != 1 )
	{
		printf("testColors : conf %d - color of vertex %d  = %d != 1\n", conf_i, 1, conf->getColorOfVertex(1));
		result = false;
	}
	if ( conf->getColorOfVertex(2) != 2 )
	{
		printf("testColors : conf %d - color of vertex %d  = %d != 2\n", conf_i, 2, conf->getColorOfVertex(2));
		result = false;
	}
	if ( conf->getColorOfVertex(4) != 3 )
	{
		printf("testColors : conf %d - color of vertex %d  = %d != 3\n", conf_i, 4, conf->getColorOfVertex(4));
		result = false;
	}
	if ( !result )
	{
		conf->print();
		delete conf;
		return false;
	}
	delete conf;

	conf_i++;
	conf = new Configuration(grid);

	conf->addVertexToShape(0);
	conf->addVertexToShape(1);
	conf->addVertexToShape(2);
	conf->addVertexToShape(4);
	conf->addElement(4);

	conf->computeSymmetry(true);

	if ( conf->getNumVertexColors() != 3 )
	{
		printf("testColors : conf %d - Num Colors  = %d != 3\n", conf_i, conf->getNumVertexColors());
		result = false;
	}

	if ( conf->getColorOfVertex(0) != 0 )
	{
		printf("testColors : conf %d - color of vertex %d  = %d != 0\n", conf_i, 0, conf->getColorOfVertex(0));
		result = false;
	}
	if ( conf->getColorOfVertex(1) != 1 )
	{
		printf("testColors : conf %d - color of vertex %d  = %d != 1\n", conf_i, 1, conf->getColorOfVertex(1));
		result = false;
	}
	if ( conf->getColorOfVertex(2) != 1 )
	{
		printf("testColors : conf %d - color of vertex %d  = %d != 1\n", conf_i, 2, conf->getColorOfVertex(2));
		result = false;
	}
	if ( conf->getColorOfVertex(4) != 2 )
	{
		printf("testColors : conf %d - color of vertex %d  = %d != 2\n", conf_i, 4, conf->getColorOfVertex(4));
		result = false;
	}
	if ( !result )
	{
		conf->print();
		delete conf;
		return false;
	}
	delete conf;

	conf_i++;
	conf = new Configuration(grid);

	conf->addVertexToShape(0);
	conf->addVertexToShape(1);
	conf->addVertexToShape(2);
	conf->addVertexToShape(4);
	conf->addElement(4);

	conf->setCenter(0, true);

	conf->computeSymmetry(false);

	if ( conf->getNumVertexColors() != 4 )
	{
		printf("testColors : conf %d - Num Colors  = %d != 3\n", conf_i, conf->getNumVertexColors());
		result = false;
	}

	if ( conf->getColorOfVertex(0) != 0 )
	{
		printf("testColors : conf %d - color of vertex %d  = %d != 0\n", conf_i, 0, conf->getColorOfVertex(0));
		result = false;
	}
	if ( conf->getColorOfVertex(1) != 1 )
	{
		printf("testColors : conf %d - color of vertex %d  = %d != 1\n", conf_i, 1, conf->getColorOfVertex(1));
		result = false;
	}
	if ( conf->getColorOfVertex(2) != 2 )
	{
		printf("testColors : conf %d - color of vertex %d  = %d != 2\n", conf_i, 2, conf->getColorOfVertex(2));
		result = false;
	}
	if ( conf->getColorOfVertex(4) != 3 )
	{
		printf("testColors : conf %d - color of vertex %d  = %d != 3\n", conf_i, 4, conf->getColorOfVertex(4));
		result = false;
	}
	if ( !result )
	{
		conf->print();
		delete conf;
		return false;
	}
	delete conf;

	conf_i++;
	conf = new Configuration(grid);

	conf->addVertexToShape(0);
	conf->addVertexToShape(1);
	conf->addVertexToShape(2);
	conf->addVertexToShape(4);
	conf->addElement(4);
	conf->setCenter(0, true);

	conf->computeSymmetry(true);

	if ( conf->getNumVertexColors() != 3 )
	{
		printf("testColors : conf %d - Num Colors  = %d != 3\n", conf_i, conf->getNumVertexColors());
		result = false;
	}

	if ( conf->getColorOfVertex(0) != 0 )
	{
		printf("testColors : conf %d - color of vertex %d  = %d != 0\n", conf_i, 0, conf->getColorOfVertex(0));
		result = false;
	}
	if ( conf->getColorOfVertex(1) != 1 )
	{
		printf("testColors : conf %d - color of vertex %d  = %d != 1\n", conf_i, 1, conf->getColorOfVertex(1));
		result = false;
	}
	if ( conf->getColorOfVertex(2) != 1 )
	{
		printf("testColors : conf %d - color of vertex %d  = %d != 1\n", conf_i, 2, conf->getColorOfVertex(2));
		result = false;
	}
	if ( conf->getColorOfVertex(4) != 2 )
	{
		printf("testColors : conf %d - color of vertex %d  = %d != 2\n", conf_i, 4, conf->getColorOfVertex(4));
		result = false;
	}
	if ( !result )
	{
		conf->print();
		delete conf;
		return false;
	}
	delete conf;

	return result;
}

bool test_Snapshots()
{
	bool result = true;

	Configuration* conf = new Configuration(grid);

	conf->addVertexToShape(0);
	conf->addVertexToShape(1);
	conf->addVertexToShape(2);
	conf->addVertexToShape(4);
	conf->addFaceToShape(0);
	conf->addFaceToShape(6);

	conf->snapshot();
	Configuration* conf2 = conf->duplicate();

	conf->addElement(0);
	conf->addNonElement(1);

	conf->snapshot();
	Configuration* conf3 = conf->duplicate();

	conf->addFaceToShape(2);

	conf->snapshot();

	Configuration* conf4 = conf->duplicate();

	conf->addElement(2);




	conf->rollback();

	if ( conf->equals(conf4, true) == false )
	{
		result = false;
		printf("After 1 rollback, conf != conf4\n");
	}
	conf->rollback();

	if ( conf->equals(conf3,true) == false )
	{
		result = false;
		printf("After 2 rollback, conf != conf3\n");
	}
	conf->rollback();

	if ( conf->equals(conf2,true) == false )
	{
		result = false;
		printf("After 3 rollback, conf != conf2\n");
	}

	delete conf;
	delete conf2;
	delete conf3;
	delete conf4;

	return result;
}


bool test_Snapshots2()
{
	bool result = true;
	Configuration* conf = new Configuration(grid);

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



bool test_WriteRead()
{
	bool result = true;
	Configuration* conf = new Configuration(grid);

	conf->addFaceToShape(0);
	conf->print();

    for (int i = 0; i < grid->getFaceDegree(0); i++) {
        conf->snapshot();

        if (i % 2) {
            conf->addNonElement(grid->neighborFV(0, i));
        } else {
            conf->addElement(grid->neighborFV(0, i));
        }
    }

    FILE* out = fopen("test_config.txt","w");
    conf->write(out);
    fclose(out);

    FILE* in = fopen("test_config.txt", "r");
    Configuration* conf2 = Configuration::read(grid, in);
    fclose(in);

    printf("Write/Read Test\n");
    conf->print();
    conf2->print();
    result = conf2->equals(conf, true);

    delete conf;
    delete conf2;

	return result;
}


