#pragma once

namespace Misc
{
	int Random(int min, int max); //Returns a random number in the range (min, max)
	
	template<typename T> void CopyArray(T* source, T* destination, int arraySize) /*
	Copies the elements of the source to the destination*/
	{
		//Copying the elements
		for (int a = 0; a < arraySize; ++a)
		{
			destination[a] = source[a];
		}

	}

	signed char Signum(double x);

	double Sigmoid(double x);
	double DeSigmoid(double y);

	double Tanh(double x);
	double TanhInverse(double y);

	double ReLu(double x);
	double DeReLu(double y);

	double Average(double* vals, int n);

	int PowWrt10(double x); /*Returns the power of x wrt 10*/
}