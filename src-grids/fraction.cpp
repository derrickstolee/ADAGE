/*
 * fraction.cpp
 *
 *  Created on: Apr 17, 2014
 *      Author: stolee
 */

#include "fraction.hpp"
#include <math.h>
#include <numeric>

/**
 * Shamelessly "borrowed" from Wikipedia:
 * http://en.wikipedia.org/wiki/Binary_GCD_algorithm
 */
unsigned long long int gcd(unsigned long long int u, unsigned long long int v)
{
	long long int shift;

	/* GCD(0,v) == v; GCD(u,0) == u, GCD(0,0) == 0 */
	if ( u == 0 )
		return v;
	if ( v == 0 )
		return u;

	/* Let shift := lg K, where K is the greatest power of 2
	 dividing both u and v. */
	for ( shift = 0; ((u | v) & 1) == 0; ++shift )
	{
		u >>= 1;
		v >>= 1;
	}

	while ( (u & 1) == 0 )
		u >>= 1;

	/* From here on, u is always odd. */
	do
	{
		/* remove all factors of 2 in v -- they are not common */
		/*   note: v is not zero, so while will terminate */
		while ( (v & 1) == 0 ) /* Loop X */
			v >>= 1;

		/* Now u and v are both odd. Swap if necessary so u <= v,
		 then set v = v - u (which is even). For bignums, the
		 swapping is just pointer movement, and the subtraction
		 can be done in-place. */
		if ( u > v )
		{
			unsigned long long int t = v;
			v = u;
			u = t;
		}  // Swap u and v.
		v = v - u;                       // Here v >= u.
	} while ( v != 0 );

	/* restore common factors of 2 */
	return u << shift;
}

FractionException::FractionException()
{
}

FractionException::~FractionException()
{
}

const char* FractionException::message()
{
	return "FractionException";
}

OverflowException::OverflowException()
{
}
OverflowException::~OverflowException()
{
}

const char* OverflowException::message()
{
	return "OverflowException";
}

UnderflowException::UnderflowException()
{
}

UnderflowException::~UnderflowException()
{
}

const char* UnderflowException::message()
{
	return "UnderflowException";
}

fraction::fraction()
{
	this->a = 0;
	this->b = 1;
}

fraction::fraction(int a, int b)
{
	this->a = a;
	this->b = b;
}

fraction fraction::makeFraction(double d, double max_denom)
{
	fraction f;
	f.a = 0;
	f.b = 1;
	double best_approx = d;

	for ( double db = 1.0; db <= max_denom; db++ )
	{
		double da = round(d * db);

		double approx = (d - (da / db));

		if ( approx < 0 )
		{
			approx = -approx;
		}

		if ( approx < best_approx )
		{
			best_approx = approx;
			f.a = (int) da;
			f.b = (int) db;
		}
	}
	return f;
}

fraction::~fraction()
{
}

void fraction::simplify()
{
	long long int g = 1;
	if ( a > 0 )
	{
		g = (long long int) gcd((unsigned long long int) (this->a), this->b);
	}
	else if ( a < 0 )
	{
		g = (long long int) gcd((unsigned long long int) (-this->a), this->b);
	}

	if ( g > 1 )
	{
		this->a = this->a / g;
		this->b = this->b / g;
	}
}

fraction fraction::operator+(fraction f)
{
	fraction sum;

	if ( this->b == f.b )
	{
		sum.b = this->b;
		sum.a = this->a + f.a;
	}
	else
	{
		// TODO: check for overflow/underflow here.
		sum.b = this->b * f.b;
		sum.a = f.b * this->a + this->b * f.a;
	}

	sum.simplify();

	return sum;
}

fraction fraction::operator-(fraction f)
{
	fraction sum;

	if ( this->b == f.b )
	{
		sum.b = this->b;
		sum.a = this->a - f.a;
	}
	else
	{
		// TODO: check for overflow/underflow here.
		sum.b = this->b * f.b;
		sum.a = f.b * this->a - this->b * f.a;
	}

	sum.simplify();

	return sum;
}

fraction fraction::operator/(fraction f)
{
	fraction quotient;

	quotient.a = this->a * f.b;
	quotient.b = this->b * f.a;

	quotient.simplify();

	return quotient;
}

fraction fraction::operator*(fraction f)
{
	fraction product;

	product.a = this->a * f.a;
	product.b = this->b * f.b;

	product.simplify();

	return product;
}

bool fraction::operator>(fraction f)
{
	return (this->a * f.b > f.a * this->b);
}

bool fraction::operator>=(fraction f)
{
	return (this->a * f.b >= f.a * this->b);
}

bool fraction::operator<(fraction f)
{
	return (this->a * f.b < f.a * this->b);
}

bool fraction::operator<=(fraction f)
{
	return (this->a * f.b <= f.a * this->b);
}

bool fraction::operator==(fraction f)
{
	this->simplify();
	f.simplify();
	return (this->a == f.a && this->b == f.b);
}
