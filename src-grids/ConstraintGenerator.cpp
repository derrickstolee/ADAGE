/*
 * ConstraintGenerator.cpp
 *
 *  Created on: Apr 17, 2014
 *      Author: stolee
 */

#include "ConstraintGenerator.hpp"
#include "RuleShape.hpp"
#include "LinearConstraint.hpp"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <queue>

#include "macros.hpp"

using namespace adage::grids;

void ConstraintGenerator::initConfigurations()
{
    this->blank_conf = new Configuration(this->grid);
    this->full_conf = new Configuration(this->grid);
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
    // printf("determining variations...\n");
	for ( int i = 0; i < this->num_shapes; i++ )
	{
		Configuration* c = this->rule_shapes[i]->getShape();

		if ( this->center_is_face )
		{
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
                    this->grid->transformFF(this->fperm, this->vperm, f1, f2, g1, g2);
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

                        this->grid->transformFV(this->fperm, this->vperm, f1, v2, g1, u2);

                        bool center_is_puller = false;
                        this->addVariation(new RuleShape(this->rule_shapes[i], this->fperm, this->vperm,  this->center_id, center_is_puller));
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

                    this->grid->transformFV(this->fperm, this->vperm, f1, v2, g1, u2);

                    bool center_is_puller = true;
                    this->addVariation(new RuleShape(this->rule_shapes[i], this->fperm, this->vperm,  this->center_id, center_is_puller));
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
		else
		{
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

                    this->grid->transformVF(this->fperm, this->vperm, v1, f2, u1, g2);

                    bool center_is_puller = true;
                    this->addVariation(new RuleShape(this->rule_shapes[i], this->fperm, this->vperm, this->center_id,  center_is_puller));
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

                    this->grid->transformVV(this->fperm, this->vperm, v1, v2, u1, u2);

                    bool center_is_puller = false;
                    this->addVariation(new RuleShape(this->rule_shapes[i], this->fperm, this->vperm,  this->center_id, center_is_puller));
                
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

                            this->grid->transformVV(this->fperm, this->vperm, v1, v2, u1, u2);

                            bool center_is_puller = false;
                            this->addVariation(new RuleShape(this->rule_shapes[i], this->fperm, this->vperm, this->center_id,  center_is_puller));
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

                        this->grid->transformVF(this->fperm, this->vperm, v1, f2, u1, g2b);

                        bool center_is_puller = false;
                        this->addVariation(new RuleShape(this->rule_shapes[i], this->fperm, this->vperm, this->center_id,  center_is_puller));
                    }
                }
            }
		}
	}

    // this->full_conf->print();

    // for ( int i = 0; i < this->num_variations; i++ )
    // {
    //     this->shape_variations[i]->getShape()->print();
    // }
}

void ConstraintGenerator::addVariation(RuleShape* variation)
{
    if (this->num_variations >= this->size_variations) {
        this->size_variations = this->size_variations + 500;
        this->shape_variations = (RuleShape**)realloc(this->shape_variations, this->size_variations * sizeof(RuleShape*));
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
            this->conf->addFaceToShape(f); // these are never added!
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
bool ConstraintGenerator::writeConstraint()
{
    int w_coeff = 0;
    int rhs = 0;
    int inequality_mode = INEQUALITY_GEQ;

    if ( this->center_is_face == false )
    {
        w_coeff = -1;

        if ( this->conf->isElement(this->center_id) )
        {
            rhs = -1;
        }
    }

    LinearConstraint* constraint = new LinearConstraint(w_coeff, rhs, inequality_mode);

    bool fail = false;
    for ( int i = 0; i < this->num_variations; i++ )
    {  
        this->shape_variations[i]->modifyCoefficients(this->conf, constraint);
    }

    char* buffer = constraint->getString();

    if ( buffer != 0 )
    {  
        buffer = (char*)realloc(buffer, strlen(buffer)+2);

        if (   this->constraint_trie->contains(buffer) == false )
        {
            this->constraint_trie->insert(buffer);
            printf("%s\n", buffer);
            fflush(stdout);

            if ( this->constraint_trie->getSize() >= this->max_constraint_trie_size / 2 )
            {
                char* buffcopy = (char*)malloc(strlen(buffer)+1);
                strcpy(buffcopy, buffer);
                this->constraint_trie_backup->insert(buffcopy);
            }

            if ( this->constraint_trie->getSize() >= this->max_constraint_trie_size )
            {
                delete this->constraint_trie;
                this->constraint_trie = this->constraint_trie_backup;
                this->constraint_trie_backup = new Trie();
            }
        }
        else
        {   
            free(buffer);
        }
    }

    // size_t buflen = 400;
    // char* buffer = (char*)malloc(buflen);
    // buffer[0] = 0;

    // bool seen_before = false;
    // for (int i = 0; i <= this->max_variable_index; i++) {
    //     if (this->coefficients[i] != 0) {
    //         if ( strlen(buffer) + 20 >= buflen )
    //         {
    //             buflen += 100;
    //             buffer = (char*)realloc(buffer, buflen);
    //         }

    //         if (this->coefficients[i] == 1) {
    //             if (seen_before) {
    //                 strcat(buffer," + ");
    //             }

    //             sprintf(buffer + strlen(buffer), "x[%d]", i);
    //         } else if (this->coefficients[i] == -1) {
    //             if (seen_before) {
    //                 strcat(buffer, " ");
    //             }
    //             sprintf(buffer + strlen(buffer), "- x[%d]", i);
    //         } else if (this->coefficients[i] > 0) {
    //             if (seen_before) {
    //                 strcat(buffer," + ");
    //             }
    //             sprintf(buffer + strlen(buffer), "%d x[%d]", this->coefficients[i], i);
    //         } else {
    //             if (seen_before) {
    //                 strcat(buffer, " ");
    //             }
    //             sprintf(buffer + strlen(buffer), "- %d x[%d]", -this->coefficients[i], i);
    //         }

    //         seen_before = true;
    //     }
    // }

    // if ( strlen(buffer) + 20 >= buflen )
    // {
    //     buflen += 50;
    //     buffer = (char*)realloc(buffer, buflen);
    // }

    // if ( this->vertices_distribute_evenly )
    // {
    // 	int f = this->conf->getCenter();
    // 	fraction charge_expected(0,1);
    // 	fraction charge_in(0,1);

    // 	for ( int i = 0; i < this->grid->getFaceDegree(f); i++ )
    // 	{
    // 		int v = this->grid->neighborFV(f,i);

    // 		charge_expected = charge_expected + fraction(1,this->grid->getVertexDegree(v));

    // 		if ( this->conf->isElement(v) )
    // 		{
    // 			charge_in = charge_in + fraction(1,this->grid->getVertexDegree(v));
    // 		}
    // 	}


    // 	// need a certain amount of charge, and we receive a certain amount to start!
    // 	if ( charge_expected.b > 1)
    // 	{
    // 		sprintf(buffer + strlen(buffer), " - (%lld / %llu) w >= - %lld / %llu",  charge_expected.a, charge_expected.b, charge_in.a, charge_in.b );
    // 	}
    // 	else
    // 	{
    // 		sprintf(buffer + strlen(buffer), " - %lld w >= - %lld / %llu",  charge_expected.a, charge_in.a, charge_in.b );
    // 	}
    // }
    // else
    // {
    // 	if ( this->center_is_face )
    // 	{
    // 		sprintf(buffer + strlen(buffer), " >= 0");
    // 	}
    // 	else
    // 	{
    // 		// we are centered around a vertex!
    // 		if ( this->conf->isElement(this->conf->getCenter()))
    // 		{
    // 			// Starts with 1 
    // 			// Ends with >= w
    // 			sprintf(buffer + strlen(buffer), " - w >= - 1");
    // 		}
    // 		else
    // 		{
    // 			// Starts with 0 
    // 			// Ends with >= w
    // 			sprintf(buffer + strlen(buffer), " - w >= 0");
    //    		}
    // 	}
    // }

    delete constraint;

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

ConstraintGenerator::ConstraintGenerator(Grid* grid, Configuration* conf) : SearchManager()
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

    this->vertices_distribute_evenly = false;

    this->root = new SearchNode(0);

    this->constraint_trie = new Trie();
    this->constraint_trie_backup = new Trie();
    this->max_constraint_trie_size = 10000;
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
    delete this->constraint_trie;
    delete this->constraint_trie_backup;

    FREE_ARRAY(this->fperm);
    FREE_ARRAY(this->vperm);
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
        // we have not decided on the center orbit!
        if (next_child >= 2) {
            return -1;
        }

        if ( this->vertices_distribute_evenly && next_child >= 1 )
        {
        	return -1;
        }
    }
    else if (this->stack.size() == 1) {
        // we have not decided on the center orbit!
        if ( this->center_is_face && next_child >= this->grid->getNumFacialOrbits()) {
            return -1;
        }
        else if ( !this->center_is_face && next_child >= this->grid->getNumVertexOrbits() )
    	{
    		return -1;
    	}
    } else if (next_child >= 2) {
        // we have tried nonelement and element!
        return -1;
    } else if ( this->getNextUndeterminedVertex() < 0) {
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
    // printf("pushTo(%llX)\n", child);
    SearchNode* parent = 0;

    if (this->stack.size() == 0) {
        parent = this->root;
    } else {
        parent = this->stack.back();
    }

    parent->curChild = child;

    this->snapshot();

    if (this->stack.size() == 0) {
    	if ( child == 0 )
    	{
    		// Face!
    		this->center_is_face = true;
    	}
    	else
    	{
    		// Vertex!
    		this->center_is_face = false;
    	}
    } else if (this->stack.size() == 1) {
        // for this, we select the orbit!
        if ( this->center_is_face )
        {
	        int f = this->grid->getFacialOrbitRepresentative(child);

	        this->full_conf->addFaceToShape(f);
	        this->conf->addFaceToShape(f);
	        this->blank_conf->addFaceToShape(f);

	        this->full_conf->setCenter(f, false);
	        this->conf->setCenter(f, false);
	        this->blank_conf->setCenter(f, false);

            this->center_id = f;
	    }
	    else
	    {
	        int f = this->grid->getVertexOrbitRepresentative(child);

	        this->full_conf->addVertexToShape(f);
	        this->conf->addVertexToShape(f);
	        this->blank_conf->addVertexToShape(f);

	        this->full_conf->setCenter(f, true);
	        this->conf->setCenter(f, true);
	        this->blank_conf->setCenter(f, true);

            this->center_id = f;
	    }

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
    // printf("pop()\n");

    SearchNode* node = 0;
    LONG_T label = 0;


    if (this->stack.size() == 0) {
        node = this->root;
        label = node->label;
    } else {
        node = this->stack.back();
        this->stack.pop_back();

        label = node->label;
        delete node;

        if ( this->stack.size() == 1 )
        {
            this->freeVariations();
            this->initVariations();
        }
    }

    this->rollback();

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
        this->writeConstraint();
    }

    return 0;
}

void ConstraintGenerator::importArguments(int argc, char** argv)
{
    this->max_constraint_trie_size = 10000;
    SearchManager::importArguments(argc, argv);

    for ( int i = 0; i < argc; i++ )
    {
        if ( strcmp(argv[i], "--triesize") == 0 )
        {
            this->max_constraint_trie_size = atoi(argv[i+1]);
        }
        else if ( strcmp(argv[i], "--rules") == 0 )
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
                        if ( strcmp(argv[j]+2, shape->getRuleName()) == 0 )
                        {
                            found = true;
                            this->addRuleShape(shape);
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
    }
}
