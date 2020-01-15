
/**************************Preprocessor Directives****************/
#include "pch.h"
#include <random>
#include <time.h>
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