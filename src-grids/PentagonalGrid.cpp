/*
 * PentagonalGrid.cpp
 *
 *  Created on: Aug 12, 2014
 *      Author: stolee
 */

#include "PentagonalGrid.hpp"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

using namespace adage::grids;

#define IN_BOUNDS_OR_NULL(x,y,max,array)  ( x < 0 || y < 0 || y>= max || x>= max) ? -1 : array[x][y]

int PentagonalGrid::reserveNewFace()
{
	int f_index = this->max_facial_index;
	(this->max_facial_index)++;

	this->fd[f_index] = 5;
	this->f[f_index] = 5 * f_index;
	this->max_f_value = 5 * (f_index + 1);
	this->fo[f_index] = 0;

	// Fill edges with blanks!
	for ( int i = 0; i < this->fd[f_index]; i++ )
	{
		this->fve[this->f[f_index] + i] = -1;
		this->ffe[this->f[f_index] + i] = -1;
		this->fbe[2 * this->f[f_index] + 2 * i] = -1;
		this->fbe[2 * this->f[f_index] + 2 * i + 1] = -1;
	}

	return f_index;
}

int PentagonalGrid::reserveNewVertex(int deg)
{
	int v_index = this->max_vertex_index;
	(this->max_vertex_index)++;

	this->vd[v_index] = deg;
	this->v[v_index] = this->cur_vve_index;
	this->cur_vve_index = this->cur_vve_index + deg;

	this->max_v_value = deg * (v_index + 1);
	this->vo[v_index] = 0;

	if ( deg == 4 )
	{
		this->vo[v_index] = 1;
	}

	// Fill edges with blanks!
	for ( int i = 0; i < deg; i++ )
	{
		this->vve[this->v[v_index] + i] = -1;
		this->vfe[this->v[v_index] + i] = -1;
		this->vbe[2 * this->v[v_index] + 2 * i] = -1;
		this->vbe[2 * this->v[v_index] + 2 * i + 1] = -1;
	}

	return v_index;
}

PentagonalGrid::PentagonalGrid(int max_magnitude)
{
	this->num_vertex_orbits = 2;
	this->num_facial_orbits = 1;
	
	// We will use the Z^2 parameterization to assist with our goals!
	int num_faces = 0;
	int num_verts = 0;
	this->cur_vve_index = 0;
	// Use the diameter to build the Square grid.

	int max_coord = max_magnitude + 2;

	int* vertex_x = (int*) malloc(10 * max_coord * max_coord * sizeof(int));
	int* vertex_y = (int*) malloc(10 * max_coord * max_coord * sizeof(int));
	int** xy_vertex = (int**) malloc(4 * (max_coord + 4) * sizeof(int*));
	int* face_x = (int*) malloc(10 * max_coord * max_coord * sizeof(int));
	int* face_y = (int*) malloc(10 * max_coord * max_coord * sizeof(int));
	int** xy_face = (int**) malloc(4 * (max_coord + 4) * sizeof(int*));

	for ( int i = 0; i <= 2 * (max_coord + 1); i++ )
	{
		xy_vertex[i] = (int*) malloc(2 * (max_coord + 1) * sizeof(int));
		xy_face[i] = (int*) malloc(2 * (max_coord + 1) * sizeof(int));

		for ( int j = 0; j < 2 * (max_magnitude + 3); j++ )
		{
			// in case not filled in later...

			xy_vertex[i][j] = -1;
			xy_face[i][j] = -1;
		}
	}

	for ( int magnitude = 0; magnitude <= max_coord; magnitude++ )
	{
		for ( int x = -magnitude; x <= magnitude; x++ )
		{
			int y = magnitude - abs(x);

			bool use_y = true;
			if ( y % 2 != 0 )
			{
				if ( y % 4 == 1 || y % 4 == -3 )
				{
					if ( x % 2 == 0 )
					{
						use_y = false;
					}
				}
				else
				{
					if ( x % 2 != 0 )
					{
						use_y = false;
					}

				}
			}

			if ( use_y )
			{
				// add this vertex
				xy_vertex[max_coord + x][max_coord + y] = num_verts;
				vertex_x[num_verts] = x;
				vertex_y[num_verts] = y;
				num_verts++;
			}

			if ( y != 0 )
			{
				// mirror version!
				y = -y;

				use_y = true;
				if ( y % 2 != 0 )
				{
					if ( y % 4 == 1 || y % 4 == -3 )
					{
						if ( x % 2 == 0 )
						{
							use_y = false;
						}
					}
					else
					{
						if ( x % 2 != 0 )
						{
							use_y = false;
						}

					}
				}

				if ( use_y )
				{
					// add this vertex
					xy_vertex[max_coord + x][max_coord + y] = num_verts;
					vertex_x[num_verts] = x;
					vertex_y[num_verts] = y;
					num_verts++;
				}
			}
		}
	}

	for ( int magnitude = 0; magnitude <= max_coord; magnitude++ )
	{
		for ( int x = -magnitude; x <= magnitude; x++ )
		{
			int y = magnitude - abs(x);

			if ( y % 2 != 0 )
			{
				continue;
			}

			// facial vertex!
			xy_face[max_coord + x][max_coord + y] = num_faces;
			face_x[num_faces] = x;
			face_y[num_faces] = y;
			num_faces++;

			if ( y != 0 )
			{
				y = -y;

				// facial vertex!
				xy_face[max_coord + x][max_coord + y] = num_faces;
				face_x[num_faces] = x;
				face_y[num_faces] = y;
				num_faces++;
			}
		}
	}

	this->lengthenVArrays(num_verts, 4 * num_verts);
	this->lengthenFArrays(num_faces, 5 * num_faces);

	// Now, fill in everything!
	for ( int i = 0; i < num_faces; i++ )
	{
		int x = face_x[i];
		int y = face_y[i];

		if ( i != xy_face[max_coord + x][max_coord + y] )
		{
			printf("--ERROR: xy_face[%d+%d=%d][%d+%d=%d] = %d != %d...", max_coord, x, max_coord + x, max_coord, y, max_coord + y,
					xy_face[max_coord + x][max_coord + y], i);
		}

		int findex = i;

		this->reserveNewFace();

		if ( (x % 2 == 0 && y % 4 == 0) || (x % 2 != 0 && y % 4 != 0) )
		{
			// Fill in bordering vertices and faces...
			this->fve[this->f[findex] + 0] = IN_BOUNDS_OR_NULL(max_coord + x + 0, max_coord + y + 0, 2*max_coord, xy_vertex);
			this->fve[this->f[findex] + 1] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y + 0, 2*max_coord, xy_vertex);
			this->fve[this->f[findex] + 2] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y + 1, 2*max_coord, xy_vertex);
			this->fve[this->f[findex] + 3] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y + 1, 2*max_coord, xy_vertex);
			this->fve[this->f[findex] + 4] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y + 0, 2*max_coord, xy_vertex);

			this->ffe[this->f[findex] + 0] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y + 0, 2*max_coord, xy_face);
			this->ffe[this->f[findex] + 1] = IN_BOUNDS_OR_NULL(max_coord + x + 2, max_coord + y + 0, 2*max_coord, xy_face);
			this->ffe[this->f[findex] + 2] = IN_BOUNDS_OR_NULL(max_coord + x + 0, max_coord + y + 2, 2*max_coord, xy_face);
			this->ffe[this->f[findex] + 3] = IN_BOUNDS_OR_NULL(max_coord + x - 2, max_coord + y + 0, 2*max_coord, xy_face);
			this->ffe[this->f[findex] + 4] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y + 0, 2*max_coord, xy_face);

			this->fbe[2 * this->f[findex] + 1] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y + 0, 2*max_coord, xy_face);
			this->fbe[2 * this->f[findex] + 3] = IN_BOUNDS_OR_NULL(max_coord + x + 2, max_coord + y + 0, 2*max_coord, xy_face);
			this->fbe[2 * this->f[findex] + 5] = IN_BOUNDS_OR_NULL(max_coord + x + 0, max_coord + y + 2, 2*max_coord, xy_face);
			this->fbe[2 * this->f[findex] + 7] = IN_BOUNDS_OR_NULL(max_coord + x - 2, max_coord + y + 0, 2*max_coord, xy_face);
			this->fbe[2 * this->f[findex] + 9] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y + 0, 2*max_coord, xy_face);

			this->fbe[2 * this->f[findex] + 0] = IN_BOUNDS_OR_NULL(max_coord + x + 0, max_coord + y + 0, 2*max_coord, xy_vertex);
			this->fbe[2 * this->f[findex] + 2] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y + 0, 2*max_coord, xy_vertex);
			this->fbe[2 * this->f[findex] + 4] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y + 1, 2*max_coord, xy_vertex);
			this->fbe[2 * this->f[findex] + 6] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y + 1, 2*max_coord, xy_vertex);
			this->fbe[2 * this->f[findex] + 8] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y + 0, 2*max_coord, xy_vertex);
		}
		else
		{
			this->fve[this->f[findex] + 0] = IN_BOUNDS_OR_NULL(max_coord + x + 0, max_coord + y + 0, 2*max_coord, xy_vertex);
			this->fve[this->f[findex] + 1] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y + 0, 2*max_coord, xy_vertex);
			this->fve[this->f[findex] + 2] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y - 1, 2*max_coord, xy_vertex);
			this->fve[this->f[findex] + 3] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y - 1, 2*max_coord, xy_vertex);
			this->fve[this->f[findex] + 4] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y + 0, 2*max_coord, xy_vertex);

			this->ffe[this->f[findex] + 0] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y + 0, 2*max_coord, xy_face);
			this->ffe[this->f[findex] + 1] = IN_BOUNDS_OR_NULL(max_coord + x - 2, max_coord + y + 0, 2*max_coord, xy_face);
			this->ffe[this->f[findex] + 2] = IN_BOUNDS_OR_NULL(max_coord + x + 0, max_coord + y - 2, 2*max_coord, xy_face);
			this->ffe[this->f[findex] + 3] = IN_BOUNDS_OR_NULL(max_coord + x + 2, max_coord + y + 0, 2*max_coord, xy_face);
			this->ffe[this->f[findex] + 4] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y + 0, 2*max_coord, xy_face);

			this->fbe[2 * this->f[findex] + 1] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y + 0, 2*max_coord, xy_face);
			this->fbe[2 * this->f[findex] + 3] = IN_BOUNDS_OR_NULL(max_coord + x - 2, max_coord + y + 0, 2*max_coord, xy_face);
			this->fbe[2 * this->f[findex] + 5] = IN_BOUNDS_OR_NULL(max_coord + x + 0, max_coord + y - 2, 2*max_coord, xy_face);
			this->fbe[2 * this->f[findex] + 7] = IN_BOUNDS_OR_NULL(max_coord + x + 2, max_coord + y + 0, 2*max_coord, xy_face);
			this->fbe[2 * this->f[findex] + 9] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y + 0, 2*max_coord, xy_face);

			this->fbe[2 * this->f[findex] + 0] = IN_BOUNDS_OR_NULL(max_coord + x + 0, max_coord + y + 0, 2*max_coord, xy_vertex);
			this->fbe[2 * this->f[findex] + 2] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y + 0, 2*max_coord, xy_vertex);
			this->fbe[2 * this->f[findex] + 4] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y - 1, 2*max_coord, xy_vertex);
			this->fbe[2 * this->f[findex] + 6] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y - 1, 2*max_coord, xy_vertex);
			this->fbe[2 * this->f[findex] + 8] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y + 0, 2*max_coord, xy_vertex);
		}
	}

	// Now, fill in everything!
	for ( int i = 0; i < num_verts; i++ )
	{
		int x = vertex_x[i];
		int y = vertex_y[i];

		if ( i != xy_vertex[max_coord + x][max_coord + y] )
		{
			printf("--ERROR: xy_vertex[%d+%d=%d][%d+%d=%d] = %d != %d...", max_coord, x, max_coord + x, max_coord, y, max_coord + y,
					xy_vertex[max_coord + x][max_coord + y], i);
		}

		int vindex = i;

		if ( y % 2 == 0 )
		{
			this->reserveNewVertex(3);
			this->vo[vindex] = 0;
			if (  (x % 2 == 0 && y % 4 == 0) || (x % 2 != 0 && y % 4 != 0) )
			{
				this->vve[this->v[vindex] + 0] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y + 0, 2*max_coord, xy_vertex);
				this->vve[this->v[vindex] + 1] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y + 0, 2*max_coord, xy_vertex);
				this->vve[this->v[vindex] + 2] = IN_BOUNDS_OR_NULL(max_coord + x + 0, max_coord + y - 1, 2*max_coord, xy_vertex);

				this->vfe[this->v[vindex] + 0] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y + 0, 2*max_coord, xy_face);
				this->vfe[this->v[vindex] + 1] = IN_BOUNDS_OR_NULL(max_coord + x + 0, max_coord + y + 0, 2*max_coord, xy_face);
				this->vfe[this->v[vindex] + 2] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y + 0, 2*max_coord, xy_face);

				this->vbe[2 * this->v[vindex] + 1] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y + 0, 2*max_coord, xy_vertex);
				this->vbe[2 * this->v[vindex] + 3] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y + 0, 2*max_coord, xy_vertex);
				this->vbe[2 * this->v[vindex] + 5] = IN_BOUNDS_OR_NULL(max_coord + x + 0, max_coord + y - 1, 2*max_coord, xy_vertex);

				this->vbe[2 * this->v[vindex] + 0] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y + 0, 2*max_coord, xy_face);
				this->vbe[2 * this->v[vindex] + 2] = IN_BOUNDS_OR_NULL(max_coord + x + 0, max_coord + y + 0, 2*max_coord, xy_face);
				this->vbe[2 * this->v[vindex] + 4] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y + 0, 2*max_coord, xy_face);
			}
			else
			{
				this->vve[this->v[vindex] + 0] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y + 0, 2*max_coord, xy_vertex);
				this->vve[this->v[vindex] + 1] = IN_BOUNDS_OR_NULL(max_coord + x + 0, max_coord + y + 1, 2*max_coord, xy_vertex);
				this->vve[this->v[vindex] + 2] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y + 0, 2*max_coord, xy_vertex);

				this->vfe[this->v[vindex] + 0] = IN_BOUNDS_OR_NULL(max_coord + x + 0, max_coord + y + 0, 2*max_coord, xy_face);
				this->vfe[this->v[vindex] + 1] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y + 0, 2*max_coord, xy_face);
				this->vfe[this->v[vindex] + 2] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y + 0, 2*max_coord, xy_face);

				this->vbe[2 * this->v[vindex] + 1] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y + 0, 2*max_coord, xy_vertex);
				this->vbe[2 * this->v[vindex] + 3] = IN_BOUNDS_OR_NULL(max_coord + x + 0, max_coord + y + 1, 2*max_coord, xy_vertex);
				this->vbe[2 * this->v[vindex] + 5] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y + 0, 2*max_coord, xy_vertex);

				this->vbe[2 * this->v[vindex] + 0] = IN_BOUNDS_OR_NULL(max_coord + x + 0, max_coord + y + 0, 2*max_coord, xy_face);
				this->vbe[2 * this->v[vindex] + 2] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y + 0, 2*max_coord, xy_face);
				this->vbe[2 * this->v[vindex] + 4] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y + 0, 2*max_coord, xy_face);
			}
		}
		else
		{
			this->reserveNewVertex(4);
			this->vo[vindex] = 1;
			this->vve[this->v[vindex] + 0] = IN_BOUNDS_OR_NULL(max_coord + x + 2, max_coord + y + 0, 2*max_coord, xy_vertex);
			this->vve[this->v[vindex] + 1] = IN_BOUNDS_OR_NULL(max_coord + x + 0, max_coord + y + 1, 2*max_coord, xy_vertex);
			this->vve[this->v[vindex] + 2] = IN_BOUNDS_OR_NULL(max_coord + x - 2, max_coord + y + 0, 2*max_coord, xy_vertex);
			this->vve[this->v[vindex] + 3] = IN_BOUNDS_OR_NULL(max_coord + x + 0, max_coord + y - 1, 2*max_coord, xy_vertex);

			this->vfe[this->v[vindex] + 0] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y - 1, 2*max_coord, xy_face);
			this->vfe[this->v[vindex] + 1] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y + 1, 2*max_coord, xy_face);
			this->vfe[this->v[vindex] + 2] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y + 1, 2*max_coord, xy_face);
			this->vfe[this->v[vindex] + 3] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y - 1, 2*max_coord, xy_face);

			this->vbe[2 * this->v[vindex] + 1] = IN_BOUNDS_OR_NULL(max_coord + x + 2, max_coord + y + 0, 2*max_coord, xy_vertex);
			this->vbe[2 * this->v[vindex] + 3] = IN_BOUNDS_OR_NULL(max_coord + x + 0, max_coord + y + 1, 2*max_coord, xy_vertex);
			this->vbe[2 * this->v[vindex] + 5] = IN_BOUNDS_OR_NULL(max_coord + x - 2, max_coord + y + 0, 2*max_coord, xy_vertex);
			this->vbe[2 * this->v[vindex] + 7] = IN_BOUNDS_OR_NULL(max_coord + x + 0, max_coord + y - 1, 2*max_coord, xy_vertex);

			this->vbe[2 * this->v[vindex] + 0] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y - 1, 2*max_coord, xy_face);
			this->vbe[2 * this->v[vindex] + 2] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y + 1, 2*max_coord, xy_face);
			this->vbe[2 * this->v[vindex] + 4] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y + 1, 2*max_coord, xy_face);
			this->vbe[2 * this->v[vindex] + 6] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y - 1, 2*max_coord, xy_face);
		}

	}

// Now, deallocate these arrays!
	for ( int i = 0; i <= 2 * (max_coord + 1); i++ )
	{
		free(xy_vertex[i]);
		free(xy_face[i]);
	}
	free(xy_vertex);
	free(xy_face);
	free(vertex_x);
	free(vertex_y);
	free(face_x);
	free(face_y);
}

PentagonalGrid::~PentagonalGrid()
{
}

