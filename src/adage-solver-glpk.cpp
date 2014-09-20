/*
 * adage-solver-glpk.cpp
 *
 *  Created on: Sep 1, 2013
 *      Author: stolee
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "Trie.hpp"

#include "constraintset.hpp"
#include "ADAGELinearProgram.hpp"

int main(int argc, char** argv)
{
	char* filename = argv[1];

	bool do_solve = true;
	FILE* infile = fopen(filename, "r");

	ADAGELinearProgram* lp = new ADAGELinearProgram();
	bool minimize_density = true;

	for ( int i = 0; i < argc; i++ )
	{
		if ( strcmp(argv[i], "min") == 0 )
		{
			minimize_density = true;
		}

		if ( strcmp(argv[i], "max") == 0 )
		{
			minimize_density = false;
		}
		if ( strcmp(argv[i], "--nosolve") == 0 )
		{
			// just build adage.lp!
			do_solve = false;
		}

	}

	// When we see a new variable, we add its label to the trie!
	// Let's add w as variable 0!
	Trie* variable_trie = new Trie();

	char* wvar = (char*)malloc(2);
	strcpy(wvar, "w");
	char* wkey = (char*)malloc(3);
	strcpy(wkey, "-1");
	variable_trie->insert(wvar, wkey);

	constraintset* c = new constraintset();
	initConstraintSet(c);

	int cur_index = 0;
	int* indices = (int*) malloc(100000 * sizeof(int));
	int* coeffs = (int*) malloc(100000 * sizeof(int));

	size_t buflen = 1000;
	char* buffer = (char*)malloc(buflen);
	char next;
	bool done = false;
	int count = 0;
	int count_increment = 100000;

	time_t start_time = time(NULL);

	FILE* variable_out = fopen("variables.txt", "w");

	while ( getline(&buffer, &buflen, infile) > 0 )
	{
		// ok, we have a line. Let's figure it out!
		int start = 0;
		int rhs = 1;
		int l = strlen(buffer) - 1;
		int buf_pos = l;
		cur_index = 0;

		for ( int i = start; i < l; i++ )
		{
			// ok, interpret characters one by one
			if ( buffer[i] == 'w' )
			{
				indices[cur_index] = atoi(variable_trie->getValue("w")); 

				int start_c = i - 1;
				int end_num = i;
				while ( start_c >= start && buffer[start_c] != '-' && buffer[start_c] != '+'  && buffer[start_c] != ':' )
				{
					if ( end_num == i && (buffer[start_c] == '*' || buffer[start_c] == 'x') )
					{
						end_num = start_c;
					}
					start_c--;
				}

				bool number_exists = false;
				for ( int j = start_c+1; j < end_num; j++ )
				{
					if ( buffer[j] >= '0' && buffer[j] <= '9' )
					{
						number_exists = true;
					}
				}

				buffer[end_num] = 0;
				if ( number_exists )
				{
					coeffs[cur_index] = atoi(buffer + (start_c + 1));
				}
				else
				{
					coeffs[cur_index] = 1;
				}

				if ( buffer[start_c] == '-' )
				{
					coeffs[cur_index] *= -1;
				}

				// printf("+ %d * w", coeffs[cur_index]);

				cur_index++;
			}
			else if ( buffer[i] == '(' )
			{
				// This is a kernelized variable!
				// ok, we must determine what the index and the coefficient are..
				int end = i + 2;
				while ( end < buf_pos && buffer[end] != ')' )
				{
					end++;
				}
				end++;

				// give it an end
				buffer[end] = 0;

				int varstart = i;
				while ( varstart >= 0 && buffer[varstart] != ' ' )
				{
					varstart--;
				}

				char* varname = (char*)malloc(end - varstart);
				strcpy(varname, buffer + varstart);

				if ( variable_trie->contains(varname) == false )
				{
					char* variable_key = (char*)malloc(16);
					indices[cur_index] = variable_trie->getSize() - 1;
					sprintf(variable_key,"%d", variable_trie->getSize() - 1);
					variable_trie->insert(varname, variable_key);

					fprintf(variable_out, "%s = x%s\n", varname, variable_key);
				}
				else
				{
					// start after x[ and go until end.
					indices[cur_index] = atoi(variable_trie->getValue(varname));
					free(varname);
				}

				int start_c = i - 1;
				int end_num = i;
				while ( start_c >= start && buffer[start_c] != '-' && buffer[start_c] != '+' )
				{
					if ( end_num == i && buffer[start_c] == '*' )
					{
						end_num = start_c;
					}
					start_c--;
				}

				bool number_exists = false;
				for ( int j = start_c; j < end_num; j++ )
				{
					if ( buffer[j] >= '0' && buffer[j] <= '9' )
					{
						number_exists = true;
					}
				}

				buffer[end_num] = 0;
				if ( buffer[start_c] == '-' )
				{
					if ( !number_exists )
					{
						coeffs[cur_index] = -1;
					}
					else
					{
						coeffs[cur_index] = -atoi(buffer + (start_c + 1));
					}
				}
				else if ( number_exists )
				{
					coeffs[cur_index] = atoi(buffer + (start_c + 1));
				}
				else
				{
					coeffs[cur_index] = 1;
				}

				// printf(" + %d * x[%d] ", coeffs[cur_index], indices[cur_index]);

				cur_index++;
			}
			else if ( buffer[i] == '>' )
			{
				// add a constraint and a RHS!
				// has a division?
				
				char* astart = buffer + i + 2;

				char* negative = strchr(astart, '-');

				if ( negative != 0 )
				{
					astart = negative + 1;
				}

				int a = atoi(astart);

				if ( negative != 0 )
				{
					a = -a;
				}

				char* division = strchr(buffer + i + 2, '/');

				int b = 1;

				if ( division != 0 )
				{
					b = atoi(division + 1);
				}

				// multiply everything by b to have all-integer things!
				if ( b > 1 )
				{
					for ( int j = 0; j < cur_index; j++ )
					{
						coeffs[j] *= b;
					}
				}

				rhs = a;

				// printf(">= %d / %d\n", a, b);
			}
			else if ( buffer[i] == '<' )
			{
				char* astart = buffer + i + 2;

				char* negative = strchr(astart, '-');

				if ( negative != 0 )
				{
					astart = negative + 1;
				}

				int a = atoi(astart);

				if ( negative != 0 )
				{
					a = -a;
				}

				char* division = strchr(buffer + i + 2, '/');

				int b = 1;

				if ( division != 0 )
				{
					b = atoi(division + 1);
				}

				// multiply everything by b to have all-integer things!
				if ( b > 1 )
				{
					for ( int j = 0; j < cur_index; j++ )
					{
						coeffs[j] *= b;
					}
				}

				rhs = a;
			}
		}

		if ( cur_index > 0 )
		{
			// printf("\n");
			// printf( "%d w?\n", coeffs[cur_index-1]);

			addConstraint(c, cur_index, indices, coeffs, rhs);

			count++;

			if ( (count % count_increment) == 0 )
			{
				printf("...added %d constraints in %d seconds.\n", count, time(NULL) - start_time);
				fflush(stdout);

				if ( count >= count_increment * 12 )
				{
					count_increment *= 10;
				}
			}
		}
	}

	printf("...done in %d seconds.\n\n\n", time(NULL) - start_time);
	fflush(stdout);
	fclose(infile);
	fclose(variable_out);

	double value = lp->solveLP(c, minimize_density, do_solve);


	if ( ! do_solve )
	{
		return 0;
	}


	if ( minimize_density )
	{
		printf("MAXIMUM Lower Bound: ");
	}
	else
	{
		printf("MINIMUM Upper Bound: ");
	}

	printf("w = %01.10f\n", value);


	if ( value > 0 )
	{
		lp->convertToFractions(c);


		FILE* variable_in = fopen("variables.txt", "r");
		FILE* variable_value = fopen("variable_values.txt", "w");
		printf("\nValues on the Variables:\n");
		buffer = 0;
		buflen = 0;
	    while ( getline(&buffer, &buflen, variable_in) > 0  )
		{
			*(buffer + strlen(buffer)-1) = 0; // newline

			char* key = buffer;

			char* value = strstr(buffer, "=") + 2;


			*(value - 3) = 0;

			int i = atoi(value+1);

			printf("%s = %s = %4d / %4d == %1.10lf\n", key, value, c->variable_numerators[i], c->variable_denominators[i], c->variable_values[i]);
			fprintf(variable_value, "%s = %s = %4d / %4d == %1.10lf\n", key, value, c->variable_numerators[i], c->variable_denominators[i], c->variable_values[i]);
		}

		fflush(stdout);
		fclose(variable_in);
		fclose(variable_value);
	}

	delete lp;

	freeConstraintSet(c);
	delete c;

	time_t endtime = time(NULL);

	printf("LP took %lld seconds.\n", endtime-start_time);

	return 0;
}

