/*
 * RuleGenerator.cpp
 *
 *  Created on: Apr 17, 2014
 *      Author: stolee
 */

#include "RuleGenerator.hpp"
#include "RuleShape.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "macros.hpp"

using namespace adage::grids;

void RuleGenerator::addBaseConfiguration(Configuration* c, int to_color, int num_from, int* from)
{
    if (this->hasIsomorphicConfiguration(c)) {
        return;
    }

    if (this->num_base_confs >= this->size_base_confs) {
        this->size_base_confs += 100;
        this->base_confs = (Configuration**)realloc(this->base_confs, this->size_base_confs * sizeof(Configuration*));

        this->to_colors = (int*)realloc(this->to_colors, this->size_base_confs * sizeof(int));
        this->from_colors = (int**)realloc(this->from_colors, this->size_base_confs * sizeof(int*));
        this->num_from = (int*)realloc(this->num_from, this->size_base_confs * sizeof(int));
    }

    this->base_confs[this->num_base_confs] = c->duplicate();
    this->base_confs[this->num_base_confs]->computeSymmetry();

    this->to_colors[this->num_base_confs] = to_color;
    this->num_from[this->num_base_confs] = num_from;
    this->from_colors[this->num_base_confs] = (int*)malloc(num_from * sizeof(int));
    bcopy(from, this->from_colors[this->num_base_confs], num_from * sizeof(int));

    (this->num_base_confs)++;
}

bool RuleGenerator::hasIsomorphicConfiguration(Configuration* c)
{
    c->computeSymmetry();

    for (int i = 0; i < this->num_base_confs; i++) {
        if (strcmp(c->getCanonicalString(), this->base_confs[i]->getCanonicalString()) == 0) {
            return true;
        }
    }

    return false;
}

void RuleGenerator::initBaseConfigurations()
{
    this->num_base_confs = 0;
    this->size_base_confs = 100;
    this->base_confs = (Configuration**)malloc(this->size_base_confs * sizeof(Configuration*));
    this->to_colors = (int*)malloc(this->size_base_confs * sizeof(int));
    this->from_colors = (int**)malloc(this->size_base_confs * sizeof(int*));
    this->num_from = (int*)malloc(this->size_base_confs * sizeof(int));
}

void RuleGenerator::freeBaseConfigurations()
{
    DELETE_AND_FREE_ARRAY(this->base_confs, this->num_base_confs);
    FREE_ARRAY(this->to_colors);
    FREE_ARRAY(this->num_from);
    FREE_ARRAY2(this->from_colors, this->num_base_confs);
    this->num_base_confs = 0;
    this->size_base_confs = 0;
}

void RuleGenerator::addRule(Configuration* c)
{
    if (this->size_rules <= this->num_rules) {
        // this->size_rules += 100;
        // this->rules = (Rule**)realloc(this->rules, this->size_rules * sizeof(Rule*));

        // for (int i = this->num_rules; i < this->size_rules; i++) {
        //     this->rules[i] = 0;
        // }
    }

    c->computeSymmetry();

    if ( 1 )
    {
        if ( this->canonical_trie->contains(c->getCanonicalString()) )
        {
            return;
        }
    }
    else
    {
        for (int i = 0; i < this->num_rules; i++) {
            if (strcmp(this->rules[i]->getConfiguration()->getCanonicalString(), c->getCanonicalString()) == 0) {
                return;
            }
        }
    }

    for ( int i = 0; i < c->getNumFacesInShape(); i++ )
    {
        int f = c->getFaceFromShape(i);

        int elts = 0;
        int nonelts = 0;

        for ( int j = 0; j < this->grid->getFaceDegree(f); j++ )
        {
            int v = this->grid->neighborFV(f,j);

            if ( c->isElement(v) )
            {
                elts++;
            }
            else if ( c->isNonElement(v) )
            {
                nonelts++;
            }
        }

        if ( elts != c->getNumElementsInFace(f) || nonelts != c->getNumNonElementsInFace(f) )
        {
            printf("The following configuration has %d elts and %d nonelts in face %d, but it thinks it has %d and %d, respectively!\n", elts, nonelts, f, c->getNumElementsInFace(f), c->getNumNonElementsInFace(f));
            c->print();
        }
    }



    int* new_from = (int*)malloc(this->current_num_from * sizeof(int));
    int new_num_from = 0;

    if ( this->current_rule_type == VARIABLE_TYPE_F2V || this->current_rule_type == VARIABLE_TYPE_F2F )
    {
        for (int l = 0; l < this->current_num_from; l++) {
            int c = conf->getColorOfFace(this->current_from_colors[l]);

            // look for duplicate colors!
            bool dup = false;
            for (int j = 0; j < new_num_from; j++) {
                if (new_from[j] == c) {
                    dup = true;
                }
            }

            if (!dup) {
                new_from[new_num_from++] = c;
            }
        }
    }
    else
    {
        // use VERTICES!
        for (int l = 0; l < this->current_num_from; l++) {
            int c = conf->getColorOfVertex(this->current_from_colors[l]);

            // look for duplicate colors!
            bool dup = false;
            for (int j = 0; j < new_num_from; j++) {
                if (new_from[j] == c) {
                    dup = true;
                }
            }

            if (!dup) {
                new_from[new_num_from++] = c;
            }
        }
    }

    Rule* rule = new Rule(c, this->current_rule_type, this->current_to_color, new_from,
                                            new_num_from, this->cur_num_vars);
    rule->setShapeConfiguration(this->current_base_conf);

    free(new_from);
    new_from = 0;

    (this->cur_num_vars) += rule->getNumVariables();

    if ( 1 ) {
        char* s = c->getCanonicalString();
        char* buffer = (char*)malloc(strlen(s)+2);
        strcpy(buffer, s);
        this->canonical_trie->insert(buffer);
        rule->write(stdout);
        fflush(stdout);
    }

    delete rule;

    (this->num_rules)++;
}

void RuleGenerator::initRuleList()
{
    this->size_rules = 1000;
    this->num_rules = 0;
    //this->rules = (Rule**)malloc(this->size_rules * sizeof(Rule*));
    //bzero(this->rules, this->size_rules * sizeof(Rule*));

    this->canonical_trie = new Trie();
}

void RuleGenerator::freeRuleList()
{
    fflush(stdout); 
    // DELETE_AND_
    //FREE_ARRAY(this->rules);
    this->num_rules = 0;
    this->size_rules = 0;

    delete this->canonical_trie;
}

// This recursive algorithm simply colors each vertex that it comes across.
void RuleGenerator::recurseGenerateRules()
{
    if (this->conf->propagate() == false) {
        return;
    }

    // Look through the configuration to find the next vertex to check.
    int next_vert = -1;

    if (this->restrict_center_face) {
        int f = this->conf->getCenter();

        int csize = 0;
        for (int i = 0; i < this->grid->getFaceDegree(f); i++) {
            int v = this->grid->neighborFV(f, i);

            if (this->conf->isElement(v)) {
                csize++;
            }
        }

        if (csize > this->restrict_center_size) {
            // too many!
            return;
        }
    }

    // Select an open vertex incident to a face involved in the rule.

    for (int i = 0; i < this->cur_base->getNumVerticesInShape(); i++) {
        int v = this->cur_base->getVertexFromShape(i);

        if (this->conf->isElement(v) || this->conf->isNonElement(v)) {
            continue;
        }

        next_vert = v;
        break;
    }

    if (next_vert >= 0) {
        this->conf->snapshot();
        this->conf->addElement(next_vert);
        this->recurseGenerateRules();
        this->conf->rollback();

        this->conf->snapshot();
        this->conf->addNonElement(next_vert);
        this->recurseGenerateRules();
        this->conf->rollback();
        return;
    }

    if ( this->set_face_sizes )
    {
        // Ok, how about a face whose size is not set?
        for (int i = 0; i < this->cur_base->getNumFacesInShape(); i++) {
            int f = this->cur_base->getFaceFromShape(i);

            if (this->conf->getFaceSize(f) >= 0) {
                continue;
            }

            int mins = this->conf->getNumElementsInFace(f);
            int maxs = this->grid->getFaceDegree(f) - this->conf->getNumNonElementsInFace(f);

            // printf("Selecting face %d to have a size between %d and %d\n", f, mins, maxs);
            for (int s = mins; s <= maxs; s++) {
                this->conf->snapshot();
                this->conf->setFaceSize(f, s);
                this->recurseGenerateRules();
                this->conf->rollback();
            }
            return;
        }
    }

    this->addRule(this->conf);
}

RuleGenerator::RuleGenerator(Grid* grid, Configuration* conf, bool write)
{
    this->grid = grid;
    this->conf = conf;

    this->size_base_confs = 0;
    this->num_base_confs = 0;
    this->base_confs = 0;

    this->cur_num_vars = 0;

    this->size_rules = 0;
    this->num_rules = 0;
    this->rules = 0;
    this->write_as_inserted = write;
    this->current_rule_type = 0;

    this->initBaseConfigurations();
    this->initRuleList();
}

RuleGenerator::~RuleGenerator()
{
    this->freeBaseConfigurations();
    this->freeRuleList();
}

/**
 * Generate rules to pull/push from a center vertex to its adjacent faces,
 *   where the ball of diameter "vert_diameter" is specified exactly.
 *
 * for all vertex orbits:
 *    Create all adjacent faces and add vertices within the given distance (and within these faces?)
 *    Make the center vertex an element.
 *    Recursively check all arrangements of elements around the center.
 */
int RuleGenerator::generateVertexToFaceRules(int vert_diameter, bool restrict_to_faces, bool use_face_sizes)
{
    Configuration* conf = this->conf->duplicate();

    this->restrict_center_face = false;

    this->set_face_sizes = use_face_sizes;
    
    int start_num_rules = this->num_rules;
    int start_base = this->num_base_confs;
    this->current_rule_type = VARIABLE_TYPE_F2V;

    // Step 1: Generate all possible bases
    for (int i = 0; i < this->grid->getNumVertexOrbits(); i++) {
        int v = this->grid->getVertexOrbitRepresentative(i);

        conf->snapshot();

        int ball_size = 0;
        int* ball = this->grid->getVertexBall(v, vert_diameter, ball_size);

        for ( int i = 0; i < ball_size; i++ )
        {
            conf->addVertexToShape(ball[i]);
        }
        free(ball);
        ball = 0;

        conf->setCenter(v, true);

        int num_from = this->grid->getVertexDegree(v);
        int* from = (int*)malloc(num_from * sizeof(int));

        for (int j = 0; j < num_from; j++) {
            int f = this->grid->neighborVF(v, j);

            from[j] = f;
            conf->addFaceToShape(f);
        }

        this->addBaseConfiguration(conf, 0, num_from, from);
        free(from);
        conf->rollback();
    }


    // Step 2: For each non-isomorphic base, make selections!
    Configuration* oldconf = this->conf;
    for (int i = start_base; i < this->num_base_confs; i++) {
        this->conf = this->base_confs[i]->duplicate();
        this->cur_base = this->base_confs[i];

        this->current_to_color = this->to_colors[i];
        this->current_num_from = this->num_from[i];
        this->current_from_colors = this->from_colors[i];
        this->current_rule_type = VARIABLE_TYPE_F2V;

        RuleShape* rs = new RuleShape(this->base_confs[i], VARIABLE_TYPE_F2V, this->base_confs[i]->getCenter(), !this->conf->isCenterVertex());

        int v = this->conf->getCenter();
        int num_from = this->grid->getVertexDegree(v);

        for (int j = 0; j < num_from; j++) {
            int f = this->grid->neighborVF(v, j);

            // printf("Adding face %d...\n", f);
            rs->addChargeable(f);
        }

        rs->print();
        delete rs;

        this->current_base_conf = this->base_confs[i];

        this->conf->snapshot();
        this->recurseGenerateRules();

        delete this->conf;
        this->conf = 0;
    }

    delete conf;
    this->conf = oldconf;

    return this->num_rules - start_num_rules;
}

/**
 * Generate rules to pull/push from a center face, where there are "facial_degree" adjacent faces.
 * The middle "pull_degree" of them actually create the rules.
 */
int RuleGenerator::generateFaceToFaceRules(int facial_degree, int pull_degree, int max_pull_face_size, int full_face_degree)
{
    Configuration* conf = this->conf->duplicate();

    if ( full_face_degree < 0 )
    {
        full_face_degree = pull_degree;
    }

    if (max_pull_face_size >= 0) {
        this->restrict_center_face = true;
        this->restrict_center_size = max_pull_face_size;
    } else {
        this->restrict_center_face = false;
    }

    // (4,2) -> 1
    // (3, 1) -> 1
    // (4,1) -> 1
    // (3,2) -> 0
    int gap_before = (facial_degree - pull_degree) / 2;
    int detail_gap_before = (facial_degree - full_face_degree) / 2;

    int start_num_rules = this->num_rules;
    int start_base = this->num_base_confs;
    this->current_rule_type = VARIABLE_TYPE_F2F;

    // Step 1: Generate all possible bases
    for (int i = 0; i < this->grid->getNumFacialOrbits(); i++) {
        int f = this->grid->getFacialOrbitRepresentative(i);

        conf->snapshot();
        conf->addFaceToShape(f);
        conf->setCenter(f, false);

        for (int j = 0; j < this->grid->getFaceDegree(f); j++) {
            conf->addVertexToShape(this->grid->neighborFV(f, j));
        }

        int num_from = pull_degree;
        int* from = (int*)malloc(pull_degree * sizeof(int));

        for (int j = 0; j < this->grid->getFaceDegree(f); j++) {
            // These are all starting positions!
            conf->snapshot();

            // Add facial_degree consecutive faces
            // The ones in [gap_before, gap_before+pull_degree) are for variables!
            for (int k = 0; k < facial_degree; k++) {
                int g = this->grid->neighborFF(f, j + k);

                conf->addFaceToShape(g);

                if (k >= gap_before && k < gap_before + pull_degree) {
                    from[k - gap_before] = g;
                }
                
                if ( k>= detail_gap_before && k < gap_before + full_face_degree )
                {
                    // Only add the vertices next to detail faces!
                    for (int l = 0; l < this->grid->getFaceDegree(g); l++) {
                        int u = this->grid->neighborFV(g, l);

                        conf->addVertexToShape(u);
                    }
                }
            }

            this->addBaseConfiguration(conf, 0, num_from, from);

            conf->rollback();
        }

        free(from);
        conf->rollback();
    }

    // Step 2: For each non-isomorphic base, make selections!
    Configuration* oldconf = this->conf;
    for (int i = start_base; i < this->num_base_confs; i++) {
        
        this->generateFaceToFaceRules(this->base_confs[i], this->to_colors[i], this->num_from[i], this->from_colors[i], max_pull_face_size);
    }

    this->conf = oldconf;

    return this->num_rules - start_num_rules;
}

int RuleGenerator::generateFaceToFaceRules(Configuration* base_configuration, int to_color, int num_from, int* from_colors, int max_pull_face_size)
{
    int start_num_rules = this->num_rules;

    RuleShape* rs = new RuleShape(base_configuration, VARIABLE_TYPE_F2F, base_configuration->getCenter(), !base_configuration->isCenterVertex());

    for ( int j = 0; j < num_from; j++ )
    {
        rs->addChargeable(from_colors[j]);
    }

    rs->print();
    delete rs;
    
    this->current_base_conf = base_configuration;

    this->current_rule_type = VARIABLE_TYPE_F2F;

    if (max_pull_face_size >= 0) {
        this->restrict_center_face = true;
        this->restrict_center_size = max_pull_face_size;
    } else {
        this->restrict_center_face = false;
    }

    this->conf = base_configuration->duplicate();
    this->cur_base = base_configuration;

    this->current_rule_type = VARIABLE_TYPE_F2F;
    this->current_to_color = to_color;
    this->current_num_from = num_from;
    this->current_from_colors = from_colors;

    this->conf->snapshot();
    this->recurseGenerateRules();

    delete this->conf;
    this->conf = 0;

    return this->num_rules - start_num_rules;
}

int RuleGenerator::generateVertexToFaceRules(Configuration* base_configuration, int to_color, int num_from, int* from_colors)
{
    int start_num_rules = this->num_rules;

    RuleShape* rs = new RuleShape(base_configuration, VARIABLE_TYPE_V2F, base_configuration->getCenter(), !base_configuration->isCenterVertex());

    for ( int j = 0; j < num_from; j++ )
    {
        rs->addChargeable(from_colors[j]);
    }

    rs->print();
    delete rs;
    
    this->current_base_conf = base_configuration;

    this->current_rule_type = VARIABLE_TYPE_V2F;

    this->conf = base_configuration->duplicate();
    this->cur_base = base_configuration;

    this->current_rule_type = VARIABLE_TYPE_V2F;
    this->current_to_color = to_color;
    this->current_num_from = num_from;
    this->current_from_colors = from_colors;
    this->restrict_center_face = false;

    this->conf->snapshot();
    this->recurseGenerateRules();

    delete this->conf;
    this->conf = 0;

    return this->num_rules - start_num_rules;
}

int RuleGenerator::generateVertexToVertexRules(Configuration* base_configuration, int to_color, int num_from, int* from_colors)
{
    delete this->canonical_trie;
    this->canonical_trie = new Trie();

    int start_num_rules = this->num_rules;

    RuleShape* rs = new RuleShape(base_configuration, VARIABLE_TYPE_V2V, base_configuration->getCenter(), !base_configuration->isCenterVertex());

    for ( int j = 0; j < num_from; j++ )
    {
        rs->addChargeable(from_colors[j]);
    }

    rs->print();
    delete rs;
    
    this->current_base_conf = base_configuration;

    this->current_rule_type = VARIABLE_TYPE_V2V;

    this->restrict_center_face = false;

    this->conf = base_configuration->duplicate();
    this->cur_base = base_configuration;

    this->current_rule_type = VARIABLE_TYPE_V2V;
    this->current_to_color = to_color;
    this->current_num_from = num_from;
    this->current_from_colors = from_colors;

    this->conf->snapshot();
    this->recurseGenerateRules();

    delete this->conf;
    this->conf = 0;

    return this->num_rules - start_num_rules;
}

int RuleGenerator::generateFaceToVertexRules(Configuration* base_configuration, int to_color, int num_from, int* from_colors)
{
    delete this->canonical_trie;
    this->canonical_trie = new Trie();


    int start_num_rules = this->num_rules;

    RuleShape* rs = new RuleShape(base_configuration, VARIABLE_TYPE_F2V, base_configuration->getCenter(), !base_configuration->isCenterVertex());

    for ( int j = 0; j < num_from; j++ )
    {
        rs->addChargeable(from_colors[j]);
    }

    rs->print();
    delete rs;
    
    this->current_base_conf = base_configuration;

    this->current_rule_type = VARIABLE_TYPE_F2V;

    this->restrict_center_face = false;

    this->conf = base_configuration->duplicate();
    this->cur_base = base_configuration;

    this->current_rule_type = VARIABLE_TYPE_F2V;
    this->current_to_color = to_color;
    this->current_num_from = num_from;
    this->current_from_colors = from_colors;

    this->conf->snapshot();
    this->recurseGenerateRules();

    delete this->conf;
    this->conf = 0;

    return this->num_rules - start_num_rules;
}

void RuleGenerator::setFaceSizes(bool val)
{
    this->set_face_sizes = val;
}

void RuleGenerator::writeAllRules(FILE* file)
{
    for (int i = 0; i < this->num_rules; i++) {
        //  this->rules[i]->write(file);
    }
}
