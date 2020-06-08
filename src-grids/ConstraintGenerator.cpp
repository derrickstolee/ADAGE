/*
 * ConstraintGenerator.cpp
 *
 *  Created on: Apr 17, 2014
 *      Author: stolee
 */

#include "RuleShape.hpp"
#include "LinearConstraint.hpp"
#include "LinearProgram.hpp"
#include "ConstraintGenerator.hpp"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <queue>

#include "macros.hpp"

using namespace adage::grids;
using namespace adage;

void ConstraintGenerator::initConfigurations()
{
    this->blank_conf = new Configuration(this->grid);
    this->full_conf = new Configuration(this->grid);

    this->num_constraint_bases = this->grid->getNumVertexOrbits() + this->grid->getNumFacialOrbits();
    this->constraint_bases = (Configuration**)malloc(this->num_constraint_bases*sizeof(Configuration*));

    for ( int i = 0; i < this->grid->getNumVertexOrbits(); i++ )
    {
        this->constraint_bases[i] = new Configuration(this->grid);
        this->constraint_bases[i]->addVertexToShape(this->grid->getVertexOrbitRepresentative(i));
        this->constraint_bases[i]->setCenter(this->grid->getVertexOrbitRepresentative(i), true);
    }

    for ( int i = 0; i < this->grid->getNumFacialOrbits(); i++ )
    {
        this->constraint_bases[ this->grid->getNumVertexOrbits() + i ] = new Configuration(this->grid);
        this->constraint_bases[ this->grid->getNumVertexOrbits() + i ]->addFaceToShape(this->grid->getFacialOrbitRepresentative(i));
        this->constraint_bases[ this->grid->getNumVertexOrbits() + i ]->setCenter(this->grid->getFacialOrbitRepresentative(i), false);
    }
}

void ConstraintGenerator::freeConfigurations()
{
    if (this->blank_conf != 0) {
        delete this->blank_conf;
        this->blank_conf = 0;
    }

    if (this->full_conf != 0) {
        delete this->full_conf;
        this->full_conf = 0;
    }

    DELETE_AND_FREE_ARRAY(this->constraint_bases, this->num_constraint_bases);
}

void ConstraintGenerator::initVariations()
{
    this->size_variations = this->size_shapes * 10;
    this->num_variations = 0;

    this->shape_variations = (RuleShape**)malloc(this->size_variations * sizeof(RuleShape*));
}

void ConstraintGenerator::freeVariations()
{
    DELETE_AND_FREE_ARRAY(this->shape_variations, this->num_variations);
    this->num_variations = 0;
    this->size_variations = 0;
}


void ConstraintGenerator::initRuleShapes()
{
	this->num_shapes = 0;
	this->size_shapes = 10;

	this->rule_shapes = (RuleShape**)malloc(this->size_shapes * sizeof(RuleShape*));
}

void ConstraintGenerator::freeRuleShapes()
{
    DELETE_AND_FREE_ARRAY(this->rule_shapes, this->num_shapes);
	this->num_shapes = 0;
	this->size_shapes = 0;
}


void ConstraintGenerator::addRuleShape(RuleShape* rule_shape)
{
    if (this->num_shapes >= this->size_shapes) {
        this->size_shapes = this->size_shapes + 20;
        this->rule_shapes = (RuleShape**)realloc(this->rule_shapes, this->size_shapes * sizeof(RuleShape*));

        for (int i = this->num_shapes; i < this->size_shapes; i++) {
            this->rule_shapes[i] = 0;
        }
    }

    this->rule_shapes[this->num_shapes] = rule_shape;
    (this->num_shapes)++;
}

/**
 * Given the current center face orbit of conf/blank_conf/full_conf, determine the list of variations.
 */
void ConstraintGenerator::determineVariations()
{
    Configuration* cur_base = this->constraint_bases[this->cur_constraint_base];

    this->center_is_face = false;
    for ( int ii = 0; ii < cur_base->getNumVerticesInShape(); ii++ )
    {
        this->center_id = cur_base->getVertexFromShape(ii);

        for ( int i = 0; i < this->num_shapes; i++ )
        {
            Configuration* c = this->rule_shapes[i]->getShape();

            // center is a vertex!
            if ( this->rule_shapes[i]->getType() == VARIABLE_TYPE_F2V )
            {
                // ok... center vertex only!
                for ( int j = 0; j < this->grid->getVertexDegree(this->center_id); j++ )
                {
                    int u1 = this->center_id;
                    int g2 = this->grid->neighborVF(this->center_id, j);

                    int v1 = c->getCenter();
                    int f2 = this->grid->neighborVF(v1, 0);

                    if ( this->grid->transformVF(this->fperm, this->vperm, v1, f2, u1, g2) )
                    {
                        bool center_is_puller = true;
                        this->addVariation(new RuleShape(this->rule_shapes[i], this->fperm, this->vperm, this->center_id,  center_is_puller));
                    }
                }
            }
            else if ( this->rule_shapes[i]->getType() == VARIABLE_TYPE_V2V )
            {
                for ( int j = 0; j < this->grid->getVertexDegree(this->center_id); j++ )
                {
                    int u1 = this->center_id;
                    int u2 = this->grid->neighborVV(this->center_id, j);

                    // center vertex options:
                    int v1 = c->getCenter();
                    int v2 = this->grid->neighborVV(v1,0);

                    if ( this->grid->transformVV(this->fperm, this->vperm, v1, v2, u1, u2) )
                    {
                        bool center_is_puller = false;
                        this->addVariation(new RuleShape(this->rule_shapes[i], this->fperm, this->vperm,  this->center_id, center_is_puller));
                    }
                }

                {
                    int u1 = this->center_id;
                    int u2 = this->grid->neighborVV(this->center_id, 0);

                    // pullable options:
                    for ( int k = 0; k < this->rule_shapes[i]->getNumChargeable(); k++ )
                    {
                        int v1 = this->rule_shapes[i]->getChargeable(k);

                        // NOT just any neighbor will do!
                        for ( int l = 0; l < this->grid->getVertexDegree(v1); l++ )
                        {
                            int v2 = this->grid->neighborVV(v1, l);

                            if ( this->grid->transformVV(this->fperm, this->vperm, v1, v2, u1, u2) )
                            {
                                bool center_is_puller = false;
                                this->addVariation(new RuleShape(this->rule_shapes[i], this->fperm, this->vperm, this->center_id,  center_is_puller));
                            }
                        }
                    }
                }
            }
            else if ( this->rule_shapes[i]->getType() == VARIABLE_TYPE_V2F )
            {
                int u1 = this->center_id;
		this->grid->neighborVF(this->center_id, 0);

                for (int j = 0; j < this->rule_shapes[i]->getNumChargeable(); j++ )
                {
                    // for every way to pull...
                    int v1 = this->rule_shapes[i]->getChargeable(j);
                    int f2 = this->grid->neighborVF(v1, 0);

                    for ( int k = 0; k < this->grid->getVertexDegree(u1); k++ )
                    {
                        // lay this down in every direction!
                        int g2b = this->grid->neighborVF(u1,k);

                        if ( this->grid->transformVF(this->fperm, this->vperm, v1, f2, u1, g2b) )
                        {
                            bool center_is_puller = false;
                            this->addVariation(new RuleShape(this->rule_shapes[i], this->fperm, this->vperm, this->center_id,  center_is_puller));
                        }
                    }
                }
            }
        }
    }

    if ( cur_base->getNumFacesInShape() > 0 )
    {
        this->center_is_face = true;
        for ( int ii = 0; ii < cur_base->getNumFacesInShape(); ii++ )
        {
            this->center_id = cur_base->getFaceFromShape(ii);

            for ( int i = 0; i < this->num_shapes; i++ )
            {
                Configuration* c = this->rule_shapes[i]->getShape();

                if ( this->rule_shapes[i]->getType() == VARIABLE_TYPE_F2F )
                {
                    // both the centered versions AND the other faces will work!
                    for ( int j = 0; j < this->grid->getFaceDegree(this->center_id); j++ )
                    {
                        int g1 = this->center_id;
                        int g2 = this->grid->neighborFF(this->center_id, j);

                        int f1 = c->getCenter();
                        int f2 = this->grid->neighborFF(f1, 0);

                        // center-to-center
                        if ( this->grid->transformFF(this->fperm, this->vperm, f1, f2, g1, g2) )
                        {
                            bool center_is_puller = true;
                            this->addVariation(new RuleShape(this->rule_shapes[i], this->fperm, this->vperm, this->center_id, center_is_puller));

                            for ( int k = 0; k < this->rule_shapes[i]->getNumChargeable(); k++ )
                            {
                                int f1 = this->rule_shapes[i]->getChargeable(k);
                                int f2 = this->grid->neighborFF(f1, 0);

                                // chargeable-to-center
                                this->grid->transformFF(this->fperm, this->vperm, f1, f2, g1, g2);
                                bool center_is_puller = false;
                                this->addVariation(new RuleShape(this->rule_shapes[i], this->fperm, this->vperm, this->center_id, center_is_puller));
                            }
                        }
                    }
                }
                else if ( this->rule_shapes[i]->getType() == VARIABLE_TYPE_F2V )
                {
                    for ( int j = 0; j < this->grid->getFaceDegree(this->center_id); j++ )
                    {
                        int g1 = this->center_id;
                        int u2 = this->grid->neighborFV(this->center_id, j);

                        for ( int kk = 0; kk < this->rule_shapes[i]->getNumChargeable(); kk++ )
                        {
                            int f1 = this->rule_shapes[i]->getChargeable(kk);
                            int v2 = this->grid->neighborFV(f1,0);

                            if ( this->grid->transformFV(this->fperm, this->vperm, f1, v2, g1, u2) )
                            {
                                bool center_is_puller = false;
                                this->addVariation(new RuleShape(this->rule_shapes[i], this->fperm, this->vperm,  this->center_id, center_is_puller));
                            }
                        }
                    }
                }
                else if ( this->rule_shapes[i]->getType() == VARIABLE_TYPE_V2F )
                {
                    for ( int j = 0; j < this->grid->getFaceDegree(this->center_id); j++ )
                    {
                        int g1 = this->center_id;
                        int u2 = this->grid->neighborFV(this->center_id, j);

                        int f1 = c->getCenter();
                        int v2 = this->grid->neighborFV(f1, 0);

                        if ( this->grid->transformFV(this->fperm, this->vperm, f1, v2, g1, u2) )
                        {
                            bool center_is_puller = true;
                            this->addVariation(new RuleShape(this->rule_shapes[i], this->fperm, this->vperm,  this->center_id, center_is_puller));
                        }
                    }
                }
                else
                {
                    // printf("...shape %d is F2V and we are centered on a vertex (%d)!\n", i, this->center_id);
                    // this->rule_shapes[i]->print();
                    // printf("Warning! Variable type %d not implemented!\n", this->rule_shapes[i]->getType());
                    // this->rule_shapes[i]->print();
                }
            }
        }
    }

    //  this->full_conf->print();

    // for ( int i = 0; i < this->num_variations; i++ )
    // {
    //     this->shape_variations[i]->write(stdout);
    // }
}

void ConstraintGenerator::addVariation(RuleShape* variation)
{
    if (this->num_variations >= this->size_variations) {
        this->size_variations = this->size_variations + 500;
        this->shape_variations = (RuleShape**)realloc(this->shape_variations, this->size_variations * sizeof(RuleShape*));
    }


    // test for being there already!
    for ( int i = 0; i < this->num_variations; i++ )
    {
        if ( this->shape_variations[i]->getType() == variation->getType() &&
            this->shape_variations[i]->getShape()->equals(variation->getShape()) &&
             this->shape_variations[i]->getShape()->getCenter() == variation->getShape()->getCenter()  )
        {
            // check that the chargeables are the same!
            bool same = true;
            for ( int j = 0; j < variation->getNumChargeable(); j++ )
            {
                int x = variation->getChargeable(j);

                bool has_chargeable = false;
                for ( int k = 0; k < this->shape_variations[i]->getNumChargeable(); k++ )
                {
                    if ( x == this->shape_variations[i]->getChargeable(k) )
                    {
                        has_chargeable = true;
                    }
                }

                if ( !has_chargeable )
                {
                    same = false;
                }
            }

            if ( same )
            {
                delete variation;
                return;
            }
        }
    }

    this->shape_variations[(this->num_variations)++] = variation;

    for ( int i = 0;i  < variation->getShape()->getNumVerticesInShape(); i++ )
    {
        int v = variation->getShape()->getVertexFromShape(i);

        if ( this->full_conf->isVertexInShape(v) == false )
        {
            this->full_conf->addVertexToShape(v);
        }
    }

    for ( int i = 0;i  < variation->getShape()->getNumFacesInShape(); i++ )
    {
        int f = variation->getShape()->getFaceFromShape(i);

        if ( this->full_conf->isFaceInShape(f) == false )
        {
            this->full_conf->addFaceToShape(f);
        }

        if ( variation->getShape()->getFaceSize(f) >= 0 )
        {
            // this face has a size!
            // so we must fill it!
            for ( int j = 0; j < this->grid->getFaceDegree(f); j++ )
            {
                int v = this->grid->neighborFV(f,j);

                if ( this->full_conf->isVertexInShape(v) == false )
                {
                    this->full_conf->addVertexToShape(v);
                }
            }
        }
    }
}

/**
 * add the current constraint, as we have exactly determined every rule!
 *
 */
bool ConstraintGenerator::addConstraint()
{
    int w_coeff = 0;
    int rhs = 0;
    int inequality_mode = INEQUALITY_GEQ;

    for ( int i = 0; i < this->constraint_bases[this->cur_constraint_base]->getNumVerticesInShape(); i++ )
    {
        int v = this->constraint_bases[this->cur_constraint_base]->getVertexFromShape(i);

        // need one more w for every vertex (but it is on the left)
        w_coeff--;

        if ( this->conf->isElement(v) )
        {
            // initial charge goes up for elements (but it is on the right)
            rhs--;
        }
    }

    LinearConstraint* constraint = new LinearConstraint(w_coeff, rhs, inequality_mode);

    for ( int i = 0; i < this->num_variations; i++ )
    {
        this->shape_variations[i]->modifyCoefficients(this->conf, constraint);
    }

    this->lp->addConstraint(constraint);
    constraint = 0;

    if( this->max_num_constraints > 0 && this->lp->getNumConstraints() > this->max_num_constraints )
    {
        printf("Quitting due to too many constraints!\n");
        exit(0);
    }

    return true;
}

/**
 * Given the current configuration, select the best vertex that remains undetermined and
 * get its index. Will be used to select as element or as nonelement.
 */
int ConstraintGenerator::getNextUndeterminedVertex()
{
    // Selects to fill a rule first!
	for ( int i = 0; i < this->num_variations; i++ )
	{
        if ( this->shape_variations[i]->isComplete(this->conf) == false )
        {
            int best_v = -1;
            int best_num_kernels = -1;
            for ( int j = 0; j < this->shape_variations[i]->getShape()->getNumVerticesInShape(); j++ )
            {
        		int v = this->shape_variations[i]->getShape()->getVertexFromShape(j);

        		if ( this->conf->isElement(v) == false && this->conf->isNonElement(v) == false )
        		{
                    int num_kernels = this->shape_variations[i]->getKernelCount(v);

                    if (num_kernels > best_num_kernels )
                    {
                        best_num_kernels = num_kernels;
                        best_v = v;
                    }
        		}
            }

            if ( best_v >= 0 )
            {
                return best_v;
            }
        }
	}

    return -1;
}

ConstraintGenerator::ConstraintGenerator(Grid* grid, Configuration* conf, LinearProgram* lp) : SearchManager()
{
    this->grid = grid;
    this->conf = conf->duplicate();

    this->blank_conf = 0;
    this->full_conf = 0;

    this->fperm = 0;
    this->vperm = 0;

    this->size_shapes = 0;
    this->num_shapes = 0;
    this->rule_shapes = 0;

    this->size_variations = 0;
    this->num_variations = 0;
    this->shape_variations = 0;

    this->center_is_face = false;
    this->center_id = -1;

    this->max_num_constraints = -1;

    this->vertices_distribute_evenly = false;

    this->root = new SearchNode(0);

    this->lp = lp;

    this->initConfigurations();
    this->initRuleShapes();
    this->initVariations();
}

ConstraintGenerator::~ConstraintGenerator()
{
    this->freeRuleShapes();
    this->freeVariations();
    this->freeConfigurations();
    delete this->conf;

    FREE_ARRAY(this->fperm);
    FREE_ARRAY(this->vperm);

    this->lp = 0;
}

void ConstraintGenerator::clear()
{
    while ( this->stack.size() > 0 )
    {
    	this->rollback();

    	delete this->stack.back();
    	this->stack.pop_back();
    }
    this->root->curChild = -1;

    this->rollback();
}

void ConstraintGenerator::snapshot()
{
    this->conf->snapshot();
    this->blank_conf->snapshot();
}

void ConstraintGenerator::rollback()
{
    this->blank_conf->rollback();
    this->conf->rollback();
}

/**
 * pushNext -- deepen the search to the next child
 * 	of the current node.
 *
 * @return the label for the new node. -1 if none.
 */
LONG_T ConstraintGenerator::pushNext()
{
    SearchNode* parent = 0;
    LONG_T next_child = 0;

    if (this->stack.size() == 0) {
        parent = this->root;
        next_child = parent->curChild + 1;
    } else {
        parent = this->stack.back();
        next_child = parent->curChild + 1;
    }

    if (this->stack.size() == 0) {
        // we have not decided on the constraint base!
        if ( next_child >= this->num_constraint_bases ) {
            return -1;
        }
    } else if ( this->getNextUndeterminedVertex() < 0) {
        return -1;
    } else if ( next_child >= 2 )
    {
        // we have tried element and non-element!
        return -1;
    }

    return this->pushTo(next_child);
}

/**
 * pushTo -- deepen the search to the specified child
 * 	of the current node.
 *
 * @param child the specified label for the new node
 * @return the label for the new node. -1 if none, or failed.
 */
LONG_T ConstraintGenerator::pushTo(LONG_T child)
{
    SearchNode* parent = 0;

    if (this->stack.size() == 0) {
        parent = this->root;
    } else {
        parent = this->stack.back();
    }

    parent->curChild = child;

    this->snapshot();

    if (this->stack.size() == 0) {
        this->cur_constraint_base = child;

        this->determineVariations();
    } else {
        int v = this->getNextUndeterminedVertex();

        if (child == 0) {
            this->conf->addElement(v);
        } else {
            this->conf->addNonElement(v);
        }
    }

    SearchNode* cnode = new SearchNode(child);

    this->stack.push_back(cnode);

    return child;
}

/**
 * pop -- remove the current node and move up the tree.
 *
 * @return the label of the node after the pop.
 * 		This return value is used for validation purposes
 * 		to check proper implementation of push*() and pop().
 */
LONG_T ConstraintGenerator::pop()
{
    SearchNode* node = 0;
    LONG_T label = 0;

    this->rollback();

    if (this->stack.size() == 0) {
        node = this->root;
        label = node->label;
    } else {
        node = this->stack.back();
        this->stack.pop_back();

        label = node->label;
        delete node;

        if ( this->stack.size() == 0 )
        {
            this->freeVariations();
            this->initVariations();
        }
    }

    return label;
}

/**
 * prune -- Perform a check to see if this node should be pruned.
 *
 * @return 0 if no prune should occur, 1 if prune should occur.
 */
int ConstraintGenerator::prune()
{
    if (this->conf->propagate() == false ) {
        // printf("Warning: The following configuration gave a false return to propagate(): \n");
        // this->conf->print();
        return 1;
    }

    return 0;
}

/**
 * isSolution -- Perform a check to see if a solution exists
 * 		at this point.
 *
 * @return 0 if no solution is found, 1 if a solution is found.
 */
int ConstraintGenerator::isSolution()
{
    if ( this->stack.size() > 2 && this->getNextUndeterminedVertex() < 0) {
        // this->conf->print();
        this->addConstraint();
    }

    return 0;
}

void ConstraintGenerator::importArguments(int argc, char** argv)
{
    SearchManager::importArguments(argc, argv);

    bool found_mode = false;

    for ( int i = 0; i < argc; i++ )
    {
        if ( strcmp(argv[i],"run") == 0 || strcmp(argv[i],"generate") == 0 )
        {
            found_mode = true;
        }
        if ( strcmp(argv[i],"--maxconstraints") == 0 )
        {
            this->max_num_constraints = atoi(argv[i+1]);
        }
        if ( strcmp(argv[i], "--rules") == 0 )
        {
            // load rule shapes!
            FILE* file = fopen(argv[i+1], "r");

            char* buffer = 0;
            size_t buflen = 0;

            while ( getline( &buffer, &buflen, file ) > 0 )
            {
                if ( strcmp(buffer, "#begin RuleShape\n") == 0)
                {
                    RuleShape* shape = RuleShape::read(this->grid, file);

                    // Add to the list only if "--name" is in the arguments!
                    bool found = false;
                    for ( int j = 0; j < argc; j++ )
                    {
                        if ( strstr( shape->getRuleName(), argv[j]+2) == shape->getRuleName() && strlen(shape->getRuleName()) <= strlen(argv[j]+2) + 1 )
                        {
                            found = true;
                            this->addRuleShape(shape);

                            // shape->write(stdout);
                        }
                    }

                    if ( !found )
                    {
                        delete shape;
                    }
                }
            }

            fclose(file);
            if ( buflen > 0 )
            {
                free(buffer);
            }
        }
        else if ( strcmp(argv[i], "--bases") == 0 )
        {
            // load rule shapes!
            FILE* file = fopen(argv[i+1], "r");

            // clear old bases
            for ( int i = 0; i < this->num_constraint_bases; i++ )
            {
                delete this->constraint_bases[i];
                this->constraint_bases[i] = 0;
            }

            this->num_constraint_bases = 0;
            this->constraint_bases = (Configuration**)realloc(this->constraint_bases, 50*sizeof(Configuration*));

            char* buffer = 0;
            size_t buflen = 0;

            while ( getline( &buffer, &buflen, file ) > 0 )
            {
                if ( strcmp(buffer, "#begin Configuration\n") == 0)
                {
                    this->constraint_bases[this->num_constraint_bases] = Configuration::read(this->grid, file);
                    (this->num_constraint_bases)++;
                }
            }

            fclose(file);
            if ( buflen > 0 )
            {
                free(buffer);
            }
        }
    }

    if ( !found_mode )
    {
        this->maxDepth = 1000;
        this->killtime = 60 * 60 * 24 * 30; // month!
        this->deepeningMode = DEEPEN_FULL;
    }
}
