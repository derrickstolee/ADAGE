/*
 * MinMaxSelector.hpp
 *
 *  Created on: Jan 15, 2015
 *      Author: stolee
 */

#ifndef MinMaxSelector_HPP_
#define MinMaxSelector_HPP_

#include "fraction.hpp"
#include <stack>

namespace adage
{
namespace grids
{

/**
 * The MinMaxSelector class is meant to store the values of discharging rules for a single shape.
 *
 * The rules give different values according to some "kernelization" which takes a tuple of integer parameters.
 *
 * As we build a "sharp" discharging example, we will be able to "lock down" the values of these parameters, one at a time.
 *
 * This will adjust the possible charges across this rule.
 *
 * We can also alert the user that the min and max are equal, and hence we no longer need to specify any more. This can be used to 
 * reduce how many parameters we specify!
 */
class MinMaxSelector
{
protected:
	int num_params;
	char* params;
	void snapshotParams();
	void rollbackParams();
	std::stack<int> snapshot_params_i;
	std::stack<int> snapshot_params_size;

	int min_key_index;
	int max_key_index;
	void snapshotKeyIndices();
	void rollbackKeyIndices();
	std::stack<int> min_key_stack;
	std::stack<int> max_key_stack;
	void clearKeyIndices();

	bool is_sorted;

	// an integer array of keys. (Parameters should not be more than 127 in value, or we have strange parameters.)
	// instead of a double-array, we break them into chunks of size "num_params"
	bool owns_keys_and_values;
	char* keys;
	fraction* values;
	char* keys_still_valid;
	int num_keys;
	int size_keys;
	void quicksort(int start, int end);
	void markAsInvalid(int index);
	void snapshotKeysValid();
	void rollbackKeysValid();
	std::stack<int> snapshot_keys_valid_i;
	std::stack<int> snapshot_keys_valid_size;


	/**
	 * protected constructor for making a shallow copy of the keys and values.
	 */
	MinMaxSelector(MinMaxSelector* parent);

public:
	/**
	 * Build a MinMaxSelector with the given number of parameters.
	 */
	MinMaxSelector(int num_params);

	/**
	 * Destructor
	 */
	virtual ~MinMaxSelector();

	void snapshot();
	void rollback();
	bool minEqualsMax();

	fraction getMax();
	fraction getMin();

	void addKeyValuePair(char* params, fraction value);

	/**
	 * specifyParameter restricts the possible keys to be those
	 * where the given parameter has the given value.
	 * This will adjust the max and min values accordingly.
	 */
	void specifyParameter(int param, char value);

	/**
	 * sortValues is a way to 
	 */
	void sortValues();

	void reset();

	/**
	 * The same rule will be used for multiple scenarios.
	 *
	 * So we would rather copy the encoded MinMaxSelector after the sort instead of
	 * duplicating that effort several times!
	 *
	 * CHEAT: We can use the same arrays for keys and values, as long as we are careful
	 * about who deletes them! We need new arrays for "keys_still_valid".
	 */
	MinMaxSelector* duplicate();
};


}
}

#endif

