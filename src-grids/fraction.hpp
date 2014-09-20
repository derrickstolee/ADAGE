/*
 * fraction.hpp
 *
 *  Created on: Apr 17, 2014
 *      Author: stolee
 */

#ifndef FRACTION_HPP_
#define FRACTION_HPP_

class FractionException
{
public:
	FractionException();
	virtual ~FractionException();

	virtual const char* message();
};

class OverflowException: public FractionException
{
public:
	OverflowException();
	virtual ~OverflowException();

	virtual const char* message();
};

class UnderflowException: public FractionException
{
public:
	UnderflowException();
	virtual ~UnderflowException();

	virtual const char* message();
};

class fraction
{
public:
	fraction();
	fraction(int a, int b);
	~fraction();

	static fraction makeFraction(double d, double max_denom);

	long long int a;
	unsigned long long int b;

	void simplify();

	fraction operator+(fraction f) throw (FractionException);
	fraction operator-(fraction f);
	fraction operator/(fraction f) throw (UnderflowException);
	fraction operator*(fraction f) throw (OverflowException);

	bool operator>(fraction f);
	bool operator>=(fraction f);
	bool operator<(fraction f);
	bool operator<=(fraction f);
	bool operator==(fraction f);
};

#endif /* FRACTION_HPP_ */
