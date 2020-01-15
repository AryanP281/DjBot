#pragma once
/**************************Preprocessor Directives****************/
#include<fstream>
#include <string>

/**************************Classes****************/
template<typename T> class Table
{
private:
	//Variables
	std::fstream fileStream; //Stream connected to file for input and output
	int* tableStruct; //The structure of the table i.e num of rows and columns
	std::string fileAddr; //The address of the file

	//Functions
	void InitializeTable(T** values = nullptr); //Initializes the table with the given values
	void OpenFile(); //Opens the binary file storing the table data

public:
	//Constructors And Destructors
	Table(std::string fileAddr, int* tableStruct);
	Table(std::string fileAddr, int* tableStruct, T** vals);
	Table(const Table& table);
	Table();
	~Table();

	//Methods
	void RetrieveFromTable(T** buffer, int valuesToRead, int row, int column = 0); /*
	Retrieves the data in the entire row or from the specified coordinates and stores in provided buffer*/
	void WriteToTable(T* data, int dataCount, int row, int column = 0); /*Copies the 
	provided data to the table*/

	//Operators
	void operator=(const Table& table);
};
