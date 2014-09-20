/*
 * macros.hpp
 *
 *  Created on: Apr 1, 2014
 *      Author: dstolee
 */

#ifndef MACROS_HPP_
#define MACROS_HPP_


#define CLEAR_STACK(S) \
	while ( S.size() > 0 ) \
	{ \
		S.pop();\
	}


#define FREE_ARRAY(A) \
		if ( A != 0 )\
		{\
			free(A);\
			A = 0;\
		}\


#define FREE_ARRAY2(A, size) \
	if ( A != 0 )\
	{\
		for ( int i = 0; i < size; i++ )\
		{\
			if ( A[i] != 0 ) \
			{\
				free(A[i]);\
				A[i] = 0;\
			}\
		}\
		free(A);\
		A = 0;\
	}\


#define DELETE_AND_FREE_ARRAY(A,size) \
		if ( A != 0 ) \
		{\
			for ( int i = 0; i < size; i++ ) \
			{ \
				if ( A[i] != 0 ) \
				{ \
					delete A[i]; \
					A[i] = 0; \
				}\
			}\
			free(A);\
			A = 0;\
		}\



#define ARRAY_BLANK_ROLLBACK( SIZESTACK, INDEXSTACK, A, V ) \
	{\
		unsigned int to_size = 0;\
		if ( SIZESTACK.size() > 0 ) \
		{\
			to_size = SIZESTACK.top(); \
			SIZESTACK.pop();\
		}\
		while ( INDEXSTACK.size() > to_size )\
		{\
			int i = INDEXSTACK.top();\
			INDEXSTACK.pop();\
			A[i] = V;\
		}\
	}\
	

#define ARRAY_REPLACE_ROLLBACK( SIZESTACK, INDEXSTACK, VALUESTACK, A ) \
	{\
		unsigned int to_size = 0;\
		if ( SIZESTACK.size() > 0 ) \
		{\
			to_size = SIZESTACK.top(); \
			SIZESTACK.pop();\
		}\
		while ( INDEXSTACK.size() > to_size )\
		{\
			int i = INDEXSTACK.top();\
			INDEXSTACK.pop();\
			A[i] = VALUESTACK.top();\
			VALUESTACK.pop();\
		}\
	}\



#define ARRAY_INCREMENT_ROLLBACK( SIZESTACK, INDEXSTACK, A) \
	{\
		unsigned int to_size = 0;\
		if ( SIZESTACK.size() > 0 ) \
		{\
			to_size = SIZESTACK.top(); \
			SIZESTACK.pop();\
		}\
		while ( INDEXSTACK.size() > to_size )\
		{\
			int i = INDEXSTACK.top();\
			INDEXSTACK.pop();\
			A[i] = A[i] - 1;\
		}\
	}\



#define ARRAY_DECREMENT_ROLLBACK( SIZESTACK, INDEXSTACK, A) \
	{\
		unsigned int to_size = 0;\
		if ( SIZESTACK.size() > 0 ) \
		{\
			to_size = SIZESTACK.top(); \
			SIZESTACK.pop();\
		}\
		while ( INDEXSTACK.size() > to_size )\
		{\
			int i = INDEXSTACK.top();\
			INDEXSTACK.pop();\
			A[i] = A[i] + 1;\
		}\
	}\
	



#endif /* MACROS_HPP_ */
