/*
 * LinearConstraint.cpp
 *
 *  Created on: Apr 17, 2014
 *      Author: stolee
 */

#include "LinearConstraint.hpp"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <queue>

#include "macros.hpp"

using namespace adage::grids;



Monomial::Monomial()
{
	this->coefficient = 0;
	this->varname = 0;
	this->num_keys = 0;
	this->keys = 0;
	this->next = 0;
	this->prev = 0;
}

Monomial::~Monomial()
{
	if ( this->next != 0 )
	{
		delete this->next;
		this->next = 0;
	}

	this->prev = 0;
	FREE_ARRAY(this->varname);
	FREE_ARRAY(this->keys);
}

/**
 * This insert method may add to the linked list!
 */
Monomial* Monomial::insert(int coefficient, char* varname, int num_keys, int* keys)
{
	bool insert_in_middle = false;
	if ( this->varname == 0 /* || this->next == 0 */ )
	{
		// these conditions should really be the same thing!
		insert_in_middle = true;
	}
	else if ( strcmp(this->varname, varname) == 0 ) 
	{
		// correct variable, what about the key?
		bool equal = true;
		for ( int i = 0; i < num_keys; i++ )
		{
			if ( this->keys[i] == keys[i] )
			{
				equal = true;
			}
			else if ( this->keys[i] > keys[i] )
			{
				equal = false;
				insert_in_middle= true;
				break;
			}
			else if ( this->keys[i] < keys[i] )
			{
				equal = false;
				break;
			}
		}

		if ( equal )
		{
			// modify this coefficient!
			(this->coefficient) += coefficient;
			return this;
		}
		else if ( !insert_in_middle )
		{
			this->next = this->next->insert(coefficient, varname, num_keys, keys);
		}
	}
	else if ( strcmp(this->varname, varname ) < 0 )
	{
		// this variable belongs AFTER me!
		this->next = this->next->insert(coefficient, varname, num_keys, keys);
	}
	else if ( strcmp(this->varname, varname ) > 0 )
	{
		// this variable belongs BEFORE me!
		insert_in_middle = true;
	}

	if ( insert_in_middle )
	{
		// ok, we must add it here!
		this->prev = new Monomial(); 

		this->prev->varname = (char*)malloc(strlen(varname)+1);
		this->prev->num_keys = num_keys;
		this->prev->keys = (int*)malloc(num_keys * sizeof(int));
		
		strcpy(this->prev->varname, varname);

		for ( int i = 0; i < num_keys; i++ )
		{
			this->prev->keys[i] = keys[i];
		}

		this->prev->coefficient = coefficient;
		this->prev->next = this;

		return this->prev;
	}

	return this;
}

char* Monomial::getString()
{
	if ( this->varname == 0 || this->next == 0 )
	{
		// I should be an empty monomial, so let's be an empty string!
		return 0;
	}

	if ( this->coefficient == 0 )
	{
		return this->next->getString();
	}
	
	int origl = strlen(this->varname) + 10 + 10 * this->num_keys;
	char* news = (char*)malloc( origl );

	news[0] = 0;
	sprintf(news, "%d * %s(", this->coefficient, this->varname);

	for ( int i = 0; i < this->num_keys; i++ )
	{
		sprintf(news + strlen(news), "%d", this->keys[i] );

		if ( i < this->num_keys - 1 )
		{
			strcat(news, ",");		
		}	
	}
	strcat(news, ")");

	char* s = this->next->getString();

	if ( s != 0 )
	{
		int newsl = strlen(news);
		int sl = strlen(s);

		news = (char*)realloc(news, newsl + sl + 15);
		
		strcat(news, " + ");
		strcat(news, s);	

		free(s);
	}

	return news;
}


LinearConstraint::LinearConstraint(int w_coeff, int rhs, int inequality_mode)
{
	this->w_coeff = w_coeff;
	this->rhs = rhs;
	this->inequality_mode = inequality_mode;
	this->monomial_list = new Monomial();
}

LinearConstraint::~LinearConstraint()
{
	delete this->monomial_list;
}

void LinearConstraint::insertMonomial(int coefficient, char* varname, int num_keys, int* keys)
{
	this->monomial_list = this->monomial_list->insert(coefficient, varname, num_keys, keys);
}

char* LinearConstraint::getString()
{
	char* costr = this->monomial_list->getString();

	if ( costr == 0 )
	{
		return 0;
	}

	costr = (char*)realloc(costr, strlen(costr) + 30);

	if ( this->w_coeff > 0 )
	{
		sprintf(costr + strlen(costr), " + %d * w", this->w_coeff);
	}
	else
	{
		sprintf(costr + strlen(costr), " - %d * w", -this->w_coeff);
	}

	switch( this->inequality_mode )
	{
		case INEQUALITY_GEQ:
		strcat(costr, " >= ");
		break;
		case INEQUALITY_EQ:
		strcat(costr, " == ");
		break;
		case INEQUALITY_LEQ:
		strcat(costr, " <= ");
		break;
	}


	sprintf(costr + strlen(costr), "%d", this->rhs);

	return costr;
}



