#pragma once
/***********************Preprocessor Directives***************/

/***********************Enums***************/
enum ActivationFunctionType {Sigmoid, Tanh, ReLu};

/***********************Class***************/
class Neuron
{
private:
	//Variables
	int numOfInputs; /*The number of inputs the neuron has*/
	double (*activationFunction)(double); //A pointer to the activation function to be used
	double (*activationFunctionKey)(double); /*A pointer to the function which can 
	determine th original output 'x' from the activate output 'y' */

	//Functions
	void InitializeWeights(double* vals = nullptr); /*Initializes the weights to the 
	given values (if provided) or else to random values*/

	//Typdefs
	typedef double(*ActivationFunction)(double);
	typedef double(*ActivationFuncKey)(double);

public:
	//Variables
	double* weights; /*The weights of the neuron with the previous layer neurons*/
	const double BIAS; //The value of the bias

	//Constructors And Destructors
	Neuron(int inputCount = 0, ActivationFunctionType activationFunc = 
		ActivationFunctionType::Sigmoid);
	Neuron(const Neuron& neuron);
	~Neuron();

	//Methods
	double GiveOutput(double* inputs); //Calculates and returns the output of the neuron
	const ActivationFunction ReturnActivationFunc() const; /*Returns a pointer to the
	activation function used by the neuron*/
	const ActivationFuncKey ReturnActivationFuncKey() const; /*Returns a pointer to the
	key of the activation function used by the neuron*/

	//Operators
	void operator=(const Neuron& neuron);

};
