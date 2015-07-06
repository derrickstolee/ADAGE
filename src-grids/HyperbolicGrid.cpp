/*
 * HyperbolicGrid.cpp
 *
 *  Created on: Jul 1, 2015
 *      Author: stolee
 */

#include "HyperbolicGrid.hpp"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <queue>

using namespace adage::grids;

int HyperbolicGrid::reserveNewFace()
{
	int f_index = this->max_facial_index;
	(this->max_facial_index)++;

	this->fd[f_index] = this->face_len;
	this->f[f_index] = this->face_len * f_index;
	this->max_f_value = this->face_len * (f_index + 1);
	this->fo[f_index] = 0;

	// Fill edges with blanks!
	for ( int i = 0; i < this->face_len; i++ )
	{
		this->fve[this->f[f_index] + i] = -1;
		this->ffe[this->f[f_index] + i] = -1;
		this->fbe[2 * this->f[f_index] + 2 * i] = -1;
		this->fbe[2 * this->f[f_index] + 2 * i + 1] = -1;
	}

	return f_index;
}

int HyperbolicGrid::reserveNewVertex()
{
	int v_index = this->max_vertex_index;
	(this->max_vertex_index)++;

	this->vd[v_index] = this->vertex_degree;
	this->v[v_index] = this->vertex_degree * v_index;

	this->max_v_value = this->vertex_degree * (v_index + 1);
	this->vo[v_index] = 0;

	// Fill edges with blanks!
	for ( int i = 0; i < this->vertex_degree; i++ )
	{
		this->vve[this->v[v_index] + i] = -1;
		this->vfe[this->v[v_index] + i] = -1;
		this->vbe[2 * this->v[v_index] + 2 * i] = -1;
		this->vbe[2 * this->v[v_index] + 2 * i + 1] = -1;
	}

	return v_index;
}

HyperbolicGrid::HyperbolicGrid(int deg, int len, int raduis)
{
	this->reserveNewVertex();

	std::queue<int> vertex_queue;
}

HyperbolicGrid::~HyperbolicGrid()
{
}

