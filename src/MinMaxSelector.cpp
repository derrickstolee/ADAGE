/*
 * MinMaxSelector.cpp
 *
 *  Created on: Jan 15, 2015
 *      Author: stolee
 */

#include "fraction.hpp"
#include "MinMaxSelector.hpp"
#include "macros.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

using namespace adage::grids;

void MinMaxSelector::snapshotParams()
{
	this->snapshot_params_size.push(this->snapshot_params_i.size());
}
void MinMaxSelector::rollbackParams()
{
	ARRAY_BLANK_ROLLBACK(this->snapshot_params_size, this->snapshot_params_i, this->params, -1);
}

void MinMaxSelector::snapshotKeyIndices()
{
	this->min_key_stack.push(this->min_key_index);
	this->max_key_stack.push(this->max_key_index);
}
void MinMaxSelector::rollbackKeyIndices()
{
	if ( this->min_key_stack.size() > 0 )
	{
		this->min_key_index = this->min_key_stack.top();
		this->min_key_stack.pop();
	}
	else
	{
		this->min_key_index = 0;
	}

	if ( this->max_key_stack.size() > 0 )
	{
		this->max_key_index = this->max_key_stack.top();
		this->max_key_stack.pop();
	}
	else
	{
		this->max_key_index = this->num_keys - 1;
	}
}

void MinMaxSelector::clearKeyIndices()
{
	CLEAR_STACK(this->min_key_stack);
	CLEAR_STACK(this->max_key_stack);

	this->min_key_index = 0;
	this->max_key_index = this->num_keys - 1;
}

void MinMaxSelector::quicksort(int start, int end)
{
	int pivot = (rand() % (end - start)) + start;

	fraction pvalue = this->values[pivot];

	int rstart = start;
	int lend = end;

	while ( rstart <= lend )
	{
		while ( rstart <= lend && this->values[rstart] < pvalue )
		{
			rstart++;
		}

		while ( rstart <= lend && this->values[lend] >= pvalue )
		{
			lend--;
		}

		if (  rstart < lend )
		{
			// swap!
			for ( int i = 0; i < this->num_params; i++ )
			{
				int t =this->keys[this->num_params*rstart+i];
				this->keys[this->num_params*rstart+i] = this->keys[this->num_params*lend+i];
				this->keys[this->num_params*lend+i] = t;
			}

			fraction f = this->values[rstart];
			this->values[rstart] = this->values[lend];
			this->values[lend] = f;

			rstart++;
			lend--;
		}
	}

	if ( rstart == start )
	{
		lend = end;
		// ALL are >= pvalue!
		while ( start <= lend )
		{
			while ( start <= lend && this->values[start] <= pvalue )
			{
				start++;
			}

			while ( start <= lend && this->values[lend] > pvalue )
			{
				lend--;
			}
			
			if ( start < lend )
			{
				// swap!
				for ( int i = 0; i < this->num_params; i++ )
				{
					int t =this->keys[this->num_params*start+i];
					this->keys[this->num_params*start+i] = this->keys[this->num_params*lend+i];
					this->keys[this->num_params*lend+i] = t;
				}

				fraction f = this->values[start];
				this->values[start] = this->values[lend];
				this->values[lend] = f;

				start++;
				lend--;
			}
		}

		if ( lend != end )
		{
			// only sort the ONE side!
			this->quicksort(start,end);	
		}
	}
	else
	{
		this->quicksort(start, lend);
		this->quicksort(rstart, end);
	}
}

void MinMaxSelector::markAsInvalid(int index)
{
	if ( this->keys_still_valid[index] <= 0 )
	{
		return;
	}

	this->keys_still_valid[index] = 0;
	this->snapshot_keys_valid_i.push(index);
}

void MinMaxSelector::snapshotKeysValid()
{
	this->snapshot_keys_valid_size.push(this->snapshot_keys_valid_i.size());
}

void MinMaxSelector::rollbackKeysValid()
{
	ARRAY_BLANK_ROLLBACK(this->snapshot_keys_valid_size,this->snapshot_keys_valid_i,this->keys_still_valid,1);
}




/**
 * protected constructor for making a shallow copy of the keys and values.
 */
MinMaxSelector::MinMaxSelector(MinMaxSelector* parent)
{
	this->num_params = parent->num_params;
	this->params = (char*)malloc(parent->num_params);
	for ( int i = 0; i < parent->num_params; i++ )
	{
		this->params[i] = parent->params[i];
	}

	this->min_key_index = parent->min_key_index;
	this->max_key_index = parent->max_key_index;

	this->is_sorted = parent->is_sorted;

	this->owns_keys_and_values = false;
	this->num_keys = parent->num_keys;
	this->size_keys = parent->size_keys;
	this->keys = parent->keys;
	this->values = parent->values;

	this->keys_still_valid = (char*)malloc(parent->size_keys);
	for ( int i = 0; i < parent->num_keys; i++ )
	{
		this->keys_still_valid[i] = parent->keys_still_valid[i];
	}
}

/**
 * Build a MinMaxSelector with the given number of parameters.
 */
MinMaxSelector::MinMaxSelector(int num_params)
{
	srand(time(NULL));

	this->num_params = num_params;
	this->params = (char*)malloc(num_params);
	for ( int i = 0; i < num_params; i++ )
	{
		this->params[i] = -1;
	}

	this->min_key_index = 0;
	this->max_key_index = -1;

	this->is_sorted = false;

	this->owns_keys_and_values = true;
	this->num_keys = 0;
	this->size_keys = 1000;
	this->keys = (char*)malloc(this->size_keys*num_params);
	this->values = (fraction*)malloc(this->size_keys*sizeof(fraction));

	this->keys_still_valid = (char*)malloc(this->size_keys);
	for ( int i = 0; i < this->size_keys; i++ )
	{
		this->keys_still_valid[i] = 1;
	}
}


/**
 * Destructor
 */
MinMaxSelector::~MinMaxSelector()
{
	this->clearKeyIndices();

	FREE_ARRAY(this->params);
	if ( this->owns_keys_and_values )
	{
		FREE_ARRAY(this->keys);
		FREE_ARRAY(this->values);
	}

	FREE_ARRAY(this->keys_still_valid);
}


void MinMaxSelector::snapshot()
{
	this->snapshotParams();
	this->snapshotKeysValid();
	this->snapshotKeyIndices();
}

void MinMaxSelector::rollback()
{
	this->rollbackParams();
	this->rollbackKeysValid();
	this->rollbackKeyIndices();
}

bool MinMaxSelector::minEqualsMax()
{
	return (this->min_key_index >= this->max_key_index) || (this->values[this->max_key_index] == this->values[this->min_key_index]);
}

fraction MinMaxSelector::getMax()
{
	return this->values[this->max_key_index];
}

fraction MinMaxSelector::getMin()
{
	return this->values[this->min_key_index];
}

void MinMaxSelector::addKeyValuePair(char* params, fraction value)
{
	if ( this->num_keys >= this->size_keys )
	{
		if ( this->owns_keys_and_values == false )
		{
			printf("ERROR: Trying to extend keys that are not owned!\n");
			exit(1);
		}

		this->size_keys = this->size_keys + 5000;
		this->keys = (char*)realloc(this->keys, this->size_keys * this->num_params);
		this->values = (fraction*)realloc(this->values, this->size_keys * sizeof(fraction));
		this->keys_still_valid = (char*)realloc(this->keys_still_valid, this->size_keys);
	}

	for ( int i = 0; i < this->num_params; i++ )
	{
		this->keys[this->num_keys*num_params+i] = params[i];
	}

	this->values[this->num_keys] = value;
	this->keys_still_valid[this->num_keys] = 1;

	this->max_key_index = this->num_keys;
	this->is_sorted = false;
	(this->num_keys)++;
}

/**
 * specifyParameter restricts the possible keys to be those
 * where the given parameter has the given value.
 * This will adjust the max and min values accordingly.
 */
void MinMaxSelector::specifyParameter(int param, char value)
{
	this->params[param] = value;	
	this->snapshot_params_i.push(param);

	bool still_invalid = true;
	int last_valid = this->min_key_index;
	for ( int i = this->min_key_index; i <= this->max_key_index; i++ )
	{
		if ( this->keys_still_valid[i] == true )
		{
			if ( this->keys[this->num_params * i + param] == value )
			{
				if ( still_invalid )
				{
					// this is the new minimum!
					this->min_key_index = i;
				}

				still_invalid = false;
				last_valid = i;
			}
			else
			{
				this->markAsInvalid(i);
			}
		}
	}

	this->max_key_index = last_valid;
}

/**
 * sortValues is a way to 
 */
void MinMaxSelector::sortValues()
{
	this->quicksort(0, this->num_keys - 1);
	this->min_key_index = 0;
	this->max_key_index = this->num_keys - 1;
}

void MinMaxSelector::reset()
{
	this->clearKeyIndices();
}