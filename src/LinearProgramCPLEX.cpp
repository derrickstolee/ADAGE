/*
 * LinearProgram.cpp
 *
 *  Created on: May 26, 2015
 *      Author: stolee
 */

#include "fraction.hpp"
#include "LinearConstraint.hpp"
#include "LinearProgram.hpp"
#include "LinearProgramCPLEX.hpp"
#include "Trie.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


extern "C"
{
#include "ilcplex/cplex.h"
#include "ilcplex/cpxconst.h"
}

using namespace adage;

LinearProgramCPLEX::LinearProgramCPLEX() : LinearProgram()
{
    /* initialize the LP */
    int status = 0;
    this->env = CPXopenCPLEX(&status);
    this->lpx = CPXcreateprob(this->env, &status, "ADAGE");
    CPXchgobjsen(this->env, this->lpx, CPX_MAX);

    this->variable_reverse_lookup = 0;
    this->variable_forward_lookup = 0;
    this->has_gap_columns = false;

    // w is bounded below by 0.
    double* obj = new double[1];
    obj[0] = 1.0;
 	status = CPXnewcols(this->env, this->lpx, 1, obj, NULL, NULL, NULL, NULL);        
    CPXchgobjsen(this->env, this->lpx, CPX_MAX);
    delete obj;

    this->max_constraint_error_with_w = fraction(0,1);
    this->max_constraint_error_without_w = fraction(0,1);

    this->use_simplex = false;

    this->num_primes = 25;

    int p [25] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97 };
    this->primes = new int[num_primes];

    for ( int i =0; i < 25; i++ )
    {
        this->primes[i] = p[i];
    }

    this->powers = new int[num_primes];
    this->powers[0] = 15;
    this->powers[1] = 9;
    this->powers[2] = 6;
    this->powers[3] = 5;
    this->powers[4] = 4;
    this->powers[5] = 3;
    
    for ( int i = 6; i < this->num_primes; i++ )
    {
        this->powers[i] = 2;
    }

    this->num_linearconstraints = 0;
    this->size_constraint_list = 10000;
    this->constraint_list = (LinearConstraint**)malloc(this->size_constraint_list * sizeof(LinearConstraint*));
    this->constraint_active = (char*)malloc(this->size_constraint_list);
    this->constraint_forward_lookup = (int*)malloc(this->size_constraint_list*sizeof(int));
}

LinearProgramCPLEX::~LinearProgramCPLEX()
{
	CPXdelrows(this->env, this->lpx, 0, this->num_remaining_constraints);

    CPXfreeprob(this->env, &(this->lpx));
	CPXcloseCPLEX(&(this->env));
}




/**
 * The default cost is to maximize 'w' but if there is a constraint 'w == opt' and we add "slack"
 * variables to make only important constraints sharp, then we will want the slack variables to be 
 * maximized. This must be done by the client.
 */
void LinearProgramCPLEX::setCost(LinearConstraint* constraint, bool maximize)
{
	if ( maximize )
	{
        CPXchgobjsen(this->env, this->lpx, CPX_MAX);
        this->optimum = 1.0;
	}
	else
	{
        CPXchgobjsen(this->env, this->lpx, CPX_MIN);
	}

    // TODO: change obj to match constraint
}

/**
 * If solve() is called, it will run the LP and wait for completion.
 *
 * Then isFeasible and optimum will return the results. Thus the client can stop
 * if an infeasible situation occurs, or if the optimum is below the goal value.
 */
void LinearProgramCPLEX::solve()
{
    // clear up some memory (that may be a lot!)
    delete this->constraint_trie;
    delete this->constraint_trie_backup;
    this->constraint_trie = 0;
    this->constraint_trie_backup = 0;

    this->num_remaining_constraints = this->num_constraints;

    this->constraint_trie = new Trie<int>();
    this->constraint_trie_backup = new Trie<int>();

    this->variable_forward_lookup = (int*)realloc(this->variable_forward_lookup, (this->num_variables+this->num_constraints) * sizeof(int));
    this->variable_reverse_lookup = (int*)realloc(this->variable_reverse_lookup, (this->num_variables+this->num_constraints) * sizeof(int));

    for ( int i = 0; i < this->num_variables + this->num_constraints; i++ )
    {
        this->variable_forward_lookup[i] = i;
        this->variable_reverse_lookup[i] = i;
    }

    // Solve the CPLEX LP using the Barrier method!
    int status = 0;

    for ( int i = 1; i < this->num_variables; i++ )
    {
        // null
        this->exact_solution[i] = fraction(-1,0);
    }
    this->num_remaining_variables = this->num_variables;

    // CPXsetintparam(this->env, CPX_PARAM_DEPIND, 3);
    CPXsetdblparam(this->env, CPX_PARAM_EPMRK, 0.99); // be super-accurate!
    CPXsetintparam(this->env, CPX_PARAM_NUMERICALEMPHASIS, 1); // numerical emphasis!

    if( !(this->use_simplex) )
    {
        // Set the tolerance to be higher than normal

        printf("Solving with barrier method...");
        fflush(stdout);
        status = CPXbaropt(this->env, this->lpx);
        printf("done!\n");

        if (  status != 0 )
        {
        	// error was thrown?
        	printf("CPXbaropt ended with status %d!\n", status);
        	exit(1);
        }
    }
    else
    {
        printf("Solving with simplex method...");
        fflush(stdout);
        status = CPXprimopt(this->env, this->lpx);
        printf("done!\n");

        printf("CPXlpopt ended with status %d!\n", status);
        if (  status != 0 )
        {
            // error was thrown?
            printf("CPXlpopt ended with status %d!\n", status);
            exit(1);
        }
    }

    int lpstat = CPXgetstat(this->env, this->lpx);
    printf("CPXlpopt ended with lpstat %d!\n", lpstat);

    if ( lpstat == CPX_STAT_INFEASIBLE )
    {
    	this->is_feasible = false;
    }
    else
    {
    	this->is_feasible = true;
    }

    if ( lpstat == CPX_STAT_OPTIMAL )
    {
        int rounding_stage = 0;
        this->solution = (double*)realloc(this->solution, (this->num_variables + this->num_constraints) * sizeof(double));
        this->exact_solution = (fraction*)realloc(this->exact_solution, this->num_variables * sizeof(fraction));

        double obj_val = 0;
	    status = CPXsolution(this->env, this->lpx, &lpstat, &obj_val, this->solution, NULL, NULL, NULL);

	    this->optimum = this->solution[0];
		this->exact_solution[0] = fraction::makeFractionFromDivisors(this->solution[0], this->num_primes, this->primes, this->powers, 200);
        this->exact_optimum = this->exact_solution[0];

        if ( this->do_rounding )
        {
            // This is the rounding process:
            // 0. Set the exact optimal value and resolve with gaps.
            // 1. We find variables with good estimates for given max denominator
            // 2. If no variables are close enough, increase the denominator and go to 1
            // 3. Reduce the LP by removing the exact variables and resolve (with gaps!), go to 1.
            printf("Obj: w = %d / %d == %lf\n", this->exact_optimum.a, this->exact_optimum.b, this->optimum);

            // 0. Set the exact optimal value and resolve with gaps.
            printf("Assigning optimum fraction and reducing LP.\n");  fflush(stdout);
            int num_rounded = 1;
            int* vars = new int[this->num_variables];
            vars[0] = 0;
            fraction* fractions = new fraction[this->num_variables];
            fractions[0] = this->exact_optimum;
            num_rounded = 1;

            for ( int i = 1; i < this->num_variables; i++ )
            {
                if ( fabs(this->solution[i]) < 0.000001 )
                {
                    this->exact_solution[i] = fraction(0,1);
                    vars[num_rounded] = i;
                    fractions[num_rounded] = fraction(0,1);
                    num_rounded++;
                }
            }

            double error_term = 0.0000001;

            while (  this->num_remaining_variables > 0 )
            {
                // 1. We find variables with good estimates for given max denominator
                bool updated = false;
                double temp_error_term = error_term;
                rounding_stage++;

                int max_denom = 2 * this->exact_optimum.b;
                int temp_max_denom = 50000;
                
                while (  this->num_remaining_variables - num_rounded > 0 && temp_error_term < 0.000005 )
                {   
                    printf("temp_error_term = %1.10lf\n",temp_error_term);
                    printf("num_rounded = %d, num_remaining = %d\n", num_rounded, this->num_remaining_variables);
                    
                    max_denom = 5 * this->exact_optimum.b;
                    temp_max_denom = 50000;

                    while ( max_denom <= temp_max_denom )
                    {
                        for ( int use_primes = 0; use_primes < 2; use_primes++ )
                        {
                            for ( int i = 0; i < this->num_variables; i++ )
                            {
                                if ( this->exact_solution[i].b == 0 )
                                {
                                    // try rounding!
                                    if ( use_primes == 0 ) 
                                    {
                                        // round with the prime divisors
                                        double i_error = fraction::bestApproximationFactorFromDivisors(this->solution[this->variable_forward_lookup[i]], this->num_primes, this->primes, this->powers, max_denom * 2);

                                        if (  i_error < temp_error_term )
                                        {                                    
                                            this->exact_solution[i] = fraction::makeFractionFromDivisors(this->solution[this->variable_forward_lookup[i]], this->num_primes, this->primes, this->powers, max_denom * 2);

                                            vars[num_rounded] = i;
                                            fractions[num_rounded] = this->exact_solution[i];

                                            if ( !updated )
                                            {
                                                temp_max_denom = max_denom * 3;
                                            }

                                            num_rounded++;
                                            updated = true;
                                        }
                                    }
                                    else 
                                    {
                                        // round with just a max denominator
                                        double i_error = fraction::bestApproximationFactor(this->solution[this->variable_forward_lookup[i]], max_denom);

                                        if (  i_error < temp_error_term )
                                        {                                    
                                            this->exact_solution[i] = fraction::makeFraction(this->solution[this->variable_forward_lookup[i]],  max_denom);

                                            vars[num_rounded] = i;
                                            fractions[num_rounded] = this->exact_solution[i];

                                            if ( !updated )
                                            {
                                                temp_max_denom = max_denom * 3;
                                            }

                                            num_rounded++;
                                            updated = true;
                                        }
                                    }
                                }
                            }
                        }

                        max_denom *= 2;
                    }  

                    temp_error_term *= 1.1;                

                    if ( num_rounded >= this->num_remaining_variables || num_rounded > 10 )
                    {
                        updated = true;
                        this->setFractions(num_rounded, vars, fractions, max_denom > 250 );
                        printf("Now %10d of %10d variables and %10d of %10d constraints are remaining.\n",  this->num_remaining_variables, this->num_variables, this->num_remaining_constraints, this->num_constraints );

                        // CPXcopybase(CPXCENVptr env, CPXLPptr lp, int const * cstat, int const * rstat)
                        CPXcompletelp(this->env, this->lpx);
                        // printf("Writing to adage%d.lp...", rounding_stage);
                        // fflush(stdout);
                        // char* buffer = new char[30];
                        // sprintf(buffer,"adage%d.lp", rounding_stage);
                        // status = CPXwriteprob(env, lpx, buffer, "LP");
                        // printf("done.\n");
                        // delete[] buffer;

                        if ( this->num_remaining_variables > 0 )
                        {
                            CPXFILEptr logfile = CPXfopen("adage.log","w");
                            CPXsetlogfile(this->env, logfile);

                            if( !(this->use_simplex) )
                            {
                                printf("Solving with barrier method...");
                                fflush(stdout);
                                status = CPXbaropt(this->env, this->lpx);
                                printf("done!\n");
                                fflush(stdout);

                                if (  status != 0 )
                                {
                                    // error was thrown?
                                    printf("CPXbaropt ended with status %d!\n", status);
                                    exit(1);
                                }
                            }
                            else
                            {
                                printf("Solving with primal simplex method...");
                                fflush(stdout);
                                CPXgetbase(this->env, this->lpx, NULL, NULL);
                                status = CPXprimopt(this->env, this->lpx);
                                printf("done!\n");

                                if (  status != 0 )
                                {
                                    // error was thrown?
                                    printf("CPXprimopt ended with status %d!\n", status);
                                    exit(1);
                                }
                            }

                            CPXfclose(logfile);

                            lpstat = CPXgetstat(this->env, this->lpx);
                            printf("CPX*opt ended with lpstat %d!\n", lpstat);

                            if ( lpstat == CPX_STAT_INFEASIBLE )
                            {
                                printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n!!!!!!!!!!!!!LP is infeasible!!!!!!!!\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
                                this->is_feasible = false;
                                delete[] vars;
                                delete[] fractions;
                                return;
                            }
                            else
                            {
                                this->is_feasible = true;
                            }

                            status = CPXsolution(this->env, this->lpx, &lpstat, NULL, this->solution, NULL, NULL, NULL);  
                        }

                        // reset denominator!
                        max_denom = 0;
                        temp_error_term = error_term;
                    	num_rounded = 0;
                    }
                    else 
                    {
                        temp_error_term = 1.25 * temp_error_term;
                    }

                }
            }


            delete[] vars;
            delete[] fractions;
        }
	}

    printf("Maximum error with w : %lld / %lld == %lf\n", this->max_constraint_error_with_w.a, this->max_constraint_error_with_w.b, (double)this->max_constraint_error_with_w.a / (double)this->max_constraint_error_with_w.b  );
    printf("Maximum error without w : %lld / %lld == %lf\n", this->max_constraint_error_without_w.a, this->max_constraint_error_without_w.b, (double)this->max_constraint_error_without_w.a / (double)this->max_constraint_error_without_w.b  );
}


void LinearProgramCPLEX::addConstraint(LinearConstraint* constraint)
{
    char* buffer = constraint->getString();

    if ( buffer != 0 )
    {  
        buffer = (char*)realloc(buffer, strlen(buffer)+2);

        if ( this->constraint_trie->contains(buffer) == false )
        {
            this->constraint_trie->insert(buffer);

            if ( this->num_linearconstraints >= this->size_constraint_list )
            {
                this->size_constraint_list = this->size_constraint_list * 2;
                this->constraint_list = (LinearConstraint**)realloc(this->constraint_list, this->size_constraint_list * sizeof(LinearConstraint*));
                this->constraint_active = (char*)realloc(this->constraint_active, this->size_constraint_list);
                this->constraint_forward_lookup = (int*)realloc(this->constraint_forward_lookup, this->size_constraint_list * sizeof(int));
            }

            this->constraint_list[this->num_linearconstraints] = constraint->duplicate();
            this->constraint_active[this->num_linearconstraints] = 1;
            this->constraint_forward_lookup[this->num_linearconstraints] = this->num_linearconstraints;
            (this->num_linearconstraints)++;

            // TODO: Test for monomials not indexed
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

            char* sense = new char[1]; // senses
            double* rhs = new double[1]; // right hand side
            int *rmatbeg = new int[1]; // where do rows begin in sparse representation?
            int *rmatind = new int[num_monomials]; // what columns are used?
            double *rmatval = new double[num_monomials]; //  what values are used

            Monomial* m = constraint->getMonomial(0);

            if ( constraint->getWCoeff() != 0 )
            {
                rmatind[0] = 0;
                rmatval[0] = constraint->getWCoeff();
                num_nonzero_monomials++;
            }

            double* lb = new double[num_monomials];
            double* ub = new double[num_monomials];
            int num_to_add = 0;
            do
            {
                // something
                char* key = m->getBasicString();

                // Careful: the last monomial has an empty variable!
                if ( key != 0 && m->getCoefficient() != 0 )
                {
                    bool has_var = this->variables->contains(key);
	                if ( !has_var )
	                {
	                	this->variables->insert(key, this->num_variables);

                        if ( this->num_variables >= this->size_variables )
                        {
                            this->size_variables = this->size_variables * 2;
                            this->variable_list = (char**)realloc(this->variable_list, this->size_variables * sizeof(char*));
                        }

                        this->variable_list[this->num_variables] = key;

                        (this->num_variables)++;

                        // no need to discharge more than this, right?
                        lb[num_to_add] = -CPX_INFBOUND; //-10.0
                        // ub[num_to_add] = CPX_INFBOUND; //10.0;

	                	num_to_add++;
	                }

                	rmatval[num_nonzero_monomials] = m->getCoefficient();
                	rmatind[num_nonzero_monomials] = this->variables->getValue(key);
                	num_nonzero_monomials++;

                    if ( has_var )
                    {
                        free(key);
                    }
                }
                
                m = m->getNext();
            } while ( m != 0 );


            if ( num_to_add > 0 )
            {
                // Insert variables into CPLEX
                // Lower Bounds: -INFINITY
                // Upper Bounds: INFINITY
                // Objective Values: 0
                // ADD lb
                int status = CPXnewcols(this->env, this->lpx, num_to_add, NULL, lb, NULL, NULL, NULL);

                // Test status?
               	if ( status != 0 )
               	{
               		printf("CPXnewcols returned status %d!\n", status);

               		printf("num_to_add = %d\n", num_to_add);

               		exit(1);
               	}

               	while ( this->variables->getSize() > this->size_solution )
               	{
               		this->size_solution = this->size_solution * 2;

               		this->solution = (double*)realloc(this->solution, this->size_solution * sizeof(double));
               		this->exact_solution = (fraction*)realloc(this->exact_solution, this->size_solution * sizeof(fraction));
               	}
            }

            // Insert constraint into CPLEX
            
            if ( constraint->getInequality() == INEQUALITY_GEQ )
            {
                sense[0] = 'G';
            }
            if ( constraint->getInequality() == INEQUALITY_LEQ )
            {
                sense[0] = 'L';
            }
            if ( constraint->getInequality() == INEQUALITY_EQ )
            {
                sense[0] = 'E';
            }


            rmatbeg[0] = 0;
            rhs[0] = (double)constraint->getRHS();

            int status = CPXaddrows(this->env, this->lpx, 0, 1, num_nonzero_monomials, rhs, sense, rmatbeg, rmatind, rmatval, NULL, NULL);
            if ( status != 0 )
            {
            	printf("CPXaddrows returned status %d!\n", status);
            	exit(1);
            }

            (this->num_constraints)++;

            if ( (this->num_constraints % 10000) == 0 )
            {
                printf("...%d constraints.\n", this->num_constraints);
                fflush(stdout);
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


            if ( num_monomials > 0 )
            {
                delete lb;
                delete ub;
                delete rmatind;
                delete rmatval;
                lb = 0;
                ub = 0;
            }
            delete sense;
            delete rhs;
            delete rmatbeg;
        }
        else
        {   
            free(buffer);
        }
    }

    delete constraint;
    constraint = 0;
}


void LinearProgramCPLEX::setFractions(int num_vars, int* vars, fraction* fractions, bool add_gaps )
{
    int removed_gaps = 0;

    for ( int i = 0; i < num_vars; i++ )
    {
        if ( vars[i] < this->num_variables )
        {
            this->exact_solution[vars[i]] = fractions[i];

            (this->num_remaining_variables)--;

            printf("%35s = %10lld / %10lld == %2.10lf =/= %2.10lf\n", this->variable_list[vars[i]], this->exact_solution[vars[i]].a, this->exact_solution[vars[i]].b, (double) this->exact_solution[vars[i]].a / (double) this->exact_solution[vars[i]].b, this->solution[this->variable_forward_lookup[vars[i]]] );
        }
    }
    printf("\n");
    fflush(stdout);

    for ( int i = 0; i < num_vars; i++ )
    {
        int status = CPXdelcols(this->env, this->lpx, this->variable_forward_lookup[vars[i]], this->variable_forward_lookup[vars[i]]);

        if ( status != 0 )
        {
            fprintf(stderr, "Problem during CPXdelcols A: %d\n", status);
            printf("Removing column vars[%d] = %d, but the forward lookup is %d and with modifications is %d\n", i, vars[i], this->variable_forward_lookup[vars[i]], this->variable_forward_lookup[vars[i]]);
            exit(1);
        }

        int cur_col_idx = this->variable_forward_lookup[vars[i]];
        this->variable_forward_lookup[vars[i]] = -1;

        int max_vars = this->num_variables;

        if ( this->has_gap_columns )
        {
            max_vars += this->num_constraints;
        }

        for ( int j = vars[i] + 1; j < max_vars; j++ )
        {
        	if ( this->variable_forward_lookup[j] >= 0 )
        	{
                this->solution[cur_col_idx] = this->solution[this->variable_forward_lookup[j]];
                this->variable_forward_lookup[j] = cur_col_idx;
                cur_col_idx++;
        	}
        }
    }

    int status = 0;

    int num_set_by_equality = 0;
    int size_equality = 100;
    int* eq_vars = (int*)malloc(size_equality*sizeof(int));
    fraction* eq_fracs = (fraction*)malloc(size_equality*sizeof(fraction));

    int* rhs_indices = new int[1];
    double* rhs = new double[1];
    for ( int i = 0; i < this->num_constraints; i++ )
    {
    	if ( this->constraint_active[i] == 0 )
    	{
    		continue;
    	}

        // modify the RHS for this constraint based on the exact variable values!
        LinearConstraint* c = this->constraint_list[i];

        fraction r = fraction( c->getRHS(), 1);

        bool has_w = (c->getWCoeff() != 0 );
        r = r - ( fraction(c->getWCoeff(),1) * this->exact_optimum );

        int num_variables_open = 0;
        int last_open_var = -1;
        fraction last_coeff = fraction(0,1);

        for ( int j = 0; j < c->getNumMonomials(); j++ )
        {
            Monomial* mon = c->getMonomial(j);

            if ( mon->getVariableName() != 0 )
            {
                char* key = mon->getBasicString();
                int var_i = this->variables->getValue(key);
                free(key);

                if ( var_i < 0 )
                {
                    continue;
                }
                
                fraction coeff = fraction(mon->getCoefficient(),1);
                fraction value = this->exact_solution[var_i];

                if ( value.b != 0 )
                {
                    // we have a known value
                    // so remove its contribution from LHS and RHS
                    r = r - (coeff * value);
                }
                else
                {
                    num_variables_open++;
                    last_open_var = var_i;
                    last_coeff = coeff;
                }
            }
        }

        // modify the right-hand side, no matter what!
        rhs_indices[0] = this->constraint_forward_lookup[i];

        fraction tr = r;
        while ( tr.b > 50000 )
        {
            tr.a = tr.a >> 1;
            tr.b = tr.b >> 1;
        }
        rhs[0] = (double)tr.a / (double)tr.b;



        // printf("assigning %lf to the rhs of constraint %d\n", rhs[0], i);
        status = CPXchgrhs(this->env, this->lpx, 1, rhs_indices, rhs);    

        if ( status != 0 )
        {
            printf("CPXchgrhs returned status %d when changing row %d (%lf)\n", status, rhs_indices[0], rhs[0]);
        }  

        if ( num_variables_open == 0 )
        {
            // our constraint looks like: 0 >= RHS. Is that consistent?
            if ( r > fraction(0,1) )
            {
                if ( has_w && r > this->max_constraint_error_with_w )
                {
                    this->max_constraint_error_with_w = r;
                    printf("\nConstraint %d appears to be inconsistent with RHS %lld / %lld == %lld / %lld == %lf \n", i, r.a, r.b, tr.a, tr.b,  (double)tr.a / (double)tr.b);
                    char* s = c->getString();

                    printf("%s\n", s);
                    fflush(stdout);

                    free(s);    
                }
                else if ( r > this->max_constraint_error_without_w )
                {
                    this->max_constraint_error_without_w = r;
                    printf("\nConstraint %d appears to be inconsistent with RHS %lld / %lld == %lld / %lld == %lf \n", i, r.a, r.b, tr.a, tr.b, (double)tr.a / (double)tr.b);
                    char* s = c->getString();

                    printf("%s\n", s);
                    fflush(stdout);

                    free(s);
                }
            }

            this->constraint_active[i] = 0;
            (this->num_remaining_constraints)--;

            // remove the variable from the problem!
            if ( this->has_gap_columns )
            {
                // remove this variable!       // add it to the list!
                if ( num_set_by_equality >= size_equality )
                {
                    size_equality += 100;
                    eq_vars = (int*)realloc(eq_vars, size_equality * sizeof(int));
                    eq_fracs = (fraction*)realloc(eq_fracs, size_equality * sizeof(fraction));
                }

                eq_vars[num_set_by_equality] = this->num_variables + i;
                eq_fracs[num_set_by_equality] = fraction(0,1);
                num_set_by_equality++;
            }

            CPXdelrows(this->env, this->lpx, this->constraint_forward_lookup[i], this->constraint_forward_lookup[i]);

            int cur_row_indx = this->constraint_forward_lookup[i];

            this->constraint_forward_lookup[i] = -1;
            for ( int ii = i + 1; ii < this->num_constraints; ii++ )
            {
                if ( this->constraint_forward_lookup[ii] >= 0 )
                {
                    this->constraint_forward_lookup[ii] = cur_row_indx;
                    cur_row_indx++;
                }
            }

        }
        else if ( num_variables_open == 1 )
        {
            // check solution for being close or not... if within eps, then set with equality!
            double approx = this->solution[this->variable_forward_lookup[last_open_var]];

            int max_denom = this->exact_optimum.b;

            while ( fraction::bestApproximationFactor(approx,  max_denom) > 0.000001  && max_denom < 10000 ) 
            {
                max_denom *= 1.5;
            }

            fraction f = fraction::makeFraction( approx,  max_denom);


            // if close enough...
            if ( r - (last_coeff * f) >= fraction(0,1) )
            {
                // printf("r - c*f= %lld / %lld (c = %d, f = %lld/%lld)\n", (r - (last_coeff * f)).a, (r - (last_coeff * f)).b, last_coeff.a, f.a, f.b);
                // using later rounding could make the constraint inconsistent!
                // so let's set the fraction with _equality_
                if ( r - (last_coeff * f) > fraction(0,1) )
                {
                    f = (r / last_coeff);
                }

                // look for this variable in the list!
                bool found = false;
                for ( int j = 0; !found && j < num_set_by_equality; j++ )
                {
                    if ( eq_vars[j] == last_open_var )
                    {   
                        found = true;
                        // we need r - last_coeff*f <= 0, so we want last_coeff*f to be as BIG possible...

                        if ( last_coeff * f > last_coeff * eq_fracs[j] )
                        {
                            eq_fracs[j] = f;
                        }
                        else
                        {
                            // this one is still good!
                        }
                    }
                }

                if ( !found )
                {
                    // add it to the list!
                    if ( num_set_by_equality >= size_equality )
                    {
                        size_equality += 100;
                        eq_vars = (int*)realloc(eq_vars, size_equality * sizeof(int));
                        eq_fracs = (fraction*)realloc(eq_fracs, size_equality * sizeof(fraction));
                    }

                    eq_vars[num_set_by_equality] = last_open_var;
                    eq_fracs[num_set_by_equality] = f;
                    num_set_by_equality++;
                }
            }
        }
    }

    if ( num_set_by_equality > 0 )
    {
        this->setFractions(num_set_by_equality, eq_vars, eq_fracs, add_gaps);
    }

    free(eq_vars);
    free(eq_fracs);

    if ( num_set_by_equality > 0 )
    {
        // the rest was done already!
        delete[] rhs;
        delete[] rhs_indices;
        return;
    }

    if ( this->num_remaining_variables == 0 )
    {
        delete[] rhs;
        delete[] rhs_indices;
        return;
    }

	if ( !this->use_simplex && add_gaps && this->has_gap_columns == false )
    {
        printf("Adding gap variables...\n");
        fflush(stdout);
        // first, we will add "gap" columns!
        double* opt = new double[this->num_constraints];
        double* ub = new double[this->num_constraints];
        int cur_gap_var = 0;
        for ( int i = 0; i < this->num_constraints; i++ )
        {
            if ( this->constraint_active[i] >= 1 )
            {
                opt[cur_gap_var] = 1.0;

                ub[cur_gap_var++] = 1.0 / (double)this->exact_optimum.b;
            }
        }

        int status = CPXnewcols(this->env, this->lpx, cur_gap_var, opt, NULL, ub, NULL, NULL);

        if ( status != 0 )
        {
            printf("CPXnewcols returned status %d!\n", status);
            exit(1);
        }
        delete[] opt;
        delete[] ub;

        double* vallist = new double[this->num_constraints];
        int* rowlist = new int[this->num_constraints];
        int* collist = new int[this->num_constraints];
        char* sense = new char[this->num_constraints];
        int* colbase = new int[this->num_variables + this->num_constraints];
        int* rowbase = new int[this->num_constraints];

        for ( int i = 0; i < this->num_remaining_variables; i++ )
        {
            colbase[i] = CPX_FREE_SUPER;
        }

        int cur_col_idx = this->num_remaining_variables;
        cur_gap_var = 0;
        for ( int i = 0; i < this->num_constraints; i++ )
        {
            rowbase[i] = CPX_AT_LOWER;
            if ( this->constraint_active[i] >= 1 )
            {
                this->variable_forward_lookup[this->num_variables + i] = cur_col_idx;
                vallist[cur_gap_var] = -1.0; // want to maximize the GAP for a >= constraint
                rowlist[cur_gap_var] = this->constraint_forward_lookup[i];
                collist[cur_gap_var] = cur_col_idx;
                sense[cur_gap_var] = 'E';
                // this->constraint_active[i] = 2; // superactive!

                colbase[cur_col_idx] = CPX_BASIC; // basic!

                cur_gap_var++;
                cur_col_idx++;
            }
            else
            {
                this->variable_forward_lookup[this->num_variables + i] = -1;
            }
        }

        status = CPXchgcoeflist(this->env, this->lpx, cur_gap_var, rowlist, collist, vallist);
        if ( status != 0 )
        {
            printf("CPXnewcols returned status %d!\n", status);
            exit(1);
        }

        // status = CPXchgsense(this->env, this->lpx, cur_gap_var, rowlist, sense);
        // if ( status != 0 )
        // {
        //     printf("CPXchgsense returned status %d!\n", status);
        //     exit(1);
        // }

        // status = CPXcopybase(this->env, this->lpx, colbase, rowbase);
        // if ( status != 0 )
        // {
        //     printf("CPXcopybase returned status %d!\n", status);
        //     exit(1);
        // }
        this->has_gap_columns = true;

        delete[] vallist;
        delete[] rowlist;
        delete[] collist;
        delete[] colbase;
        delete[] rowbase;
        delete[] sense;
    }

    delete[] rhs;
    delete[] rhs_indices;

    // now the problem is updated!
}




void LinearProgramCPLEX::importArguments(int argc, char** argv)
{
    this->exact_optimum = fraction(-1,1);

    for ( int i = 0; i < argc; i++ )
    {
        if ( strcmp(argv[i], "--exact") == 0 )
        {
            this->do_rounding = true;
        }
        if ( strcmp(argv[i], "--simplex") == 0 )
        {
            this->use_simplex = true;
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
        if ( i < argc - 2 && strcmp(argv[i], "-w") == 0 )
        {
            this->exact_optimum.a = atoi(argv[i+1]);
            this->exact_optimum.b = atoi(argv[i+2]);
    
        }
    }
}

