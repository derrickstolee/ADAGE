

#include "Grid.hpp"
#include "Configuration.hpp"
#include "SearchManager.hpp"
#include "fraction.hpp"
#include "GridAutomManager.hpp"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <queue>

using namespace adage::grids;

GridAutomManager::GridAutomManager(Grid* grid, Configuration* conf)
    : SearchManager()
{
    this->grid = grid;
    this->conf = conf->duplicate();
    this->base_conf = new Configuration(grid);
    this->shape_conf = 0;

    // TODO: Replace with better...
    this->best_density = fraction(6, 7);

    this->size_autom_list = 10;
    this->num_automs = 0;
    this->f_automs = (int**)malloc(this->size_autom_list * sizeof(int*));
    this->v_automs = (int**)malloc(this->size_autom_list * sizeof(int*));

    for (int i = 0; i < this->size_autom_list; i++) {
        this->f_automs[i] = 0;
        this->v_automs[i] = 0;
    }

    this->root = new SearchNode(0);
}

GridAutomManager::~GridAutomManager()
{
    delete this->conf;
    for (int i = 0; i < this->num_automs; i++) {
        free(this->f_automs[i]);
        free(this->v_automs[i]);
    }
    free(this->f_automs);
    free(this->v_automs);
}

void GridAutomManager::addAutomF2F(int f1, int f2, int g1, int g2)
{
    if (this->num_automs + 2 >= this->size_autom_list) {
        this->size_autom_list += 100;

        this->f_automs = (int**)realloc(this->f_automs, this->size_autom_list * sizeof(int*));
        this->v_automs = (int**)realloc(this->v_automs, this->size_autom_list * sizeof(int*));

        for (int i = this->num_automs; i < this->size_autom_list; i++) {
            this->f_automs[i] = 0;
            this->v_automs[i] = 0;
        }
    }

    this->grid->transformFF(this->f_automs[this->num_automs], this->v_automs[this->num_automs], f1, f2, g1, g2);
    (this->num_automs)++;
    this->grid->transformFF(this->f_automs[this->num_automs], this->v_automs[this->num_automs], g1, g2, f1, f2);
    (this->num_automs)++;
}

void GridAutomManager::addAutomV2V(int v1, int v2, int u1, int u2)
{
    if (this->num_automs + 2 >= this->size_autom_list) {
        this->size_autom_list += 100;

        this->f_automs = (int**)realloc(this->f_automs, this->size_autom_list * sizeof(int*));
        this->v_automs = (int**)realloc(this->v_automs, this->size_autom_list * sizeof(int*));

        for (int i = this->num_automs; i < this->size_autom_list; i++) {
            this->f_automs[i] = 0;
            this->v_automs[i] = 0;
        }
    }

    this->grid->transformVV(this->f_automs[this->num_automs], this->v_automs[this->num_automs], v1, v2, u1, u2);
    (this->num_automs)++;
    this->grid->transformVV(this->f_automs[this->num_automs], this->v_automs[this->num_automs], u1, u2, v1, v2);
    (this->num_automs)++;
}

void GridAutomManager::createShapeConf()
{
	for ( int i = 0; i < this->num_automs; i++)
	{
		for ( int v = 0; v < this->grid->getMaxVertexIndex(); v++ )
		{
			if ( this->v_automs[i][v] == v )
			{
				printf("We have a fixed point: %d\n", v);
				exit(0);
			}
		}
	}

    this->shape_conf = new Configuration(grid);
    Configuration* autom_conf = new Configuration(grid);
    Configuration* covered_conf = new Configuration(grid);

    int v = 0;

    this->shape_conf->addVertexToShape(v);
    autom_conf->addVertexToShape(v);

    bool updated = true;
    while (updated) {
        updated = false;
        for (int j = 0; j < autom_conf->getNumVerticesInShape(); j++) {

            int u = autom_conf->getVertexFromShape(j);
            if (autom_conf->isElement(u)) {
                continue;
            }

            autom_conf->addElement(u);
            for (int i = 0; i < this->num_automs; i++) {
                int z = this->v_automs[i][u];
                if (z >= 0 && autom_conf->isVertexInShape(z) == false) {
                    autom_conf->addVertexToShape(z);
                    updated = true;
                }
            }
        }
    }

    std::queue<int> q;

    q.push(v);

    while (q.size() > 0) {
        int u = q.front();
        q.pop();

        for (int i = 0; i < this->grid->getVertexDegree(u); i++) {
            int y = this->grid->neighborVV(u, i);

            if (covered_conf->isVertexInShape(y) == false) {
                q.push(y);
                covered_conf->addVertexToShape(y);

                if (autom_conf->isVertexInShape(y) == false) {

                    bool has_valid_automs = true;
                    for (int k = 0; k < this->num_automs; k++) {
                        if ( this->v_automs[k][y] < 0)
                        {
                        	has_valid_automs = false;
                        }
                    }

                    if ( has_valid_automs )
                    {
                    	this->shape_conf->addVertexToShape(y);
                    }

                    autom_conf->addVertexToShape(y);


                    updated = true;
                    while (updated) {
                        updated = false;
                        for (int j = 0; j < autom_conf->getNumVerticesInShape(); j++) {

                            int x = autom_conf->getVertexFromShape(j);
                            if (autom_conf->isElement(x)) {
                                continue;
                            }

                            autom_conf->addElement(x);
                            for (int k = 0; k < this->num_automs; k++) {
                                int z = this->v_automs[k][x];

                                if (z >= 0 && autom_conf->isVertexInShape(z) == false) {
                                    autom_conf->addVertexToShape(z);
                                    updated = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // this->shape_conf->print();
    delete covered_conf;

    int N = this->shape_conf->getNumVerticesInShape();
    printf("%d vertices in tile.\n", N);
    for ( int i = 0.4* N; i < 0.45*N; i++  )
    {
    	printf("%d / %d = %lf\n", i, N, double(i)/ double(N));
    }

    this->best_density = fraction( 4.0 * this->shape_conf->getNumVerticesInShape() / 7.0, this->shape_conf->getNumVerticesInShape() );

    delete autom_conf;

    fflush(stdout);
}

LONG_T GridAutomManager::pushNext()
{
    SearchNode* node = 0;

    if (this->stack.size() > 0) {
        node = this->stack.back();
    } else {
        node = this->root;
    }

    LONG_T next = node->curChild + 1;

    if (next > 1) {
        return -1;
    }

    return this->pushTo(next);
}

LONG_T GridAutomManager::GridAutomManager::pushTo(LONG_T child)
{
    SearchNode* node = 0;
    if (this->stack.size() > 0) {
        node = this->stack.back();
    } else {
        node = this->root;
    }

    int nextv = -1;
    for (int i = 0; i < this->shape_conf->getNumVerticesInShape(); i++) {
        int v = this->shape_conf->getVertexFromShape(i);

        if (!this->conf->isElement(v) && !this->conf->isNonElement(v)) {
            nextv = v;
            break;
        }
    }

    if (nextv < 0) {
        return -1;
    }


    node->curChild = child;

    this->stack.push_back(new SearchNode(child));

    this->conf->snapshot();
    this->base_conf->snapshot();

    LONG_T child_for_nonelement = 0;

    int num_elts = 0;
    int num_nonelts = 0;
    for ( int i = 0; i < this->grid->getVertexDegree(nextv); i++ )
    {
        int v = this->grid->neighborVV(nextv, i);

        if ( this->conf->isElement(v) )
        {
            num_elts++;
        }
        else if ( this->conf->isNonElement(v) )
        {
            num_nonelts++;
        }
    }

    if ( num_nonelts > num_elts + 1 )
    {
        child_for_nonelement = 1;
    }

    if (child == child_for_nonelement ) {
        this->conf->addNonElement(nextv);
    } else {
        this->conf->addElement(nextv);
    }

    return child;
}

int GridAutomManager::prune()
{
	if ( fraction(this->best_density.a - 1, this->shape_conf->getNumVerticesInShape()) < fraction(5,12) )
	{
		return 1;
	}

    bool updated = true;

    while (updated) {
        updated = false;

        for (int i = 0; i < this->conf->getNumVerticesInShape(); i++) {
            int v = this->conf->getVertexFromShape(i);

            if (this->conf->isElement(v) && !this->base_conf->isElement(v)) {
                updated = true;
                this->base_conf->addElement(v);
                // AND ADD ITS AUTOMORPHISMS!

                for (int j = 0; j < this->num_automs; j++) {
                    int u = this->v_automs[j][v];

                    if ( u >= 0 && this->conf->isElement(u) == false) {
                    	if ( this->conf->isNonElement(u) )
                    	{
                    		return 1;
                    	}

                        this->conf->addElement(u);
                    }
                }
            } else if (this->conf->isNonElement(v) && !this->base_conf->isNonElement(v)) {
                updated = true;
                this->base_conf->addNonElement(v);

                // AND ADD ITS AUTOMORPHISMS!
                for (int j = 0; j < this->num_automs; j++) {
                    int u = this->v_automs[j][v];

                    if ( u >= 0 && this->conf->isNonElement(u) == false) {
                    	if ( this->conf->isElement(u) )
                    	{
                    		return 1;
                    	}

                        this->conf->addNonElement(u);
                    }
                }
            }
        }
    }

    int num_elts = 0;
    int num_total = this->shape_conf->getNumVerticesInShape();

    for (int i = 0; i < this->shape_conf->getNumVerticesInShape(); i++) {
        int v = this->shape_conf->getVertexFromShape(i);

        if (this->conf->isElement(v)) {
            num_elts++;
        }
    }

    fraction current(num_elts, num_total);

    if ( current >= this->best_density )
    {
    	return 1;
    }

    return 0;
}

LONG_T GridAutomManager::pop()
{
    LONG_T label = 0;

    if (this->stack.size() > 0) {
        SearchNode* node = this->stack.back();
        this->stack.pop_back();

        label = node->label;
        delete node;
    } else {
        label = this->root->label;
    }

    this->conf->rollback();
    this->base_conf->rollback();

    return label;
}

void GridAutomManager::clear()
{
    this->conf->rollback();
    this->base_conf->rollback();
}

int GridAutomManager::isSolution()
{
    bool is_done = true;

    int num_elts = 0;
    int num_total = this->shape_conf->getNumVerticesInShape();

    for (int i = 0; is_done && i < this->shape_conf->getNumVerticesInShape(); i++) {
        int v = this->shape_conf->getVertexFromShape(i);

        if (!this->conf->isElement(v) && !this->conf->isNonElement(v)) {
            is_done = false;
        }

        if (this->conf->isElement(v)) {
            num_elts++;
        }
    }

    fraction current(num_elts, num_total);

    if (is_done && current < this->best_density) {
        this->shape_conf->snapshot();
        for ( int i = 0;i < this->shape_conf->getNumVerticesInShape(); i++ )
        {
        	int v = this->shape_conf->getVertexFromShape(i);

        	if ( this->conf->isElement(v) )
        	{
        		this->shape_conf->addElement(v);
        	}
        	else
        	{
        		this->shape_conf->addNonElement(v);
        	}
        }
        // this->shape_conf->print();
        this->shape_conf->rollback();
        
        Configuration* rotated = new Configuration(this->grid);

        for ( int i = 0; i < this->shape_conf->getNumVerticesInShape(); i++ )
        {
        	int v = this->shape_conf->getVertexFromShape(i);

        	if ( this->conf->isElement(v) )
        	{
        		rotated->addElement(v);

        		int u = v;

        		while ( this->v_automs[0][u] != v )
        		{
        			u = this->v_automs[0][u];

        			rotated->addElement(u);
        		}
        	}
        	else
        	{
        		rotated->addNonElement(v);
        		int u = v;

        		while ( this->v_automs[0][u] != v )
        		{
        			u = this->v_automs[0][u];

        			rotated->addNonElement(u);
        		}
        	}
        }
        rotated->print();


        delete rotated;

        // this->conf->print();
        this->best_density = current;

        current.simplify();
        printf("\nDensity : %lld / %llu == %lf\n", current.a, current.b, double(current.a) / double(current.b));
        

        return 1;
    }

    return 0;
}
