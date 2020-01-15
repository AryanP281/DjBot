#pragma once
/***********************Preprocessor Directives***************/
#include <stdio.h>
#include "NeuralNet.h"

/***********************Variables***************/
NeuralNet net;

/***********************Functions***************/
extern "C"
{
	_declspec(dllexport) void InitializeNet(int* netStruct, double momentumFactor = 0.9,
		double networkLearningRate = 0.25, char activationFuctionType = 0);
	/*Initializes the neural net with given learning rate*/
	_declspec(dllexport) void GetOutput(double* inputs, double* outputBuffer); /*Gets output
	from the neural net for the given input*/
	_declspec(dllexport) void TrainNetwork(double* inputs, double* expectedOutputs,
		int dataSetSize, int epochs = 0, double errorChangeSensitivity = 0.0001); /*Trains
		the neural network*/

}

