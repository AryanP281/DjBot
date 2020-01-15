
/**************************Preprocessor Directives****************/
#include "pch.h"
#include <random>
#include <time.h>
#include <exception>
#include "Misc.h"

/*************************Functions****************/
int Misc::Random(int min, int max)
{
	int numOfPossibleVals = (max - min) - 1;

	int a = 0;
	while (a == 0)
	{
		a = rand() % numOfPossibleVals;
	}

	return (min + a);
}

signed char Misc::Signum(double x)
{
	if (x < 0)
		return -1;
	else if (x > 0)
		return 1;
	else
		return 0;
}

double Misc::Sigmoid(double x)
{
	return (1.0 / (1.0 + exp(-x)));
}

double Misc::DeSigmoid(double y)
{
	return (log(y) - log(1.0 - y));
}

double Misc::Tanh(double x)
{
	return tanh(x);
}

double Misc::TanhInverse(double y)
{
	return atanh(y);
}

double Misc::ReLu(double x)
{
	if (x > 0)
		return x;
	else
		return 0;
}

double Misc::DeReLu(double y)
{
	return y;
}

double Misc::Average(double* vals, int n)
{
	double sum = 0.0;
	for (int a = 0; a < n; ++a)
	{
		sum += vals[a];
	}

	return (sum / n);
}

int Misc::PowWrt10(double x)
{
	int powWrt2 = 0; /*The power of x wrt 2*/
	std::frexp(x, &powWrt2); /*Getting the power wrt 2*/
	int powWrt10 = ceil(powWrt2 * (log(2) / log(10))); /*Calculating the pow wrt 10*/

	return powWrt10;
}