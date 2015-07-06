/*
 * grid-rules.cpp
 *
 *  Created on: Jul 2, 2015
 *      Author: stolee
 */

#include "Grid.hpp"
#include "Rule.hpp"
#include "Configuration.hpp"
#include "HexagonalGrid.hpp"
#include "HexagonalGridNoRotate.hpp"
#include "SquareGrid.hpp"
#include "TriangularGrid.hpp"
#include "PentagonalGrid.hpp"
#include "KingsGrid.hpp"
#include "LinearProgram.hpp"
#include "IdentifyingCodeConfiguration.hpp"

#include "ConstraintGenerator.hpp"
#include "SharpnessGenerator.hpp"
#include "LinearProgram.hpp"

// #include "Trie.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace adage;
using namespace adage::grids;

/**
 * grid-rules.exe [grid details] 
 *
 * Generate the rules to fill a rule file for a given grid.
 */
int main(int argc, char** argv)
{
	Grid* grid = new HexagonalGrid(20);

	for ( int i = 1; i < argc; i++ )
	{
		if ( strcmp(argv[i],"--hexagonal")== 0)
		{
			delete grid;
			grid = new HexagonalGrid(20);
		}
		if ( strcmp(argv[i],"--hexnorot")== 0)
		{
			delete grid;
			grid = new HexagonalGridNoRotate(20);
		}
		if ( strcmp(argv[i],"--square")== 0)
		{
			delete grid;
			grid = new SquareGrid(20);
		}
		if ( strcmp(argv[i],"--triangular")== 0)
		{
			delete grid;
			grid = new TriangularGrid(20);
		}
		if ( strcmp(argv[i],"--pentagonal")== 0)
		{
			delete grid;
			grid = new PentagonalGrid(20);
		}
		
		if ( strcmp(argv[i],"--tblowup")== 0)
		{
			Grid* oldgrid = grid;
			grid = oldgrid->getTightBlowup();
			delete oldgrid;
		}
		if ( strcmp(argv[i],"--blowup")== 0)
		{
			Grid* oldgrid = grid;
			grid = oldgrid->getBlowup();
			delete oldgrid;
		}
		if ( strcmp(argv[i],"--dual")== 0)
		{
			Grid* oldgrid = grid;
			grid = oldgrid->getDual();
			delete oldgrid;
		}
		if ( strcmp(argv[i],"--snub")== 0)
		{
			Grid* oldgrid = grid;
			grid = oldgrid->getSnub();
			delete oldgrid;
		}
	}

	int max_r = 4;

	Trie<int>* seen_canon = new Trie<int>();

	{
		// d1rX Rules
		int d = 1;

		char num_canon_seen = -1;

		for ( int i = 0; i < grid->getNumVertexOrbits(); i++ )
		{
			int v = grid->getVertexOrbitRepresentative(i);


			for ( int j = 0; j < grid->getVertexDegree(v); j++ )
			{
				int u = grid->neighborVV(v,j);

				Configuration* canon = new Configuration(grid);
				canon->addVertexToShape(v);
				canon->addVertexToShape(u);

				char* canon_str = canon->getCanonicalString();

				if ( seen_canon->contains(canon_str) )
				{
					continue;
				}

				char* new_str = (char*)malloc(strlen(canon_str)+1);
				strcpy(new_str, canon_str);
				seen_canon->insert(new_str, true);	
				num_canon_seen++;


				for ( int r = 1; r <= max_r; r++ )
				{
					Configuration* c = new Configuration(grid);

					c->setCenter(v, true);
					c->addVertexToShape(v);
					c->addVertexToShape(u);

					int ball_size1 = 0;
					int* ball1 = grid->getVertexBall(v, r, ball_size1);

					int ball_size2 = 0;
					int * ball2 = grid->getVertexBall(u, r, ball_size2);

					for ( int k = 0; k < ball_size1; k++ )
					{
						for ( int kk = 0; kk < ball_size2; kk++ )
						{
							if ( ball1[k] == ball2[kk] )
							{
								c->addVertexToShape(ball1[k]);
							}
						}
					}



					printf("#begin RuleShape\n");
					printf("rule_name = \"d%dr%d%c\";\n", d, r, 'a' + num_canon_seen);
					printf("var_name = \"d%dr%d%c\";\n", d, r, 'a' + num_canon_seen);
					printf("rule_type = %d;\n", VARIABLE_TYPE_V2V); 
					printf("keys = [ \\\n");

					for ( int k = 0; k < c->getNumVerticesInShape(); k++ )
					{
						int t = c->getVertexFromShape(k);

						printf("    1 * x[%d], \\\n", t);
					}
					printf("  ];\n");
					printf("center_id = %d;\n", v);
					printf("center_is_face = false;\n");
					printf("chargeable = [ %d ];\n", u);
					printf("kernels = [ \\\n");

					for ( int t = 2; t < c->getNumVerticesInShape(); t += 2 )
					{
						bool first = true;
						int count = 0;
						for ( int k = 0; k < t; k++ )
						{
							int tv = c->getVertexFromShape(k);

							if ( !first )
							{
								printf(" + ");
							}
							first = false;

							printf("1 * x[%d]", tv);
							count++;
						}

						if ( t > 2 )
						{
							printf(" >= %d, \\\n", count - ( (count - 1) / 2 ) );
						}
						else
						{
							printf(" >= %d, \\\n", count );
						}
					}

					printf("  ];\n");

					c->write(stdout);

					printf("#end RuleShape\n\n");

					delete c;
				}

				// face version...
				Configuration* c = new Configuration(grid);
					c->setCenter(v, true);
				c->addVertexToShape(v);
				c->addVertexToShape(u);

				for ( int j = 0; j < grid->getVertexDegree(v); j++ )
				{
					int f = grid->neighborVF(v, j);

					for ( int k = 0; k < grid->getVertexDegree(u); k++ )
					{
						if ( grid->neighborVF(u,k) == f )
						{
							c->addFaceToShape(f);
						}
					}
				}

				for ( int j = 0; j < c->getNumFacesInShape(); j++ )
				{
					int f = c->getFaceFromShape(j);

					for ( int k = 0; k < grid->getFaceDegree(f); k++ )
					{
						int x = grid->neighborFV(f, k);

						if ( c->isVertexInShape(x) == false )
						{
							c->addVertexToShape(x);
						}
					}
				}


				printf("#begin RuleShape\n");
				printf("rule_name = \"d%dface1%c\";\n", d, 'a' + num_canon_seen);
				printf("var_name = \"d%df1%c\";\n", d, 'a' + num_canon_seen);
				printf("rule_type = %d;\n", VARIABLE_TYPE_V2V); 
				printf("keys = [ \\\n");

				for ( int k = 0; k < c->getNumVerticesInShape(); k++ )
				{
					int t = c->getVertexFromShape(k);

					printf("    1 * x[%d], \\\n", t);
				}
				printf("  ];\n");
				printf("center_id = %d;\n", v);
				printf("center_is_face = false;\n");
				printf("chargeable = [ %d ];\n", u);
				printf("kernels = [ \\\n");

					for ( int t = 2; t < c->getNumVerticesInShape(); t += 2 )
					{
						bool first = true;
						int count = 0;
						for ( int k = 0; k < t; k++ )
						{
							int tv = c->getVertexFromShape(k);

							if ( !first )
							{
								printf(" + ");
							}
							first = false;

							printf("1 * x[%d]", tv);
							count++;
						}


						if ( t > 2 )
						{
							printf(" >= %d, \\\n", count - ( (count - 1) / 2 ) );
						}
						else
						{
							printf(" >= %d, \\\n", count );
						}
					}
				printf("  ];\n");
				c->write(stdout);
				printf("#end RuleShape\n\n");

				delete c;


				// d#f2#
				c = new Configuration(grid);
				c->setCenter(v, true);
				c->addVertexToShape(v);
				c->addVertexToShape(u);

				for ( int j = 0; j < grid->getVertexDegree(v); j++ )
				{
					int f = grid->neighborVF(v, j);

					for ( int k = 0; k < grid->getVertexDegree(u); k++ )
					{
						if ( grid->neighborVF(u, k) == f )
						{
							c->addFaceToShape(f);
						}
					}
				}

				for ( int j = 0; j < c->getNumFacesInShape(); j++ )
				{
					int f = c->getFaceFromShape(j);

					for ( int k = 0; k < grid->getFaceDegree(f); k++ )
					{
						int x = grid->neighborFV(f, k);

						if ( c->isVertexInShape(x) == false )
						{
							c->addVertexToShape(x);
						}

						for ( int kk = 0; kk < grid->getVertexDegree(x); kk++ )
						{
							int y = grid->neighborVV(x, kk);

							if ( c->isVertexInShape(y) == false )
							{
								c->addVertexToShape(y);
							}	
						}
					}
				}


				printf("#begin RuleShape\n");
				printf("rule_name = \"d%dface2%c\";\n", d, 'a' + num_canon_seen);
				printf("var_name = \"d%df2%c\";\n", d, 'a' + num_canon_seen);
				printf("rule_type = %d;\n", VARIABLE_TYPE_V2V); 
				printf("keys = [ \\\n");

				for ( int k = 0; k < c->getNumVerticesInShape(); k++ )
				{
					int t = c->getVertexFromShape(k);

					printf("    1 * x[%d], \\\n", t);
				}
				printf("  ];\n");
				printf("center_id = %d;\n", v);
				printf("center_is_face = false;\n");
				printf("chargeable = [ %d ];\n", u);
				printf("kernels = [ \\\n");

				
				for ( int t = 2; t < c->getNumVerticesInShape(); t += 2 )
				{
					bool first = true;
					int count = 0;
					for ( int k = 0; k < t; k++ )
					{
						int tv = c->getVertexFromShape(k);

						if ( !first )
						{
							printf(" + ");
						}
						first = false;

						printf("1 * x[%d]", tv);
						count++;
					}


						if ( t > 2 )
						{
							printf(" >= %d, \\\n", count - ( (count - 1) / 2 ) );
						}
						else
						{
							printf(" >= %d, \\\n", count );
						}
				}

				printf("  ];\n");
				c->write(stdout);
				printf("#end RuleShape\n\n");

				delete c;
			}
		}
	}


	{
		// d2rX Rules
		int d = 2;
		char num_canon_seen = -1;
		for ( int i = 0; i < grid->getNumVertexOrbits(); i++ )
		{
			int v = grid->getVertexOrbitRepresentative(i);

			for ( int j = 0; j < grid->getVertexDegree(v); j++ )
			{
				int between = grid->neighborVV(v,j);

				for ( int jj = 0; jj < grid->getVertexDegree(between); jj++ )
				{
					int u = grid->neighborVV(between, jj);

					bool uv_edge = false;
					for ( int kk = 0; kk < grid->getVertexDegree(u); kk++ )
					{
						if ( grid->neighborVV(u,kk) == v )
						{
							uv_edge = true;
						}
					}

					if ( uv_edge )
					{
						continue;
					}

					Configuration* canon = new Configuration(grid);
					canon->addVertexToShape(v);
					canon->addVertexToShape(between);
					canon->addVertexToShape(u);

				char* canon_str = canon->getCanonicalString();

				if ( seen_canon->contains(canon_str) )
				{
					continue;
				}

				char* new_str = (char*)malloc(strlen(canon_str)+1);
				strcpy(new_str, canon_str);
				seen_canon->insert(new_str, true);	
				num_canon_seen++;


					for ( int r = 1; r <= max_r; r++ )
					{
						Configuration* c = new Configuration(grid);

						c->addVertexToShape(v);
						c->addVertexToShape(u);

						int ball_size1 = 0;
						int* ball1 = grid->getVertexBall(v, r, ball_size1);

						int ball_size2 = 0;
						int * ball2 = grid->getVertexBall(u, r, ball_size2);

						for ( int k = 0; k < ball_size1; k++ )
						{
							for ( int kk = 0; kk < ball_size2; kk++ )
							{
								if ( ball1[k] == ball2[kk] )
								{
									c->addVertexToShape(ball1[k]);
								}
							}
						}



						printf("#begin RuleShape\n");
						printf("rule_name = \"d%dr%d%c\";\n", d, r, 'a' + num_canon_seen);
						printf("var_name = \"d%dr%d%c\";\n", d, r, 'a' + num_canon_seen);
						printf("rule_type = %d;\n", VARIABLE_TYPE_V2V); 
						printf("keys = [ \\\n");

						for ( int k = 0; k < c->getNumVerticesInShape(); k++ )
						{
							int t = c->getVertexFromShape(k);

							printf("    1 * x[%d], \\\n", t);
						}
						printf("  ];\n");
						printf("center_id = %d;\n", v);
						printf("center_is_face = false;\n");
						printf("chargeable = [ %d ];\n", u);
						printf("kernels = [ \\\n");

					
						for ( int t = 2; t < c->getNumVerticesInShape(); t += 2 )
						{
							bool first = true;
							int count = 0;
							for ( int k = 0; k < t; k++ )
							{
								int tv = c->getVertexFromShape(k);

								if ( !first )
								{
									printf(" + ");
								}
								first = false;

								printf("1 * x[%d]", tv);
								count++;
							}


						if ( t > 2 )
						{
							printf(" >= %d, \\\n", count - ( (count - 1) / 2 ) );
						}
						else
						{
							printf(" >= %d, \\\n", count );
						}
						}

						printf("  ];\n");

						c->write(stdout);

						printf("#end RuleShape\n\n");
					}

					// face version...
					Configuration* c = new Configuration(grid);
					c->setCenter(v, true);
					c->addVertexToShape(v);
					c->addVertexToShape(u);

					for ( int j = 0; j < grid->getVertexDegree(v); j++ )
					{
						int f = grid->neighborVF(v, j);

						for ( int k = 0; k < grid->getVertexDegree(u); k++ )
						{
							if ( grid->neighborVF(u,k) == f )
							{
								c->addFaceToShape(f);
							}
						}
					}

					for ( int j = 0; j < c->getNumFacesInShape(); j++ )
					{
						int f = c->getFaceFromShape(j);

						for ( int k = 0; k < grid->getFaceDegree(f); k++ )
						{
							int x = grid->neighborFV(f, k);

							if ( c->isVertexInShape(x) == false )
							{
								c->addVertexToShape(x);
							}
						}
					}

					printf("#begin RuleShape\n");
					printf("rule_name = \"d%dface1%c\";\n", d, 'a' + num_canon_seen);
					printf("var_name = \"d%df1%c\";\n", d, 'a' + num_canon_seen);
					printf("rule_type = %d;\n", VARIABLE_TYPE_V2V); 
					printf("keys = [ \\\n");

					for ( int k = 0; k < c->getNumVerticesInShape(); k++ )
					{
						int t = c->getVertexFromShape(k);

						printf("    1 * x[%d], \\\n", t);
					}
					printf("  ];\n");
					printf("center_id = %d;\n", v);
					printf("center_is_face = false;\n");
					printf("chargeable = [ %d ];\n", u);
					printf("kernels = [ \\\n");

					
					for ( int t = 2; t < c->getNumVerticesInShape(); t += 2 )
					{
						bool first = true;
						int count = 0;
						for ( int k = 0; k < t; k++ )
						{
							int tv = c->getVertexFromShape(k);

							if ( !first )
							{
								printf(" + ");
							}
							first = false;

							printf("1 * x[%d]", tv);
							count++;
						}


						if ( t > 2 )
						{
							printf(" >= %d, \\\n", count - ( (count - 1) / 2 ) );
						}
						else
						{
							printf(" >= %d, \\\n", count );
						}
					}

					printf("  ];\n");
					c->write(stdout);
					printf("#end RuleShape\n\n");

					delete c;


					// d#f2#
					c = new Configuration(grid);
					c->setCenter(v, true);
					c->addVertexToShape(v);
					c->addVertexToShape(u);

					for ( int j = 0; j < grid->getVertexDegree(v); j++ )
					{
						int f = grid->neighborVF(v, j);

						for ( int k = 0; k < grid->getVertexDegree(u); k++ )
						{
							if ( grid->neighborVF(u,k) == f )
							{
								c->addFaceToShape(f);
							}
						}
					}

					for ( int j = 0; j < c->getNumFacesInShape(); j++ )
					{
						int f = c->getFaceFromShape(j);

						for ( int k = 0; k < grid->getFaceDegree(f); k++ )
						{
							int x = grid->neighborFV(f, k);

							if ( c->isVertexInShape(x) == false )
							{
								c->addVertexToShape(x);
							}

							for ( int kk = 0; kk < grid->getVertexDegree(x); kk++ )
							{
								int y = grid->neighborVV(x, kk);

								if ( c->isVertexInShape(y) == false )
								{
									c->addVertexToShape(y);
								}	
							}
						
						}
					}


					printf("#begin RuleShape\n");
					printf("rule_name = \"d%dface2%c\";\n", d, 'a' + num_canon_seen);
					printf("var_name = \"d%df2%c\";\n", d, 'a' + num_canon_seen);
					printf("rule_type = %d;\n", VARIABLE_TYPE_V2V); 
					printf("keys = [ \\\n");

					for ( int k = 0; k < c->getNumVerticesInShape(); k++ )
					{
						int t = c->getVertexFromShape(k);

						printf("    1 * x[%d], \\\n", t);
					}
					printf("  ];\n");
					printf("center_id = %d;\n", v);
					printf("center_is_face = false;\n");
					printf("chargeable = [ %d ];\n", u);
					printf("kernels = [ \\\n");

					
					for ( int t = 2; t < c->getNumVerticesInShape(); t += 2 )
					{
						bool first = true;
						int count = 0;
						for ( int k = 0; k < t; k++ )
						{
							int tv = c->getVertexFromShape(k);

							if ( !first )
							{
								printf(" + ");
							}
							first = false;

							printf("1 * x[%d]", tv);
							count++;
						}


						if ( t > 2 )
						{
							printf(" >= %d, \\\n", count - ( (count - 1) / 2 ) );
						}
						else
						{
							printf(" >= %d, \\\n", count );
						}
					}

					printf("  ];\n");
					c->write(stdout);
					printf("#end RuleShape\n\n");

					delete c;
				}
			}
		}
	}

	{
		// d3rX Rules
		int d = 3;
		char num_canon_seen = -1;
		for ( int i = 0; i < grid->getNumVertexOrbits(); i++ )
		{
			int v = grid->getVertexOrbitRepresentative(i);

			for ( int j = 0; j < grid->getVertexDegree(v); j++ )
			{
				int between1 = grid->neighborVV(v,j);

				for ( int jj = 0; jj < grid->getVertexDegree(between1); jj++ )
				{
					int between2 = grid->neighborVV(between1, jj);


					bool ub2_edge = false;
					for ( int kk = 0; kk < grid->getVertexDegree(between2); kk++ )
					{
						if ( grid->neighborVV(between2,kk) == v )
						{
							ub2_edge = true;
						}
					}

					if ( ub2_edge )
					{
						continue;
					}

					for ( int jjj = 0; jjj < grid->getVertexDegree(between2); jjj++ )
					{
						int u = grid->neighborVV(between2,jjj);

						bool uv_edge = false;
						for ( int kk = 0; kk < grid->getVertexDegree(u); kk++ )
						{
							if ( grid->neighborVV(u,kk) == v )
							{
								uv_edge = true;
							}
							else
							{
								int t = grid->neighborVV(u,kk);
								for ( int kkk = 0; kkk < grid->getVertexDegree(t); kkk++ )
								{
									if ( grid->neighborVV(t,kkk) == v )
									{
										uv_edge = true;
									}
								}
							}
						}

						if ( uv_edge )
						{
							continue;
						}



						Configuration* canon = new Configuration(grid);
						canon->addVertexToShape(v);
						canon->addVertexToShape(between1);
						canon->addVertexToShape(between2);
						canon->addVertexToShape(u);

				char* canon_str = canon->getCanonicalString();

				if ( seen_canon->contains(canon_str) )
				{
					continue;
				}

				char* new_str = (char*)malloc(strlen(canon_str)+1);
				strcpy(new_str, canon_str);
				seen_canon->insert(new_str, true);	
				num_canon_seen++;


						for ( int r = 1; r <= max_r; r++ )
						{
							Configuration* c = new Configuration(grid);

							c->addVertexToShape(v);
							c->addVertexToShape(u);

							int ball_size1 = 0;
							int* ball1 = grid->getVertexBall(v, r, ball_size1);

							int ball_size2 = 0;
							int * ball2 = grid->getVertexBall(u, r, ball_size2);

							for ( int k = 0; k < ball_size1; k++ )
							{
								for ( int kk = 0; kk < ball_size2; kk++ )
								{
									if ( ball1[k] == ball2[kk] )
									{
										c->addVertexToShape(ball1[k]);
									}
								}
							}

							printf("#begin RuleShape\n");
							printf("rule_name = \"d%dr%d%c\";\n", d, r, 'a' + num_canon_seen);
							printf("var_name = \"d%dr%d%c\";\n", d, r, 'a' + num_canon_seen);
							printf("rule_type = %d;\n", VARIABLE_TYPE_V2V); 
							printf("keys = [ \\\n");

							for ( int k = 0; k < c->getNumVerticesInShape(); k++ )
							{
								int t = c->getVertexFromShape(k);

								printf("    1 * x[%d], \\\n", t);
							}
							printf("  ];\n");
							printf("center_id = %d;\n", v);
							printf("center_is_face = false;\n");
							printf("chargeable = [ %d ];\n", u);
							printf("kernels = [ \\\n");

							
					for ( int t = 2; t < c->getNumVerticesInShape(); t += 2 )
					{
						bool first = true;
						int count = 0;
						for ( int k = 0; k < t; k++ )
						{
							int tv = c->getVertexFromShape(k);

							if ( !first )
							{
								printf(" + ");
							}
							first = false;

							printf("1 * x[%d]", tv);
							count++;
						}


						if ( t > 2 )
						{
							printf(" >= %d, \\\n", count - ( (count - 1) / 2 ) );
						}
						else
						{
							printf(" >= %d, \\\n", count );
						}
					}

							printf("  ];\n");

							c->write(stdout);

							printf("#end RuleShape\n\n");
						}

						// face version...
						Configuration* c = new Configuration(grid);
						c->setCenter(v, true);
						c->addVertexToShape(v);
						c->addVertexToShape(u);

						for ( int j = 0; j < grid->getVertexDegree(v); j++ )
						{
							int f = grid->neighborVF(v, j);

							for ( int k = 0; k < grid->getVertexDegree(u); k++ )
							{
								if ( grid->neighborVF(u,k) == f )
								{
									c->addFaceToShape(f);
								}
							}
						}

						for ( int j = 0; j < c->getNumFacesInShape(); j++ )
						{
							int f = c->getFaceFromShape(j);

							for ( int k = 0; k < grid->getFaceDegree(f); k++ )
							{
								int x = grid->neighborFV(f, k);

								if ( c->isVertexInShape(x) == false )
								{
									c->addVertexToShape(x);
								}
							}
						}

						printf("#begin RuleShape\n");
						printf("rule_name = \"d%dface1%c\";\n", d, 'a' + num_canon_seen);
						printf("var_name = \"d%df1%c\";\n", d, 'a' + num_canon_seen);
						printf("rule_type = %d;\n", VARIABLE_TYPE_V2V); 
						printf("keys = [ \\\n");

						for ( int k = 0; k < c->getNumVerticesInShape(); k++ )
						{
							int t = c->getVertexFromShape(k);

							printf("    1 * x[%d], \\\n", t);
						}
						printf("  ];\n");
						printf("center_id = %d;\n", v);
						printf("center_is_face = false;\n");
						printf("chargeable = [ %d ];\n", u);
						printf("kernels = [ \\\n");

						
					for ( int t = 2; t < c->getNumVerticesInShape(); t += 2 )
					{
						bool first = true;
						int count = 0;
						for ( int k = 0; k < t; k++ )
						{
							int tv = c->getVertexFromShape(k);

							if ( !first )
							{
								printf(" + ");
							}
							first = false;

							printf("1 * x[%d]", tv);
							count++;
						}


						if ( t > 2 )
						{
							printf(" >= %d, \\\n", count - ( (count - 1) / 2 ) );
						}
						else
						{
							printf(" >= %d, \\\n", count );
						}
					}

						printf("  ];\n");
						c->write(stdout);
						printf("#end RuleShape\n\n");

						delete c;


						// d#f2#
						c = new Configuration(grid);
						c->setCenter(v, true);
						c->addVertexToShape(v);
						c->addVertexToShape(u);

						for ( int j = 0; j < grid->getVertexDegree(v); j++ )
						{
							int f = grid->neighborVF(v, j);

							for ( int k = 0; k < grid->getVertexDegree(u); k++ )
							{
								if ( grid->neighborVF(u,k) == f )
								{
									c->addFaceToShape(f);
								}
							}
						}

						for ( int j = 0; j < c->getNumFacesInShape(); j++ )
						{
							int f = c->getFaceFromShape(j);

							for ( int k = 0; k < grid->getFaceDegree(f); k++ )
							{
								int x = grid->neighborFV(f, k);

								if ( c->isVertexInShape(x) == false )
								{
									c->addVertexToShape(x);
								}

								for ( int kk = 0; kk < grid->getVertexDegree(x); kk++ )
								{
									int y = grid->neighborVV(x, kk);

									if ( c->isVertexInShape(y) == false )
									{
										c->addVertexToShape(y);
									}	
								}
							
							}
						}


						printf("#begin RuleShape\n");
						printf("rule_name = \"d%dface2%c\";\n", d, 'a' + num_canon_seen);
						printf("var_name = \"d%df2%c\";\n", d, 'a' + num_canon_seen);
						printf("rule_type = %d;\n", VARIABLE_TYPE_V2V); 
						printf("keys = [ \\\n");

						for ( int k = 0; k < c->getNumVerticesInShape(); k++ )
						{
							int t = c->getVertexFromShape(k);

							printf("    1 * x[%d], \\\n", t);
						}
						printf("  ];\n");
						printf("center_id = %d;\n", v);
						printf("center_is_face = false;\n");
						printf("chargeable = [ %d ];\n", u);
						printf("kernels = [ \\\n");

						
					for ( int t = 2; t < c->getNumVerticesInShape(); t += 2 )
					{
						bool first = true;
						int count = 0;
						for ( int k = 0; k < t; k++ )
						{
							int tv = c->getVertexFromShape(k);

							if ( !first )
							{
								printf(" + ");
							}
							first = false;

							printf("1 * x[%d]", tv);
							count++;
						}


						if ( t > 2 )
						{
							printf(" >= %d, \\\n", count - ( (count - 1) / 2 ) );
						}
						else
						{
							printf(" >= %d, \\\n", count );
						}
					}

						printf("  ];\n");
						c->write(stdout);
						printf("#end RuleShape\n\n");

						delete c;
					}
				}
			}
		}
	}

	{
		// Nearby
		char num_canon_seen = -1;
		for ( int i = 0; i < grid->getNumVertexOrbits(); i++ )
		{
			int v = grid->getVertexOrbitRepresentative(i);

			for ( int j = 0; j < grid->getVertexDegree(v); j++ )
			{
				int f = grid->neighborVF(v,j);

				Configuration* canon = new Configuration(grid);

				// canon->addVertexToShape(v);
				canon->addFaceToShape(f);

				for ( int k = 0; k < grid->getFaceDegree(f); k++ )
				{
					canon->addVertexToShape(grid->neighborFV(f,k));
				}

				canon->setCenter(v, true);
				
				char* canon_str = canon->getCanonicalString();

				if ( seen_canon->contains(canon_str) )
				{
					continue;
				}

				char* new_str = (char*)malloc(strlen(canon_str)+1);
				strcpy(new_str, canon_str);
				seen_canon->insert(new_str, true);	
				num_canon_seen++;

				delete canon;

				{
					for ( int r = 1; r <= 3; r++ )
					{
						Configuration* vconf = new Configuration(grid);
						vconf->setCenter(v, true);
						vconf->addFaceToShape(f);

						int ballsizer = 0;
						int* ballr = grid->getVertexBall(v, r, ballsizer);

						for ( int k = 0; k < ballsizer; k++ )
						{
							vconf->addVertexToShape(ballr[k]);
						}
						free(ballr);

						printf("#begin RuleShape\n");
						printf("rule_name = \"v%d%c\";\n", r, 'a' + num_canon_seen);
						printf("var_name = \"v%d%c\";\n", r, 'a' + num_canon_seen);
						printf("rule_type = %d;\n", VARIABLE_TYPE_F2V); 
						printf("keys = [ \\\n");

						for ( int k = 0; k < vconf->getNumVerticesInShape(); k++ )
						{
							int t = vconf->getVertexFromShape(k);

							printf("    1 * x[%d], \\\n", t);
						}
						printf("  ];\n");
						printf("center_id = %d;\n", v);
						printf("center_is_face = false;\n");
						printf("chargeable = [ %d ];\n", f);
						printf("kernels = [ \\\n");

								
						for ( int t = 1; t <= r; t ++ )
						{	
							ballr = grid->getVertexBall(v, t, ballsizer);
							bool first = true;
							int count = 0;
							
							if ( t == 1 )
							{
								for ( int k = 0; k < ballsizer; k++ )
								{
									int tv = ballr[k];

									if ( !first )
									{
										printf(" + ");
									}
									first = false;

									printf("1 * x[%d]", tv);
									count++;
								}

								printf(" >= %d, \\\n", ballsizer );
							}
							else
							{
								count = 0;
								// last radius
								int balltmsize = 0;
								int* balltm = grid->getVertexBall(v, t-1, balltmsize);

								for ( int k = 0; k < balltmsize; k++ )
								{
									int tv = balltm[k];

									if ( !first )
									{
										printf(" + ");
									}
									first = false;

									printf("1 * x[%d]", tv);
									count++;
								}

								for ( int k = 0; k < ballsizer; k++ )
								{
									int tv = ballr[k];

									bool is_adj_to_f = false;
									for ( int kk = 0; kk < grid->getFaceDegree(f); kk++ )
									{
										if ( tv == grid->neighborFV(f, kk) )
										{
											is_adj_to_f = true;
										}
									}

									if ( !is_adj_to_f )
									{
										continue;
									}

									bool found = false;
									for ( int kk = 0; kk < balltmsize; kk++ )
									{
										if ( tv == balltm[kk] )
										{
											found = true;
										}
									}

									if ( !found )
									{	
										if ( !first )
										{
											printf(" + ");
										}
										first = false;

										printf("1 * x[%d]", tv);
										count++;
									}
								}

								free(balltm);
 								printf(" >= %d, \\\n", count - (count/2) );


 								if ( t < r )
 								{
	 								count = 0;
									for ( int k = 0; k < ballsizer; k++ )
									{
										int tv = ballr[k];

										if ( !first )
										{
											printf(" + ");
										}
										first = false;

										printf("1 * x[%d]", tv);
										count++;
									}

	 								printf(" >= %d, \\\n", ballsizer - (ballsizer / 2) );
 								}
							}
						}
						
						printf("  ];\n");

						vconf->write(stdout);
						printf("#end RuleShape\n\n");
					}
				}

				Configuration* face1 = new Configuration(grid);
				face1->addVertexToShape(v);
				face1->setCenter(v, true);
				face1->addFaceToShape(f);

				for ( int jj = 0; jj < grid->getFaceDegree(f); jj++ )
				{
					int u = grid->neighborFV(f, jj);

					if ( face1->isVertexInShape(u) == false )
					{
						face1->addVertexToShape(u);
					}
				}

				printf("#begin RuleShape\n");
				printf("rule_name = \"face1%c\";\n",  'a' + num_canon_seen);
				printf("var_name = \"f1%c\";\n", 'a' + num_canon_seen);
				printf("rule_type = %d;\n", VARIABLE_TYPE_F2V); 
				printf("keys = [ \\\n");

				for ( int k = 0; k < face1->getNumVerticesInShape(); k++ )
				{
					int t = face1->getVertexFromShape(k);

					printf("    1 * x[%d], \\\n", t);
				}
				printf("  ];\n");
				printf("center_id = %d;\n", v);
				printf("center_is_face = false;\n");
				printf("chargeable = [ %d ];\n", f);
				printf("kernels = [ \\\n");

						
				for ( int t = 2; t < face1->getNumVerticesInShape(); t += 2 )
				{
					bool first = true;
					int count = 0;
					for ( int k = 0; k < t; k++ )
					{
						int tv = face1->getVertexFromShape(k);

						if ( !first )
						{
							printf(" + ");
						}
						first = false;

						printf("1 * x[%d]", tv);
						count++;
					}


						if ( t > 2 )
						{
							printf(" >= %d, \\\n", count - ( (count - 1) / 2 ) );
						}
						else
						{
							printf(" >= %d, \\\n", count );
						}
				}
				
					printf("  ];\n");

				face1->write(stdout);
				printf("#end RuleShape\n\n");



				Configuration* face2 = new Configuration(grid);
				face2->addVertexToShape(v);
				face2->setCenter(v, true);
				face2->addFaceToShape(f);

				for ( int k = 0; k < face1->getNumVerticesInShape(); k++ )
				{
					int x = face1->getVertexFromShape(k);

					if ( face2->isVertexInShape(x) == false )
					{
						face2->addVertexToShape(x);
					}

					for ( int kk = 0; kk < grid->getVertexDegree(x); kk++ )
					{
						int y = grid->neighborVV(x,kk);

						if ( face2->isVertexInShape(y) == false )
						{
							face2->addVertexToShape(y);
						}
					}
				}


				printf("#begin RuleShape\n");
				printf("rule_name = \"face2%c\";\n",  'a' + num_canon_seen);
				printf("var_name = \"f2%c\";\n", 'a' + num_canon_seen);
				printf("rule_type = %d;\n", VARIABLE_TYPE_F2V); 
				printf("keys = [ \\\n");

				for ( int k = 0; k < face2->getNumVerticesInShape(); k++ )
				{
					int t = face2->getVertexFromShape(k);

					printf("    1 * x[%d], \\\n", t);
				}
				printf("  ];\n");
				printf("center_id = %d;\n", v);
				printf("center_is_face = false;\n");
				printf("chargeable = [ %d ];\n", f);

				printf("kernels = [ \\\n");

						
					for ( int t = 2; t < face2->getNumVerticesInShape(); t += 2 )
					{
						bool first = true;
						int count = 0;
						for ( int k = 0; k < t; k++ )
						{
							int tv = face2->getVertexFromShape(k);

							if ( !first )
							{
								printf(" + ");
							}
							first = false;

							printf("1 * x[%d]", tv);
							count++;
						}


						if ( t > 2 )
						{
							printf(" >= %d, \\\n", count - ( (count - 1) / 2 ) );
						}
						else
						{
							printf(" >= %d, \\\n", count );
						}
					}
					
						printf("  ];\n");
				face2->write(stdout);
				printf("#end RuleShape\n\n");





				Configuration* nearby = new Configuration(grid);

				nearby->addVertexToShape(v);
				nearby->setCenter(v, true);

				for ( int k = 0; k < grid->getVertexDegree(v); k++ )
				{
					int g = grid->neighborVF(v, k);

					if ( nearby->isFaceInShape(g) == false )
					{
						nearby->addFaceToShape(g);
					}

					for ( int jj = 0; jj < grid->getFaceDegree(g); jj++ )
					{
						int u = grid->neighborFV(g, jj);

						if ( nearby->isVertexInShape(u) == false )
						{
							nearby->addVertexToShape(u);
						}
					}
				}

				printf("#begin RuleShape\n");
				printf("rule_name = \"nearby%c\";\n",  'a' + num_canon_seen);
				printf("var_name = \"n%c\";\n", 'a' + num_canon_seen);
				printf("rule_type = %d;\n", VARIABLE_TYPE_F2V); 
				printf("keys = [ \\\n");

				for ( int k = 0; k < nearby->getNumVerticesInShape(); k++ )
				{
					int t = nearby->getVertexFromShape(k);

					printf("    1 * x[%d], \\\n", t);
				}
				printf("  ];\n");
				printf("center_id = %d;\n", v);
				printf("center_is_face = false;\n");
				printf("chargeable = [ %d ];\n", f);

				printf("kernels = [ \\\n");

						
					for ( int t = 6; t < nearby->getNumVerticesInShape(); t += 2 )
					{
						bool first = true;
						int count = 0;
						for ( int k = 0; k < t; k++ )
						{
							int tv = nearby->getVertexFromShape(k);

							if ( !first )
							{
								printf(" + ");
							}
							first = false;

							printf("1 * x[%d]", tv);
							count++;
						}


						if ( t > 2 )
						{
							printf(" >= %d, \\\n", count - ( (count - 1) / 2 ) );
						}
						else
						{
							printf(" >= %d, \\\n", count );
						}
					}
					
						printf("  ];\n");
				nearby->write(stdout);
				printf("#end RuleShape\n\n");


				Configuration* nearbyplus = new Configuration(grid);

				nearbyplus->addVertexToShape(v);
				nearbyplus->setCenter(v, true);

				for ( int i = 0; i < nearby->getNumVerticesInShape(); i++ )
				{
					int vi = nearby->getVertexFromShape(i);

					if ( nearbyplus->isVertexInShape(vi) == false )
					{
						nearbyplus->addVertexToShape(vi);
					}
				}

				for ( int i = 0; i < grid->getFaceDegree(f); i++ )
				{
					int vi = grid->neighborFV(f,i);

					for ( int j = 0; j < grid->getVertexDegree(vi); j++ )
					{
						int uj = grid->neighborVV(vi, j);

						if ( nearbyplus->isVertexInShape(uj) == false )
						{
							nearbyplus->addVertexToShape(uj);
						}
					}
				}

				printf("#begin RuleShape\n");
				printf("rule_name = \"nearbyplus%c\";\n",  'a' + num_canon_seen);
				printf("var_name = \"np%c\";\n", 'a' + num_canon_seen);
				printf("rule_type = %d;\n", VARIABLE_TYPE_F2V); 
				printf("keys = [ \\\n");

				for ( int k = 0; k < nearbyplus->getNumVerticesInShape(); k++ )
				{
					int t = nearbyplus->getVertexFromShape(k);

					printf("    1 * x[%d], \\\n", t);
				}
				printf("  ];\n");
				printf("center_id = %d;\n", v);
				printf("center_is_face = false;\n");
				printf("chargeable = [ %d ];\n", f);

				printf("kernels = [ \\\n");

						
					for ( int t = 6; t < nearbyplus->getNumVerticesInShape(); t += 2 )
					{
						bool first = true;
						int count = 0;
						for ( int k = 0; k < t; k++ )
						{
							int tv = nearbyplus->getVertexFromShape(k);

							if ( !first )
							{
								printf(" + ");
							}
							first = false;

							printf("1 * x[%d]", tv);
							count++;
						}

						if ( t > 2 )
						{
							printf(" >= %d, \\\n", count - ( (count - 1) / 2 ) );
						}
						else
						{
							printf(" >= %d, \\\n", count );
						}
					}
					
						printf("  ];\n");
				nearbyplus->write(stdout);
				printf("#end RuleShape\n\n");



				Configuration* nearbyplus2 = new Configuration(grid);

				nearbyplus2->addVertexToShape(v);
				nearbyplus2->setCenter(v, true);

				for ( int i = 0; i < nearby->getNumVerticesInShape(); i++ )
				{
					int vi = nearby->getVertexFromShape(i);

					if ( nearbyplus2->isVertexInShape(vi) == false )
					{
						nearbyplus2->addVertexToShape(vi);
					}
				}

				for ( int i = 0; i < grid->getFaceDegree(f); i++ )
				{
					int vi = grid->neighborFV(f,i);

					for ( int j = 0; j < grid->getVertexDegree(vi); j++ )
					{
						int uj = grid->neighborVV(vi, j);

						if ( nearbyplus2->isVertexInShape(uj) == false )
						{
							nearbyplus2->addVertexToShape(uj);
						}
					}
				}

				int ballsizev = 0;
				int* ballv = grid->getVertexBall(v, 2, ballsizev);

				for ( int i = 0; i < ballsizev; i++ )
				{
					if ( nearbyplus2->isVertexInShape(ballv[i]) == false )
					{
						nearbyplus->addVertexToShape(ballv[i]);
					}
				}

				free(ballv);

				printf("#begin RuleShape\n");
				printf("rule_name = \"nearbyplus2%c\";\n",  'a' + num_canon_seen);
				printf("var_name = \"np2%c\";\n", 'a' + num_canon_seen);
				printf("rule_type = %d;\n", VARIABLE_TYPE_F2V); 
				printf("keys = [ \\\n");

				for ( int k = 0; k < nearbyplus2->getNumVerticesInShape(); k++ )
				{
					int t = nearbyplus2->getVertexFromShape(k);

					printf("    1 * x[%d], \\\n", t);
				}
				printf("  ];\n");
				printf("center_id = %d;\n", v);
				printf("center_is_face = false;\n");
				printf("chargeable = [ %d ];\n", f);

				printf("kernels = [ \\\n");

						
					for ( int t = 6; t < nearbyplus2->getNumVerticesInShape(); t += 2 )
					{
						bool first = true;
						int count = 0;
						for ( int k = 0; k < t; k++ )
						{
							int tv = nearbyplus2->getVertexFromShape(k);

							if ( !first )
							{
								printf(" + ");
							}
							first = false;

							printf("1 * x[%d]", tv);
							count++;
						}
						if ( t > 2 )
						{
							printf(" >= %d, \\\n", count - ( (count - 1) / 2 ) );
						}
						else
						{
							printf(" >= %d, \\\n", count );
						}
					}
					
						printf("  ];\n");
				nearbyplus2->write(stdout);
				printf("#end RuleShape\n\n");
			}
		}
	}


	delete grid;
	return 0;
}

