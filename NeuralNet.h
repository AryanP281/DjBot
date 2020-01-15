#pragma once
/***********************Preprocessor Directives***************/
#include <vector>
#include "Neuron.h"

/***********************Class***************/
class NeuralNet
{
private:
	//Variables 
	double momentumFactor; //The momentum factor of the network
	double learningRate; //The learning rate of the network
	int* netStruct; //The structure of the network
	Neuron** neurons; //The neurons in the network

	//Functions
	void InitializeNeurons(ActivationFunctionType activationFunc); 
	/*Initializes the neurons of the network to default values*/
	void EpochBasedTraining(double** inputs, double** expectedOutputs,
		int dataSetSize, int epochs, double** networkOutput, 
		double*** derivatives, double*** previousDerivatives, 
		double* mse); /*Trains the network for a fixed number of epochs*/
	void MaxErrorMinimizationBasedTraining(double** inputs, double** expectedOutputs,
		int dataSetSize, double** networkOutput,
		double*** derivatives, double*** previousDerivatives,
		double* mse, double errorChangeSensitivity = 0.0001);
	/*Trains the network untill the error has been minimized to the max*/
	void InitializeNeurons(Neuron** neuronVals);
	/*Initializes the neurons of the network*/
	void ResetDerivativesBuffer(double*** buffer); /*Resets each value in the derivatives
	buffer to 0.0*/
	void GetEntireNetworkOutput(double* inputs, double** buffer); /*Stores in the buffer
	the output of each neuron in the network*/
	void CalculateDerivatives(double* inputs, double* expectedOutputs, 
		double** neuronOutputs, double*** buffer, double* mse); /*
	Calculates and stores weight derivatives in the buffer*/
	void CalculateOutputLayerDerivatives(double** neuronOutputs, double* expectedOutputs,
		double** buffer, double* mse); /*Calculates and stores the output layer's
		derivatives in the buffer*/
	void CalculateHiddenLayerDerivatives(double* inputs, double** neuronOutputs,
		double* expectedOutputs, double** buffer); /*Calculates and stores the hidden layer's derivatives in the buffer*/
	void CalculateAverageDerivatives(double*** derivativesBuffer, int dataSetSize); /*
		Averages the derivatives for the dataset*/
	void AdjustWeights(double*** previousDerivativesBuffer, double*** derivativesBuffer); 
	/*Adjusts the weights according to the calculated derivatives*/
	void DeallocateDerivativesBuffer(double*** buffer, double*** previousDerivativesBuffer); 
	/*Deallocates the memory used for the derivatives buffer*/
	void DeallocateNetworkOutputBuffer(double** buffer); /*Deallocates the buffer used for
		storing the outputs from the neurons*/

public:
	//Constructors And Destructors
	NeuralNet(int* netStruct, double momentumFactor = 0.9, double learningRate = 0.25, 
		ActivationFunctionType activationFunc = ActivationFunctionType::Sigmoid);
	NeuralNet(const NeuralNet& net);
	NeuralNet();
	~NeuralNet();

	//Methods
	void GiveOutput(double* inputs, double* outputBuffer); //Calculates and returns the net's outputs
	void TrainNetwork(double** inputs, double** expectedOutputs,int dataSetSize, 
		int epochs = 0, double errorChangeSensitivity = 0.0001); /*Trains the neural 
		network*/
	std::vector<int> NetworkStructure() const; /*Returns the structure of the network*/

	//Operators
	void operator=(const NeuralNet& net);

};