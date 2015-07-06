/*
 * Grid.cpp
 *
 *  Created on: Apr 17, 2014
 *      Author: stolee
 */

#include <stdlib.h>
#include <stdio.h>
#include <queue>
#include "Grid.hpp"
#include "macros.hpp"

using namespace adage::grids;

void Grid::lengthenVArrays(int length, int e_length)
{
	if ( length > this->v_len )
	{
		int old_vlen = this->v_len;

		this->v_len = length;
		this->v = (int*) realloc(this->v, length * sizeof(int));
		this->vd = (int*) realloc(this->vd, length * sizeof(int));
		this->vo = (int*) realloc(this->vo, length * sizeof(int));

		for ( int i = old_vlen; i < this->v_len; i++ )
		{
			this->vd[i] = 0;
		}
	}

	if ( e_length > this->ve_len )
	{
		int old_ve_len = this->ve_len;
		this->ve_len = e_length;
		this->vve = (int*) realloc(this->vve, e_length * sizeof(int));
		this->vfe = (int*) realloc(this->vfe, e_length * sizeof(int));
		this->vbe = (int*) realloc(this->vbe, 2 * e_length * sizeof(int));

		for ( int i = old_ve_len; i < this->ve_len; i++ )
		{
			this->vve[i] = -1;
			this->vfe[i] = -1;
			this->vbe[2*i+0] = -1;
			this->vbe[2*i+1] = -1;
		}
	}
}

void Grid::lengthenFArrays(int length, int e_length)
{
	int old_f_len = this->f_len;
	this->f_len = length;
	this->f = (int*) realloc(this->f, length * sizeof(int));
	this->fd = (int*) realloc(this->fd, length * sizeof(int));
	this->fo = (int*) realloc(this->fo, length * sizeof(int));

	for ( int i = old_f_len; i < this->f_len; i++ )
	{
		this->fd[i] = 0;
	}

	int old_fe_len = this->fe_len;
	this->fe_len = e_length;
	this->fve = (int*) realloc(this->fve, e_length * sizeof(int));
	this->ffe = (int*) realloc(this->ffe, e_length * sizeof(int));
	this->fbe = (int*) realloc(this->fbe, 2 * e_length * sizeof(int));


	for ( int i = old_fe_len; i < this->fe_len; i++ )
	{
		this->fve[i] = -1;
		this->ffe[i] = -1;
		this->fbe[2*i+0] = -1;
		this->fbe[2*i+1] = -1;
	}
}

Grid::Grid()
{
	this->max_vertex_index = 0;
	this->max_facial_index = 0;
	this->num_facial_orbits = 1;
	this->num_vertex_orbits = 1;

	this->f_len = 0;
	this->f = 0;
	this->max_f_value = 0;
	this->fd = 0;
	this->fe_len = 0;
	this->fve = 0;
	this->ffe = 0;
	this->fbe = 0;
	this->fo = 0;

	this->max_v_value = 0;
	this->v_len = 0;
	this->v = 0;
	this->vd = 0;
	this->ve_len = 0;
	this->vve = 0;
	this->vfe = 0;
	this->vbe = 0;
	this->vo = 0;
}

Grid::~Grid()
{
	FREE_ARRAY(this->f);
	FREE_ARRAY(this->fd);
	FREE_ARRAY(this->fo);
	FREE_ARRAY(this->ffe);
	FREE_ARRAY(this->fve);
	FREE_ARRAY(this->fbe);
	FREE_ARRAY(this->v);
	FREE_ARRAY(this->vd);
	FREE_ARRAY(this->vo);
	FREE_ARRAY(this->vve);
	FREE_ARRAY(this->vfe);
	FREE_ARRAY(this->vbe);
}

Grid* Grid::getDual() const
{
	Grid* dual = new Grid();

	dual->lengthenVArrays(this->max_facial_index, this->fe_len);
	dual->lengthenFArrays(this->max_vertex_index, this->ve_len);

	dual->num_vertex_orbits = this->num_facial_orbits;
	dual->num_facial_orbits = this->num_vertex_orbits;
	
	dual->max_facial_index = this->max_vertex_index;
	dual->max_vertex_index = this->max_facial_index;
	
	for ( int i = 0; i < this->max_facial_index; i++ )
	{
		dual->v[i] = this->f[i];
		dual->vd[i] = this->fd[i];
		dual->vo[i] = this->fo[i];
	}
	
	for ( int i = 0; i < this->max_vertex_index; i++ )
	{
		dual->f[i] = this->v[i];
		dual->fd[i] = this->vd[i];
		dual->fo[i] = this->vo[i];
	}

	for ( int i = 0; i < this->fe_len; i++ )
	{
		dual->vve[i] = this->ffe[i];
		dual->vfe[i] = this->fve[i];
		dual->vbe[2*i] = this->fbe[2*i];
		dual->vbe[2*i+1] = this->fbe[2*i+1];
	}

	for ( int i = 0; i < this->ve_len; i++ )
	{
		dual->fve[i] = this->vfe[i];
		dual->ffe[i] = this->vve[i];
		dual->fbe[2*i] = this->vbe[2*i];
		dual->fbe[2*i+1] = this->vbe[2*i+1];
	}

	return dual;
}



Grid* Grid::getBlowup() const
{
	Grid* blowup = new Grid();

	blowup->num_vertex_orbits = this->num_vertex_orbits * this->num_vertex_orbits;
	blowup->num_facial_orbits = this->num_facial_orbits + this->num_vertex_orbits;

	int vdeg_sum = 0;
	int* deg_sums = (int*)malloc(this->max_vertex_index * sizeof(int));

	for ( int i = 0; i < this->max_vertex_index; i++ )
	{
		deg_sums[i] = vdeg_sum;
		vdeg_sum += this->vd[i];
	}

	int fdeg_sum = 0;
	for ( int i = 0; i < this->max_facial_index; i++ )
	{
		fdeg_sum += this->fd[i];
	}

	// Vertices turn into vdeg_sum vertices, each is 3-regular
	blowup->lengthenVArrays(vdeg_sum, 3 * vdeg_sum);
	bzero(blowup->vd, vdeg_sum * sizeof(int));

	// Faces double in length, faces added where vertices used to be.
	blowup->lengthenFArrays(2*this->max_vertex_index + 2*this->max_facial_index, 3 * vdeg_sum + 3 * fdeg_sum);

	bzero(blowup->fd, (2 * this->max_vertex_index + 2 * this->max_facial_index ) * sizeof(int));

	int cur_v_index = 0;
	for ( int i = 0; i < this->max_vertex_index; i++ )
	{
		for ( int j = 0; j < this->vd[i]; j++ )
		{
			int u = this->vve[this->v[i] + j];

			if ( u >= 0 )
			{
				// for every neighbor, we make a new vertex!
				blowup->v[deg_sums[i] + j] = cur_v_index;	
				blowup->vd[deg_sums[i] + j] = 3;

				blowup->vve[cur_v_index + 0] = deg_sums[i] + (( j + 1 ) % this->vd[i]);
				blowup->vve[cur_v_index + 1] = deg_sums[i] + (( this->vd[i] + j - 1 ) % this->vd[i]);

				blowup->vfe[cur_v_index + 0] = this->vfe[this->v[i] + ( (j+1) % this->vd[i] )] >= 0 ? 2*this->vfe[this->v[i] + ( (j+1) % this->vd[i] )] : -1;
				blowup->vfe[cur_v_index + 1] = 2*i+1;
				blowup->vfe[cur_v_index + 2] = this->vfe[this->v[i] + j] >= 0 ? 2*this->vfe[this->v[i] + j] : -1;

				
				int u_to_v_j = -1;
				for ( int k = 0; k < this->vd[u]; k++ )
				{
					if ( this->vve[this->v[u] + k] == i )
					{
						u_to_v_j = k;
						break;
					}
				}

				// The orbit here is the encoding of the orbit pair of this edge!
				blowup->vo[deg_sums[i] + j] = this->num_vertex_orbits * this->vo[i] + this->vo[u];

				blowup->vve[cur_v_index + 2] = deg_sums[u] + u_to_v_j;
			
				blowup->vbe[2*cur_v_index + 0] = blowup->vfe[cur_v_index+0];
				blowup->vbe[2*cur_v_index + 1] = blowup->vve[cur_v_index+0];
				blowup->vbe[2*cur_v_index + 2] = blowup->vfe[cur_v_index+1];
				blowup->vbe[2*cur_v_index + 3] = blowup->vve[cur_v_index+1];
				blowup->vbe[2*cur_v_index + 4] = blowup->vfe[cur_v_index+2];
				blowup->vbe[2*cur_v_index + 5] = blowup->vve[cur_v_index+2];

				cur_v_index += 3;
			}
		}
	}

	int cur_f_index = 0;
	for ( int i = 0; i < this->max_facial_index; i++ )
	{
		blowup->f[ 2 * i ] = cur_f_index;

		blowup->fd[ 2 * i ] = this->fd[i] * 2;

		blowup->fo[ 2 * i ] = this->fo[i];

		for ( int j = 0; j < this->fd[i]; j++ )
		{
			// Previously vertex neighbor, now a face neighbor
			blowup->ffe[cur_f_index + 2*j + 0] = (this->fve[this->f[i] + j] >= 0) ? (2 * this->fve[this->f[i] + j] + 1) : -1; 

			// Same face neighbor
			blowup->ffe[cur_f_index + 2*j + 1] = (this->ffe[this->f[i] + j] >= 0) ? (2 * this->ffe[this->f[i] + j]) : -1;

			// Now, we consider the EDGE that is shared at this facial adjacency.
			// We need to find the positions within the neighborhoods of u and v;
			int v = this->fve[this->f[i] + j];
			int u1 = this->fve[this->f[i] + ((this->fd[i]+j-1) % this->fd[i])];
			int u2 = this->fve[this->f[i] + ((j+1) % this->fd[i])];

			if ( v >= 0 && u1 >= 0 )
			{
				int u1_in_v = -1;

				for ( int k = 0; k < this->vd[v]; k++ )
				{
					if ( this->vve[this->v[v] + k] == u1 )
					{
						u1_in_v = k;
					}
				}

				blowup->fve[cur_f_index + 2*j + 0] = deg_sums[v] + u1_in_v;
			}
			else
			{
				blowup->fve[cur_f_index + 2*j + 0] = -1;
			}

			if ( v >= 0 && u2 >= 0 )
			{
				int u2_in_v = -1;

				for ( int k = 0; k < this->vd[v]; k++ )
				{
					if ( this->vve[this->v[v] + k] == u2 )
					{
						u2_in_v = k;
					}
				}

				blowup->fve[cur_f_index + 2*j + 1] = deg_sums[v] + u2_in_v;
			}
			else
			{
				blowup->fve[cur_f_index + 2*j + 1] = -1;
			}
		}

		for ( int j = 0; j < blowup->fd[ 2 * i ]; j++ )
		{
			blowup->fbe[2 * cur_f_index + 2 * j] = blowup->fve[cur_f_index + j];
			blowup->fbe[2 * cur_f_index + 2 * j + 1] = blowup->ffe[cur_f_index + j];
		}

		cur_f_index += blowup->fd[2*i];
	}

	int max_f = 2*max_facial_index;

	for ( int i = 0; i < this->max_vertex_index; i++ )
	{
		blowup->f[ 2 * i + 1] = cur_f_index;

		blowup->fd[ 2 * i + 1 ] = this->vd[i];

		blowup->fo[ 2 * i + 1 ] = this->vo[i] + this->num_facial_orbits;

		for ( int j = 0; j < this->vd[i]; j++ )
		{
			blowup->ffe[cur_f_index + j] = this->vfe[ this->v[i] + j ] >= 0 ? 2 * this->vfe[ this->v[i] + j ] : -1;
			blowup->fve[cur_f_index + j] = deg_sums[i] + j;

			blowup->fbe[2*cur_f_index + 2*j+0] = blowup->fve[ cur_f_index + j];
			blowup->fbe[2*cur_f_index + 2*j+1] = blowup->ffe[ cur_f_index + j];
		}

		cur_f_index += blowup->fd[ 2 * i + 1 ];

		if ( max_f <= 2*i+1)
		{
			max_f = 2*i+1;
		}
	}

	blowup->max_vertex_index = vdeg_sum;
	blowup->max_facial_index = max_f;

	free(deg_sums);
	return blowup;
}

Grid* Grid::getTightBlowup() const
{
	Grid* blowup = new Grid();

	blowup->num_vertex_orbits = this->num_vertex_orbits * this->num_vertex_orbits;
	blowup->num_facial_orbits = this->num_facial_orbits + this->num_vertex_orbits;

	int vdeg_sum = 0;
	int* deg_sums = (int*)malloc(this->max_vertex_index * sizeof(int));

	for ( int i = 0; i < this->max_vertex_index; i++ )
	{
		deg_sums[i] = vdeg_sum;
		vdeg_sum += this->vd[i];
	}

	int* deg_maps = (int*) malloc(vdeg_sum * sizeof(int));
	for ( int i = 0; i < vdeg_sum; i++ )
	{
		deg_maps[i] = -1;
	}

	int cur_map = 0;
	for ( int i = 0; i < this->max_vertex_index; i++ )
	{
		// fprintf(stderr, "deg_maps : %d :", i);
		for ( int j = 0; j < this->vd[i]; j++ )
		{
			int u = this->vve[this->v[i] + j];

			if ( i < u )
			{
				deg_maps[deg_sums[i] + j] = cur_map;

				for ( int k = 0; k < this->vd[u]; k++ )
				{
					if ( this->vve[this->v[u] + k] == i )
					{
						deg_maps[deg_sums[u] + k] = cur_map;
					}
				}

				cur_map++;
			}

			// fprintf(stderr, " %d", deg_maps[deg_sums[i] + j] );
		}

		// fprintf(stderr,"\n");
	}

	int fdeg_sum = 0;
	for ( int i = 0; i < this->max_facial_index; i++ )
	{
		fdeg_sum += this->fd[i];
	}

	int max_f = 0;

	// Vertices turn into vdeg_sum vertices, each is 3-regular
	blowup->lengthenVArrays(cur_map, 4 * cur_map);
	bzero(blowup->vd, cur_map * sizeof(int));

	// Faces stay same length, faces added where vertices used to be.
	blowup->lengthenFArrays(2*(this->max_vertex_index + this->max_facial_index), 2*vdeg_sum + 2*fdeg_sum);

	bzero(blowup->fd, 2*(this->max_vertex_index + this->max_facial_index)*sizeof(int));

	int cur_v_index = 0;

	for ( int i = 0; i < this->max_vertex_index; i++ )
	{
		for ( int j = 0; j < this->vd[i]; j++ )
		{
			// for every neighbor, we make a new vertex!
			int u = this->vve[this->v[i] + j];

			if ( i < u )
			{
				int u_to_v_j = -1;
				for ( int k = 0; k < this->vd[u]; k++ )
				{
					if ( this->vve[ this->v[u] + k ] == i )
					{
						u_to_v_j = k;
						break;
					}
				}

				if ( u_to_v_j < 0 )
				{
					continue;
				}

				// ok, we actually add the vertex here!
				blowup->v[deg_maps[deg_sums[i] + j]] = cur_v_index;	
				blowup->vd[deg_maps[deg_sums[i] + j]] = 4;

				blowup->vve[cur_v_index + 0] = this->vve[ this->v[i] + (( this->vd[i] + j - 1 ) % this->vd[i])] >= 0        ? deg_maps[deg_sums[i] + (( this->vd[i] + j - 1 ) % this->vd[i])] : -1;
				blowup->vve[cur_v_index + 1] = this->vve[ this->v[u] + (( u_to_v_j + 1 ) % this->vd[u])] >= 0               ? deg_maps[deg_sums[u] + (( u_to_v_j + 1 ) % this->vd[u])] : -1;
				blowup->vve[cur_v_index + 2] = this->vve[ this->v[u] + (( this->vd[u] + u_to_v_j - 1 ) % this->vd[u])] >= 0 ? deg_maps[deg_sums[u] + (( this->vd[u] + u_to_v_j - 1 ) % this->vd[u])] : -1;
				blowup->vve[cur_v_index + 3] = this->vve[ this->v[i] + (( j + 1 ) % this->vd[i])] >= 0                      ? deg_maps[deg_sums[i] + (( j + 1 ) % this->vd[i])] : -1;

				blowup->vfe[cur_v_index + 0] = 2*i+1;
				blowup->vfe[cur_v_index + 1] = this->vfe[this->v[i] + j] >= 0                      ? 2*this->vfe[this->v[i] + j] : -1;
				blowup->vfe[cur_v_index + 2] = 2*u+1;
				blowup->vfe[cur_v_index + 3] = this->vfe[this->v[i] + ((j+1) % this->vd[i])] >= 0  ? 2*this->vfe[this->v[i] + ((j+1) % this->vd[i])] : -1;


				blowup->vbe[2*cur_v_index + 0] = blowup->vfe[cur_v_index+0];
				blowup->vbe[2*cur_v_index + 1] = blowup->vve[cur_v_index+0];
				
				blowup->vbe[2*cur_v_index + 2] = blowup->vfe[cur_v_index+1];
				blowup->vbe[2*cur_v_index + 3] = blowup->vve[cur_v_index+1];
				
				blowup->vbe[2*cur_v_index + 4] = blowup->vfe[cur_v_index+2];
				blowup->vbe[2*cur_v_index + 5] = blowup->vve[cur_v_index+2];

				blowup->vbe[2*cur_v_index + 6] = blowup->vfe[cur_v_index+3];
				blowup->vbe[2*cur_v_index + 7] = blowup->vve[cur_v_index+3];

				cur_v_index += 4;

				// The orbit here is the encoding of the orbit pair of this edge!
				blowup->vo[ deg_maps[deg_sums[i] + j] ] = this->num_vertex_orbits * this->vo[i] + this->vo[u];
			}
		}
	}

	int cur_f_index = 0;
	for ( int i = 0; i < this->max_facial_index; i++ )
	{
		blowup->f[2*i] = cur_f_index;

		blowup->fd[2*i] = this->fd[i];

		blowup->fo[2*i] = this->fo[i];

		for ( int j = 0; j < this->fd[i]; j++ )
		{
			// Previously vertex neighbor, now a face neighbor
			blowup->ffe[cur_f_index + j] = this->fve[this->f[i] + j] >= 0 ? 2 * this->fve[this->f[i] + j] + 1 : -1;

			// Now, we consider the EDGE that is shared at this facial adjacency.
			// We need to find the positions within the neighborhoods of u and v;
			int v = this->fve[this->f[i] + j];
			int u1 = this->fve[this->f[i] + ((this->fd[i]+j-1) % this->fd[i])];

			if ( v >= 0 && u1 >= 0 )
			{
				int u1_in_v = -1;

				for ( int k = 0; k < this->vd[v]; k++ )
				{
					if ( this->vve[this->v[v] + k] == u1 )
					{
						u1_in_v = k;
					}
				}

				if ( u1_in_v >= 0 )
				{
					blowup->fve[cur_f_index + j] = deg_maps[deg_sums[v] + u1_in_v];
				}
				else
				{
					blowup->fve[cur_f_index + j] = -1;
				}
			}
			else
			{
				blowup->fve[cur_f_index + j] = -1;
			}
		}

		for ( int j = 0; j < blowup->fd[ 2 * i ]; j++ )
		{
			blowup->fbe[2*cur_f_index + 2*j ] = blowup->fve[cur_f_index + j];
			blowup->fbe[2*cur_f_index + 2*j+1 ] = blowup->ffe[cur_f_index + j];
		}

		cur_f_index += blowup->fd[2*i];
	}

	max_f = this->max_facial_index * 2;

	for ( int i = 0; i < this->max_vertex_index; i++ )
	{
		blowup->f[2*i+1] = cur_f_index;

		blowup->fd[2*i+1] = this->vd[i];

		blowup->fo[2*i+1] = this->vo[i] + this->num_facial_orbits;

		for ( int j = 0; j < this->vd[i]; j++ )
		{
			blowup->ffe[cur_f_index + j] = this->vfe[ this->v[i] + j ] >= 0 ? 2 * this->vfe[ this->v[i] + j ] : -1; 

			if ( this->vve[ this->v[i] + j ] >= 0 )
			{
				blowup->fve[cur_f_index + j] = deg_maps[ deg_sums[i] + j ];
			}
			else
			{
				blowup->fve[cur_f_index + j] = -1;
			}

			blowup->fbe[2*cur_f_index + 2*j+0] = blowup->fve[ cur_f_index + j];
			blowup->fbe[2*cur_f_index + 2*j+1] = blowup->ffe[ cur_f_index + j];
		}

		cur_f_index += blowup->fd[2*i+1];

		if ( max_f <= 2*i+1 )
		{
			max_f= 2*i+1;
		}
	}

	blowup->max_vertex_index = cur_map;
	blowup->max_facial_index = max_f;

	free(deg_sums);
	free(deg_maps);
	return blowup;
}


Grid* Grid::getSnub() const
{
	// Every vertex becomes a face,
	// Every _edge_ creates two triangles
	// So, we need indices for all of the EDGES in addition to the vertices/faces

	// f -> 4 * f [face]
	// v -> 4 * v + 1 [face]
	// e -> 4 * e + 2, 4 * e + 4 [faces], 2 * e, 2 * e + 1 [vertices]


	// TODO: finish the snub operation!

	return 0;
}

int Grid::getVertexDegree(int vertex)
{
	if ( vertex < 0 || vertex >= this->max_vertex_index )
	{
		return -1;
	}

	return this->vd[vertex];
}

int Grid::getFaceDegree(int face)
{
	if ( face < 0 || face >= this->max_facial_index )
	{
		return -1;
	}

	return this->fd[face];
}

int Grid::getNumVertexOrbits()
{
	return this->num_vertex_orbits;
}

int Grid::getNumFacialOrbits()
{
	return this->num_facial_orbits;
}

int Grid::getVertexOrbitRepresentative(int orbit)
{
	if ( orbit < 0 || orbit >= this->num_vertex_orbits )
	{
		return -1;
	}

	for ( int i = 0; i < this->max_vertex_index; i++ )
	{
		if ( this->vo[i] == orbit )
		{
			return i;
		}
	}

	return -1;
}

int Grid::getFacialOrbitRepresentative(int orbit)
{
	if ( orbit < 0 || orbit >= this->num_facial_orbits )
	{
		return -1;
	}

	for ( int i = 0; i < this->max_facial_index; i++ )
	{
		if ( this->fo[i] == orbit )
		{
			return i;
		}
	}

	return -1;
}

int Grid::getOrbitOfVertex(int vertex)
{
	return this->vo[vertex];
}

int Grid::getOrbitOfFace(int face)
{
	return this->fo[face];
}

int Grid::getMaxVertexIndex()
{
	return this->max_vertex_index;
}

int Grid::getMaxFacialIndex()
{
	return this->max_facial_index;
}

/**
 * Get the ith neighbor of v, modulo the degree of v!
 *
 * V : vertex
 * F : face
 * B : both
 *
 * protects against arbitrary positive rotations, and up to one negative rotation
 */
int Grid::neighborVV(int v, int i)
{
	if ( v < 0 || v >= this->max_vertex_index || this->vd[v] <= 0 )
	{
		return -1;
	}

	return this->vve[this->v[v] + ((this->vd[v] + i) % this->vd[v])];
}

int Grid::neighborVF(int v, int i)
{
	if ( v < 0 || v >= this->max_vertex_index || this->vd[v] <= 0 )
	{
		return -1;
	}

	return this->vfe[this->v[v] + ((this->vd[v] + i) % this->vd[v])];
}

int Grid::neighborVB(int v, int i)
{
	if ( v < 0 || v >= this->max_vertex_index || this->vd[v] <= 0 )
	{
		return -1;
	}

	return this->vbe[2 * this->v[v] + ((2 * this->vd[v] + i) % (2 * this->vd[v]))];
}

int Grid::neighborFV(int v, int i)
{
	if ( v < 0 || v >= this->max_facial_index || this->fd[v] <= 0 )
	{
		return -1;
	}

	return this->fve[this->f[v] + ((this->fd[v] + i) % this->fd[v])];
}

int Grid::neighborFF(int v, int i)
{
	if ( v < 0 || v >= this->max_facial_index || this->fd[v] <= 0 )
	{
		return -1;
	}

	return this->ffe[this->f[v] + ((this->fd[v] + i) % this->fd[v])];
}

int Grid::neighborFB(int v, int i)
{
	if ( v < 0 || v >= this->max_facial_index || this->fd[v] <= 0)
	{
		return -1;
	}

	return this->fbe[2 * this->f[v] + ((i + 2 * this->fd[v]) % (2 * this->fd[v]))];
}

/**
 * TODO: Implement the transform?? methods.
 *
 *
 * The transform?? methods fill two permutations for how to transform vertices and faces
 * according to the given "rooted" objects.
 */
bool Grid::transformVV(int*& fperm, int*& vperm, int from_v1, int from_v2, int to_v1, int to_v2, bool flip)
{
	if ( from_v1 < 0 || from_v2 < 0 || to_v1 < 0 || to_v2 < 0 || from_v1 >= this->max_vertex_index || from_v2 >= this->max_vertex_index
			|| to_v1 >= this->max_vertex_index || to_v2 >= this->max_vertex_index )
	{
		// printf("--[transformVV(*,*,%d,%d,%d,%d,*)] Something out of bounds!\n", from_v1, from_v2, to_v1, to_v2);
		return false;
	}

	if ( this->vo[from_v1] != this->vo[to_v1] || this->vo[from_v2] != this->vo[to_v2] || this->vd[from_v1] != this->vd[to_v1] || this->vd[from_v2] != this->vd[to_v2] )
	{
		// printf("--[transformVV(*,*,%d,%d,%d,%d,*)] Orbits do not match!\n", from_v1, from_v2, to_v1, to_v2);
		return false;
	}

	fperm = (int*) realloc(fperm, this->max_facial_index * sizeof(int));
	vperm = (int*) realloc(vperm, this->max_vertex_index * sizeof(int));

	// Clear out the permutations!
	for ( int i = 0; i < this->max_vertex_index; i++ )
	{
		vperm[i] = -1;
	}

	for ( int j = 0; j < this->max_facial_index; j++ )
	{
		fperm[j] = -1;
	}

	// Now, create a "predecessor" array, so we know which things are FIXED
	int* pred = (int*) malloc((this->max_vertex_index + this->max_facial_index) * sizeof(int));

	// Perform BFS from the two starting vertices, growing out until everything is found!
	pred[from_v1] = from_v2;
	pred[from_v2] = from_v1;

	vperm[from_v1] = to_v1;
	vperm[from_v2] = to_v2;

	std::queue<int> q;
	q.push(from_v1);
	q.push(from_v2);

	int sign = 1;
	if ( flip )
	{
		// change directions!
		sign = -1;
	}

	while ( q.size() > 0 )
	{
		 // printf("Fperm: ");
	  //   for ( int i = 0; i < 20; i++ )
	  //   {
	  //       printf("%d -> %d, ", i, fperm[i]);
	  //   }
	  //   printf("\n");
	  //   printf("Vperm: ");
	  //   for ( int i = 0; i < 20; i++ )
	  //   {
	  //       printf("%d -> %d, ", i, vperm[i]);
	  //   }
	  //   printf("\n");

		int v = q.front();
		q.pop();

		if ( v < 0 )
		{
			continue;
		}

		if ( v < this->max_vertex_index )
		{
			// This is a VERTEX
			int u = vperm[v];

			int voffset = 0;
			int uoffset = 0;

			int p = pred[v];
			int pv = p;
			int pf = -1;
			int pu = 0;
			int pg = 0;

			if ( p >= this->max_vertex_index )
			{
				pf = p - this->max_vertex_index;
				pg = fperm[pf];

				if ( this->fo[pf] != this->fo[pg] )
				{
					free(pred);
					return false;
				}

				// find pf in the neighborhood of f
				for ( int i = 0; i < this->getVertexDegree(v); i++ )
				{
					if ( this->neighborVB(v, 2 * i) == pf )
					{
						voffset = 2 * i;
						break;
					}
				}

				// find pg in the neighborhood of u
				for ( int i = 0; i < this->getVertexDegree(u); i++ )
				{
					if ( this->neighborVB(u, 2 * i) == pg )
					{
						uoffset = 2 * i;
						break;
					}
				}

				// Now, if we switch directions, no need to change
				// this offset from what we have!
			}
			else
			{
				pu = vperm[pv];

				if ( this->vo[pu] != this->vo[pv] )
				{
					free(pred);
					return false;
				}

				// find pv in the neighborhood of f
				for ( int i = 0; i < this->getVertexDegree(v); i++ )
				{
					if ( this->neighborVB(v, 2 * i + 1) == pv )
					{
						voffset = 2 * i + 1;
						break;
					}
				}
				// find pu in the neighborhood of g
				for ( int i = 0; i < this->getVertexDegree(u); i++ )
				{
					if ( this->neighborVB(u, 2 * i + 1) == pu )
					{
						uoffset = 2 * i + 1;
						break;
					}
				}

				// Now, if we switch directions, no need to change
				// this offset from what we have!
			}

			// Loop through all facial and vertex neighbors.
			// Update their permutations if they are not there yet!
			for ( int i = 0; i < 2 * this->getVertexDegree(v); i++ )
			{
				int x = this->neighborVB(v, voffset + i);
				int y = this->neighborVB(u, uoffset + sign * i);

				if ( x < 0 || y < 0 )
				{
					continue;
				}

				if ( ((voffset + i) % 2) == 1 )
				{
					// is a vertex!
					if ( x >= 0 && vperm[x] != y )
					{
						vperm[x] = y;

						if ( x >= 0 && y >= 0 && this->vo[x] != this->vo[y] )
						{
							// printf("trying to send vert %d to vert %d\n", x, y);
							free(pred);
							return false;
						}

						pred[x] = v;
						q.push(x);
					}
				}
				else
				{
					// is a face!
					if ( x >= 0 && fperm[x] != y )
					{
						fperm[x] = y;

						if (x >= 0 && y >= 0 && this->fo[x] != this->fo[y] )
						{
							// printf("trying to send face %d to face %d\n", x, y);
							free(pred);
							return false;
						}

						pred[x + this->max_vertex_index] = v;
						q.push(x + this->max_vertex_index);
					}
				}
			}
		}
		else
		{
			// This is a FACE
			int foffset = 0;
//			int voffset = 0;
			int goffset = 0;
//			int uoffset = 0;

			int f = v - this->max_vertex_index;
			int g = fperm[f];

			int p = pred[v];
			int pv = p;
			int pf = -1;
			int pu = 0;
			int pg = 0;

			if ( p >= this->max_vertex_index )
			{
				pf = p - this->max_vertex_index;
				pg = fperm[pf];

				if (pg >= 0 && pf >= 0 && this->fo[pg] != this->fo[pf] )
				{
							// printf("trying to send face %d to face %d\n", pf, pg);
					free(pred);
					return false;
				}

				// find pf in the neighborhood of f
				for ( int i = 0; i < this->getFaceDegree(f); i++ )
				{
					if ( this->neighborFB(f, 2 * i + 1) == pf )
					{
						foffset = 2 * i + 1;
						break;
					}
				}
				// find pg in the neighborhood of g
				for ( int i = 0; i < this->getFaceDegree(g); i++ )
				{
					if ( this->neighborFB(g, 2 * i + 1) == pg )
					{
						goffset = 2 * i + 1;
						break;
					}
				}

				// Now, if we switch directions, no need to change
				// this offset from what we have!
			}
			else
			{
				pu = vperm[pv];

				if ( pu >= 0 && pv >= 0 && (this->vo[pu] != this->vo[pv] || this->vd[pu] != this->vd[pv]) ) 
				{
							// printf("trying to send vert %d to vert %d\n", pu, pv);
					free(pred);
					return false;
				}

				// find pv in the neighborhood of f
				for ( int i = 0; i < this->getFaceDegree(f); i++ )
				{
					if ( this->neighborFB(f, 2 * i) == pv )
					{
						foffset = 2 * i;
						break;
					}
				}
				// find pu in the neighborhood of g
				for ( int i = 0; i < this->getFaceDegree(g); i++ )
				{
					if ( this->neighborFB(g, 2 * i) == pu )
					{
						goffset = 2 * i;
						break;
					}
				}

				// Now, if we switch directions, no need to change
				// this offset from what we have!
			}

			// Loop through all facial and vertex neighbors.
			// Update their permutations if they are not there yet!
			for ( int i = 0; i < 2 * this->getFaceDegree(f); i++ )
			{
				int x = this->neighborFB(f, foffset + i);
				int y = this->neighborFB(g, goffset + sign * i);

				
				if ( x < 0 || y < 0 )
				{
					continue;
				}

				if ( ((foffset + i) % 2) == 0 )
				{
					// is a vertex!
					if ( x >= 0 && vperm[x] != y )
					{
						vperm[x] = y;

						if ( x >= 0 && y >= 0 &&  (this->vo[x] != this->vo[y] || this->vd[x] != this->vd[y]) )
						{
							// printf("trying to send vert %d to vert %d\n", x, y);
							free(pred);
							return false;
						}

						pred[x] = f + this->max_vertex_index;
						q.push(x);
					}
				}
				else
				{
					// is a face!
					if ( x >= 0 && fperm[x] != y )
					{
						fperm[x] = y;

						if (x >= 0 && y >= 0 && this->fo[x] != this->fo[y] )
						{
							// printf("trying to send face %d to face %d\n", x, y);
							free(pred);
							return false;
						}

						pred[x + this->max_vertex_index] = f + this->max_vertex_index;
						q.push(x + this->max_vertex_index);
					}
				}
			}
		}
	}

	free(pred);


	return true;
}

bool Grid::transformVF(int*& fperm, int*& vperm, int from_v1, int from_f2, int to_v1, int to_f2, bool flip)
{
	// Assume that the face and vertex are incident
	// Find a vertex that corresponds to this change!
	int v1_in_f2n = -1;
	int f2_in_v1n = -1;
	int u1_in_g2n = -1;
	int g2_in_u1n = -1;

	if ( this->vo[from_v1] != this->vo[to_v1] )
	{
		return false;
	}

	if ( this->vd[from_v1] != this->vd[to_v1] )
	{
		return false;
	}

	for ( int i = 0; i < this->getFaceDegree(from_f2); i++ )
	{
		if ( from_v1 == this->neighborFV(from_f2, i) )
		{
			v1_in_f2n = i;
			break;
		}
	}
	for ( int i = 0; i < this->getVertexDegree(from_v1); i++ )
	{
		if ( from_f2 == this->neighborVF(from_v1, i) )
		{
			f2_in_v1n = i;
			break;
		}
	}

	for ( int i = 0; i < this->getFaceDegree(to_f2); i++ )
	{
		if ( to_v1 == this->neighborFV(to_f2, i) )
		{
			u1_in_g2n = i;
			break;
		}
	}
	for ( int i = 0; i < this->getVertexDegree(to_v1); i++ )
	{
		if ( to_f2 == this->neighborVF(to_v1, i) )
		{
			g2_in_u1n = i;
			break;
		}
	}

	if ( v1_in_f2n < 0 || f2_in_v1n < 0 || u1_in_g2n < 0 || g2_in_u1n < 0 )
	{
		// printf("--[transformVF(*,*,%d,%d,%d,%d,*)] Nonadjacent face/vertex combo!\n", from_v1, from_f2, to_v1, to_f2);
		// if ( v1_in_f2n < 0 )
		// {
		// 	printf("v1_in_f2n < 0.\n");
		// }
		// if ( f2_in_v1n < 0 )
		// {
		// 	printf("f2_in_v1n < 0.\n");
		// }
		// if ( u1_in_g2n < 0 )
		// {
		// 	printf("u1_in_g2n < 0.\n");
		// }
		// if ( g2_in_u1n < 0 )
		// {
		// 	printf("g2_in_u1n < 0.\n");
		// }
		return false; // false?
	}

	int v2 = this->neighborVV(from_v1, f2_in_v1n); // vertex follows face
	int u2 = this->neighborVV(to_v1, g2_in_u1n); // vertex follows face
	if ( flip )
	{
		u2 = this->neighborVV(to_v1, g2_in_u1n - 1); // vertex precedes face
	}

	return this->transformVV(fperm, vperm, from_v1, v2, to_v1, u2, flip);
}

bool Grid::transformFV(int*& fperm, int*& vperm, int from_f1, int from_v2, int to_f1, int to_v2, bool flip)
{
	// Simple change of coordinates!
	return this->transformVF(fperm, vperm, from_v2, from_f1, to_v2, to_f1, flip);
}

bool Grid::transformFF(int*& fperm, int*& vperm, int from_f1, int from_f2, int to_f1, int to_f2, bool flip)
{
	// Assume that the faces are adjacent
	// Find two vertices to transform into each other.
	int f2_in_f1n = -1;
	int f1_in_f2n = -1;
	int g2_in_g1n = -1;
	int g1_in_g2n = -1;

	for ( int i = 0; i < this->getFaceDegree(from_f1); i++ )
	{
		if ( from_f2 == this->neighborFF(from_f1, i) )
		{
			f2_in_f1n = i;
			break;
		}
	}

	for ( int i = 0; i < this->getFaceDegree(from_f2); i++ )
	{
		if ( from_f1 == this->neighborFF(from_f2, i) )
		{
			f1_in_f2n = i;
			break;
		}
	}

	for ( int i = 0; i < this->getFaceDegree(to_f2); i++ )
	{
		if ( to_f1 == this->neighborFF(to_f2, i) )
		{
			g1_in_g2n = i;
			break;
		}
	}

	for ( int i = 0; i < this->getFaceDegree(to_f1); i++ )
	{
		if ( to_f2 == this->neighborFF(to_f1, i) )
		{
			g2_in_g1n = i;
			break;
		}
	}

	if ( f1_in_f2n < 0 || f2_in_f1n < 0 || g1_in_g2n < 0 || g2_in_g1n < 0 )
	{
		// printf("--[transformFF(*,*,%d,%d,%d,%d,*)] Nonadjacent faces!\n", from_f1, from_f2, to_f1, to_f2);
		return false; // false?
	}

	int v1 = this->neighborFV(from_f1, f2_in_f1n);
	int v2 = this->neighborFV(from_f1, f2_in_f1n + 1);

	int u1 = this->neighborFV(to_f1, g2_in_g1n);
	int u2 = this->neighborFV(to_f1, g2_in_g1n + 1);

	if ( flip )
	{
		// swap these resulting vertices!
		int t = u1;
		u1 = u2;
		u2 = t;
	}

	bool result = this->transformVV(fperm, vperm, v1, v2, u1, u2, flip);

	if ( fperm[from_f1] != to_f1 || fperm[from_f2] != to_f2 )
	{
		// printf("[transformFF] Error! %d -> %d (not %d) or %d -> %d (not %d)\n", from_f1, fperm[from_f1], to_f1, from_f2, fperm[from_f2], to_f2);
		return false;
	}

	return result;
}

void Grid::printDualAdjLists()
{
//	printf("Vertices: %d\nFaces: %d\n", this->max_vertex_index, this->max_facial_index);

	for ( int v = 0; v < this->max_vertex_index; v++ )
	{
		printf("%d : ", v);
		for ( int i = 0; i < this->getVertexDegree(v); i++ )
		{
			// face, then vertex
			if ( this->vbe[this->v[v] * 2 + 2 * i] >= 0 )
			{
				printf("%d ", this->vbe[this->v[v] * 2 + 2 * i] + this->max_vertex_index);
			}
			else
			{
				printf("-1 ");
			}
			printf("%d ", this->vbe[this->v[v] * 2 + 2 * i + 1]);
		}
		// printf(" : N = ");
		// for ( int i = 0; i < this->getVertexDegree(v); i++ )
		// {
		// 	printf("%d ", this->neighborVV(v,i) );
		// }

		// printf(" : F = ");
		// for ( int i = 0; i < this->getVertexDegree(v); i++ )
		// {
		// 	printf("%d ", this->neighborVF(v,i) );
		// }
		printf("\n");
	}

	for ( int f = 0; f < this->max_facial_index; f++ )
	{
		printf("%d : ", f + this->max_vertex_index);
		for ( int i = 0; i < this->fd[f]; i++ )
		{
			// vertex, then face
			printf("%d ", this->fbe[this->f[f] * 2 + 2 * i]);

			if ( this->fbe[this->f[f] * 2 + 2 * i + 1] >= 0 )
			{
				printf("%d ", this->fbe[this->f[f] * 2 + 2 * i + 1] + this->max_vertex_index);
			}
			else
			{
				printf("-1 ");
			}
		}
		// printf(" : N = ");
		// for ( int i = 0; i < this->getFaceDegree(f); i++ )
		// {
		// 	printf("%d ", this->neighborFV(f,i) );
		// }

		// printf(" : F = ");
		// for ( int i = 0; i < this->getFaceDegree(f); i++ )
		// {
		// 	printf("%d ", this->neighborFF(f,i) );
		// }
		printf("\n");
	}
}

/**
 * Given a vertex, determine the ball of a given radius.
 */
int* Grid::getVertexBall(int vertex, int radius, int& ball_size)
{
	int array_size = 100;
	int* ball = (int*) malloc(array_size * sizeof(int));
	static int* dists = (int*) malloc(this->max_vertex_index * sizeof(int));

	ball[0] = vertex;

	for ( int i = 0; i < this->max_vertex_index; i++ )
	{
		dists[i] = -1;
	}

	dists[vertex] = 0;
	ball_size = 1;

	std::queue<int> q;
	q.push(vertex);

	while ( q.size() > 0 )
	{
		int u = q.front();
		q.pop();

		for ( int i = 0; i < this->getVertexDegree(u); i++ )
		{
			int x = this->neighborVV(u, i);

			if ( x < 0 )
			{
				continue;
			}

			if ( dists[x] < 0 )
			{
				dists[x] = dists[u] + 1;

				while ( ball_size >= array_size )
				{
					array_size *= 2;
					ball = (int*) realloc(ball, array_size * sizeof(int));
				}

				if ( dists[x] <= radius )
				{
					ball[ball_size++] = x;
				}

				if ( dists[x] < radius )
				{
					q.push(x);
				}
			}
		}
	}

	return ball;
}

