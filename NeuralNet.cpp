
/***********************Preprocessor Directives***************/
#include "pch.h"
#include <math.h>
#include "Misc.h"
#include "NeuralNet.h"

/***********************Constructors And Destructors***************/
NeuralNet::NeuralNet(int* netStruct, double momentumFactor, double learningRate,
	ActivationFunctionType activationFunc)
{
	this->netStruct = new int[3];
	Misc::CopyArray<int>(netStruct, this->netStruct, 3);

	this->momentumFactor = momentumFactor;
	this->learningRate = learningRate;

	//Initializing the neurons
	InitializeNeurons(activationFunc);
}

NeuralNet::NeuralNet(const NeuralNet& net)
{
	*this = net;
}

NeuralNet::NeuralNet()
{
	this->netStruct = nullptr;
	this->learningRate = 0.0;
}

NeuralNet::~NeuralNet()
{
	if (netStruct != nullptr)
	{
		delete[] netStruct;

		for (char a = 0; a < 2; ++a)
		{
			delete[] neurons[a];
		}
		delete[] neurons;
	}
}

/***********************Functions***************/
void NeuralNet::InitializeNeurons(ActivationFunctionType activationFunc)
{
	neurons = new Neuron * [2];
	for (char a = 0; a < 2; ++a)
	{
		neurons[a] = new Neuron[netStruct[a + 1]];
		for (int b = 0; b < netStruct[a + 1]; ++b)
		{
			neurons[a][b] = Neuron(netStruct[a], activationFunc);
		}
	}
}

void NeuralNet::InitializeNeurons(Neuron** neuronVals)
{
	this->neurons = new Neuron * [2];
	for (char a = 0; a < 2; ++a)
	{
		this->neurons[a] = new Neuron[netStruct[a + 1]];
		for (int b = 0; b < netStruct[a + 1]; ++b)
		{
			this->neurons[a][b] = neuronVals[a][b];
		}
	}
}

void NeuralNet::EpochBasedTraining(double** inputs, double** expectedOutputs,
	int dataSetSize, int epochs, double** networkOutput,
	double*** derivatives, double*** previousDerivatives,
	double* mse)
{
	//Training for epochs
	for (int a = 0; a < epochs; ++a)
	{
		//Resetting the mse to 0.0
		for (int i = 0; i < netStruct[2]; ++i)
		{
			mse[i] = 0.0;
		}

		//Training for datasets
		for (int b = 0; b < dataSetSize; ++b)
		{
			//Resetting the current iteration derivatives to zero
			ResetDerivativesBuffer(derivatives);

			double* normalizedInputs = new double[netStruct[0]];
			//Normalising the inputs
			for (int c = 0; c < netStruct[0]; ++c)
			{
				normalizedInputs[c] = neurons[0][0].ReturnActivationFunc()(inputs[b][c]);
			}

			double* normalizedExpectedOutputs = new double[netStruct[2]];
			for (int c = 0; c < netStruct[2]; ++c)
			{

				normalizedExpectedOutputs[c] = neurons[1][c].ReturnActivationFunc()(expectedOutputs[b][c]);
			}

			//Getting the neuron outputs
			GetEntireNetworkOutput(normalizedInputs, networkOutput);

			//Calculating the derivatives
			CalculateDerivatives(normalizedInputs, normalizedExpectedOutputs, networkOutput,
				derivatives, mse);

			//Deallocating memory after use
			delete[] normalizedInputs;
			delete[] normalizedExpectedOutputs;
		}

		//Calculating the mse
		for (int i = 0; i < netStruct[2]; ++i)
		{
			mse[i] /= (2.0 * dataSetSize);
		}

		//Calculating average derivatives for the epoch
		CalculateAverageDerivatives(derivatives, dataSetSize);

		//Adjusting the weights after completing all datasets
		AdjustWeights(previousDerivatives, derivatives);
	}

}

void NeuralNet::MaxErrorMinimizationBasedTraining(double** inputs,
	double** expectedOutputs, int dataSetSize, double** networkOutput,
	double*** derivatives, double*** previousDerivatives,
	double* mse, double errorChangeSensitivity)
{
	double avgPreviousMse = 0.0; /*The average mse for all outputs during the previous
	iteration*/
	double currentAvgMse = 0.0; /*The average mse for all outputs during current iteration*/
	double stationaryPoint = 0.0; /*The value of mse at which the change becomes zero
	or positive*/
	
	while (true)
	{
		//Resetting the mse to 0.0
		for (int i = 0; i < netStruct[2]; ++i)
		{
			mse[i] = 0.0;
		}

		//Training for datasets
		for (int b = 0; b < dataSetSize; ++b)
		{
			//Resetting the current iteration derivatives to zero
			ResetDerivativesBuffer(derivatives);

			double* normalizedInputs = new double[netStruct[0]];
			//Normalising the inputs
			for (int c = 0; c < netStruct[0]; ++c)
			{
				normalizedInputs[c] = neurons[0][0].ReturnActivationFunc()(inputs[b][c]);
			}

			double* normalizedExpectedOutputs = new double[netStruct[2]];
			for (int c = 0; c < netStruct[2]; ++c)
			{

				normalizedExpectedOutputs[c] = neurons[1][c].ReturnActivationFunc()(expectedOutputs[b][c]);
			}

			//Getting the neuron outputs
			GetEntireNetworkOutput(normalizedInputs, networkOutput);

			//Calculating the derivatives
			CalculateDerivatives(normalizedInputs, normalizedExpectedOutputs, networkOutput,
				derivatives, mse);

			//Deallocating memory after use
			delete[] normalizedInputs;
			delete[] normalizedExpectedOutputs;
		}

		//Calculating the mse
		for (int i = 0; i < netStruct[2]; ++i)
		{
			mse[i] /= (2.0 * dataSetSize);
		}

		//Deciding whether to stop training
		currentAvgMse = Misc::Average(mse, netStruct[2]); /*Calculating the avg. mse for
		the current iteration*/
		double changeInMse = (currentAvgMse - avgPreviousMse);
		if (changeInMse == 0) /*Checking if the error has stopped 
		i.e the min error has been achieved*/
		{
			static char stationaryCount = 0; /*The number of iterations the mse hasnt
			changed*/
			if (stationaryPoint == 0) 
				stationaryPoint = currentAvgMse;
			else
			{
				if (stationaryCount < 4)
					++stationaryCount;
				else
					break;
			}
		}
		else if (changeInMse > 0) /*If the error is increasing, 
		training will be stopped*/
		{
			if (stationaryPoint == 0)
				stationaryPoint = currentAvgMse;
			else
			{
				double mseIncrease = currentAvgMse - stationaryPoint; /*The increase in
				the mse wrt to the stationary point*/

				if (mseIncrease >= (errorChangeSensitivity * stationaryPoint ))
					break;

			}
		}
		else
			stationaryPoint = 0.0;

		avgPreviousMse = currentAvgMse;

		//Calculating average derivatives for the epoch
		CalculateAverageDerivatives(derivatives, dataSetSize);

		//Adjusting the weights after completing all datasets
		AdjustWeights(previousDerivatives, derivatives);
	}
}

void NeuralNet::ResetDerivativesBuffer(double*** buffer)
{
	for (char a = 0; a < 2; ++a)
	{
		for (int b = 0; b < netStruct[a + 1]; ++b)
		{
			for (int c = 0; c < netStruct[a] + 1; ++c)
			{
				buffer[a][b][c] = 0.0f;
			}
		}
	}
}

void NeuralNet::GetEntireNetworkOutput(double* inputs, double** buffer)
{
	//Getting the outputs of the hidden layer
	for (int a = 0; a < netStruct[1]; ++a)
	{
		buffer[0][a] = neurons[0][a].GiveOutput(inputs);
	}

	//Getting the outputs for the output layer
	for (int a = 0; a < netStruct[2]; ++a)
	{
		buffer[1][a] = neurons[1][a].GiveOutput(buffer[0]);
	}
}

void NeuralNet::CalculateDerivatives(double* inputs, double* expectedOutputs,
	double** neuronOutputs, double*** buffer, double* mse)
{
	//Calculating the output layer weights' derivatives
	CalculateOutputLayerDerivatives(neuronOutputs, expectedOutputs, buffer[1], mse);

	//Calculating the hidden layer weights' derivatives
	CalculateHiddenLayerDerivatives(inputs, neuronOutputs, expectedOutputs, buffer[0]);
}

void NeuralNet::CalculateOutputLayerDerivatives(double** neuronOutputs,
	double* expectedOutputs, double** buffer, double* mse)
{
	for (int a = 0; a < netStruct[2]; ++a)
	{
		double errorWrtOutput = neuronOutputs[1][a] - expectedOutputs[a];
		double outputWrtWeightedSum = neuronOutputs[1][a] * (1.0 - neuronOutputs[1][a]);

		//Calculating the derivative wrt bias weight
		buffer[a][0] += errorWrtOutput * outputWrtWeightedSum * neurons[1][a].BIAS;

		//Calculating the derivatives wrt other weights
		for (int b = 1; b < netStruct[1] + 1; ++b)
		{
			buffer[a][b] += errorWrtOutput * outputWrtWeightedSum * neuronOutputs[0][b - 1];
		}

		//Calculating the mse
		mse[a] += pow(errorWrtOutput, 2);
	}
}

void NeuralNet::CalculateHiddenLayerDerivatives(double* inputs, double** neuronOutputs,
	double* expectedOutputs, double** buffer)
{
	for (int a = 0; a < netStruct[1]; ++a)
	{
		float outputByWeightedSum = neuronOutputs[0][a] * (1.0 - neuronOutputs[0][a]);

		for (int b = 0; b < netStruct[2]; ++b)
		{
			float errorByNextNeuronOutput = neuronOutputs[1][b] - expectedOutputs[b];
			float nextNeuronOutputByItsWeightedSum = neuronOutputs[1][b] * (1.0 -
				neuronOutputs[1][b]);
			float nextNeuronWeightedSumByCurrentOutput = neurons[1][b].weights[a + 1];

			//Calculating derivative for bias weight
			buffer[a][0] += errorByNextNeuronOutput * nextNeuronOutputByItsWeightedSum
				* nextNeuronWeightedSumByCurrentOutput * outputByWeightedSum *
				neurons[0][a].BIAS;

			//Calculating derivatives for other weights
			for (int c = 1; c < netStruct[0] + 1; ++c)
			{
				buffer[a][c] += errorByNextNeuronOutput * nextNeuronOutputByItsWeightedSum
					* nextNeuronWeightedSumByCurrentOutput * outputByWeightedSum
					* inputs[c - 1];
			}
		}
	}
}

void NeuralNet::CalculateAverageDerivatives(double*** derivativesBuffer, int dataSetSize)
{
	for (char a = 0; a < 2; ++a)
	{
		for (int b = 0; b < netStruct[a + 1]; ++b)
		{
			for (int c = 0; c < netStruct[a] + 1; ++c)
			{
				derivativesBuffer[a][b][c] /= (float)dataSetSize;
			}
		}
	}
}

void NeuralNet::AdjustWeights(double*** previousDerivativesBuffer, 
	double*** derivativesBuffer)
{
	for (char a = 0; a < 2; ++a)
	{
		for (int b = 0; b < netStruct[a + 1]; ++b)
		{
			for (int c = 0; c < netStruct[a] + 1; ++c)
			{
				double netAdjustment = (momentumFactor * previousDerivativesBuffer[a][b][c])
					- ((1.0 - momentumFactor) * learningRate * derivativesBuffer[a][b][c]);
				neurons[a][b].weights[c] -= netAdjustment;
			}
		}
	}

	//Setting the current iteration derivatives as the previous iteration derivatives for the next iteration
	for (char a = 0; a < 2; ++a)
	{
		for (int b = 0; b < netStruct[a + 1]; ++b)
		{
			Misc::CopyArray<double>(derivativesBuffer[a][b], previousDerivativesBuffer[a][b],
				netStruct[a] + 1);
		}
	}


}

void NeuralNet::DeallocateDerivativesBuffer(double*** buffer, double*** previousDerivativesBuffer)
{
	for (char a = 0; a < 2; ++a)
	{
		for (int b = 0; b < netStruct[a + 1]; ++b)
		{
			delete[] buffer[a][b];
			delete[] previousDerivativesBuffer[a][b];
		}
		delete[] buffer[a];
		delete[] previousDerivativesBuffer[a];
	}

	delete[] buffer;
	delete[] previousDerivativesBuffer;
}

void NeuralNet::DeallocateNetworkOutputBuffer(double** buffer)
{
	for (char a = 0; a < 2; ++a)
	{
		delete[] buffer[a];
	}

	delete[] buffer;
}

/***********************Methods***************/
void NeuralNet::GiveOutput(double* inputs, double* outputBuffer)
{
	double* nextLayerInputs = new double[netStruct[0]]; /*The inputs to the next layer's
	neurons*/
	Misc::CopyArray<double>(inputs, nextLayerInputs, netStruct[0]);

	//Normalising the inputs
	for (int a = 0; a < netStruct[0]; ++a)
	{
		nextLayerInputs[a] = neurons[0][0].ReturnActivationFunc()(inputs[a]);
	}

	//Calcuating the outputs
	for (char a = 0; a < 2; ++a)
	{
		double* currentLayerOutputs = new double[netStruct[a + 1]]; /*The outputs from
		the current layer's neurons*/

		for (int b = 0; b < netStruct[a + 1]; ++b)
		{
			currentLayerOutputs[b] = neurons[a][b].GiveOutput(nextLayerInputs);
		}

		//Changing the next layer's inputs to this layer's outputs
		delete[] nextLayerInputs;
		nextLayerInputs = new double[netStruct[a + 1]];
		Misc::CopyArray<double>(currentLayerOutputs, nextLayerInputs, netStruct[a + 1]);

		//Deleting the outputs
		delete[] currentLayerOutputs;
	}

	//Copying the output to the buffer
	Misc::CopyArray<double>(nextLayerInputs, outputBuffer, netStruct[2]);

	//Cancelling normalization
	for (int a = 0; a < netStruct[2]; ++a)
	{
		outputBuffer[a] = neurons[1][a].ReturnActivationFuncKey()(outputBuffer[a]);
	}

}

void NeuralNet::TrainNetwork(double** inputs, double** expectedOutputs, int dataSetSize,
	int epochs, double errorChangeSensitivity)
{
	double** networkOutput = new double* [2]; /*The outputs of the neurons in the network*/
	double*** derivatives = new double** [2]; /*The derivatives of error w.r.t neuron weights*/
	double*** previousDerivatives = new double** [2]; /*The derivatives of the previous
	iteration, used for momentum*/
	for (char a = 0; a < 2; ++a)
	{
		networkOutput[a] = new double[netStruct[a + 1]];
		derivatives[a] = new double* [netStruct[a + 1]];
		previousDerivatives[a] = new double* [netStruct[a + 1]];
		for (int b = 0; b < netStruct[a + 1]; ++b)
		{
			derivatives[a][b] = new double[netStruct[a] + 1];
			previousDerivatives[a][b] = new double[netStruct[a] + 1];
			for (int c = 0; c < netStruct[a] + 1; ++c)
			{
				previousDerivatives[a][b][c] = 0.0;
			}
		}
	}
	double* mse = new double[netStruct[2]]; /*The mse of the outputs*/
	
	if (epochs == 0)
	{
		MaxErrorMinimizationBasedTraining(inputs, expectedOutputs, dataSetSize, networkOutput,
			derivatives, previousDerivatives, mse, errorChangeSensitivity);
	}
	else
	{
		//Training the network for the given epochs
		EpochBasedTraining(inputs, expectedOutputs, dataSetSize, epochs, networkOutput,
			derivatives, previousDerivatives, mse);
	}

	//Deallocating used buffers
	DeallocateDerivativesBuffer(derivatives, previousDerivatives);
	DeallocateNetworkOutputBuffer(networkOutput);
	delete[] mse;
}

std::vector<int> NeuralNet::NetworkStructure() const
{
	std::vector<int> networkStruct; /*A vector object containing the network structure*/
	for (char a = 0; a < 3; ++a)
	{
		networkStruct.push_back(netStruct[a]);
	}

	return networkStruct;
}

/***********************Operators***************/
void NeuralNet::operator=(const NeuralNet& net)
{
	//Destroying the current net
	this->~NeuralNet();

	//Copying the net structure
	this->netStruct = new int[3];
	Misc::CopyArray<int>(net.netStruct, this->netStruct, 3);

	//Copying the momentum factor
	this->momentumFactor = net.momentumFactor;

	//Copying the learning rate
	this->learningRate = net.learningRate;

	//Initializing the neurons
	InitializeNeurons(net.neurons);

}