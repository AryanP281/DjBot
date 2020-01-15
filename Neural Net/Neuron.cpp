
/***********************Preprocessor Directives***************/
#include "pch.h"
#include <random>
#include <time.h>
#include "Misc.h"
#include "Neuron.h"

//Typdefs
typedef double(*ActivationFunction)(double);
typedef double(*ActivationFuncKey)(double);

/***********************Constructors And Destructors***************/
Neuron::Neuron(int inputCount, ActivationFunctionType activationFunc) : BIAS(1.0)
{
	this->numOfInputs = inputCount;

	//Setting the activation function
	switch (activationFunc)
	{
	case ActivationFunctionType::Sigmoid : 
		this->activationFunction = Misc::Sigmoid; 
		this->activationFunctionKey = Misc::DeSigmoid;
		break;
	case ActivationFunctionType::Tanh :
		this->activationFunction = Misc::Tanh; 
		this->activationFunctionKey = Misc::TanhInverse;
		break;
	case ActivationFunctionType::ReLu :
		this->activationFunction = Misc::ReLu; 
		this->activationFunctionKey = Misc::DeReLu;
		break;
	}

	//Initializing the weights
	InitializeWeights();
}

Neuron::Neuron(const Neuron& neuron) : BIAS(1.0f)
{
	*this = neuron;
}

Neuron::~Neuron()
{
	if (numOfInputs > 0)
		delete[] weights;
}

/***********************Functions***************/
void Neuron::InitializeWeights(double* vals)
{
	//Allocating memory for the weights
	weights = new double[numOfInputs + 1];

	//Checking if initializatin values have been specified
	if (vals != nullptr)
		Misc::CopyArray<double>(vals, weights, numOfInputs + 1);
	else //Initializing to random values
	{
		//Seeding random number generator
		srand(time(NULL));

		for (int a = 0; a < numOfInputs + 1; ++a)
		{
			weights[a] = std::sin(rand());
		}
	}

}

/***********************Methods***************/
double Neuron::GiveOutput(double* inputs)
{
	double output = BIAS * weights[0]; //The output of the neuron

	//Calculating the output
	for (int a = 1; a < numOfInputs + 1; ++a)
	{
		output += inputs[a - 1] * weights[a];
	}

	return activationFunction(output);
}

const ActivationFunction Neuron::ReturnActivationFunc() const
{
	return this->activationFunction;
}

const ActivationFuncKey Neuron::ReturnActivationFuncKey() const
{
	return activationFunctionKey;
}

/***********************Operators***************/
void Neuron::operator=(const Neuron& neuron)
{
	this->numOfInputs = neuron.numOfInputs;
	InitializeWeights(neuron.weights);
	this->activationFunction = neuron.activationFunction;
	this->activationFunctionKey = neuron.activationFunctionKey;
}
