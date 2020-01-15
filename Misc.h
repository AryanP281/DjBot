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

}