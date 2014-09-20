/*
 * HexagonalGrid.cpp
 *
 *  Created on: Apr 17, 2014
 *      Author: stolee
 */

#include "HexagonalGrid.hpp"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

using namespace adage::grids;

#define IN_BOUNDS_OR_NULL(x,y,max,array)  ( x < 0 || y < 0 || y>= max || x>= max) ? -1 : array[x][y]

int HexagonalGrid::reserveNewFace()
{
	int f_index = this->max_facial_index;
	(this->max_facial_index)++;

	this->fd[f_index] = 6;
	this->f[f_index] = 6 * f_index;
	this->max_f_value = 6 * (f_index + 1);
	this->fo[f_index] = 0;

	// Fill edges with blanks!
	for ( int i = 0; i < 6; i++ )
	{
		this->fve[this->f[f_index] + i] = -1;
		this->ffe[this->f[f_index] + i] = -1;
		this->fbe[2 * this->f[f_index] + 2 * i] = -1;
		this->fbe[2 * this->f[f_index] + 2 * i + 1] = -1;
	}

	return f_index;
}

int HexagonalGrid::reserveNewVertex()
{
	int v_index = this->max_vertex_index;
	(this->max_vertex_index)++;

	this->vd[v_index] = 3;
	this->v[v_index] = 3 * v_index;

	this->max_v_value = 3 * (v_index + 1);
	this->vo[v_index] = 0;

	// Fill edges with blanks!
	for ( int i = 0; i < 3; i++ )
	{
		this->vve[this->v[v_index] + i] = -1;
		this->vfe[this->v[v_index] + i] = -1;
		this->vbe[2 * this->v[v_index] + 2 * i] = -1;
		this->vbe[2 * this->v[v_index] + 2 * i + 1] = -1;
	}

	return v_index;
}

HexagonalGrid::HexagonalGrid(int max_magnitude)
{
	// We will use the Z^2 parameterization to assist with our goals!
	int num_faces = 0;
	int num_verts = 0;
	// Use the diameter to build the hexagonal grid.

	int max_coord = max_magnitude + 2;

	int* vertex_x = (int*) malloc(10 * max_coord * max_coord * sizeof(int));
	int* vertex_y = (int*) malloc(10 * max_coord * max_coord * sizeof(int));
	int** xy_vertex = (int**) malloc(2 * (max_coord + 4) * sizeof(int*));
	int* face_x = (int*) malloc(10 * max_coord * max_coord * sizeof(int));
	int* face_y = (int*) malloc(10 * max_coord * max_coord * sizeof(int));
	int** xy_face = (int**) malloc(2 * (max_coord + 4) * sizeof(int*));

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

			// add this vertex
			xy_vertex[max_coord + x][max_coord + y] = num_verts;
			vertex_x[num_verts] = x;
			vertex_y[num_verts] = y;
			num_verts++;

			if ( ((x + y + 2 * max_coord) % 2) == 0 )
			{
				// facial vertex!
				xy_face[max_coord + x][max_coord + y] = num_faces;
				face_x[num_faces] = x;
				face_y[num_faces] = y;
				num_faces++;
			}

			if ( y != 0 )
			{
				// mirror version!
				y = -y;

				// add this vertex
				xy_vertex[max_coord + x][max_coord + y] = num_verts;
				vertex_x[num_verts] = x;
				vertex_y[num_verts] = y;
				num_verts++;

				if ( ((x + y + 2 * max_coord) % 2) == 0 )
				{
					// facial vertex!
					xy_face[max_coord + x][max_coord + y] = num_faces;
					face_x[num_faces] = x;
					face_y[num_faces] = y;
					num_faces++;
				}
			}
		}
	}

	this->lengthenVArrays(num_verts, 3 * num_verts);
	this->lengthenFArrays(num_faces, 6 * num_faces);

	// Now, fill in everything!
	for ( int i = 0; i < num_faces; i++ )
	{
		this->reserveNewFace();
		int x = face_x[i];
		int y = face_y[i];

		if ( i != xy_face[max_coord + x][max_coord + y] )
		{
			printf("--ERROR: xy_face[%d+%d=%d][%d+%d=%d] = %d != %d...", max_coord, x, max_coord + x, max_coord, y, max_coord + y,
					xy_face[max_coord + x][max_coord + y], i);
		}

		int findex = i;

		// Fill in bordering vertices and faces...
		this->ffe[this->f[findex] + 0] = IN_BOUNDS_OR_NULL(max_coord + x + 2, max_coord + y, 2*max_coord, xy_face);
		this->ffe[this->f[findex] + 1] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y + 1, 2*max_coord, xy_face);
		this->ffe[this->f[findex] + 2] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y + 1, 2*max_coord, xy_face);
		this->ffe[this->f[findex] + 3] = IN_BOUNDS_OR_NULL(max_coord + x - 2, max_coord + y, 2*max_coord, xy_face);
		this->ffe[this->f[findex] + 4] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y - 1, 2*max_coord, xy_face);
		this->ffe[this->f[findex] + 5] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y - 1, 2*max_coord, xy_face);

		this->fve[this->f[findex] + 0] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y - 1, 2*max_coord, xy_vertex);
		this->fve[this->f[findex] + 1] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y, 2*max_coord, xy_vertex);
		this->fve[this->f[findex] + 2] = IN_BOUNDS_OR_NULL(max_coord + x, max_coord + y, 2*max_coord, xy_vertex);
		this->fve[this->f[findex] + 3] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y, 2*max_coord, xy_vertex);
		this->fve[this->f[findex] + 4] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y - 1, 2*max_coord, xy_vertex);
		this->fve[this->f[findex] + 5] = IN_BOUNDS_OR_NULL(max_coord + x, max_coord + y - 1, 2*max_coord, xy_vertex);

		this->fbe[2 * this->f[findex] + 1] = IN_BOUNDS_OR_NULL(max_coord + x + 2, max_coord + y, 2*max_coord, xy_face);
		this->fbe[2 * this->f[findex] + 3] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y + 1, 2*max_coord, xy_face);
		this->fbe[2 * this->f[findex] + 5] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y + 1, 2*max_coord, xy_face);
		this->fbe[2 * this->f[findex] + 7] = IN_BOUNDS_OR_NULL(max_coord + x - 2, max_coord + y, 2*max_coord, xy_face);
		this->fbe[2 * this->f[findex] + 9] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y - 1, 2*max_coord, xy_face);
		this->fbe[2 * this->f[findex] + 11] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y - 1, 2*max_coord, xy_face);

		this->fbe[2 * this->f[findex] + 0] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y - 1, 2*max_coord, xy_vertex);
		this->fbe[2 * this->f[findex] + 2] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y, 2*max_coord, xy_vertex);
		this->fbe[2 * this->f[findex] + 4] = IN_BOUNDS_OR_NULL(max_coord + x, max_coord + y, 2*max_coord, xy_vertex);
		this->fbe[2 * this->f[findex] + 6] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y, 2*max_coord, xy_vertex);
		this->fbe[2 * this->f[findex] + 8] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y - 1, 2*max_coord, xy_vertex);
		this->fbe[2 * this->f[findex] + 10] = IN_BOUNDS_OR_NULL(max_coord + x, max_coord + y - 1, 2*max_coord, xy_vertex);
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
		this->reserveNewVertex();

		// Fill in bordering vertices and faces...
		if ( (x + y + 2 * max_coord) % 2 == 0 )
		{
			this->vve[this->v[vindex] + 0] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y, 2*max_coord, xy_vertex);
			this->vve[this->v[vindex] + 1] = IN_BOUNDS_OR_NULL(max_coord + x, max_coord + y + 1, 2*max_coord, xy_vertex);
			this->vve[this->v[vindex] + 2] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y, 2*max_coord, xy_vertex);

			this->vfe[this->v[vindex] + 0] = IN_BOUNDS_OR_NULL(max_coord + x, max_coord + y, 2*max_coord, xy_face);
			this->vfe[this->v[vindex] + 1] = IN_BOUNDS_OR_NULL(max_coord + x+1, max_coord + y + 1, 2*max_coord, xy_face);
			this->vfe[this->v[vindex] + 2] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y + 1, 2*max_coord, xy_face);

			this->vbe[2 * this->v[vindex] + 1] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y, 2*max_coord, xy_vertex);
			this->vbe[2 * this->v[vindex] + 3] = IN_BOUNDS_OR_NULL(max_coord + x, max_coord + y + 1, 2*max_coord, xy_vertex);
			this->vbe[2 * this->v[vindex] + 5] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y, 2*max_coord, xy_vertex);

			this->vbe[2 * this->v[vindex] + 0] = IN_BOUNDS_OR_NULL(max_coord + x, max_coord + y, 2*max_coord, xy_face);
			this->vbe[2 * this->v[vindex] + 2] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y + 1, 2*max_coord, xy_face);
			this->vbe[2 * this->v[vindex] + 4] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y + 1, 2*max_coord, xy_face);
		}
		else
		{
			this->vve[this->v[vindex] + 0] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y, 2*max_coord, xy_vertex);
			this->vve[this->v[vindex] + 1] = IN_BOUNDS_OR_NULL(max_coord + x-1, max_coord + y, 2*max_coord, xy_vertex);
			this->vve[this->v[vindex] + 2] = IN_BOUNDS_OR_NULL(max_coord + x , max_coord + y - 1, 2*max_coord, xy_vertex);

			this->vfe[this->v[vindex] + 0] = IN_BOUNDS_OR_NULL(max_coord + x+1, max_coord + y, 2*max_coord, xy_face);
			this->vfe[this->v[vindex] + 1] = IN_BOUNDS_OR_NULL(max_coord + x, max_coord + y + 1, 2*max_coord, xy_face);
			this->vfe[this->v[vindex] + 2] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y, 2*max_coord, xy_face);

			this->vbe[2 * this->v[vindex] + 1] = IN_BOUNDS_OR_NULL(max_coord + x + 1, max_coord + y, 2*max_coord, xy_vertex);
			this->vbe[2 * this->v[vindex] + 3] = IN_BOUNDS_OR_NULL(max_coord + x-1, max_coord + y , 2*max_coord, xy_vertex);
			this->vbe[2 * this->v[vindex] + 5] = IN_BOUNDS_OR_NULL(max_coord + x, max_coord + y - 1, 2*max_coord, xy_vertex);

			this->vbe[2 * this->v[vindex] + 0] = IN_BOUNDS_OR_NULL(max_coord + x+1, max_coord + y, 2*max_coord, xy_face);
			this->vbe[2 * this->v[vindex] + 2] = IN_BOUNDS_OR_NULL(max_coord + x, max_coord + y + 1, 2*max_coord, xy_face);
			this->vbe[2 * this->v[vindex] + 4] = IN_BOUNDS_OR_NULL(max_coord + x - 1, max_coord + y, 2*max_coord, xy_face);
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

HexagonalGrid::~HexagonalGrid()
{
}

