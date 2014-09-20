/*
 * Configuration.cpp
 *
 *  Created on: Apr 17, 2014
 *      Author: stolee
 */

#include "Configuration.hpp"
#include "macros.hpp"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <queue>

using namespace adage::grids;

/********************************************************************
 *  SHAPE                                                                                         *
 ********************************************************************/
void Configuration::initShapeVertices()
{
    this->size_shape_vertices = 100;
    this->vertices_in_shape = 0;
    this->shape_vertices = (int*)malloc(this->size_shape_vertices * sizeof(int));
}

void Configuration::freeShapeVertices()
{
    FREE_ARRAY(this->shape_vertices);
    CLEAR_STACK(this->snapshot_num_vertices_in_shape);
}

void Configuration::snapshotShapeVertices()
{
    this->snapshot_num_vertices_in_shape.push(this->vertices_in_shape);
}

void Configuration::rollbackShapeVertices()
{
    if (this->snapshot_num_vertices_in_shape.size() > 0) {
        this->vertices_in_shape = this->snapshot_num_vertices_in_shape.top();
        this->snapshot_num_vertices_in_shape.pop();
    } else {
        this->vertices_in_shape = 0;
    }
}

/********************************************************************
 *  ELEMENTS                                                                                    *
 ********************************************************************/
void Configuration::initElements()
{
    this->size_elements = 100;
    this->num_elements = 0;
    this->elements = (int*)malloc(this->size_elements * sizeof(int));

    this->size_nonelements = 100;
    this->num_nonelements = 0;
    this->nonelements = (int*)malloc(this->size_elements * sizeof(int));
}

void Configuration::freeElements()
{
    FREE_ARRAY(this->elements);
    FREE_ARRAY(this->nonelements);

    CLEAR_STACK(this->snapshot_num_elements);
    CLEAR_STACK(this->snapshot_num_nonelements);
}

void Configuration::snapshotElements()
{
    this->snapshot_num_elements.push(this->num_elements);
    this->snapshot_num_nonelements.push(this->num_nonelements);
}

void Configuration::rollbackElements()
{
    if (this->snapshot_num_elements.size() > 0) {
        this->num_elements = this->snapshot_num_elements.top();
        this->snapshot_num_elements.pop();
    } else {
        this->num_elements = 0;
    }

    if (this->snapshot_num_nonelements.size() > 0) {
        this->num_nonelements = this->snapshot_num_nonelements.top();
        this->snapshot_num_nonelements.pop();
    } else {
        this->num_nonelements = 0;
    }
}

/********************************************************************
 *  BITMASKS                                                                                    *
 ********************************************************************/
void Configuration::initBitmasks()
{
    this->num_bits_per = 60;

    this->max_v_index = this->grid->getMaxVertexIndex();

    this->max_v_index += (num_bits_per - (this->max_v_index % num_bits_per));
    this->bitmask_length = this->max_v_index / num_bits_per;

    this->vertex_in_shape = (unsigned long long int*)malloc(this->bitmask_length * sizeof(unsigned long long int));
    this->vertex_is_element = (unsigned long long int*)malloc(this->bitmask_length * sizeof(unsigned long long int));
    this->vertex_is_nonelement = (unsigned long long int*)malloc(this->bitmask_length * sizeof(unsigned long long int));

    bzero(this->vertex_in_shape, this->bitmask_length * sizeof(unsigned long long int));
    bzero(this->vertex_is_element, this->bitmask_length * sizeof(unsigned long long int));
    bzero(this->vertex_is_nonelement, this->bitmask_length * sizeof(unsigned long long int));
}

void Configuration::freeBitmasks()
{
    FREE_ARRAY(this->vertex_in_shape);
    FREE_ARRAY(this->vertex_is_element);
    FREE_ARRAY(this->vertex_is_nonelement);

    CLEAR_STACK(this->snapshot_bitmask_shape_i);
    CLEAR_STACK(this->snapshot_bitmask_element_i);
    CLEAR_STACK(this->snapshot_bitmask_nonelement_i);
    CLEAR_STACK(this->snapshot_bitmask_shape_size);
    CLEAR_STACK(this->snapshot_bitmask_element_size);
    CLEAR_STACK(this->snapshot_bitmask_nonelement_size);
}

void Configuration::snapshotBitmasks()
{
    this->snapshot_bitmask_shape_size.push(this->snapshot_bitmask_shape_i.size());
    this->snapshot_bitmask_element_size.push(this->snapshot_bitmask_element_i.size());
    this->snapshot_bitmask_nonelement_size.push(this->snapshot_bitmask_nonelement_i.size());
}

void Configuration::rollbackBitmasks()
{
    unsigned int shape_to_size = 0;
    if (this->snapshot_bitmask_shape_size.size() > 0) {
        shape_to_size = this->snapshot_bitmask_shape_size.top();
        this->snapshot_bitmask_shape_size.pop();
    }

    while (this->snapshot_bitmask_shape_i.size() > shape_to_size) {
        int i = this->snapshot_bitmask_shape_i.top();
        this->snapshot_bitmask_shape_i.pop();

        this->turnBitOff(this->vertex_in_shape, i);
    }

    unsigned int element_to_size = 0;
    if (this->snapshot_bitmask_element_size.size() > 0) {
        element_to_size = this->snapshot_bitmask_element_size.top();
        this->snapshot_bitmask_element_size.pop();
    }

    while (this->snapshot_bitmask_element_i.size() > element_to_size) {
        int i = this->snapshot_bitmask_element_i.top();
        this->snapshot_bitmask_element_i.pop();

        this->turnBitOff(this->vertex_is_element, i);
    }

    unsigned int nonelement_to_size = 0;
    if (this->snapshot_bitmask_nonelement_size.size() > 0) {
        nonelement_to_size = this->snapshot_bitmask_nonelement_size.top();
        this->snapshot_bitmask_nonelement_size.pop();
    }

    while (this->snapshot_bitmask_nonelement_i.size() > nonelement_to_size) {
        int i = this->snapshot_bitmask_nonelement_i.top();
        this->snapshot_bitmask_nonelement_i.pop();

        this->turnBitOff(this->vertex_is_nonelement, i);
    }
}

bool Configuration::isBitOn(unsigned long long int* array, int i)
{
    if ( array[i / num_bits_per] & (((unsigned long long int)1) << (unsigned long long int)(i % num_bits_per))) {
        return true;
    }

    return false;
}

void Configuration::turnBitOn(unsigned long long int* array, int i)
{
    (array[i / num_bits_per]) |= (((unsigned long long int)1) << (unsigned long long int)(i % (num_bits_per)));
}

void Configuration::turnBitOff(unsigned long long int* array, int i)
{
    (array[i / num_bits_per]) &= ~(((unsigned long long int)1) << (unsigned long long int)(i % (num_bits_per)));
}

void Configuration::setVertexInShapeBitmask(int i)
{
    if (i < 0 || i >= this->size_shape_vertices) {
        return;
    }

    if (this->isBitOn(this->vertex_in_shape, i)) {
        return;
    }

    this->turnBitOn(this->vertex_in_shape, i);
    this->snapshot_bitmask_shape_i.push(i);
}

void Configuration::setIsElementBitmask(int i)
{
    if (i < 0 || i >= this->size_shape_vertices) {
        return;
    }

    if (this->isBitOn(this->vertex_is_element, i)) {
        return;
    }

    this->turnBitOn(this->vertex_is_element, i);
    this->snapshot_bitmask_element_i.push(i);
}

void Configuration::setIsNonElementBitmask(int i)
{
    if (i < 0 || i >= this->size_shape_vertices) {
        return;
    }

    if (this->isBitOn(this->vertex_is_nonelement, i)) {
        return;
    }

    this->turnBitOn(this->vertex_is_nonelement, i);
    this->snapshot_bitmask_nonelement_i.push(i);
}

/********************************************************************
 *  FACES                                                                                         *
 ********************************************************************/
void Configuration::initFaces()
{
    this->faces_length = this->grid->getMaxFacialIndex();

    this->faces_in_shape = 0;

    this->face_index = (int*)malloc(this->faces_length * sizeof(int));
    this->face_to_index = (int*)malloc(this->faces_length * sizeof(int));
    this->face_elts = (int*)malloc(this->faces_length * sizeof(int));
    this->face_nonelts = (int*)malloc(this->faces_length * sizeof(int));
    this->face_undets = (int*)malloc(this->faces_length * sizeof(int));
    this->face_sizes = (int*)malloc(this->faces_length * sizeof(int));

    for (int i = 0; i < this->faces_length; i++) {
        this->face_to_index[i] = -1;
        this->face_elts[i] = 0;
        this->face_nonelts[i] = 0;
        this->face_undets[i] = this->grid->getFaceDegree(i);
        this->face_sizes[i] = -1;
    }
}

void Configuration::freeFaces()
{
    FREE_ARRAY(this->face_index);
    FREE_ARRAY(this->face_to_index);
    FREE_ARRAY(this->face_elts);
    FREE_ARRAY(this->face_nonelts);
    FREE_ARRAY(this->face_undets);
    FREE_ARRAY(this->face_sizes);

    CLEAR_STACK(this->snapshot_face_in_shape);
    CLEAR_STACK(this->snapshot_face_elt_i);
    CLEAR_STACK(this->snapshot_face_nonelt_i);
    CLEAR_STACK(this->snapshot_face_elt_size);
    CLEAR_STACK(this->snapshot_face_nonelt_size);
    CLEAR_STACK(this->snapshot_face_size_i);
    CLEAR_STACK(this->snapshot_face_size_size);
}

void Configuration::snapshotFaces()
{
    this->snapshot_face_in_shape.push(this->faces_in_shape);
    this->snapshot_face_elt_size.push(this->snapshot_face_elt_i.size());
    this->snapshot_face_nonelt_size.push(this->snapshot_face_nonelt_i.size());
    this->snapshot_face_size_size.push(this->snapshot_face_size_i.size());
}

void Configuration::rollbackFaces()
{
    int old_faces_in_shape = this->faces_in_shape;
    if (this->snapshot_face_in_shape.size() > 0) {
        this->faces_in_shape = this->snapshot_face_in_shape.top();
        this->snapshot_face_in_shape.pop();
    } else {
        this->faces_in_shape = 0;
    }

    // Clear out this pointer (which also serves as a Random-Access Inclusion!)
    for (int i = this->faces_in_shape; i < old_faces_in_shape; i++) {
        this->face_to_index[this->face_index[i]] = -1;
    }

    unsigned int elts_to_size = 0;
    if (this->snapshot_face_elt_size.size() > 0) {
        elts_to_size = this->snapshot_face_elt_size.top();
        this->snapshot_face_elt_size.pop();
    }

    while (this->snapshot_face_elt_i.size() > elts_to_size) {
        int i = this->snapshot_face_elt_i.top();
        this->snapshot_face_elt_i.pop();

        (this->face_elts[i])--;
        (this->face_undets[i])++;
    }

    unsigned int nonelts_to_size = 0;
    if (this->snapshot_face_nonelt_size.size() > 0) {
        nonelts_to_size = this->snapshot_face_nonelt_size.top();
        this->snapshot_face_nonelt_size.pop();
    }

    while (this->snapshot_face_nonelt_i.size() > nonelts_to_size) {
        int i = this->snapshot_face_nonelt_i.top();
        this->snapshot_face_nonelt_i.pop();

        (this->face_nonelts[i])--;
        (this->face_undets[i])++;
    }

    unsigned int face_size_to_size = 0;
    if (this->snapshot_face_size_size.size() > 0) {
        face_size_to_size = this->snapshot_face_size_size.top();
        this->snapshot_face_size_size.pop();
    }

    while (this->snapshot_face_size_i.size() > face_size_to_size) {
        int i = this->snapshot_face_size_i.top();
        this->snapshot_face_size_i.pop();
        this->face_sizes[i] = -1;
    }

	// ARRAY_BLANK_ROLLBACK(this->snapshot_face_size_size, this->snapshot_face_size_i, this->face_sizes, -1);    
}

void Configuration::addElementToFace(int f)
{
    if (f < 0 || f >= this->faces_length) {
        return;
    }

    (this->face_elts[f])++;
    (this->face_undets[f])--;
    this->snapshot_face_elt_i.push(f);

    if ( this->face_undets[f] == 0 )
    {
    	this->setFaceSize(f, this->face_elts[f]);
    }
}

void Configuration::addNonElementToFace(int f)
{
    if (f < 0 || f >= this->faces_length) {
        return;
    }

    (this->face_nonelts[f])++;
    (this->face_undets[f])--;
    this->snapshot_face_nonelt_i.push(f);

    if ( this->face_undets[f] == 0 )
    {
    	this->setFaceSize(f, this->face_elts[f]);
    }
}

int Configuration::getFaceSize(int f)
{
	for ( int i = 0; i < this->faces_in_shape; i++ )
	{
		if ( this->face_index[i] == f )
		{
			return this->face_sizes[i];
		}
	}

	return -1;
}

void Configuration::setFaceSize(int f, int s)
{
    for (int i = 0; i < this->faces_in_shape; i++) {
        if (this->face_index[i] == f) {
            if (this->face_sizes[i] >= 0) {
                if (this->face_sizes[i] != s) {
                    printf("Warning: Face %d already has a size: %d! It also has %d elts, %d nonelts, and %d undet.\n", f, this->face_sizes[i], this->face_elts[f], this->face_nonelts[f], this->face_undets[f]);
    				this->computeSymmetry();
                    this->print();
                }
                
                return;
            }

            this->face_sizes[i] = s;
            this->snapshot_face_size_i.push(i);
            return;
        }
    }
}

/********************************************************************
 *  COLORS                                                                                       *
 ********************************************************************/

/**
 * We can compute the symmetry of this object, subject to certain symmetry requirements.
 */
void Configuration::initColors()
{
    this->num_vertex_colors = 0;
    this->num_face_colors = 0;

    this->vertex_colors = (int*)malloc(this->size_shape_vertices * sizeof(int));
    bzero(this->vertex_colors, this->size_shape_vertices * sizeof(int));

    this->face_colors = (int*)malloc(this->faces_length * sizeof(int));
    bzero(this->face_colors, this->faces_length * sizeof(int));

    this->canonical_string = 0;
}

void Configuration::freeColors()
{
    FREE_ARRAY(this->vertex_colors);
    FREE_ARRAY(this->face_colors);
    FREE_ARRAY(this->canonical_string);
}

/********************************************************************
 *  CONSTRUCTORS                                                                          *
 ********************************************************************/
Configuration::Configuration(Grid* grid)
{
    this->grid = grid;

    this->vertices_in_shape = 0;
    this->size_shape_vertices = 0;
    this->shape_vertices = 0;

    this->size_elements = 0;
    this->num_elements = 0;
    this->elements = 0;
    this->size_nonelements = 0;
    this->num_nonelements = 0;
    this->nonelements = 0;

    this->max_v_index = 0;
    this->bitmask_length = 0;
    this->num_bits_per = 60;
    this->vertex_in_shape = 0;
    this->vertex_is_element = 0;
    this->vertex_is_nonelement = 0;

    this->has_center = false;
    this->center_index = 0;
    this->center_is_vertex = false;

    this->faces_length = 0;
    this->faces_in_shape = 0;
    this->face_index = 0;
    this->face_to_index = 0;
    this->face_elts = 0;
    this->face_nonelts = 0;
    this->face_undets = 0;

    this->num_vertex_colors = 0;
    this->num_face_colors = 0;
    this->vertex_colors = 0;
    this->face_colors = 0;
    this->canonical_string = 0;

    this->initAll();
}

Configuration::~Configuration()
{
    this->freeAll();
}

void Configuration::initAll()
{
    this->initShapeVertices();
    this->initElements();
    this->initBitmasks();
    this->initFaces();
    this->initColors();
}
void Configuration::freeAll()
{
    this->freeShapeVertices();
    this->freeElements();
    this->freeBitmasks();
    this->freeFaces();
    this->freeColors();
}

/********************************************************************
 *  SNAPSHOT/ROLLBACK                                                                 *
 ********************************************************************/
void Configuration::snapshot()
{
    this->snapshotShapeVertices();
    this->snapshotElements();
    this->snapshotBitmasks();
    this->snapshotFaces();
}

void Configuration::rollback()
{
    this->rollbackFaces();
    this->rollbackBitmasks();
    this->rollbackElements();
    this->rollbackShapeVertices();
}

/********************************************************************
 *  ACCESSORS                                                                                  *
 ********************************************************************/

Grid* Configuration::getGrid()
{
    return this->grid;
}


bool Configuration::isVertexInShape(int i)
{
    if (i < 0 || i >= this->max_v_index) {
        return false;
    }

    return this->isBitOn(this->vertex_in_shape, i);
}
bool Configuration::isElement(int i)
{
    if (i < 0 || i >= this->max_v_index) {
        return false;
    }

    return this->isBitOn(this->vertex_is_element, i);
}
bool Configuration::isNonElement(int i)
{
    if (i < 0 || i >= this->max_v_index) {
        return false;
    }

    return this->isBitOn(this->vertex_is_nonelement, i);
}

bool Configuration::isFaceInShape(int f)
{
    if (f < 0 || f >= this->faces_length) {
        return false;
    }

    return this->face_to_index[f] >= 0;
}

int Configuration::getNumVerticesInShape()
{
    return this->vertices_in_shape;
}

int Configuration::getVertexFromShape(int i)
{
    return this->shape_vertices[i];
}

int Configuration::getNumFacesInShape()
{
    return this->faces_in_shape;
}

int Configuration::getFaceFromShape(int i)
{
    return this->face_index[i];
}

int Configuration::getNumElementsInFace(int f)
{
	return this->face_elts[f];
}

int Configuration::getNumNonElementsInFace(int f)
{
	return this->face_nonelts[f];
}

/********************************************************************
 *  ADDING VERTICES                                                                        *
 ********************************************************************/
void Configuration::addVertexToShape(int i)
{
    if (i < 0 || i >= this->grid->getMaxVertexIndex()) {
        return;
    }

    if (this->isBitOn(this->vertex_in_shape, i)) {
        return;
    }

    if (this->vertices_in_shape >= this->size_shape_vertices) {
        this->size_shape_vertices = 2 * this->size_shape_vertices;
        this->shape_vertices = (int*)realloc(this->shape_vertices, this->size_shape_vertices * sizeof(int));
    }

    this->shape_vertices[this->vertices_in_shape] = i;
    (this->vertices_in_shape)++;

    this->turnBitOn(this->vertex_in_shape, i);
    this->snapshot_bitmask_shape_i.push(i);
}

void Configuration::addElement(int i)
{
    if (i < 0 || i >= this->grid->getMaxVertexIndex()) {
        return;
    }

    if (this->isBitOn(this->vertex_is_element, i)) {
        return;
    }

    if (this->isBitOn(this->vertex_is_nonelement, i)) {
        printf("[Configuration::addElement(%d)] This vertex is alread a nonelement!\n", i);
        exit(1);
    }

    if (!this->isBitOn(this->vertex_in_shape, i)) {
        this->addVertexToShape(i);
    }

    // Iterable
    if (this->num_elements >= this->size_elements) {
        this->size_elements = 2 * this->size_elements;
        this->elements = (int*)realloc(this->elements, this->size_elements * sizeof(int));
    }

    this->elements[this->num_elements] = i;
    (this->num_elements)++;

    // Random-access
    this->turnBitOn(this->vertex_is_element, i);
    this->snapshot_bitmask_element_i.push(i);

    // Face Counts
    for (int j = 0; j < this->grid->getVertexDegree(i); j++) {
        int face = this->grid->neighborVF(i, j);

        this->addElementToFace(face); // even if not in shape!
    }
}

void Configuration::addNonElement(int i)
{
    if (i < 0 || i >= this->grid->getMaxVertexIndex()) {
        return;
    }

    if (this->isBitOn(this->vertex_is_nonelement, i)) {
        // printf("[Configuration::addElement(%d)] This vertex is alread a nonelement!\n", i);
        // exit(1);
        return;
    }

    if (this->isBitOn(this->vertex_is_element, i)) {
        printf("[Configuration::addElement(%d)] This vertex is alread an element!\n", i);
        exit(1);
    }

    if (!this->isBitOn(this->vertex_in_shape, i)) {
        this->addVertexToShape(i);
    }

    if (this->num_nonelements >= this->size_nonelements) {
        this->size_nonelements = 2 * this->size_nonelements;
        this->nonelements = (int*)realloc(this->nonelements, this->size_nonelements * sizeof(int));
    }

    // Iterable
    this->nonelements[this->num_nonelements] = i;
    (this->num_nonelements)++;

    // Random-access
    this->turnBitOn(this->vertex_is_nonelement, i);
    this->snapshot_bitmask_nonelement_i.push(i);

    // Face Counts
    for (int j = 0; j < this->grid->getVertexDegree(i); j++) {
        int face = this->grid->neighborVF(i, j);

        this->addNonElementToFace(face); // even if not in shape!
    }
}

/********************************************************************
 *  ADDING FACES                                                                            *
 ********************************************************************/

void Configuration::addFaceToShape(int f)
{
    if (f < 0 || f >= this->faces_length) {
        return;
    }

    if (this->face_to_index[f] >= 0) {
        // already in the set!
        return;
    }

    this->face_index[this->faces_in_shape] = f; // iterable
    this->face_to_index[f] = this->faces_in_shape; //random-access

    (this->faces_in_shape)++;
}

/********************************************************************
 *  CENTER                                                                                       *
 ********************************************************************/

void Configuration::setCenter(int index, bool is_vertex)
{
    this->has_center = true;
    this->center_index = index;
    this->center_is_vertex = is_vertex;
}

bool Configuration::hasCenter() const
{
    return this->has_center;
}
bool Configuration::isCenterVertex() const
{
    return this->center_is_vertex;
}
int Configuration::getCenter() const
{
    return this->center_index;
}

/********************************************************************
 *  COLORS!                                                                                      *
 ********************************************************************/
/**
 * Once the canonical string is determined, we will know how the rotational order is fixed.
 * The orbits of vertices and faces are assigned colors in order of their lowest-indexed representatives,
 * with respect to the canonical order.
 */

int Configuration::getNumVertexColors()
{
    return this->num_vertex_colors;
}

int Configuration::getNumFaceColors()
{
    return this->num_face_colors;
}

int Configuration::getColorOfVertex(int i)
{
    if (this->num_vertex_colors <= 0 || i < 0 || i >= this->size_shape_vertices) {
        return -1;
    }

    for ( int j = 0; j < this->vertices_in_shape; j++ )
    {
    	if ( this->shape_vertices[j] == i )
    	{
    		return this->vertex_colors[j];
    	}
    }

    printf("Warning [getColorOfVertex]: The vertex %d does not have a color!\n", i);
    return -1;
}

int Configuration::getColorOfFace(int f)
{
    if (this->num_face_colors <= 0 || f < 0 ) {
        return -1;
    }

    for ( int j = 0; j < this->faces_in_shape; j++ )
    {
    	if ( this->face_index[j] == f )
    	{
    		return this->face_colors[j];
    	}
    }

    printf("Warning [getColorOfFace]: The face %d does not have a color!\n", f);
    this->print();
    return -1;
}

void Configuration::getVertexColorClass(int color, int*& class_list, int& class_size)
{
    class_size = 0;
    for (int i = 0; i < this->size_shape_vertices; i++) {
        if (this->vertex_colors[i] == color) {
            class_size++;
        }
    }

    class_list = (int*)realloc(class_list, class_size * sizeof(int));
    class_size = 0;
    for (int i = 0; i < this->size_shape_vertices; i++) {
        if (this->vertex_colors[i] == color) {
            class_list[class_size] = this->shape_vertices[i];
            class_size++;
        }
    }
}

void Configuration::getFaceColorClass(int color, int*& class_list, int& class_size)
{
    class_size = 0;
    for (int i = 0; i < this->faces_length; i++) {
        if (this->face_colors[i] == color) {
            class_size++;
        }
    }

    class_list = (int*)realloc(class_list, class_size * sizeof(int));
    class_size = 0;
    for (int i = 0; i < this->faces_length; i++) {
        if (this->face_colors[i] == color) {
            class_list[class_size] = this->face_index[i];
            class_size++;
        }
    }
}

int Configuration::getVertexColorClassSize(int color)
{
    int class_size = 0;
    for (int i = 0; i < this->vertices_in_shape; i++) {
        if (this->vertex_colors[i] == color) {
            class_size++;
        }
    }
    return class_size;
}

int Configuration::getFaceColorClassSize(int color)
{
    int class_size = 0;
    for (int i = 0; i < this->faces_in_shape; i++) {
        if (this->face_colors[i] == color) {
            class_size++;
        }
    }
    return class_size;
}

/********************************************************************
 *  DO CONFIGURATIONS MATCH?                                                    *
 ********************************************************************/

/**
 * Given a configuration with some common vertices, test
 * if their intersection has common agreements with
 *
 */
bool Configuration::matches(Configuration* conf)
{
    // Does CONF disagree with THIS?
    for (int i = 0; i < this->num_nonelements; i++) {
        if (conf->isBitOn(conf->vertex_is_element, this->nonelements[i])) {
            return false;
        }
    }

    for (int i = 0; i < this->num_elements; i++) {
        if (conf->isBitOn(conf->vertex_is_nonelement, this->elements[i])) {
            return false;
        }
    }

    // Does THIS disagree with CONF?
    for (int i = 0; i < conf->num_nonelements; i++) {
        if (this->isBitOn(this->vertex_is_element, conf->nonelements[i])) {
            return false;
        }
    }

    for (int i = 0; i < conf->num_elements; i++) {
        if (this->isBitOn(this->vertex_is_nonelement, conf->elements[i])) {
            return false;
        }
    }

    for ( int i = 0; i < conf->faces_in_shape; i++ )
    {
    	int f = conf->face_index[i];
    	int s = conf->face_sizes[i];

    	if ( s >= 0 && this->isFaceInShape(f)  )
    	{
    		if ( s < this->face_elts[f] || this->grid->getFaceDegree(f) - s < this->face_nonelts[f] )
    		{
    			// sizes do not match!
    			return false;
    		}
    	}
    }

    for ( int i = 0; i < this->faces_in_shape; i++ )
    {
    	int f = this->face_index[i];
    	int s = this->face_sizes[i];

    	if ( s >= 0 && conf->isFaceInShape(f)  )
    	{
    		if ( s < conf->face_elts[f] || this->grid->getFaceDegree(f) - s < conf->face_nonelts[f] )
    		{
    			// sizes do not match!
    			return false;
    		}
    	}
    }

    return true;
}

/**
 * equals: Are these configurations THE SAME?
 *
 * This is used for automorphism calculations!
 */
bool Configuration::equals(Configuration* conf, bool verbose)
{
    if (this->vertices_in_shape != conf->vertices_in_shape) {
        if (verbose) {
            printf("vertices_in_shape: %d != %d\n", this->vertices_in_shape, conf->vertices_in_shape);
        }
        return false;
    }
    if (this->num_elements != conf->num_elements) {
        if (verbose) {
            printf("num_elements: %d != %d\n", this->num_elements, conf->num_elements);
        }
        return false;
    }

    if (this->num_nonelements != conf->num_nonelements) {
        if (verbose) {
            printf("num_nonelements: %d != %d\n", this->num_nonelements, conf->num_nonelements);
        }
        return false;
    }

    if (this->faces_in_shape != conf->faces_in_shape) {
        if (verbose) {
            printf("faces_in_shape: %d != %d\n", this->faces_in_shape, conf->faces_in_shape);
        }
        return false;
    }

    if (this->has_center != conf->has_center) {
        if (verbose) {
            printf("has_center: %d != %d\n", this->has_center, conf->has_center);
        }
        return false;
    }

    if (this->has_center) {
        if (this->center_is_vertex != conf->center_is_vertex) {
            return false;
        }

        if (this->center_index != conf->center_index) {
            return false;
        }
    }

    for (int i = 0; i < this->vertices_in_shape; i++) {
        if (conf->isVertexInShape(this->shape_vertices[i]) == false) {
            if (verbose) {
                printf("vertex not in shape: %d not in conf\n", this->shape_vertices[i]);
            }
            return false;
        }
    }

    for (int i = 0; i < this->num_elements; i++) {
        if (conf->isElement(this->elements[i]) == false) {
            if (verbose) {
                printf("vertex not in elements: %d not in conf\n", this->elements[i]);
            }
            return false;
        }
    }

    for (int i = 0; i < this->num_nonelements; i++) {
        if (conf->isNonElement(this->nonelements[i]) == false) {
            if (verbose) {
                printf("vertex not nonelement: %d not in conf\n", this->nonelements[i]);
            }
            return false;
        }
    }

    for (int i = 0; i < this->faces_in_shape; i++) {
        if (conf->isFaceInShape(this->face_index[i]) == false) {
            if (verbose) {
                printf("face not in shape: %d not in conf\n", this->face_index[i]);
            }
            return false;
        }
    }

    for ( int i = 0; i < this->faces_in_shape; i++ )
    {
    	if ( conf->getFaceSize(this->face_index[i]) != this->face_sizes[i] )
    	{
    		if ( verbose) {
    			printf("face %d has size %d in this, but size %d in the configuration.\n", this->face_index[i], conf->getFaceSize(this->face_index[i]),  this->face_sizes[i]);
    		}
    		return false;
    	}
    }

    return true;
}

/********************************************************************
 *  DUPLICATION                                                                            *
 ********************************************************************/

/**
 * Create a new configuration using the same sub-class. Necessary for some features of
 * configurations. This mode will simply create a copy into a regular Configuration.
 * This may be enough for most applications.
 */
Configuration* Configuration::duplicate() const
{
    Configuration* conf = new Configuration(this->grid);

    for (int i = 0; i < this->vertices_in_shape; i++) {
        conf->addVertexToShape(this->shape_vertices[i]);
    }
    for (int i = 0; i < this->num_elements; i++) {
        conf->addElement(this->elements[i]);
    }
    for (int i = 0; i < this->num_nonelements; i++) {
        conf->addNonElement(this->nonelements[i]);
    }
    for (int i = 0; i < this->faces_in_shape; i++) {
        conf->addFaceToShape(this->face_index[i]);


        if ( this->face_sizes[i] >= 0 )
        {
            conf->setFaceSize(this->face_index[i], this->face_sizes[i]);
        }	
    }

    if (this->has_center) {
        conf->setCenter(this->center_index, this->center_is_vertex);
    }

    conf->snapshot();

    return conf;
}

/**
 * Create a new configuration using the same sub-class. Necessary for some features of
 * configurations. This mode will simply create a copy into a regular Configuration.
 * This may be enough for most applications.
 */
Configuration* Configuration::duplicate(int* fperm, int* vperm) const
{
    Configuration* conf = new Configuration(this->grid);

    for (int i = 0; i < this->vertices_in_shape; i++) {
        conf->addVertexToShape(vperm[this->shape_vertices[i]]);
    }
    for (int i = 0; i < this->num_elements; i++) {
        conf->addElement(vperm[this->elements[i]]);
    }
    for (int i = 0; i < this->num_nonelements; i++) {
        conf->addNonElement(vperm[this->nonelements[i]]);
    }
    for (int i = 0; i < this->faces_in_shape; i++) {

    	// printf("Sending face %d to face %d\n", this->face_index[i], fperm[this->face_index[i]]);

        conf->addFaceToShape(fperm[this->face_index[i]]);

        if ( this->face_sizes[i] >= 0 && conf->face_sizes[i] < 0 )
        {
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

/********************************************************************
 *  PROPAGATION                                                                             *
 ********************************************************************/

/**
 * Extensions will use the propagate method to expand the configuration to things that are "known"
 * based on the given information. Returns false if and only if something is wrong!
 *
 * Requires knowing the grid for neighborhoods and such.  (BUT WE KNOW IT ALREADY!)
 */
bool Configuration::propagate()
{
    return true;
}

/********************************************************************
 *  SYMMETRY                                                                            *
 ********************************************************************/

/**
 * Coloring things.
 *
 * If there are automorphisms (with flips, perhaps) then color each orbit of vertices
 * and faces. Necessary for defining symmetry-aware rules.
 */
void Configuration::computeSymmetry(bool allow_flip)
{
    // Compute all automorphisms (by trying all switches of things)
    int max_dihedral = 0;
    for (int i = 0; i < this->grid->getNumFacialOrbits(); i++) {
        int f = this->grid->getFacialOrbitRepresentative(i);

        int d = this->grid->getFaceDegree(f);

        if (d > max_dihedral) {
            max_dihedral = d;
        }
    }
    for (int i = 0; i < this->grid->getNumVertexOrbits(); i++) {
        int v = this->grid->getVertexOrbitRepresentative(i);

        int d = this->grid->getVertexDegree(v);

        if (d > max_dihedral) {
            max_dihedral = d;
        }
    }

    max_dihedral = 2 * max_dihedral;

    int** autom_v = (int**)malloc(max_dihedral * sizeof(int*));
    int** autom_f = (int**)malloc(max_dihedral * sizeof(int*));

    bzero(autom_v, max_dihedral * sizeof(int*));
    bzero(autom_f, max_dihedral * sizeof(int*));

    int num_automs = 0;

    // And then compute ORBITS!
    if (this->has_center) {
        if (this->center_is_vertex) {
            // for all dihedral actions at this vertex, are they automorphisms?
            int v1 = this->center_index;
            int v2 = this->grid->neighborVV(v1, 0);

            int u1 = v1;
            int u2 = v2; // for all neighbors...
            for (int i = 0; i < this->grid->getVertexDegree(u1); i++) {
                u2 = this->grid->neighborVV(u1, i);

                this->grid->transformVV(autom_f[num_automs], autom_v[num_automs], v1, v2, u1, u2, false);

                // is this an automorphism?
                Configuration* conf = this->duplicate(autom_f[num_automs], autom_v[num_automs]);

                if (this->equals(conf)) {
                    num_automs++;
                }
                delete conf;
            }

            if (allow_flip) {
                for (int i = 0; i < this->grid->getVertexDegree(u1); i++) {
                    u2 = this->grid->neighborVV(u1, i);

	                if ( v1 == u1 && v2 == u2 )
	                {
	                	continue;
	                }

                    this->grid->transformVV(autom_f[num_automs], autom_v[num_automs], v1, v2, u1, u2, true);

                    // is this an automorphism?
                    Configuration* conf = this->duplicate(autom_f[num_automs], autom_v[num_automs]);

                    if (this->equals(conf)) {
                        num_automs++;
                    }

                    delete conf;
                }
            }
        } else {
            // for all dihedral actions at this face, are they automorphisms?
            // for all dihedral actions at this vertex, are they automorphisms?
            int f1 = this->center_index;
            int v2 = this->grid->neighborFV(f1, 0);

            int g1 = f1;
            int u2 = v2; // for all neighbors...
            for (int i = 0; i < this->grid->getFaceDegree(g1); i++) {
                u2 = this->grid->neighborFV(g1, i);

                if ( f1 == g1 && v2 == u2 )
                {
                	continue;
                }

                this->grid->transformFV(autom_f[num_automs], autom_v[num_automs], f1, v2, g1, u2, false);

                // is this an automorphism?
                Configuration* conf = this->duplicate(autom_f[num_automs], autom_v[num_automs]);

                if (this->equals(conf)) {
                    num_automs++;
                }

                delete conf;
            }

            if (allow_flip) {
                for (int i = 0; i < this->grid->getFaceDegree(g1); i++) {
                    u2 = this->grid->neighborFV(g1, i);

                    this->grid->transformFV(autom_f[num_automs], autom_v[num_automs], f1, v2, g1, u2, true);

                    // is this an automorphism?
                    Configuration* conf = this->duplicate(autom_f[num_automs], autom_v[num_automs]);

                    if (this->equals(conf)) {
                        num_automs++;
                    }

                    delete conf;
                }
            }
        }
    } else {
        // ok, pick a vertex/face.
        // Now, find all ways to translate that vertex/face
        // and all dihedral operations thereof
        // List the automorphisms

        // for all dihedral actions at this vertex, are they automorphisms?
        int v1 = this->shape_vertices[0];
        int v2 = this->grid->neighborVV(v1, 0);

        // can fix the vertex.. that is ok.
        for (int j = 0; j < this->vertices_in_shape; j++) {
            int u1 = this->shape_vertices[j];
            int u2 = v2; // for all neighbors...

            // be sure to try ALL neighbors of u1.
            for (int i = 0; i < this->grid->getVertexDegree(u1); i++) {
                u2 = this->grid->neighborVV(u1, i);


                if ( v1 == u1 && v2 == u2 )
                {
                	continue;
                }

                this->grid->transformVV(autom_f[num_automs], autom_v[num_automs], v1, v2, u1, u2);

                // is this an automorphism?
                Configuration* conf = this->duplicate(autom_f[num_automs], autom_v[num_automs]);

                if (this->equals(conf)) {
                    num_automs++;
                }

                delete conf;
            }

            if (allow_flip) {
                for (int i = 0; i < this->grid->getVertexDegree(v1); i++) {
                    u2 = this->grid->neighborVV(v1, i);

                    this->grid->transformVV(autom_f[num_automs], autom_v[num_automs], v1, v2, u1, u2, true);

                    // is this an automorphism?
                    Configuration* conf = this->duplicate(autom_f[num_automs], autom_v[num_automs]);

                    if (this->equals(conf)) {
                        num_automs++;
                    }

                    delete conf;
                }
            }
        }
    }

    // We now select a canonical ordering!
    int best_center = this->center_index;
    int best_neighbor = -1;
    char* best_string = 0;
    int* vorder = (int*)malloc(this->vertices_in_shape * sizeof(int));
    int* forder = (int*)malloc(this->faces_in_shape * sizeof(int));

    if (this->has_center) {
        if (this->center_is_vertex) {
            int center = this->center_index;

            for (int j = 0; j < this->grid->getVertexDegree(center); j++) {
                int neighbor = this->grid->neighborVV(center, j);

                char* str = this->getRotationStringVV(center, neighbor, &vorder, &forder);

                if (best_string == 0 || strcmp(best_string, str) > 0) {
                    if (best_string != 0) {
                        free(best_string);
                    }

                    best_center = center;
                    best_neighbor = neighbor;
                    best_string = str;
                } else {
                    free(str);
                }
            }

            // Make sure vorder and forder are up to date!
            char* duplicate = this->getRotationStringVV(best_center, best_neighbor, &vorder, &forder);
            free(duplicate);
        } else {
            int center = this->center_index;

            for (int j = 0; j < this->grid->getFaceDegree(center); j++) {
                int neighbor = this->grid->neighborFV(center, j);

                char* str = this->getRotationStringFV(center, neighbor, &vorder, &forder);

                if (best_string == 0 || strcmp(best_string, str) > 0) {
                    if (best_string != 0) {
                        free(best_string);
                    }

                    best_center = center;
                    best_neighbor = neighbor;
                    best_string = str;
                } else {
                    free(str);
                }
            }

            // Make sure vorder and forder are up to date!
            char* duplicate = this->getRotationStringFV(best_center, best_neighbor, &vorder, &forder);
            free(duplicate);
        }
    } else {
        // for all possible centers, and all possible neighbors, then get a string
        for (int i = 0; i < this->vertices_in_shape; i++) {
            int center = this->shape_vertices[i];

            for (int j = 0; j < this->grid->getVertexDegree(center); j++) {
                int neighbor = this->grid->neighborVV(center, j);

                char* str = this->getRotationStringVV(center, neighbor, &vorder, &forder);

                if (best_string == 0 || strcmp(best_string, str) > 0) {
                    if (best_string != 0) {
                        free(best_string);
                    }

                    best_center = center;
                    best_neighbor = neighbor;
                    best_string = str;
                } else {
                    free(str);
                }
            }
        }

        char* duplicate = this->getRotationStringVV(best_center, best_neighbor, &vorder, &forder);
        free(duplicate);
    }

    this->canonical_string = best_string;

    int cur_num_colors = 0;

    for (int i = 0; i < this->vertices_in_shape; i++) {
        this->vertex_colors[i] = -1;
    }

    for (int i = 0; i < this->faces_in_shape; i++) {
        this->face_colors[i] = -1;
    }

    for (int i = 0; i < this->vertices_in_shape; i++) {
        // select the ith vertex in the shape, as determined by the canonical string.
        int v = vorder[i];
        int vindex = -1;
        for ( int j = 0; j < this->vertices_in_shape; j++ )
        {
        	if ( this->shape_vertices[j] == v )
        	{
        		vindex = j;
        		break;
        	}
        }

        if (this->vertex_colors[vindex] == -1) {
            // new color class... anything in orbit?
            this->vertex_colors[vindex] = cur_num_colors;

            for (int j = 0; j < num_automs; j++) {
            	int u = autom_v[j][v];

                if ( u != v && u >= 0 && this->isVertexInShape(u) ) {
                	int uindex = -1;

                	for ( int k = 0; k < this->vertices_in_shape; k++ )
                	{
                		if ( this->shape_vertices[k] == u )
                		{
                			uindex = k;
                			break;
                		}
                	}

                    this->vertex_colors[uindex] = cur_num_colors;
                }
            }

            cur_num_colors++;
        }
    }
    this->num_vertex_colors = cur_num_colors;

    cur_num_colors = 0;
    for (int i = 0; i < this->faces_in_shape; i++) {
        // Select the ith face in the shape, as determined by the canonical string.
        int f = forder[i];
        int findex = -1;
        for ( int j = 0; j < this->faces_in_shape; j++ )
        {
        	if ( this->face_index[j] == f )
        	{
        		findex = j;
        		break;
        	}
        }

        if (this->face_colors[findex] == -1) {
            this->face_colors[findex] = cur_num_colors;

            for (int j = 0; j < num_automs; j++) {
            	int g = autom_f[j][f];
                if ( g != f && g >= 0 && this->isFaceInShape(g)) {

                    int gindex = -1;
                    for ( int k = 0; k < this->faces_in_shape; k++) {
                        if (this->face_index[k] == g) {
                            gindex = k;
                            break;
                        }
                    }

                    this->face_colors[gindex] = cur_num_colors;
                }
            }

            cur_num_colors++;
        }
    }
    this->num_face_colors = cur_num_colors;
	    
    for (int i = 0; i <= num_automs; i++) {
        FREE_ARRAY(autom_f[i]);
        FREE_ARRAY(autom_v[i]);
    }
    FREE_ARRAY(autom_f);
    FREE_ARRAY(autom_v);
    FREE_ARRAY(vorder);
    FREE_ARRAY(forder);
}

/**
 * Canonical Strings:
 *
 * Given a vertex and one of its neighbors, we create a "rotation string" by using BFS to visit all vertex neighbors until every
 * vertex in the shape is visited, and then using BFS to visit all facial neighbors until every face in the shape is visited.
 * The BFS will respect the rotational order of the vertices. The root vertex starts its neighbor selection by the given neighbor.
 * The rest of the vertices and faces will use their BFS tree parent and follow the rotational order from that.
 *
 * If the configuration has a center, the string starts with the letter C.
 * The string starts with the orbit of the vertex or face to start.
 * Then we have the letter V if it starts at a vertex, or the letter F if it starts at a face.
 * Then we have the orbit of the vertex where the rotation starts, followed by the letter V.
 *
 * Now, a bar ('|') to signify a new order. We start visiting the vertices.
 *
 * For the vertices that we visit, we use the following split:
 * 3 : Not in the shape.
 * 2 : In the shape, but undetermined.
 * 1 : An element.
 * 0 : A nonelement.
 *
 * Now, a bar ('|') to signify a new order. We start visiting the faces.
 *
 * For the faces that we visit, we use the following split:
 * 1 : In the shape.
 * 0 : Not in the shape.
 *
 * The canonical string is the lex-minimum string of this type, respecting the center, if necessary.
 *
 * Thus, the configuration can be reconstructed from the canonical string using the Grid, if necessary.
 *
 * If vorder&forder are given as nonzero, then they point to integer pointers that can be realloced to the correct length.
 * They will then have the order of the vertices and faces as visited, which can be used to assign colors.
 */
char* Configuration::getRotationStringVV(int start_v, int first_neighbor_v, int** vorder, int** forder)
{
    int strlen = 2 * this->vertices_in_shape + this->faces_in_shape + 25;
    char* str = (char*)malloc(strlen);
    int cur_pos = 0;

    if (this->has_center) {
        if (this->center_is_vertex == false || this->center_index != start_v) {
            printf("Trying to compute symmetry about a non-center vertex! (%d instead of %d)\n", start_v, this->center_index);
            free(str);
            return 0;
        }

        str[cur_pos++] = 'C';
    }

    str[cur_pos] = 0;
    cur_pos += sprintf(str + cur_pos, "%d", this->grid->getOrbitOfVertex(start_v));

    str[cur_pos++] = 'V';

    int dv = this->grid->getVertexDegree(start_v);
    int start_neighbor_index = -1;
    for (int i = 0; i < dv; i++) {
        if (this->grid->neighborVV(start_v, i) == first_neighbor_v) {
            start_neighbor_index = i;
            break;
        }
    }

    if (start_neighbor_index < 0) {
        printf("Trying to compute a rotation string, but vertices %d and %d are not adjacent!\n", start_v, first_neighbor_v);
        free(str);
        return 0;
    }

    for (int i = 0; i < dv; i++) {
        while (cur_pos + 5 > strlen) {
            strlen += 100;
            str = (char*)realloc(str, strlen);
        }

        // Note: neighborVV uses cyclic order, and can take things out of range.
        cur_pos += sprintf(str + cur_pos, "%d", this->grid->getOrbitOfVertex(this->grid->neighborVV(start_v, i + start_neighbor_index)));
        str[cur_pos++] = 'V';
    }

    // Ok, all vertices are accounted for. We are oriented within the grid!
    bool has_v_order = (vorder != 0);
    bool has_f_order = (forder != 0);

    if (!has_v_order) {
        vorder = (int**)malloc(sizeof(int*));
    }
    if (!has_f_order) {
        forder = (int**)malloc(sizeof(int*));
    }

    *vorder = (int*)realloc(*vorder, this->vertices_in_shape * sizeof(int));
    *forder = (int*)realloc(*forder, this->faces_in_shape * sizeof(int));

    for (int i = 0; i < this->vertices_in_shape; i++) {
        (*vorder)[i] = -1;
    }
    for (int i = 0; i < this->faces_in_shape; i++) {
        (*forder)[i] = -1;
    }

    // Let's load up the BFS calls
    int* vparent = (int*)malloc(this->grid->getMaxVertexIndex() * sizeof(int));
    int* fparent = (int*)malloc(this->grid->getMaxFacialIndex() * sizeof(int));

    for (int i = 0; i < this->grid->getMaxVertexIndex(); i++) {
        vparent[i] = -1;
    }
    for (int i = 0; i < this->grid->getMaxFacialIndex(); i++) {
        fparent[i] = -1;
    }

    int num_printed = 0;
    int cur_v_order = 0;
    int cur_f_order = 0;
    std::queue<int> q;

    // First: the vertex BFS order.
    q.push(start_v);
    vparent[start_v] = first_neighbor_v;

    str[cur_pos++] = '|';
    while (q.size() > 0) {
        int v = q.front();
        q.pop();

        (*vorder)[cur_v_order++] = v;

        // print
        while (cur_pos + 5 > strlen) {
            strlen += 100;
            str = (char*)realloc(str, strlen);
        }

        char to_put = 0;
        if (this->isVertexInShape(v) == false) {
            to_put = '3';
            cur_v_order--; // remove this from the order... not important
        } else if (this->isElement(v)) {
            to_put = '1';
        } else if (this->isNonElement(v)) {
            to_put = '0';
        } else {
            to_put = '2';
        }
        str[cur_pos++] = to_put;

        num_printed++;
        if (cur_v_order >= this->vertices_in_shape) {
            break;
        }

        // find the parent.
        int pindex = -1;

        for (int i = 0; i < this->grid->getVertexDegree(v); i++) {
            if (this->grid->neighborVV(v, i) == vparent[v]) {
                pindex = i;
                break;
            }
        }

        if (pindex < 0) {
            printf("Somehow the vertex %d is not adjacent to its parent, %d!\n", v, vparent[v]);
            free(str);
            free(vparent);
            free(fparent);
            return 0;
        }

        // loop
        for (int i = 0; i < this->grid->getVertexDegree(v); i++) {
            int u = this->grid->neighborVV(v, i + pindex);

            if (vparent[u] < 0) {
                vparent[u] = v;
                q.push(u);
            }
        }
    }

    while (q.size() > 0) {
        q.pop();
    }

    str[cur_pos++] = '|';
    num_printed = 0;
    int start_index = -1;

    for (int i = 0; i < this->grid->getVertexDegree(start_v); i++) {
        if (this->grid->neighborVV(start_v, i) == first_neighbor_v) {
            start_index = i;
            break;
        }
    }

    for (int i = 0; i < this->grid->getVertexDegree(start_v); i++) {
        // for all incident faces, push to the queue to start the order. Also set the parents so they are visited.
        int f = this->grid->neighborVF(start_v, i + start_index);

        // printf("adding f = %d to list... start_v = %d, i = %d, start_index = %d..\n", f, start_v, i, start_index);
        q.push(f);
        fparent[f] = this->grid->getMaxFacialIndex() + start_v;
    }

    while (q.size() > 0) {
        int f = q.front();
        // printf("f=%d, str=%s\n", f, str);
        q.pop();

        // print
        while (cur_pos + 5 > strlen) {
            strlen += 100;
            str = (char*)realloc(str, strlen);
        }

        if (this->isFaceInShape(f)) {
            (*forder)[cur_f_order++] = f;
        }

        char to_put = this->isFaceInShape(f) ? '1' : '0';
        str[cur_pos++] = to_put;

        num_printed++;

        if (cur_f_order >= this->faces_in_shape) {
            break;
        }

        // find the parent.
        int pindex = -1;
        if (fparent[f] < this->grid->getMaxFacialIndex()) {
            for (int i = 0; i < this->grid->getFaceDegree(f); i++) {
                if (this->grid->neighborFF(f, i) == fparent[f]) {
                    pindex = i;
                    break;
                }
            }
        } else {
            for (int i = 0; i < this->grid->getFaceDegree(f); i++) {
                if (this->grid->neighborFV(f, i) == fparent[f] - this->grid->getMaxFacialIndex()) {
                    pindex = i;
                    break;
                }
            }
        }

        if (pindex < 0) {
            printf("Somehow the face %d is not adjacent to its parent, %d!\n", f, fparent[f]);
            free(str);
            return 0;
        }

        // loop
        for (int i = 0; i < this->grid->getFaceDegree(f); i++) {
            int u = this->grid->neighborFF(f, i + pindex);

            if ( fparent[u] < 0 ) {
                fparent[u] = f;
                q.push(u);
            }
        }
    }

    // Now, repeat the face order and report the face sizes, when appropriate.
    // Format: |Color:Size
    for ( int i = 0; i < cur_f_order; i++ )
    {
        while (cur_pos + 10 > strlen) {
            strlen += 100;
            str = (char*)realloc(str, strlen);
        }
    	int f = (*forder)[i];
    	int s = this->getFaceSize(f);
    	if ( s >= 0 )
    	{       // print
    		cur_pos += sprintf(str + cur_pos, "|%d", s);
    	}
    	else
    	{
    		// Blank for no size!
    		cur_pos += sprintf(str + cur_pos, "|_");
    	}
    }

    if (!has_v_order) {
        free(*vorder);
        free(vorder);
        vorder = 0;
    }
    if (!has_f_order) {
        free(*forder);
        free(forder);
        forder = 0;
    }

    free(vparent);
    free(fparent);

    str[cur_pos] = 0;

    return str;
}

char* Configuration::getRotationStringFV(int start_f, int first_neighbor_v, int** vorder, int** forder)
{

    int strlen = 2 * this->vertices_in_shape + this->faces_in_shape + 25;
    char* str = (char*)malloc(strlen);
    int cur_pos = 0;

    if (this->has_center) {
        if (this->center_is_vertex == true || this->center_index != start_f) {
            printf("Trying to compute symmetry about a non-center face! (%d instead of %d)\n", start_f, this->center_index);
            free(str);
            return 0;
        }

        str[cur_pos++] = 'C';
    }

    str[cur_pos] = 0;
    cur_pos += sprintf(str + cur_pos, "%d", this->grid->getOrbitOfFace(start_f));

    str[cur_pos++] = 'F';

    int dv = this->grid->getFaceDegree(start_f);
    int start_neighbor_index = -1;
    for (int i = 0; i < dv; i++) {
        if (this->grid->neighborFV(start_f, i) == first_neighbor_v) {
            start_neighbor_index = i;
            break;
        }
    }

    if (start_neighbor_index < 0) {
        printf("Trying to compute a rotation string, but face %d and vertex %d are not adjacent!\n", start_f, first_neighbor_v);
        free(str);
        return 0;
    }

    for (int i = 0; i < dv; i++) {
        while (cur_pos + 5 > strlen) {
            strlen += 100;
            str = (char*)realloc(str, strlen);
        }

        // Note: neighborVV uses cyclic order, and can take things out of range.
        cur_pos += sprintf(str + cur_pos, "%d", this->grid->getOrbitOfVertex(this->grid->neighborFV(start_f, i + start_neighbor_index)));
        str[cur_pos++] = 'V';
    }

    // Ok, all vertices are accounted for. We are oriented within the grid!
    bool has_v_order = (vorder != 0);
    bool has_f_order = (forder != 0);

    if (!has_v_order) {
        vorder = (int**)malloc(sizeof(int*));
    }
    if (!has_f_order) {
        forder = (int**)malloc(sizeof(int*));
    }

    *vorder = (int*)realloc(*vorder, (this->faces_in_shape * this->grid->getFaceDegree(0) + this->vertices_in_shape) * sizeof(int));
    *forder = (int*)realloc(*forder, this->faces_in_shape * sizeof(int));

    for (int i = 0; i < this->vertices_in_shape; i++) {
        (*vorder)[i] = -1;
    }
    for (int i = 0; i < this->faces_in_shape; i++) {
        (*forder)[i] = -1;
    }

    // Let's load up the BFS calls
    int* vparent = (int*)malloc(this->grid->getMaxVertexIndex() * sizeof(int));
    int* fparent = (int*)malloc(this->grid->getMaxFacialIndex() * sizeof(int));

    for (int i = 0; i < this->grid->getMaxVertexIndex(); i++) {
        vparent[i] = -1;
    }

    for (int i = 0; i < this->grid->getMaxFacialIndex(); i++) {
        fparent[i] = -1;
    }

    int num_printed = 0;
    int cur_v_order = 0;
    int cur_f_order = 0;
    std::queue<int> q;

    // First: the vertex BFS order.
    int start_index = -1;

    for (int i = 0; i < this->grid->getFaceDegree(start_f); i++) {
        if (this->grid->neighborFV(start_f, i) == first_neighbor_v) {
            start_index = i;
            break;
        }
    }

    for (int i = 0; i < this->grid->getFaceDegree(start_f); i++) {
        // for all incident faces, push to the queue to start the order. Also set the parents so they are visited.
        int v = this->grid->neighborFV(start_f, i + start_index);
        q.push(v);
        vparent[v] = this->grid->getMaxVertexIndex() + start_f;
    }

    str[cur_pos++] = '|';
    while (q.size() > 0) {
        int v = q.front();
        q.pop();

        (*vorder)[cur_v_order++] = v;

        // print
        while (cur_pos + 5 > strlen) {
            strlen += 100;
            str = (char*)realloc(str, strlen);
        }

        char to_put = 0;
        if (this->isVertexInShape(v) == false) {
            to_put = '3';
            cur_v_order--;
        } else if (this->isElement(v)) {
            to_put = '1';
        } else if (this->isNonElement(v)) {
            to_put = '0';
        } else {
            to_put = '2';
        }
        str[cur_pos++] = to_put;

        num_printed++;

        if (cur_v_order >= this->vertices_in_shape) {
            break;
        }

        // find the parent.
        int pindex = -1;

        if (vparent[v] >= this->grid->getMaxVertexIndex()) {
            for (int i = 0; i < this->grid->getVertexDegree(v); i++) {
                if (this->grid->neighborVF(v, i) == vparent[v] - this->grid->getMaxVertexIndex()) {
                    pindex = i;
                    break;
                }
            }
        } else {
            for (int i = 0; i < this->grid->getVertexDegree(v); i++) {
                if (this->grid->neighborVV(v, i) == vparent[v]) {
                    pindex = i;
                    break;
                }
            }
        }

        if (pindex < 0) {
            printf("Somehow the vertex %d is not adjacent to its parent, %d!\n", v, vparent[v]);
            free(str);
            return 0;
        }

        // loop
        for (int i = 0; i < this->grid->getVertexDegree(v); i++) {
            int u = this->grid->neighborVV(v, i + pindex);

            if (vparent[u] < 0) {
                vparent[u] = v;
                q.push(u);
            }
        }
    }

    while (q.size() > 0) {
        q.pop();
    }

    str[cur_pos++] = '|';
    num_printed = 0;

    q.push(start_f);
    fparent[start_f] = first_neighbor_v + this->grid->getMaxFacialIndex();

    while (q.size() > 0) {
        int f = q.front();
        q.pop();

        if (this->isFaceInShape(f)) {
            (*forder)[cur_f_order++] = f;
        }

        // print
        while (cur_pos + 5 > strlen) {
            strlen += 100;
            str = (char*)realloc(str, strlen);
        }

        char to_put = this->isFaceInShape(f) ? '1' : '0';
        str[cur_pos++] = to_put;

        num_printed++;

        if (cur_f_order >= this->faces_in_shape) {
            break;
        }

        // find the parent.
        int pindex = -1;
        if (fparent[f] < this->grid->getMaxFacialIndex()) {
            for (int i = 0; i < this->grid->getFaceDegree(f); i++) {
                if (this->grid->neighborFF(f, i) == fparent[f]) {
                    pindex = i;
                    break;
                }
            }
        } else {
            for (int i = 0; i < this->grid->getFaceDegree(f); i++) {
                if (this->grid->neighborFV(f, i) == fparent[f] - this->grid->getMaxFacialIndex()) {
                    pindex = i;
                    break;
                }
            }
        }

        if (pindex < 0) {
            printf("Somehow the face %d is not adjacent to its parent, %d!\n", f, fparent[f]);
            free(str);
            return 0;
        }

        // loop
        for (int i = 0; i < this->grid->getFaceDegree(f); i++) {
            int u = this->grid->neighborFF(f, i + pindex);

            if (fparent[u] < 0) {
                fparent[u] = f;
                q.push(u);
            }
        }
    }

    while (q.size() > 0) {
        q.pop();
    }

    // Now, repeat the face order and report the face sizes, when appropriate.
    // Format: |Color:Size
    for ( int i = 0; i < cur_f_order; i++ )
    {
        while (cur_pos + 10 > strlen) {
            strlen += 100;
            str = (char*)realloc(str, strlen);
        }
    	int f = (*forder)[i];
    	int s = this->getFaceSize(f);
    	if ( s >= 0 )
    	{       // print
    		cur_pos += sprintf(str + cur_pos, "|%d", s);
    	}
    	else
    	{
    		// Blank for no size!
    		cur_pos += sprintf(str + cur_pos, "|_");
    	}
    }

    if (!has_v_order) {
        free(*vorder);
        *vorder = 0;
        free(vorder);
        vorder = 0;
    }
    if (!has_f_order) {
        free(*forder);
        *forder = 0;
        free(forder);
        forder = 0;
    }

    FREE_ARRAY(vparent);
    FREE_ARRAY(fparent);

    str[cur_pos] = 0;

    return str;
}

char* Configuration::getCanonicalString()
{
    if ( this->canonical_string == 0 )
    {
        this->computeSymmetry();
    }
    
    return this->canonical_string;
}

/********************************************************************
 *  PRINTING                                                                                    *
 ********************************************************************/
void Configuration::print() const
{
    this->write(stdout);
}

void Configuration::write(FILE* out) const
{
    fprintf(out, "#begin Configuration\n");

    fprintf(out, "shape = [ ");
    for (int i = 0; i < this->vertices_in_shape; i++) {
        fprintf(out, "%d", this->shape_vertices[i]);

        if (i < this->vertices_in_shape - 1) {
            fprintf(out, ", ");
        }
    }
    fprintf(out, " ];\n");

    fprintf(out, "faces = [ ");
    for (int i = 0; i < this->faces_in_shape; i++) {
        fprintf(out, "%d", this->face_index[i]);

        if (i < this->faces_in_shape - 1) {
            fprintf(out, ", ");
        }
    }
    fprintf(out, " ];\n");

    fprintf(out, "elements = [ ");
    for (int i = 0; i < this->num_elements; i++) {
        fprintf(out, "%d", this->elements[i]);

        if (i < this->num_elements - 1) {
            fprintf(out, ", ");
        }
    }
    fprintf(out, " ];\n");

    fprintf(out, "nonelements = [ ");
    for (int i = 0; i < this->num_nonelements; i++) {
        fprintf(out, "%d", this->nonelements[i]);

        if (i < this->num_nonelements - 1) {
            fprintf(out, ", ");
        }
    }
    fprintf(out, " ];\n");

    fprintf(out, "face_sizes = [ ");
    for (int i = 0; i < this->faces_in_shape; i++) {
        fprintf(out, "%d", this->face_sizes[i]);

        if (i < this->faces_in_shape - 1) {
            fprintf(out, ", ");
        }
    }
    fprintf(out, " ];\n");

    if (this->has_center) {
        if (this->center_is_vertex) {
            fprintf(out, "center_vertex = %d;\n", this->center_index);
        } else {
            fprintf(out, "center_face = %d;\n", this->center_index);
        }
    }

    if (this->num_vertex_colors > 0) {
        fprintf(out, "vertex_colors = { ");
        for (int i = 0; i < this->vertices_in_shape; i++) {
            fprintf(out, "%d : %d", this->shape_vertices[i], this->vertex_colors[i]);

            if (i < this->vertices_in_shape - 1) {
                fprintf(out, ", ");
            }
        }
        fprintf(out, " };\n");
        fprintf(out, "face_colors = { ");
        for (int i = 0; i < this->faces_in_shape; i++) {
            fprintf(out, "%d : %d", this->face_index[i], this->face_colors[i]);

            if (i < this->faces_in_shape - 1) {
                fprintf(out, ", ");
            }
        }
        fprintf(out, " };\n");
    }

    if (this->canonical_string != 0) {
        fprintf(out, "canonical_string = %s\n", this->canonical_string);
    }

    fprintf(out, "#end Configuration\n");
}

Configuration* Configuration::read(Grid* g, FILE* in)
{
    size_t line_len = 1000;
    char* line = (char*)malloc(line_len);

    Configuration* conf = new Configuration(g);

    while (getline(&line, &line_len, in) > 0) {
        if (strcmp(line, "#begin Configuration\n") == 0) {
            continue;
        }

        if (strcmp(line, "#end Configuration\n") == 0) {
            break;
        }

        if (strstr(line, "shape = [") == line ) {
            char* next = strtok(line + strlen("shape = ["), " ,");

            while (next != 0) {
                if (next[0] == ']') {
                    break;
                }

                int val = atoi(next);
                conf->addVertexToShape(val);
                next = strtok(0, " ,");
            }
        }
        else if (strstr(line, "nonelements = [") == line) {

            char* next = strtok(line + strlen("nonelements = ["), " ,");

            while (next != 0) {
                if (next[0] == ']') {
                    break;
                }

                int val = atoi(next);
                conf->addNonElement(val);
                next = strtok(0, " ,");
            }
        } else if (strstr(line, "elements = [") == line) {
            char* next = strtok(line + strlen("elements = ["), " ,");

            while (next != 0) {
                if (next[0] == ']') {
                    break;
                }

                int val = atoi(next);
                conf->addElement(val);
                next = strtok(0, " ,");
            }
        }
        else if (strstr(line, "faces = [") == line ) {
            char* next = strtok(line + strlen("faces = ["), " ,");

            while (next != 0) {
                if (next[0] == ']') {
                    break;
                }

                int val = atoi(next);
                conf->addFaceToShape(val);
                next = strtok(0, " ,");
            }
        }
		else if (strstr(line, "face_sizes = [") == line ) {
            char* next = strtok(line + strlen("face_sizes = ["), " ,");

            int i = 0;
            while (next != 0) {
                if (next[0] == ']') {
                    break;
                }

                int val = atoi(next);
                conf->setFaceSize(conf->getFaceFromShape(i), val);

                next = strtok(0, " ,");
                i++;
            }
        }
        else if (strstr(line, "center_vertex = ") != 0) {
            int vertex = atoi(line + strlen("center_vertex = "));
            conf->setCenter(vertex, true);
        }
        else if (strstr(line, "center_face = ") != 0) {
            int face = atoi(line + strlen("center_face = "));
            conf->setCenter(face, false);
        }
    }

    free(line);

    conf->computeSymmetry();

    return conf;
}
