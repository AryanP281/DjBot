
/***********************Preprocessor Directives***************/
#include "pch.h"
#include "DllInterface.h"

/***********************Functions***************/
void InitializeNet(int* netStruct, double momentumFactor, double networkLearningRate, 
	char activationFunctionType)
{
	
	//Setting the activation function type
	ActivationFunctionType activationFunc;
	switch (activationFunctionType)
	{
	case 0: activationFunc = ActivationFunctionType::Sigmoid; break;
	case 1: activationFunc = ActivationFunctionType::Tanh; break;
	case 2: activationFunc = ActivationFunctionType::ReLu; break;
	default: activationFunc = ActivationFunctionType::Sigmoid; break;
	}

	net = NeuralNet(netStruct, momentumFactor, networkLearningRate, activationFunc);

}

void GetOutput(double* inputs, double* outputBuffer)
{
	net.GiveOutput(inputs, outputBuffer);
}

void TrainNetwork(double* inputs, double* expectedOutputs,
	int dataSetSize, int epochs, double errorChangeSensitivity)
{
	std::vector<int>* netStruct = new std::vector<int>; /*The structure of the neural
	net*/
	*netStruct = net.NetworkStructure();
	
	//Separating the input sets
	double** inputSets = new double* [dataSetSize];
	for (int a = 0; a < dataSetSize; ++a)
	{
		inputSets[a] = new double[(*netStruct)[0]];
		for (int b = 0; b < (*netStruct)[0]; ++b)
		{
			inputSets[a][b] = inputs[(a * (*netStruct)[0]) + b];
		}
	}

	//Separating the output sets
	double** outputSets = new double* [dataSetSize];
	for (int a = 0; a < dataSetSize; ++a)
	{
		outputSets[a] = new double[(*netStruct)[2]];
		for (int b = 0; b < (*netStruct)[2]; ++b)
		{
			outputSets[a][b] = expectedOutputs[(a * (*netStruct)[2]) + b];
		}
	}
	delete netStruct; //Deallocating memory after use
	
	//Training the network
	net.TrainNetwork(inputSets, outputSets, dataSetSize, epochs, errorChangeSensitivity);

	//Deallocating the memory after use
	for (int a = 0; a < dataSetSize; ++a)
	{
		delete[] inputSets[a];
		delete[] outputSets[a];
	}
	delete[] inputSets;
	delete[] outputSets;

}