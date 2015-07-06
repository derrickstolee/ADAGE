/**
 * constraint-merge.cpp
 *
 * The constraint-merge.exe program reads all of the solutions from hex-constraints.exe and merges configurations that correspond
 * to the same constraint. It also outputs the necessary constraints.
 *
 * If there are face-to-vertex rules, then it outputs the necessary constraints for those rules (where each vertex results in w charge,
 * and each element starts with charge 1.)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Rule.hpp"
#include "Grid.hpp"
#include "Configuration.hpp"
#include "HexagonalGrid.hpp"
#include "Trie.hpp"

using namespace adage::grids;

 int main(int argc, char** argv)
 {
 	// Grid* grid = new HexagonalGrid(6);
 	char* rule_file = 0;	
 	char* constraint_file = 0;
 	bool slow_mode = false;

 	for ( int i = 1; i < argc - 1; i++ )
 	{
 		if ( strcmp(argv[i], "--rules") == 0 )
 		{
 			rule_file = argv[i+1];
 		}
 		if ( strcmp(argv[i], "--slow") == 0 )
 		{
 			slow_mode = true;
 		}
 		if ( strcmp(argv[i], "--constraints") == 0 )
 		{
 			constraint_file = argv[i+1];
 		}
 	}

 	if ( constraint_file == 0 )
 	{
 		printf("Usage: ./constraint-merge.exe (--rules [file]) --constraints [file] \n");
 		exit(1);
 	}

 	// One thing: print the constraints,
 	// Second thing: save the constraint ids!

 	// Now, load the rules.
 	// FILE* rules = fopen(rule_file, "r");

 	size_t buflen = 1000;
 	char* buffer = (char*)malloc(buflen);

 	// Now, we load the constraint file! BIG!
 	FILE* constraints = fopen(constraint_file, "r");

 	if ( !slow_mode )
 	{
 		FILE* constraintout = fopen("constraints-out.txt","w");

 		Trie* trie = new Trie();

	 	while ( getline(&buffer, &buflen, constraints) > 0 )
	 	{
	 		if ( buffer[0] == 'S' || buffer[0] == 'T' || buffer[0] == 'C' || buffer[0] == 'P' || buffer[0] == 'J')
	 		{
	 			// these are special!
	 			continue;
	 		}
	 		else if ( trie->insert(buffer) )
	 		{
	 			fprintf(constraintout, "%s", buffer);
	 		}
	 	}
	 	fclose(constraints);

	 	// read a line, and then open the file!
	 	while ( getline(&buffer, &buflen, stdin) > 0 )
	 	{
	 		buffer[strlen(buffer)-1] = 0;
	 		FILE* constraints = fopen(buffer, "r");

	 		if ( constraints )
	 		{
			 	while ( getline(&buffer, &buflen, constraints) > 0 )
			 	{	
			 		if ( buffer[0] == 'S' || buffer[0] == 'T' || buffer[0] == 'C' || buffer[0] == 'P' || buffer[0] == 'J')
			 		{
			 			// these are special!
			 			continue;
			 		}
			 		else if ( trie->insert(buffer) )
			 		{
			 			fprintf(constraintout, "%s", buffer);
			 		}
			 	}
			 	fclose(constraints); 
			 }
	 	}

	 	fclose(constraintout);

 		delete trie;
 	}
 	else
 	{
 		// SLOW MODE! ALSO, INEXACT!
 		FILE* constraintout = fopen("constraints-out.txt","w");
 		int max_size = 10000000;
 		int cur_num_out = 0;
 		int reload_size = 1000000;
 		Trie* trie = new Trie();

 		while ( getline(&buffer, &buflen, constraints) > 0 )
	 	{
	 		if ( buffer[0] == 'S' || buffer[0] == 'T' || buffer[0] == 'C' || buffer[0] == 'P' || buffer[0] == 'J')
	 		{
	 			// these are special!
	 			continue;
	 		}
	 		else if ( trie->insert(buffer) )
	 		{
	 			cur_num_out++;
	 			fprintf(constraintout, "%s", buffer);
	 		}

	 		if ( trie->getSize() > max_size )
	 		{
	 			delete trie;
	 			trie = new Trie();

	 			fclose(constraintout);

	 			constraintout = fopen("constraints-out.txt","r");

	 			int count_in = 0;
		 		while ( getline(&buffer, &buflen, constraintout) > 0 )
		 		{
		 			count_in++;

		 			if ( count_in > cur_num_out - reload_size )
		 			{
		 				trie->insert(buffer);
		 			}
		 		}

		 		fclose(constraintout);
	 			constraintout = fopen("constraints-out.txt","a"); // APPEND!
	 		}
 		}
	 	fclose(constraints);
	 	fclose(constraintout);
 	}

 	return 0;
 }