/*
 * IdentifyingCodeConfiguration.cpp
 *
 *  Created on: Apr 17, 2014
 *      Author: stolee
 */

#include <stack>
#include <stdlib.h>
#include "macros.hpp"
#include "IdentifyingCodeConfiguration.hpp"

using namespace adage::grids;


int num_reducible_configurations = 0;
Configuration** reducible_configurations = 0;

void IdentifyingCodeConfiguration::initConflicts()
{
    this->vertex_conflict_start = (int*)malloc(this->num_vertices * sizeof(int));
    this->vertex_conflict_size = (int*)malloc(this->num_vertices * sizeof(int));

    this->conflicts = 0;
    int conflicts_size = 0;

    int cur_start = 0;
    for (int i = 0; i < this->num_vertices; i++) {
        int d = 0;
        int radius = 2;

        if ( this->mode == MODE_NEIGHBOR_IDENTIFYING )
        {
            // only ADJACENT vertices are necessary for tracking!
            radius = 1;
        }

        int* ball = this->grid->getVertexBall(i, radius, d);

        int num_new_conflicts = d - 1;

        this->vertex_conflict_start[i] = cur_start;
        this->vertex_conflict_size[i] = num_new_conflicts;
        cur_start += num_new_conflicts;

        if (conflicts_size < cur_start) {
            conflicts_size = d * this->num_vertices;

            if (conflicts_size < cur_start) {
                conflicts_size = cur_start;
            }

            this->conflicts = (int*)realloc(this->conflicts, conflicts_size * sizeof(int));
        }

        int ci = this->vertex_conflict_start[i];

        for (int j = 1; j < d; j++) {
            this->conflicts[ci + j - 1] = ball[j];
        }

        free(ball);
    }
}

void IdentifyingCodeConfiguration::freeConflicts()
{
    FREE_ARRAY(this->conflicts);
    FREE_ARRAY(this->vertex_conflict_start);
    FREE_ARRAY(this->vertex_conflict_size);

    CLEAR_STACK(this->snapshot_conflicts_i);
    CLEAR_STACK(this->snapshot_conflicts_vals);
    CLEAR_STACK(this->snapshot_conflicts_size);
}

void IdentifyingCodeConfiguration::snapshotConflicts()
{
    this->snapshot_conflicts_size.push(this->snapshot_conflicts_i.size());
    this->snapshot_is_inconsistent.push(this->is_inconsistent);
}

void IdentifyingCodeConfiguration::rollbackConflicts()
{
    ARRAY_REPLACE_ROLLBACK(this->snapshot_conflicts_size, this->snapshot_conflicts_i, this->snapshot_conflicts_vals, this->conflicts);

    if (this->snapshot_is_inconsistent.size() > 0) {
        this->is_inconsistent = this->snapshot_is_inconsistent.top();
        this->snapshot_is_inconsistent.pop();
    } else {
        this->is_inconsistent = false;
    }
}

void IdentifyingCodeConfiguration::removeConflict(int i)
{
    this->snapshot_conflicts_vals.push(this->conflicts[i]);
    this->snapshot_conflicts_i.push(i);
    this->conflicts[i] = -1;
}

void IdentifyingCodeConfiguration::initBalls()
{
    this->num_vertices = this->grid->getMaxVertexIndex();

    this->vertex_ball_size = (int*)malloc(3 * this->num_vertices * sizeof(int));
    this->vertex_ball_start = (int*)malloc(3 * this->num_vertices * sizeof(int));

    int cur_start = 0;

    for (int i = 0; i < this->num_vertices; i++) {
        this->vertex_ball_start[i] = cur_start;

        int d = this->grid->getVertexDegree(i) + 1;

        if ( this->mode == MODE_OPEN_NEIGHBORHOOD )
        {
            // do not include yourself!
            d--;
        }

        this->vertex_ball_size[i] = d;
        cur_start += d;
    }

    this->ball_list = (int*)malloc(cur_start * sizeof(int));

    for (int i = 0; i < this->num_vertices; i++) {
        int cur_i = this->vertex_ball_start[i];

        if ( this->mode != MODE_OPEN_NEIGHBORHOOD )
        {
            this->ball_list[cur_i++] = i;
        }

        for (int d = 0; d < this->grid->getVertexDegree(i); d++) {
            this->ball_list[cur_i++] = this->grid->neighborVV(i, d);
        }
    }
}

void IdentifyingCodeConfiguration::freeBalls()
{
    FREE_ARRAY(this->ball_list);
    FREE_ARRAY(this->vertex_ball_size);
    FREE_ARRAY(this->vertex_ball_start);
}

IdentifyingCodeConfiguration::IdentifyingCodeConfiguration(Grid* grid, int mode)
    : Configuration(grid)
{
    this->is_inconsistent = false;
    this->mode = mode;
    this->initBalls();
    this->initConflicts();
}

IdentifyingCodeConfiguration::~IdentifyingCodeConfiguration()
{
    this->freeBalls();
    this->freeConflicts();
}

/**
 * Create a new configuration using the same sub-class. Necessary for some features of
 * configurations. This mode will simply create a copy into a regular Configuration.
 * This may be enough for most applications.
 */
Configuration* IdentifyingCodeConfiguration::IdentifyingCodeConfiguration::duplicate() const
{
    IdentifyingCodeConfiguration* conf = new IdentifyingCodeConfiguration(this->grid, this->mode);

    for (int i = 0; i < this->vertices_in_shape; i++) {
        conf->addVertexToShape(this->shape_vertices[i]);
    }

    for (int i = 0; i < this->faces_in_shape; i++) {
        conf->addFaceToShape(this->face_index[i]);
    }

    for (int i = 0; i < this->num_elements; i++) {
        conf->addElement(this->elements[i]);
    }

    for (int i = 0; i < this->num_nonelements; i++) {
        conf->addNonElement(this->nonelements[i]);
    }

    for (int i = 0; i < this->faces_in_shape; i++) {
        if (this->face_sizes[i] >= 0 && conf->face_sizes[i] < 0) {
            // printf("Setting face size inside of duplicate()\n");
            conf->setFaceSize(this->face_index[i], this->face_sizes[i]);
        }
    }

    if (this->has_center) {
        conf->setCenter(this->center_index, this->center_is_vertex);
    }
    conf->snapshot();

    return conf;
}

Configuration* IdentifyingCodeConfiguration::IdentifyingCodeConfiguration::duplicate(int* fperm, int* vperm) const
{
    IdentifyingCodeConfiguration* conf = new IdentifyingCodeConfiguration(this->grid, this->mode);

    for (int i = 0; i < this->vertices_in_shape; i++) {
        conf->addVertexToShape(vperm[this->shape_vertices[i]]);
    }

    for (int i = 0; i < this->faces_in_shape; i++) {
        conf->addFaceToShape(fperm[this->face_index[i]]);
    }

    for (int i = 0; i < this->num_elements; i++) {
        conf->addElement(vperm[this->elements[i]]);
    }

    for (int i = 0; i < this->num_nonelements; i++) {
        conf->addNonElement(vperm[this->nonelements[i]]);
    }

    for (int i = 0; i < this->faces_in_shape; i++) {
        if (this->face_sizes[i] >= 0 && conf->face_sizes[i] < 0) {
            // printf("Setting face size inside of duplicate(*,*)\n");
            conf->setFaceSize(fperm[this->face_index[i]], this->face_sizes[i]);
        }
    }

    if (this->has_center) {
        if (this->center_is_vertex) {
            conf->setCenter(vperm[this->center_index], this->center_is_vertex);
        } else {
            conf->setCenter(fperm[this->center_index], this->center_is_vertex);
        }
    }
    conf->snapshot();

    return conf;
}

void IdentifyingCodeConfiguration::addElement(int i)
{
    this->addElement(i, true);
}

// If added to the symmetric difference of a constraint,
// then that constraint is removed!
void IdentifyingCodeConfiguration::addElement(int i, bool check)
{
    Configuration::addElement(i);

    if (check) {
        if ( this->mode != MODE_STRONG_IDENTIFYING )
        {
            if ( this->mode == MODE_LOCATING_DOMINATING )
            {
                // we must remove conflicts on i!
                for ( int ii = 0; ii < this->vertex_conflict_size[i]; ii++ )
                {
                    int t = this->conflicts[this->vertex_conflict_start[i] + ii];
                    if ( t >= 0 )
                    {
                        this->removeConflict(this->vertex_conflict_start[i] + ii);
                    }
                }
            }

            // For all elements j in the ball at i
            for (int ii = 0; ii < this->vertex_ball_size[i]; ii++) {
                int j = this->ball_list[this->vertex_ball_start[i] + ii];

                if (j >= 0) {
                    // For all conflicts t at j
                    for (int jj = 0; jj < this->vertex_conflict_size[j]; jj++) {
                        int t = this->conflicts[this->vertex_conflict_start[j] + jj];

                        if (t >= 0) {
                            // Ok, j and t are still in conflict.
                            // Is i in the ball at t?
                            bool contains = false;

                            // If i is not in the ball at t, then remove the conflict!
                            for (int tt = 0; tt < this->vertex_ball_size[t]; tt++) {
                                int bval = this->ball_list[this->vertex_ball_start[t] + tt];

                                if (bval == i) {
                                    contains = true;
                                }
                            }

                            if (!contains) {
                                // Remove this conflict!
                                this->removeConflict(this->vertex_conflict_start[j] + jj);
                                t = -1;
                            }
                        }
                    }
                }
            }
        }
    }
}

void IdentifyingCodeConfiguration::addNonElement(int i)
{
    this->addNonElement(i, true);
}

// Checks to see if a ball becomes fixed as empty!
void IdentifyingCodeConfiguration::addNonElement(int i, bool check)
{
    Configuration::addNonElement(i);

    if (check) {
        // For all elements j in the ball at i, we test if the ball at j
        // needs an element, or is empty!
        for (int ii = 0; ii < this->vertex_ball_size[i]; ii++) {
            int j = this->ball_list[this->vertex_ball_start[i] + ii];

            if (j < 0) {
                continue;
            }

            // Count number of undecided, elements, and nonelements in ball at j
            int num_undecided = 0;
            int num_elements = 0;
            int num_nonelts = 0;
            int last_undecided = -1;
            for (int jj = 0; jj < this->vertex_ball_size[j]; jj++) {
                int t = this->ball_list[this->vertex_ball_start[j] + jj];
                
                if (this->isElement(t)) {
                    num_elements++;
                } else if (this->isNonElement(t)) {
                    num_nonelts++;
                } else {
                    num_undecided++;
                    last_undecided = t;
                }
            }

            // if all nonelements, then inconsistent!
            if (num_undecided == 0 && num_elements == 0) {
                this->is_inconsistent = true;
                // printf("IS INCONSISTENT BECAUSE ALL EMPTY AT %d\n", j);
                return;
            }

            // if nonelements except one undecided, then fill that with an element!
            if (num_undecided == 1 && num_elements == 0) {
                // we need to add this as an element!
                // printf("ADDING ELEMENT %d TO AVOID ALL EMPTY AT %d\n", last_undecided, j);
                this->addElement(last_undecided);
            }
        }

        if ( this->mode == MODE_STRONG_IDENTIFYING )
        {
            // we need to check OPEN neighborhoods, too!
            for (int ii = 0; ii < this->vertex_ball_size[i]; ii++) {
                int j = this->ball_list[this->vertex_ball_start[i] + ii];

                if (j < 0) {
                    continue;
                }

                // Count number of undecided, elements, and nonelements in ball at j
                int num_undecided = 0;
                int num_elements = 0;
                int num_nonelts = 0;
                int last_undecided = -1;
                for (int jj = 0; jj < this->vertex_ball_size[j]; jj++) {
                    int t = this->ball_list[this->vertex_ball_start[j] + jj];
                    
                    if ( t == j )
                    {
                        // skip!
                    }
                    if (this->isElement(t)) {
                        num_elements++;
                    } else if (this->isNonElement(t)) {
                        num_nonelts++;
                    } else {
                        num_undecided++;
                        last_undecided = t;
                    }
                }

                // if all nonelements, then inconsistent!
                if (num_undecided == 0 && num_elements == 0) {
                    this->is_inconsistent = true;
                    // printf("IS INCONSISTENT BECAUSE ALL EMPTY AT %d\n", j);
                    return;
                }

                // if nonelements except one undecided, then fill that with an element!
                if (num_undecided == 1 && num_elements == 0) {
                    // we need to add this as an element!
                    // printf("ADDING ELEMENT %d TO AVOID ALL EMPTY AT %d\n", last_undecided, j);
                    this->addElement(last_undecided);
                }
            }
        }

        if ( this->mode == MODE_DOMINATING )
        {
            // no need to check conflicts!
            return;
        }

        // For all elements j in the ball at i, we now test conflicts at j!
        for (int ii = 0; ii < this->vertex_ball_size[i]; ii++) {
            int j = this->ball_list[this->vertex_ball_start[i] + ii];

            if (j < 0) {
                continue;
            }

            for (int jj = 0; jj < this->vertex_conflict_size[j]; jj++) {
                int t = this->conflicts[this->vertex_conflict_start[j] + jj];

                if (t < 0) {
                    continue;
                }

                if ( this->mode == MODE_LOCATING_DOMINATING &&  (this->isElement(j) || this->isElement(t)) )
                {
                    // skip!
                    continue;
                }

                // Ok, we will count elements, nonelements, and undecided
                // But among the symmetric difference!
                int num_undecided = 0;
                int num_elements = 0;
                int num_nonelts = 0;
                int last_undecided = -1;
                for (int jb = 0; num_elements == 0 && num_undecided < 2 && jb < this->vertex_ball_size[j]; jb++) {
                    int a = this->ball_list[this->vertex_ball_start[j] + jb];

                    bool in_t_ball = false;

                    for (int tt = 0; tt < this->vertex_ball_size[t]; tt++) {
                        int b = this->ball_list[this->vertex_ball_start[t] + tt];

                        if (a == b) {
                            in_t_ball = true;
                        }
                    }

                    if (!in_t_ball) {
                        if (this->isElement(a)) {
                            num_elements++;
                        } else if (this->isNonElement(a)) {
                            num_nonelts++;
                        } else {
                            num_undecided++;
                            last_undecided = a;
                        }
                    }
                }

                for (int tb = 0; num_elements == 0 && num_undecided < 2 && tb < this->vertex_ball_size[t]; tb++) {
                    int a = this->ball_list[this->vertex_ball_start[t] + tb];

                    bool in_j_ball = false;

                    for (int jb = 0; jb < this->vertex_ball_size[j]; jb++) {
                        int b = this->ball_list[this->vertex_ball_start[j] + jb];

                        if (a == b) {
                            in_j_ball = true;
                        }
                    }

                    if (!in_j_ball) {
                        if (this->isElement(a)) {
                            num_elements++;
                        } else if (this->isNonElement(a)) {
                            num_nonelts++;
                        } else {
                            num_undecided++;
                            last_undecided = a;
                        }
                    }
                }

                // If the symmetric difference is decided and empty,
                // then we are inconsistent!
                if (num_undecided == 0 && num_elements == 0) {

                    bool is_a_problem = true;
                    if ( this->mode == MODE_LOCATING_DOMINATING )
                    {
                        // can we add an element for one or the other?
                        // we know that neither are elements yet!
                        if ( this->isNonElement(j) && !this->isNonElement(t) )
                        {   
                            this->addElement(t);
                            is_a_problem = false;
                        }
                        else if ( this->isNonElement(t) && !this->isNonElement(j) )
                        {   
                            this->addElement(j);
                            is_a_problem = false;
                        }

                        // if BOTH are non-elements, then we have a problem! 
                        // then we label it as such and stop!
                    }

                    if ( is_a_problem )
                    {
                        this->is_inconsistent = true;
                        // printf("IS INCONSISTENT DUE TO CONFLICT BETWEEN %d and %d\n", j, t);
                        return;   
                    }
                }

                // If the symmetric difference is empty except for
                // one undecided element, then we must fill in that
                // spot with an element!
                if (num_undecided == 1 && num_elements == 0) {

                    if ( this->mode == MODE_LOCATING_DOMINATING )
                    {
                        if ( this->isNonElement(j) && this->isNonElement(t) )
                        {
                            // YES, WE NEED THIS ELEMENT!
                            // we only care about conflicts between nonelements, and these might be elements!
                            this->addElement(last_undecided);
                        }   
                    }
                    else
                    {
                        this->addElement(last_undecided);
                    }
                }

                if ( this->mode == MODE_STRONG_IDENTIFYING )
                {
                    if ( this->isElement(j) == true )
                    {
                        // We need to test N(j) vs N[t]
                        // Ok, we will count elements, nonelements, and undecided
                        // But among the symmetric difference!
                        int num_undecided = 0;
                        int num_elements = 0;
                        int num_nonelts = 0;
                        int last_undecided = -1;
                        for (int jb = 0; num_elements == 0 && num_undecided < 2 && jb < this->vertex_ball_size[j]; jb++) {
                            int a = this->ball_list[this->vertex_ball_start[j] + jb];

                            if ( a == j )
                            {
                                continue;
                            }

                            bool in_t_ball = false;

                            for (int tt = 0; tt < this->vertex_ball_size[t]; tt++) {
                                int b = this->ball_list[this->vertex_ball_start[t] + tt];

                                if (a == b) {
                                    in_t_ball = true;
                                }
                            }

                            if (!in_t_ball) {
                                if (this->isElement(a)) {
                                    num_elements++;
                                } else if (this->isNonElement(a)) {
                                    num_nonelts++;
                                } else {
                                    num_undecided++;
                                    last_undecided = a;
                                }
                            }
                        }

                        for (int tb = 0; num_elements == 0 && num_undecided < 2 && tb < this->vertex_ball_size[t]; tb++) {
                            int a = this->ball_list[this->vertex_ball_start[t] + tb];

                            bool in_j_ball = false;

                            for (int jb = 0; jb < this->vertex_ball_size[j]; jb++) {
                                int b = this->ball_list[this->vertex_ball_start[j] + jb];

                                if ( b != j && a == b ) {
                                    in_j_ball = true;
                                }
                            }

                            if (!in_j_ball) {
                                if (this->isElement(a)) {
                                    num_elements++;
                                } else if (this->isNonElement(a)) {
                                    num_nonelts++;
                                } else {
                                    num_undecided++;
                                    last_undecided = a;
                                }
                            }
                        }

                        // If the symmetric difference is decided and empty,
                        // then we are inconsistent!
                        if (num_undecided == 0 && num_elements == 0) {
                            this->is_inconsistent = true;
                            // printf("IS INCONSISTENT DUE TO CONFLICT BETWEEN %d and %d\n", j, t);
                            return;
                        }

                        // If the symmetric difference is empty except for
                        // one undecided element, then we must fill in that
                        // spot with an element!
                        if (num_undecided == 1 && num_elements == 0) {
                            this->addElement(last_undecided);
                        } 
                    }
                    if ( this->isElement(t) == true )
                    {
                        // We need to test N[j] vs N(t)
                        // Ok, we will count elements, nonelements, and undecided
                        // But among the symmetric difference!
                        int num_undecided = 0;
                        int num_elements = 0;
                        int num_nonelts = 0;
                        int last_undecided = -1;
                        for (int jb = 0; num_elements == 0 && num_undecided < 2 && jb < this->vertex_ball_size[j]; jb++) {
                            int a = this->ball_list[this->vertex_ball_start[j] + jb];

                            bool in_t_ball = false;

                            for (int tt = 0; tt < this->vertex_ball_size[t]; tt++) {
                                int b = this->ball_list[this->vertex_ball_start[t] + tt];

                                if ( b != t && a == b) {
                                    in_t_ball = true;
                                }
                            }

                            if (!in_t_ball) {
                                if (this->isElement(a)) {
                                    num_elements++;
                                } else if (this->isNonElement(a)) {
                                    num_nonelts++;
                                } else {
                                    num_undecided++;
                                    last_undecided = a;
                                }
                            }
                        }

                        for (int tb = 0; num_elements == 0 && num_undecided < 2 && tb < this->vertex_ball_size[t]; tb++) {
                            int a = this->ball_list[this->vertex_ball_start[t] + tb];
                            if ( a == t )
                            {
                                continue;
                            }

                            bool in_j_ball = false;

                            for (int jb = 0; jb < this->vertex_ball_size[j]; jb++) {
                                int b = this->ball_list[this->vertex_ball_start[j] + jb];

                                if ( a == b ) {
                                    in_j_ball = true;
                                }
                            }

                            if (!in_j_ball) {
                                if (this->isElement(a)) {
                                    num_elements++;
                                } else if (this->isNonElement(a)) {
                                    num_nonelts++;
                                } else {
                                    num_undecided++;
                                    last_undecided = a;
                                }
                            }
                        }

                        // If the symmetric difference is decided and empty,
                        // then we are inconsistent!
                        if (num_undecided == 0 && num_elements == 0) {
                            this->is_inconsistent = true;
                            // printf("IS INCONSISTENT DUE TO CONFLICT BETWEEN %d and %d\n", j, t);
                            return;
                        }

                        // If the symmetric difference is empty except for
                        // one undecided element, then we must fill in that
                        // spot with an element!
                        if (num_undecided == 1 && num_elements == 0) {
                            this->addElement(last_undecided);
                        } 
                    }
                    if ( this->isElement(j) == true && this->isElement(t) == true )
                    {
                        // We need to test N(j) vs N(t) 
                        // Ok, we will count elements, nonelements, and undecided
                        // But among the symmetric difference!
                        int num_undecided = 0;
                        int num_elements = 0;
                        int num_nonelts = 0;
                        int last_undecided = -1;
                        for (int jb = 0; num_elements == 0 && num_undecided < 2 && jb < this->vertex_ball_size[j]; jb++) {
                            int a = this->ball_list[this->vertex_ball_start[j] + jb];

                            if ( a == j )
                            {
                                continue;
                            }
                            
                            bool in_t_ball = false;

                            for (int tt = 0; tt < this->vertex_ball_size[t]; tt++) {
                                int b = this->ball_list[this->vertex_ball_start[t] + tt];

                                if ( b != t && a == b) {
                                    in_t_ball = true;
                                }
                            }

                            if (!in_t_ball) {
                                if (this->isElement(a)) {
                                    num_elements++;
                                } else if (this->isNonElement(a)) {
                                    num_nonelts++;
                                } else {
                                    num_undecided++;
                                    last_undecided = a;
                                }
                            }
                        }

                        for (int tb = 0; num_elements == 0 && num_undecided < 2 && tb < this->vertex_ball_size[t]; tb++) {
                            int a = this->ball_list[this->vertex_ball_start[t] + tb];
                            if ( a == t )
                            {
                                continue;
                            }

                            bool in_j_ball = false;

                            for (int jb = 0; jb < this->vertex_ball_size[j]; jb++) {
                                int b = this->ball_list[this->vertex_ball_start[j] + jb];

                                if ( b != j && a == b ) {
                                    in_j_ball = true;
                                }
                            }

                            if (!in_j_ball) {
                                if (this->isElement(a)) {
                                    num_elements++;
                                } else if (this->isNonElement(a)) {
                                    num_nonelts++;
                                } else {
                                    num_undecided++;
                                    last_undecided = a;
                                }
                            }
                        }

                        // If the symmetric difference is decided and empty,
                        // then we are inconsistent!
                        if (num_undecided == 0 && num_elements == 0) {
                            this->is_inconsistent = true;
                            // printf("IS INCONSISTENT DUE TO CONFLICT BETWEEN %d and %d\n", j, t);
                            return;
                        }

                        // If the symmetric difference is empty except for
                        // one undecided element, then we must fill in that
                        // spot with an element!
                        if (num_undecided == 1 && num_elements == 0) {
                            this->addElement(last_undecided);
                        } 
                    }
                }
            }
        }
    }
}

bool IdentifyingCodeConfiguration::isReducible()
{
    if ( this->mode == MODE_DOMINATING )
    {
        // can we remove an element and stay dominating?
        // TODO!
        return false;        
    }

    if ( 1 )
    {
        return false;
    }


    if ( 0 && num_reducible_configurations == 0 )
    {
        // we will create some!
        int max_num_rc = 100;

        reducible_configurations = (Configuration**)malloc(max_num_rc*sizeof(Configuration*));

        Configuration* c = 0;

        {
            // A completely full 2-ball is reducible!
            c = new Configuration(this->grid);
            c->addElement(0);

            c->addElement(1);
            c->addElement(2);
            c->addElement(4);

            c->addElement(7);
            c->addElement(5);
            c->addElement(6);
            c->addElement(10);
            c->addElement(12);
            c->addElement(11);

            reducible_configurations[num_reducible_configurations++] = c;
        }


        {
            // THIS configuration can MOVE the center into a 1-cluster
            // in order to decrease number of BIG clusters.
            c = new Configuration(this->grid);
            c->addElement(0);

            // c->addElement(1); // Leave unknown!
            c->addNonElement(2); // NON element!
            c->addElement(4);

            c->addElement(7);
            c->addElement(5);

            c->addNonElement(6);
            c->addNonElement(10);
            
            c->addElement(12);
            c->addElement(11);

            reducible_configurations[num_reducible_configurations++] = c;
        }

        {
            // THIS configuration can MOVE the center into a 1-cluster
            // in order to decrease number of BIG clusters.
            c = new Configuration(this->grid);
            c->addElement(0);

            c->addElement(1);
            c->addNonElement(2); // NON element!
            // c->addElement(4); // leave unknown!

            c->addElement(7);
            c->addElement(5);

            c->addNonElement(6);
            c->addNonElement(10);
            
            c->addElement(12);
            c->addElement(11);

            reducible_configurations[num_reducible_configurations++] = c;
        }
    }

    for ( int i = 0; i < this->getNumVerticesInShape(); i++ )
    {
        int v = this->getVertexFromShape(i);

        int num_in_ball = 0;

        for ( int j = 0; j < this->vertex_ball_size[v]; j++ )
        {
            int u = this->ball_list[this->vertex_ball_start[v] + j ];

            if( this->isElement(u) )
            {
                num_in_ball++;
            }
        }

        // WARNING: This only holds on the hexagonal grid!
        if ( num_in_ball >= 8 )
        {
            // printf("\n\nReducible (by ball)!\n");
            // this->print();
            // printf("\n\n");
            return true;
        }

        if ( num_in_ball >= 4 )
        {
            for ( int j = 0; j < this->grid->getVertexDegree(v); j++ )
            {
                int num_verts_in_f1 = 0;
                int num_verts_in_f2 = 0;
                int f1 = this->grid->neighborVF(v,j);
                int f2 = this->grid->neighborVF(v,j+1);

                for ( int k = 0; k < this->grid->getFaceDegree(f1); k++ )
                {
                    int u = this->grid->neighborFV(f1,k);

                    if ( this->isElement(u) )
                    {
                        num_verts_in_f1++;
                    }
                }

                if (  num_verts_in_f1 < 5 )
                {
                    // skip!
                    continue;
                }

                for ( int k = 0; k < this->grid->getFaceDegree(f2); k++ )
                {
                    int u = this->grid->neighborFV(f2,k);

                    if ( this->isElement(u) )
                    {
                        num_verts_in_f2++;
                    }
                }

                if ( num_verts_in_f2 >= 5 )
                {
                    // printf("\n\nReducible (by adj faces)!\n");
                    // this->print();
                    // printf("\n\n");
                    return true;
                }
            }
        }
    }

    return false;
}

/**
 * Extensions will use the propagate method to expand the configuration to things that are "known"
 * based on the given information. Returns false if and only if something is wrong!
 *
 * Requires knowing the grid for neighborhoods and such.
 */
bool IdentifyingCodeConfiguration::propagate()
{
    if (this->is_inconsistent) {
        return false;
    }
    // return false if needed...

    // Should we do any additional propagation?
    // Or should we trust that it was done with the checks?

    // TODO: Determine if the configuration is TOO DENSE!
    // This means the set is reducible!
    if (this->isReducible()) {
        return false;
    }

    return Configuration::propagate();
}

/** Public Interface for Accessing Things */
void IdentifyingCodeConfiguration::snapshot()
{
    Configuration::snapshot();

    this->snapshotConflicts();
}

void IdentifyingCodeConfiguration::rollback()
{
    this->rollbackConflicts();

    Configuration::rollback();
}
