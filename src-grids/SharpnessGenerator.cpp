/*
 * SharpnessGenerator.cpp
 *
 *  Created on: Apr 17, 2014
 *      Author: stolee
 */

#include "SharpnessGenerator.hpp"
#include "RuleShape.hpp"
#include "LinearConstraint.hpp"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <queue>

#include "macros.hpp"

using namespace adage;
using namespace adage::grids;

void SharpnessGenerator::initConfigurations()
{
    this->blank_conf = new Configuration(this->grid);
    this->full_conf = new Configuration(this->grid);

    this->num_constraint_bases = this->grid->getNumVertexOrbits() + this->grid->getNumFacialOrbits();
    this->constraint_bases = (Configuration**)malloc(this->num_constraint_bases*sizeof(Configuration*));

    for ( int i = 0; i < this->grid->getNumVertexOrbits(); i++ )
    {
        this->constraint_bases[i] = new Configuration(this->grid);
        this->constraint_bases[i]->addVertexToShape(this->grid->getVertexOrbitRepresentative(i));
    }

    for ( int i = 0; i < this->grid->getNumFacialOrbits(); i++ )
    {
        this->constraint_bases[ this->grid->getNumVertexOrbits() + i] = new Configuration(this->grid);
        this->constraint_bases[ this->grid->getNumVertexOrbits() + i]->addFaceToShape(this->grid->getFacialOrbitRepresentative(i));
    }
}

void SharpnessGenerator::freeConfigurations()
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

void SharpnessGenerator::initVariations()
{
    this->size_variations = this->size_shapes * 10;
    this->num_variations = 0;

    this->shape_variations = (RuleShape**)malloc(this->size_variations * sizeof(RuleShape*));
}

void SharpnessGenerator::freeVariations()
{
    DELETE_AND_FREE_ARRAY(this->shape_variations, this->num_variations);
    this->num_variations = 0;
    this->size_variations = 0;
}


void SharpnessGenerator::initRuleShapes()
{
	this->num_shapes = 0;
	this->size_shapes = 10;

	this->rule_shapes = (RuleShape**)malloc(this->size_shapes * sizeof(RuleShape*));
}

void SharpnessGenerator::freeRuleShapes()
{
    DELETE_AND_FREE_ARRAY(this->rule_shapes, this->num_shapes);
	this->num_shapes = 0;
	this->size_shapes = 0;
}


void SharpnessGenerator::addRuleShape(RuleShape* rule_shape)
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
void SharpnessGenerator::determineVariations()
{
    this->cur_base = this->constraint_bases[this->cur_constraint_base];

    this->center_is_face = false;
    for ( int i = 0; i < cur_base->getNumVerticesInShape(); i++ )
    {
        this->center_id = cur_base->getVertexFromShape(i);
            
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
                int g2 = this->grid->neighborVF(this->center_id, 0);
                
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

    this->center_is_face = true;
    for ( int i = 0; i < cur_base->getNumFacesInShape(); i++ )
    {
        this->center_id = cur_base->getFaceFromShape(i);
        if ( this->full_conf->isFaceInShape(this->center_id) == false )
        {
            this->full_conf->addFaceToShape(this->center_id);
        }

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

    // this->full_conf->print();

    for ( int i = 0; i < this->full_conf->getNumFacesInShape(); i++ )
    {
        int f = this->full_conf->getFaceFromShape(i);

        this->conf->addFaceToShape(f);
    }

    // for ( int i = 0; i < this->num_variations; i++ )
    // {
    //     this->shape_variations[i]->getShape()->print();
    // }
}

void SharpnessGenerator::addVariation(RuleShape* variation)
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

    for ( int i = 0; i  < variation->getShape()->getNumVerticesInShape(); i++ )
    {
        int v = variation->getShape()->getVertexFromShape(i); 

        if ( this->full_conf->isVertexInShape(v) == false )
        {
            this->full_conf->addVertexToShape(v);
        }
    }

    for ( int i = 0; i < variation->getShape()->getNumFacesInShape(); i++ )
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
 * write the current constraint, as we have exactly determined every rule!
 * 
 */
bool SharpnessGenerator::writeConfiguration()
{
    this->conf->print();

    return true;
}

/**
 * Given the current configuration, select the best vertex that remains undetermined and
 * get its index. Will be used to select as element or as nonelement.
 */
int SharpnessGenerator::getNextUndeterminedVertex()
{
    // Selects to fill a rule first!
	for ( int i = 0; i < this->num_variations; i++ )
	{
        for ( int j = 0; j < this->shape_variations[i]->getShape()->getNumVerticesInShape(); j++ )
        {
    		int v = this->shape_variations[i]->getShape()->getVertexFromShape(j);

    		if ( this->conf->isElement(v) == false && this->conf->isNonElement(v) == false )
    		{
    			return v;
    		}
        }
	}

    return -1;
}

SharpnessGenerator::SharpnessGenerator(Grid* grid, Configuration* conf, LinearProgram* lp) : SearchManager()
{
    this->grid = grid;
    this->lp = lp;
    w = lp->getExactOptimum(); // will be used! (unless specified manually)

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

    this->vertices_distribute_evenly = false;

    this->root = new SearchNode(0);

    this->initConfigurations();
    this->initRuleShapes();
    this->initVariations();
    this->initGenerators();
}

SharpnessGenerator::~SharpnessGenerator()
{
    this->freeGenerators();
    this->freeRuleShapes();
    this->freeVariations();
    this->freeConfigurations();

    FREE_ARRAY(this->fperm);
    FREE_ARRAY(this->vperm);
}


void SharpnessGenerator::initGenerators()
{
    int ball_size = 0;
    int* ball = this->grid->getVertexBall(0, 10, ball_size);

    this->num_generators = 0;
    this->size_generators = 1000;
    this->generators = (int*)malloc(this->size_generators*2*sizeof(int));

    int start_v = 0;
    int start_v_n = this->grid->neighborVV(start_v,0);

    for ( int i = 0; i < ball_size; i++ )
    {
        int to_v = ball[i];

        for ( int j = 0; j < this->grid->getVertexDegree(to_v); j++ )
        {
            int to_v_n = this->grid->neighborVV(to_v,j);

            if ( to_v_n > 0 && ( to_v != start_v || to_v_n != start_v_n ) )
            {
                // add to generators!
                if ( this->num_generators >= this->size_generators )
                {
                    this->size_generators += 1000;

                    this->generators = (int*)realloc(this->generators, this->size_generators*2*sizeof(int));
                }

                this->generators[2*this->num_generators + 0] = to_v;
                this->generators[2*this->num_generators + 1] = to_v_n;
                (this->num_generators)++;
            }
        }
    }

    this->from_v = start_v;
    this->from_v_n = start_v_n;

    free(ball);
}

void SharpnessGenerator::freeGenerators()
{
    FREE_ARRAY(this->generators);
}

void SharpnessGenerator::clear()
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

void SharpnessGenerator::snapshot()
{
    this->conf->snapshot();
    this->blank_conf->snapshot();
}

void SharpnessGenerator::rollback()
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
LONG_T SharpnessGenerator::pushNext()
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
LONG_T SharpnessGenerator::pushTo(LONG_T child)
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

        if (child == 1) {
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
LONG_T SharpnessGenerator::pop()
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
int SharpnessGenerator::prune()
{
    if (this->conf->propagate() == false ) {
        // printf("Warning: The following configuration gave a false return to propagate(): \n");
        // this->conf->print();
        return 1;
    }   

    // Ok, now let's check to see if all of our rules are done at a given element!
    for ( int i = 0; i < this->cur_base->getNumVerticesInShape(); i++ )
    {
        int v = this->cur_base->getVertexFromShape(i);
        bool is_complete = true;

        fraction vval = fraction(0,1);

        if ( this->conf->isElement(v) )
        {
            // starts with 1
            vval = fraction(1,1);
        }

        int variable_len = 100;
        char* variable_name = (char*)malloc(variable_len);
        // Now, determine: 
        // 1. Are all discharging rules determined?
        for ( int j = 0; is_complete && j < this->num_variations; j++ )
        {
            RuleShape* variation = this->shape_variations[j];

            bool v_involved = false;

            if ( variation->getCenter() == v && variation->isCenterFace() == false )
            {
                v_involved = true;

                if ( variation->isComplete( this->conf ) == false )
                {
                    is_complete = false;
                }
                else
                {
                    for ( int k = 0; k < variation->getNumChargeable(); k++ )
                    {
                        // Get the variable id for this rule!
                        variation->getVariableName(this->conf, k, variable_name, variable_len);
                        fraction f = this->lp->getValue(variable_name);

                        // pulling!
                        vval = vval + f;
                    }
                }
            }
            else if ( variation->getType() == VARIABLE_TYPE_V2V || variation->getType() == VARIABLE_TYPE_V2F )
            {
                for ( int k = 0; is_complete && !v_involved && k < variation->getNumChargeable(); k++ )
                {
                    if ( variation->getChargeable(k) == v )
                    {
                        v_involved = true;
                        if ( variation->isComplete(this->conf ) == false )
                        {
                            is_complete = false;
                        }
                        else
                        {
                            // Get the variable id for this rule!
                            variation->getVariableName(this->conf, k, variable_name, variable_len);
                            fraction f = this->lp->getValue(variable_name);

                            // pushing!
                            vval = vval - f;
                        }
                    }
                }
            }
        }

        // 2. Is the value too large? (Bigger than w?)
        if ( is_complete && vval > this->w + fraction(1,1000) )
        {
            // this face has extra charge!
        //     printf("pruning since charge at vertex %2d is %lld / %lld == %lf > %lf == %lld / %lld!\n", v, vval.a, vval.b, (double)vval.a/(double)vval.b, (double)w.a/(double)w.b, w.a, w.b  );
        //     this->writeConfiguration();


        // fraction tvval = fraction(0,1);

        // if ( this->conf->isElement(v) )
        // {
        //     // starts with 1
        //     tvval = fraction(1,1);
        // }

        // printf("Value : %d ", tvval.a);

        // for ( int j = 0; is_complete && j < this->num_variations; j++ )
        // {
        //     RuleShape* variation = this->shape_variations[j];

        //     bool v_involved = false;

        //     if ( variation->getCenter() == v && variation->isCenterFace() == false )
        //     {
        //         v_involved = true;

        //         if ( variation->isComplete( this->conf ) == false )
        //         {
        //             is_complete = false;
        //         }
        //         else
        //         {
        //             for ( int k = 0; k < variation->getNumChargeable(); k++ )
        //             {
        //                 // Get the variable id for this rule!
        //                 variation->getVariableName(this->conf, k, variable_name, variable_len);
        //                 fraction f = this->lp->getValue(variable_name);

        //                 // pulling!
        //                 printf(" + %d/%d [%s] ", f.a, f.b, variable_name);
        //                 tvval = tvval + f;
        //             }
        //         }
        //     }
        //     else if ( variation->getType() == VARIABLE_TYPE_V2V || variation->getType() == VARIABLE_TYPE_V2F )
        //     {
        //         for ( int k = 0; is_complete && !v_involved && k < variation->getNumChargeable(); k++ )
        //         {
        //             if ( variation->getChargeable(k) == v )
        //             {
        //                 v_involved = true;
        //                 if ( variation->isComplete(this->conf ) == false )
        //                 {
        //                     is_complete = false;
        //                 }
        //                 else
        //                 {
        //                     // Get the variable id for this rule!
        //                     variation->getVariableName(this->conf, k, variable_name, variable_len);
        //                     fraction f = this->lp->getValue(variable_name);

        //                     // pushing!
        //                     printf(" - %d/%d [%s] ", f.a, f.b, variable_name);
        //                     tvval = tvval + f;
        //                     tvval = tvval - f;
        //                 }
        //             }
        //         }
        //     }
        // }
        // printf(" == %d / %d\n", tvval.a, tvval.b);

            return 1;
        }
    }
   
        for ( int i = 0; i < this->cur_base->getNumFacesInShape(); i++ )
        {
            int f = this->cur_base->getFaceFromShape(i);
            bool is_complete = true;

            fraction fval = fraction(0,1);

            int variable_len = 100;
            char* variable_name = (char*)malloc(variable_len);
            // Now, determine: 
            // 1. Are all discharging rules determined?
            for ( int j = 0; is_complete && j < this->num_variations; j++ )
            {
                RuleShape* variation = this->shape_variations[j];

                bool v_involved = false;

                if ( variation->getCenter() == f && variation->isCenterFace() == true )
                {
                    v_involved = true;

                    if ( variation->isComplete( this->conf ) == false )
                    {
                        is_complete = false;
                    }
                    else
                    {
                        for ( int k = 0; k < variation->getNumChargeable(); k++ )
                        {
                            // Get the variable id for this rule!
                            variation->getVariableName(this->conf, k, variable_name, variable_len);
                            fraction fr = this->lp->getValue(variable_name);

                            // pulling!
                            fval = fval + fr;
                        }
                    }
                }
                else if ( variation->getType() == VARIABLE_TYPE_F2V || variation->getType() == VARIABLE_TYPE_F2F )
                {
                    for ( int k = 0; is_complete && !v_involved && k < variation->getNumChargeable(); k++ )
                    {
                        if ( variation->getChargeable(k) == f )
                        {
                            v_involved = true;
                            if ( variation->isComplete(this->conf ) == false )
                            {
                                is_complete = false;
                            }
                            else
                            {
                                // Get the variable id for this rule!
                                variation->getVariableName(this->conf, k, variable_name, variable_len);
                                fraction fr = this->lp->getValue(variable_name);

                                // pushing!
                                fval = fval - fr;
                            }
                        }
                    }
                }
            }

                // this face has extra charge!

            if ( is_complete && fval > fraction(1,1000) )
            {
                // printf("pruning since charge at face %2d is %lld / %lld == %lf\n", f, fval.a, fval.b, (double)fval.a/(double)fval.b );
                return 1;
            }
        }

    return 0;
}

/**
 * isSolution -- Perform a check to see if a solution exists
 * 		at this point.
 *  
 * @return 0 if no solution is found, 1 if a solution is found.
 */
int SharpnessGenerator::isSolution()
{
    if ( this->stack.size() > 2 && this->getNextUndeterminedVertex() < 0) {
        printf("Found a sharpness example?:\n");
        this->writeConfiguration();

    // Ok, now let's check to see if all of our rules are done at a given element!
        for ( int i = 0; i < this->cur_base->getNumVerticesInShape(); i++ )
        {
          int v = this->cur_base->getVertexFromShape(i);
        bool is_complete = true;

        fraction vval = fraction(0,1);

        if ( this->conf->isElement(v) )
        {
            // starts with 1
            vval = fraction(1,1);
        }

        int variable_len = 100;
        char* variable_name = (char*)malloc(variable_len);
        // Now, determine: 
        // 1. Are all discharging rules determined?
        for ( int j = 0; is_complete && j < this->num_variations; j++ )
        {
            RuleShape* variation = this->shape_variations[j];

            bool v_involved = false;

            if ( variation->getCenter() == v && variation->isCenterFace() == false )
            {
                v_involved = true;

                if ( variation->isComplete( this->conf ) == false )
                {
                    is_complete = false;
                }
                else
                {
                    for ( int k = 0; k < variation->getNumChargeable(); k++ )
                    {
                        // Get the variable id for this rule!
                        variation->getVariableName(this->conf, k, variable_name, variable_len);
                        fraction f = this->lp->getValue(variable_name);

                        // pulling!
                        vval = vval + f;
                    }
                }
            }
            else if ( variation->getType() == VARIABLE_TYPE_V2V || variation->getType() == VARIABLE_TYPE_V2F )
            {
                for ( int k = 0; is_complete && !v_involved && k < variation->getNumChargeable(); k++ )
                {
                    if ( variation->getChargeable(k) == v )
                    {
                        v_involved = true;
                        if ( variation->isComplete(this->conf ) == false )
                        {
                            is_complete = false;
                        }
                        else
                        {
                            // Get the variable id for this rule!
                            variation->getVariableName(this->conf, k, variable_name, variable_len);
                            fraction f = this->lp->getValue(variable_name);

                            // pushing!
                            vval = vval - f;
                        }
                    }
                }
            }
        }

        // 2. Is the value too large? (Bigger than w?)
        if ( !is_complete || vval > this->w + fraction(1,1000) )
        {
            // this face has extra charge!
            // printf("pruning since charge at vertex %2d is %lld / %lld == %lf > %lf == %lld / %lld!\n", v, vval.a, vval.b, (double)vval.a/(double)vval.b, (double)w.a/(double)w.b, w.a, w.b  );
            // this->writeConfiguration();
            return 0;
        }

        fraction tvval = fraction(0,1);

        if ( this->conf->isElement(v) )
        {
            // starts with 1
            tvval = fraction(1,1);
        }

        printf("v%d : %d ", v, tvval.a);

        for ( int j = 0; is_complete && j < this->num_variations; j++ )
        {
            RuleShape* variation = this->shape_variations[j];

            bool v_involved = false;

            if ( variation->getCenter() == v && variation->isCenterFace() == false )
            {
                v_involved = true;

                if ( variation->isComplete( this->conf ) == false )
                {
                    is_complete = false;
                }
                else
                {
                    for ( int k = 0; k < variation->getNumChargeable(); k++ )
                    {
                        // Get the variable id for this rule!
                        variation->getVariableName(this->conf, k, variable_name, variable_len);
                        fraction f = this->lp->getValue(variable_name);

                        // pulling!
                        printf(" + %d/%d [%s] ", f.a, f.b, variable_name);
                        tvval = tvval + f;
                    }
                }
            }
            else if ( variation->getType() == VARIABLE_TYPE_V2V || variation->getType() == VARIABLE_TYPE_V2F )
            {
                for ( int k = 0; is_complete && !v_involved && k < variation->getNumChargeable(); k++ )
                {
                    if ( variation->getChargeable(k) == v )
                    {
                        v_involved = true;
                        if ( variation->isComplete(this->conf ) == false )
                        {
                            is_complete = false;
                        }
                        else
                        {
                            // Get the variable id for this rule!
                            variation->getVariableName(this->conf, k, variable_name, variable_len);
                            fraction f = this->lp->getValue(variable_name);

                            // pushing!
                            printf(" - %d/%d [%s] ", f.a, f.b, variable_name);
                            tvval = tvval - f;
                        }
                    }
                }
            }
        }
        printf(" == %d / %d\n", tvval.a, tvval.b);

        }

    // Ok, now let's check to see if all of our rules are done at a given element!
        for ( int i = 0; i < this->cur_base->getNumFacesInShape(); i++ )
        {
            int f = this->cur_base->getFaceFromShape(i);
            bool is_complete = true;

            fraction fval = fraction(0,1);

            int variable_len = 100;
            char* variable_name = (char*)malloc(variable_len);
            // Now, determine: 
            // 1. Are all discharging rules determined?
            for ( int j = 0; is_complete && j < this->num_variations; j++ )
            {
                RuleShape* variation = this->shape_variations[j];

                bool v_involved = false;

                if ( variation->getCenter() == f && variation->isCenterFace() == true )
                {
                    v_involved = true;

                    if ( variation->isComplete( this->conf ) == false )
                    {
                        is_complete = false;
                    }
                    else
                    {
                        for ( int k = 0; k < variation->getNumChargeable(); k++ )
                        {
                            // Get the variable id for this rule!
                            variation->getVariableName(this->conf, k, variable_name, variable_len);
                            fraction fr = this->lp->getValue(variable_name);

                            // pulling!
                            fval = fval + fr;
                        }
                    }
                }
                else if ( variation->getType() == VARIABLE_TYPE_F2V || variation->getType() == VARIABLE_TYPE_F2F )
                {
                    for ( int k = 0; is_complete && !v_involved && k < variation->getNumChargeable(); k++ )
                    {
                        if ( variation->getChargeable(k) == f )
                        {
                            v_involved = true;
                            if ( variation->isComplete(this->conf ) == false )
                            {
                                is_complete = false;
                            }
                            else
                            {
                                // Get the variable id for this rule!
                                variation->getVariableName(this->conf, k, variable_name, variable_len);
                                fraction fr = this->lp->getValue(variable_name);

                                // pushing!
                                fval = fval - fr;
                            }
                        }
                    }
                }
            }

                // this face has extra charge!
            if ( fval > fraction(1,1000) )
            {
                // printf("charge at face %2d is %lld / %lld == %lf\n", f, fval.a, fval.b, (double)fval.a/(double)fval.b );
                return 0;
            }
        }

        printf("\n\nTesting for sharp grid coverage:\n");
        bool cover_found = this->findCover();

        if ( cover_found )
        {
            return 1;
        }
    }

    return 0;
}

void SharpnessGenerator::importArguments(int argc, char** argv)
{
    this->killtime = 60 * 60 * 24 * 30;
    this->max_to_v = 100;

    SearchManager::importArguments(argc, argv);

    this->w = this->lp->getExactOptimum();

    for ( int i = 0; i < argc; i++ )
    {
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
        else if ( strcmp(argv[i], "--sharpness") == 0 )
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
        else if ( strcmp(argv[i], "-w") == 0 )
        {
            this->w.a = atoi(argv[i+1]);
            this->w.b = atoi(argv[i+2]);
        }
    }

    // TODO: Match values with variable indices.
}



bool SharpnessGenerator::findCover()
{
    Configuration* testc = this->conf->getEmptyConfiguration();

    for ( int i = 0; i < this->cur_base->getNumVerticesInShape(); i++ )
    {
        int v = this->cur_base->getVertexFromShape(i);
        testc->addVertexToShape(v);

        if ( this->conf->isElement(v) )
        {
            testc->addElement(v);
        }
        else
        {
            testc->addNonElement(v);
        }
    }

    int* vperm = 0;
    int* fperm = 0;
    int* vperminv = (int*)malloc(this->grid->getMaxVertexIndex()*sizeof(int));
    int* fperminv = (int*)malloc(this->grid->getMaxFacialIndex()*sizeof(int));

    int ball_radius = 15;
    int ball_size = 0;
    int* ball = this->grid->getVertexBall(0, ball_radius, ball_size);

    int fball_size = ball_size;
    int fball_count = 0;
    int* fball = (int*)malloc(fball_size * sizeof(int));

    for ( int i = 0; i < ball_size; i++ )
    {
        for ( int j = 0; j < this->grid->getVertexDegree(ball[i]); j++ )
        {
            int f = this->grid->neighborVF(ball[i],j);

            bool in_fball = false;
            for ( int k = 0; !in_fball && k < fball_count; k++ )
            {
                if ( f == fball[k] )
                {
                    in_fball = true;
                }
            }

            if ( !in_fball )
            {
                if ( fball_size <= fball_count )
                {
                    fball_size *= 2;
                    fball = (int*)realloc(fball, fball_size * sizeof(int));
                }

                fball[fball_count++] = f;
            }
        }
    }


    // looking for two "independent" generators that will generate a tiling!
    for ( int gen1 = 0; gen1 < this->num_generators; gen1++ )
    {
        // test for compatability
        int to_v = this->generators[2*gen1+0];
        int to_v_n = this->generators[2*gen1+1];
        bool works = this->grid->transformVV(fperm, vperm, this->from_v, this->from_v_n, to_v, to_v_n );

        for ( int i = 0; i < this->grid->getMaxVertexIndex(); i++ )
        {
            vperminv[i] = -1;
        }
        for ( int i = 0; i < this->grid->getMaxVertexIndex(); i++ )
        {
            if ( vperm[i] >= 0 )
            {
                vperminv[vperm[i]] = i;
            }
        }

        for ( int i = 0; i < this->grid->getMaxFacialIndex(); i++ )
        {
            fperminv[i] = -1;
        }
        for ( int i = 0; i < this->grid->getMaxFacialIndex(); i++ )
        {
            if ( fperm[i] >= 0 )
            {
                fperminv[fperm[i]] = i;
            }
        }

        Configuration* translate = testc->duplicate();
        Configuration* base_translate = this->cur_base->duplicate();

        bool updated = true;

        while ( works && updated )
        {
            updated = false;

            for ( int i = 0; i < base_translate->getNumVerticesInShape(); i++ )
            {
                int v = base_translate->getVertexFromShape(i);

                if ( vperm[v] >= 0 && base_translate->isVertexInShape(vperm[v]) == false )
                {
                    base_translate->addVertexToShape(vperm[v]);
                    updated = true;
                }
                if ( vperminv[v] >= 0 && base_translate->isVertexInShape(vperminv[v]) == false )
                {
                    base_translate->addVertexToShape(vperminv[v]);
                    updated = true;
                }
            }

            for ( int i = 0; i < base_translate->getNumFacesInShape(); i++ )
            {
                int f = base_translate->getFaceFromShape(i);

                if ( fperm[f] >= 0 && base_translate->isFaceInShape(fperm[f]) == false )
                {
                    base_translate->addFaceToShape(fperm[f]);
                    updated = true;
                }
                if ( fperminv[f] >= 0 && base_translate->isFaceInShape(fperminv[f]) == false )
                {
                    base_translate->addFaceToShape(fperminv[f]);
                    updated = true;
                }
            }

            works = works && translate->propagate();

            for ( int i = 0; works && i < translate->getNumVerticesInShape(); i++ )
            {
                int v = translate->getVertexFromShape(i);

                if ( vperm[v] >= 0 && translate->isElement(v) && translate->isNonElement(vperm[v]) )
                {
                    works = false;
                }

                if ( vperm[v] >= 0 && translate->isNonElement(v) && translate->isElement(vperm[v]) )
                {
                    works = false;
                }

                if ( works )
                {
                    if ( vperm[v] >= 0 && translate->isElement(v) && !translate->isElement(vperm[v]) )
                    {
                        updated = true;
                        translate->addElement(vperm[v]);
                    }

                    if ( vperm[v] >= 0 && translate->isNonElement(v) && !translate->isNonElement(vperm[v]) )
                    {
                        updated = true;
                        translate->addNonElement(vperm[v]);
                    }
                }

                if ( vperminv[v] >= 0 && translate->isElement(v) && translate->isNonElement(vperminv[v]) )
                {
                    works = false;
                }

                if ( vperminv[v] >= 0 && translate->isNonElement(v) && translate->isElement(vperminv[v]) )
                {
                    works = false;
                }


                if ( works )
                {
                    if ( vperminv[v] >= 0 && translate->isElement(v) && !translate->isElement(vperminv[v]) )
                    {
                        updated = true;
                        translate->addElement(vperminv[v]);
                    }

                    if ( vperminv[v] >= 0 && translate->isNonElement(v) && !translate->isNonElement(vperminv[v]) )
                    {
                        updated = true;
                        translate->addNonElement(vperminv[v]);
                    }
                }     
            }

            works = works && translate->propagate();

            for ( int i = 0;  works &&  i < translate->getNumFacesInShape(); i++ )
            {
                int f = translate->getFaceFromShape(i);

                if ( fperm[f] >= 0 && translate->isFaceInShape(fperm[f]) == false )
                {
                    translate->addFaceToShape(fperm[f]);
                    updated = true;
                }

                if ( fperminv[f] >= 0 && translate->isFaceInShape(fperminv[f]) == false )
                {
                    translate->addFaceToShape(fperminv[f]);
                    updated = true;
                }
            }

            works = works && translate->propagate();
        }

        if ( works )
        {
            // now our translation is complete
            // test for anything NOT in the _base_ set???
            bool complete = true;
            for ( int i = 0; complete && i < ball_size; i++ )
            {
                if ( base_translate->isVertexInShape(ball[i]) == false )
                {
                    complete = false;
                }
            }

            if ( base_translate->getNumFacesInShape() > 0 )
            {
                // test only if there are faces in the base!
                for ( int i = 0; complete && i < fball_count; i++ )
                {
                    if (  base_translate->isVertexInShape(fball[i]) == false )
                    {
                        complete = false;
                    }
                }
            }

            if ( complete )
            {
                printf("\nWe have a solution for the whole grid using symmetry (%2d,%2d) -> (%2d, %2d)!\n", this->from_v, this->from_v_n, to_v, to_v_n);
                translate->print();
                delete translate;
                delete base_translate;
                free(fperm);
                free(vperm);
                free(fperminv);
                free(vperminv);
                free(ball);
                free(fball);
                delete testc;
                return true;
            }

            // Not complete... Let's continue

            printf(".");
            fflush(stdout);
            // printf("Testing using symmetry (%2d,%2d) -> (%2d, %2d). [%d of %d]\n", this->from_v, this->from_v_n, to_v, to_v_n, gen1, this->num_generators);
            for ( int gen2 = 0; gen2 < gen1; gen2++ )
            {
                // test for compatability
                int to_v_2 = this->generators[2*gen2+0];
                int to_v_n_2 = this->generators[2*gen2+1];
                works = this->grid->transformVV(fperm, vperm, this->from_v, this->from_v_n, to_v_2, to_v_n_2 );

                for ( int i = 0; i < this->grid->getMaxVertexIndex(); i++ )
                {
                    vperminv[i] = -1;
                }
                for ( int i = 0; i < this->grid->getMaxVertexIndex(); i++ )
                {
                    if ( vperm[i] >= 0 )
                    {
                        vperminv[vperm[i]] = i;
                    }
                }

                for ( int i = 0; i < this->grid->getMaxFacialIndex(); i++ )
                {
                    fperminv[i] = -1;
                }
                for ( int i = 0; i < this->grid->getMaxFacialIndex(); i++ )
                {
                    if ( fperm[i] >= 0 )
                    {
                        fperminv[fperm[i]] = i;
                    }
                }
                Configuration* translate2 = translate->duplicate();
                Configuration* base_translate2 = base_translate->duplicate();

                updated = false;

                for ( int i = 0; i < base_translate2->getNumVerticesInShape(); i++ )
                {
                    int v = base_translate2->getVertexFromShape(i);

                    if ( vperm[v] >= 0 && base_translate2->isVertexInShape(vperm[v]) == false )
                    {
                        base_translate2->addVertexToShape(vperm[v]);
                        updated = true;
                    }

                    if ( vperminv[v] >= 0 && base_translate2->isVertexInShape(vperminv[v]) == false )
                    {
                        base_translate2->addVertexToShape(vperminv[v]);
                        updated = true;
                    }
                }

                for ( int i = 0; i < base_translate2->getNumFacesInShape(); i++ )
                {
                    int f = base_translate2->getFaceFromShape(i);

                    if ( fperm[f] >= 0 && base_translate2->isFaceInShape(fperm[f]) == false )
                    {
                        base_translate2->addFaceToShape(fperm[f]);
                        updated = true;
                    }

                    if ( fperminv[f] >= 0 && base_translate2->isFaceInShape(fperminv[f]) == false )
                    {
                        base_translate2->addFaceToShape(fperminv[f]);
                        updated = true;
                    }
                }

                if ( !updated )
                {
                    // nothing new here!
                    delete translate2;
                    delete base_translate2;
                    continue;
                }

                // printf("\tand symmetry (%2d,%2d) -> (%2d, %2d). [%d of %d]\n", this->from_v, this->from_v_n, to_v_2, to_v_n_2, gen2, this->num_generators);
                while ( works && updated )
                {
                    updated = false;

                    for ( int i = 0; i < base_translate2->getNumVerticesInShape(); i++ )
                    {
                        int v = base_translate2->getVertexFromShape(i);

                        if ( vperm[v] >= 0 && base_translate2->isVertexInShape(vperm[v]) == false )
                        {
                            base_translate2->addVertexToShape(vperm[v]);
                            updated = true;
                        }

                        if ( vperminv[v] >= 0 && base_translate2->isVertexInShape(vperminv[v]) == false )
                        {
                            base_translate2->addVertexToShape(vperminv[v]);
                            updated = true;
                        }
                    }

                    for ( int i = 0; i < base_translate2->getNumFacesInShape(); i++ )
                    {
                        int f = base_translate2->getFaceFromShape(i);

                        if ( fperm[f] >= 0 && base_translate2->isFaceInShape(fperm[f]) == false )
                        {
                            base_translate2->addFaceToShape(fperm[f]);
                            updated = true;
                        }

                        if ( fperminv[f] >= 0 && base_translate2->isFaceInShape(fperminv[f]) == false )
                        {
                            base_translate2->addFaceToShape(fperminv[f]);
                            updated = true;
                        }
                    }

                    for ( int i = 0; works && i < translate2->getNumVerticesInShape(); i++ )
                    {
                        int v = translate2->getVertexFromShape(i);

                        if ( vperm[v] >= 0 && translate2->isElement(v) && translate2->isNonElement(vperm[v]) )
                        {
                            works = false;
                        }

                        if ( vperm[v] >= 0 && translate2->isNonElement(v) && translate2->isElement(vperm[v]) )
                        {
                            works = false;
                        }

                        if ( works )
                        {
                            if ( vperm[v] >= 0 && translate2->isElement(v) && !translate2->isElement(vperm[v]) )
                            {
                                updated = true;
                                translate2->addElement(vperm[v]);
                            }

                            if ( vperm[v] >= 0 && translate2->isNonElement(v) && !translate2->isNonElement(vperm[v]) )
                            {
                                updated = true;
                                translate2->addNonElement(vperm[v]);
                            }
                        }


                        if ( vperminv[v] >= 0 && translate2->isElement(v) && translate2->isNonElement(vperminv[v]) )
                        {
                            works = false;
                        }

                        if ( vperminv[v] >= 0 && translate2->isNonElement(v) && translate2->isElement(vperminv[v]) )
                        {
                            works = false;
                        }

                        if ( works )
                        {
                            if ( vperminv[v] >= 0 && translate2->isElement(v) && !translate2->isElement(vperminv[v]) )
                            {
                                updated = true;
                                translate2->addElement(vperminv[v]);
                            }

                            if ( vperminv[v] >= 0 && translate2->isNonElement(v) && !translate2->isNonElement(vperminv[v]) )
                            {
                                updated = true;
                                translate2->addNonElement(vperminv[v]);
                            }
                        }
                    }


                    for ( int i = 0;  works &&  i < translate2->getNumFacesInShape(); i++ )
                    {
                        int f = translate2->getFaceFromShape(i);

                        if ( fperm[f] >= 0 && translate2->isFaceInShape(fperm[f]) == false )
                        {
                            translate2->addFaceToShape(fperm[f]);
                            updated = true;
                        }

                        if ( fperminv[f] >= 0 && translate2->isFaceInShape(fperminv[f]) == false )
                        {
                            translate2->addFaceToShape(fperminv[f]);
                            updated = true;
                        }
                    }

                    works = works && translate2->propagate();
                }

                if ( works )
                {
                    complete = true;

                    int count_total = 0;
                    int count_in_base = 0;
                    for ( int i = 0; i < ball_size; i++ )
                    {
                        count_total++;
                        if ( base_translate2->isVertexInShape(ball[i]) == false )
                        {
                            complete = false;
                        }
                        else
                        {
                            count_in_base++;
                        }
                    }

                    if ( base_translate->getNumFacesInShape() > 0 )
                    {
                        // test only if there are faces in the base!
                        for ( int i = 0; complete && i < fball_count; i++ )
                        {
                            if ( base_translate2->isVertexInShape(fball[i]) == false )
                            {
                                complete = false;
                            }
                        }
                    }

                    if ( complete || (double)count_in_base / (double)count_total > 0.99 )
                    {
                        printf("\nWe have a solution for the whole grid using symmetries (%2d,%2d) -> (%2d, %2d) and (%2d,%2d) -> (%2d,%2d)!\n", this->from_v, this->from_v_n, to_v, to_v_n, this->from_v, this->from_v_n, to_v_2, to_v_n_2);
                        translate2->print();

                        delete translate;
                        delete base_translate;

                        delete translate2;
                        delete base_translate2;
                        free(fperm);
                        free(vperm);
                        free(fperminv);
                        free(vperminv);
                        free(ball);
                        free(fball);
                        delete testc;
                        return true;                        
                    }
                }

                delete translate2;
                delete base_translate2;
            }
        }

        delete translate;
        delete base_translate;
    }

    printf("\nNo complete system was found!\n\n");

    free(fperm);
    free(vperm);
    free(fperminv);
    free(vperminv);
    free(ball);    
    free(fball); 
                delete testc;

    return false;
}