/*
 * LinearProgram.cpp
 *
 *  Created on: May 26, 2015
 *      Author: stolee
 */

#include "fraction.hpp"
#include "LinearConstraint.hpp"
#include "LinearProgram.hpp"
#include "Trie.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace adage::grids;

LinearProgram::LinearProgram()
{
	this->num_constraints = 0;
	this->max_constraint_trie_size = 100000;
	this->constraint_trie = new Trie<int>();
	this->constraint_trie_backup = new Trie<int>();

     this->size_solution = 1000;
    this->solution = (double*)malloc(this->size_solution * sizeof(double));
    this->exact_solution = (fraction*)malloc(this->size_solution * sizeof(fraction));

    this->variables = new Trie<int>();

    this->max_constraint_trie_size = 5000000;

    this->is_feasible = true;
    this->optimum = 0;
    this->round_to_zero = 0.00001;

    char* w = (char*)malloc(2);
    w[0] = 'w';
    w[1] = 0;

    this->variables->insert(w, 0);
    this->num_nongap_variables = -1;

    this->num_variables = 1;
    this->size_variables = 1000;
    this->variable_list = (char**)malloc(this->size_variables * sizeof(char*));
    this->variable_list[0] = w;

    this->do_rounding = false;
    this->do_constraint_checking = false;
    this->write_constraints = false;
}

LinearProgram::~LinearProgram()
{
	delete this->constraint_trie;
	delete this->constraint_trie_backup;
    free(this->solution);
    this->solution = 0;

    free(this->exact_solution);
    this->exact_solution = 0;

    free(this->variable_list);
    this->variable_list = 0;
}

/**
 * The default cost is to maximize 'w' but if there is a constraint 'w == opt' and we add "slack"
 * variables to make only important constraints sharp, then we will want the slack variables to be
 * maximized. This must be done by the client.
 */
void LinearProgram::setCost(LinearConstraint* constraint, bool maximize)
{
	if ( maximize )
	{
		printf("max ");
	}
	else
	{
		printf("min ");
	}

	char* buffer = constraint->getString();

	printf("%s\n", buffer);
	fflush(stdout);

	free(buffer);
}

/**
 * If solve() is called, it will run the LP and wait for completion.
 *
 * Then isFeasible and optimum will return the results. Thus the client can stop
 * if an infeasible situation occurs, or if the optimum is below the goal value.
 */
void LinearProgram::solve()
{
    printf("[LinearProgram::solve()] is not implemented!\n");
}

double LinearProgram::getApproxValue(int var)
{
    return this->solution[var];
}

double LinearProgram::getApproxValue(const char* key)
{
    // Lookup index of key
    int i = this->variables->getValue(key);

    if ( i >= 0 )
    {
        return this->solution[i];
    }
    return 0.0;
}

int LinearProgram::getIndex(const char* key)
{
    return this->variables->getValue(key);
}

fraction LinearProgram::getValue(const char* key)
{
    // Lookup index of key
    int i = this->variables->getValue(key);

    if ( i >= 0 )
    {
        return this->exact_solution[i];
    }

    return fraction();
}


fraction LinearProgram::getValue(int var)
{
    return this->exact_solution[var];
}

char* LinearProgram::getKey(int var)
{
    return this->variable_list[var];
}

int LinearProgram::getNumVariables()
{
    return this->num_variables;
}

int LinearProgram::getNumConstraints()
{
    return this->num_constraints;
}

bool LinearProgram::isFeasible()
{
    return this->is_feasible;
}

double LinearProgram::getOptimum()
{
    return this->optimum;
}

fraction LinearProgram::getExactOptimum()
{
    return this->exact_optimum;
}


void LinearProgram::addConstraint(LinearConstraint* constraint)
{
    char* buffer = constraint->getString();

    if ( buffer != 0 )
    {
        buffer = (char*)realloc(buffer, strlen(buffer)+2);

        if ( this->constraint_trie->contains(buffer) == false )
        {
            this->constraint_trie->insert(buffer);

            int num_monomials = constraint->getNumMonomials();

            if ( num_monomials <= 0 )
            {
                return;
            }

            if ( this->write_constraints )
            {
                printf("%s\n", buffer);
                fflush(stdout);
            }

            int num_nonzero_monomials = 0;

            Monomial* m = constraint->getMonomial(0);

            if ( constraint->getWCoeff() != 0 )
            {
                num_nonzero_monomials++;
            }

            int num_to_add = 0;
            do
            {
                // something
                char* key = m->getBasicString();

                // Careful: the last monomial has an empty variable!
                if ( key != 0 && m->getCoefficient() != 0 )
                {
                    if ( this->variables->contains(key) == false )
                    {
                        this->variables->insert(key, this->num_variables);

                        if ( this->num_variables >= this->size_variables )
                        {
                            this->size_variables = this->size_variables * 2;
                            this->variable_list = (char**)realloc(this->variable_list, this->size_variables * sizeof(char*));
                        }

                        this->variable_list[this->num_variables] = key;

                        (this->num_variables)++;

                        num_to_add++;
                    }

                    num_nonzero_monomials++;

                }

                m = m->getNext();
            } while ( m != 0 );


            if ( num_to_add > 0 )
            {
                while ( this->variables->getSize() > this->size_solution )
                {
                    this->size_solution = this->size_solution * 2;

                    this->solution = (double*)realloc(this->solution, this->size_solution * sizeof(double));
                    this->exact_solution = (fraction*)realloc(this->exact_solution, this->size_solution * sizeof(fraction));
                }
            }

            (this->num_constraints)++;

            if ( (this->num_constraints % 10000) == 0 )
            {
                printf("...%d constraints.\n", this->num_constraints);
            }

            if ( this->constraint_trie->getSize() >= this->max_constraint_trie_size / 2 )
            {
                char* buffcopy = (char*)malloc(strlen(buffer)+2);
                strcpy(buffcopy, buffer);
                this->constraint_trie_backup->insert(buffcopy);
            }

            if ( this->constraint_trie->getSize() >= this->max_constraint_trie_size )
            {
                delete this->constraint_trie;
                this->constraint_trie = this->constraint_trie_backup;
                this->constraint_trie_backup = new Trie<int>();
            }

        }
        else
        {
            free(buffer);
        }
    }

    delete constraint;
    constraint = 0;
}


void LinearProgram::setOptimum(fraction f)
{
    // do nothing
}


void LinearProgram::readValues(char* filename)
{
    size_t buflen = 0;
    char* buffer = 0;

    FILE* f = fopen(filename, "r");

    while ( getline( &buffer, &buflen, f ) > 0 )
    {
        // try to read the string!
        char* equal_pos = strstr(buffer,"=");
        char* slash_pos = strstr(buffer,"/");
        char* dbleq_pos = strstr(buffer,"==");

        if ( equal_pos == 0 || slash_pos == 0 || dbleq_pos == 0 )
        {
            continue;
        }

        // end of variable id
        *(equal_pos-1) = 0;

        // end of numerator
        *slash_pos = 0;

        // end of denominator
        *dbleq_pos = 0;

        char* var_id = buffer;

        while ( *var_id == ' ' || *var_id == '\t')
        {
            var_id++;
        }

        int var_i = this->getIndex(var_id);

        if ( var_i <= 0 && (this->num_variables == 0 || strcmp(var_id, this->variable_list[0]) != 0 ) )
        {
            if ( this->num_variables >= this->size_variables )
            {
                this->size_variables = this->size_variables * 2;
                this->variable_list = (char**)realloc(this->variable_list, this->size_variables * sizeof(char*));
            }

            this->variable_list[this->num_variables] = (char*)malloc(strlen(var_id)+1);
            strcpy(this->variable_list[this->num_variables], var_id);

            this->variables->insert(this->variable_list[this->num_variables], this->num_variables);

            var_i = this->num_variables;

            (this->num_variables)++;

            while ( this->variables->getSize() > this->size_solution )
            {
                this->size_solution = this->size_solution * 2;

                this->solution = (double*)realloc(this->solution, this->size_solution * sizeof(double));
                this->exact_solution = (fraction*)realloc(this->exact_solution, this->size_solution * sizeof(fraction));
            }
        }

        long long int a = atoi(equal_pos + 1);
        long long int b = atoi(slash_pos + 1);

        this->exact_solution[var_i] = fraction(a,b);

        this->solution[var_i] = (double)a / (double)b;

        int t_var_i = this->variables->getValue(this->variable_list[var_i]);

        printf("variable[%d] = %s = %lld / %lld == %lf\n", t_var_i, this->variable_list[var_i], a, b, this->solution[var_i]);
    }

    this->optimum = this->solution[this->getIndex("w")];
    this->exact_optimum = this->exact_solution[this->getIndex("w")];

    free(buffer);
    fclose(f);
}


void LinearProgram::importArguments(int argc, char** argv)
{
    this->exact_optimum = fraction(-1,1);

    for ( int i = 0; i < argc; i++ )
    {
        if ( strcmp(argv[i], "--exact") == 0 )
        {
            this->do_rounding = true;
        }
        if ( strcmp(argv[i], "--exactconstraints") == 0 )
        {
            this->do_constraint_checking = true;
        }
        if ( strcmp(argv[i], "--writeconstraints") == 0 )
        {
            this->write_constraints = true;
        }
        if ( i < argc - 1 && strcmp(argv[i], "--triesize") == 0 )
        {
            this->max_constraint_trie_size = atoi(argv[i+1]);
        }
        if ( i < argc - 1 && strcmp(argv[i], "--values") == 0 )
        {
            this->readValues(argv[i+1]);
        }
        if ( i < argc - 2 && strcmp(argv[i], "-w") == 0 )
        {
            this->exact_optimum.a = atoi(argv[i+1]);
            this->exact_optimum.b = atoi(argv[i+2]);
        }
    }
}




