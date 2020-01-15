
/**************************Preprocessor Directives****************/
#include "pch.h"
#include "Table.h"

/**********************Constructors And Destructors****************/
template<typename T> Table<T>::Table(std::string fileAddr, int* tableStruct)
{
	//Saving the table structure
	this->tableStruct = new int[2];
	this->tableStruct[0] = tableStruct[0];
	this->tableStruct[1] = tableStruct[1];

	//Opening stream to file
	this->fileAddr = fileAddr; //Saving the file address
}

template<typename T> Table<T>::Table(std::string fileAddr, int* tableStruct, T** vals)
{
	//Saving the table structure
	this->tableStruct = new int[2];
	this->tableStruct[0] = tableStruct[0];
	this->tableStruct[1] = tableStruct[1];

	//Opening stream to file
	this->fileAddr = fileAddr; //Saving the file address

	//Initializing the table
	InitializeTable(vals);
}

template<typename T> Table<T>::Table(const Table& table)
{
	//Initializing the table structure
	this->tableStruct = new int[2];
	
	*this = table;
}

template<typename T> Table<T>::Table()
{
	//Initializing the table structure
	this->tableStruct = new int[2];
	tableStruct[0] = 0;
	tableStruct[1] = 0;

}

template<typename T> Table<T>::~Table()
{
	//Closing the file stream after use
	if(fileStream.is_open())
		fileStream.close();

	//Deallocating data
	delete[] tableStruct;
}

/**********************Functions****************/
template<typename T> void Table<T>::OpenFile()
{
	if (fileStream.is_open())
		return;

	fileStream.open(fileAddr, std::ios::in | std::ios::out | std::ios::binary);
	if (!fileStream.is_open())
	{
		//Creating a new file, assuming that the file did not open as it does not exist
		this->fileStream.open(fileAddr, std::ios::in | std::ios::out | std::ios::binary
			| std::ios::trunc);

		//Checking if file has been successfully opened
		if (!this->fileStream.is_open())
			throw std::exception("Unable to open file");
	}
}

template<typename T> void Table<T>::InitializeTable(T** values)
{
		//Opening the file
		OpenFile();
		
		for (int a = 0; a < tableStruct[0]; ++a)
		{
			char* data = reinterpret_cast<char*>(values[a]);
			fileStream.write(data, sizeof(T) * tableStruct[1]);
		}

		//Closing the file after use
		fileStream.close();
}

/**********************Methods****************/
template<typename T> void Table<T>::RetrieveFromTable(T** buffer, int valuesToRead, 
	int row, int column)
{
	//Opening the file
	OpenFile();
	
	std::streampos pointerPos = ((sizeof(T) * tableStruct[1]) * row) + 
		(sizeof(T) * column); //The position of the pointer in the file
	int bufferSize = sizeof(T) * valuesToRead; //The size of the buffer required to store the read data
	
	//Positioning the pointer
	fileStream.seekg(pointerPos, std::ios::beg);

	//Reading the data to the buffer
	char* byteBuffer = new char[bufferSize];
	fileStream.read(byteBuffer, bufferSize);

	//Converting byte buffer to value buffer
	*buffer = reinterpret_cast<float*>(byteBuffer);

	//Closing the file after use
	fileStream.close();
}

template<typename T> void Table<T>::WriteToTable(T* data, int dataCount, int row, int column)
{
	//Opening the file
	OpenFile();
	
	//Determing the position of the pointer in the file
	std::streampos pointerPos = ((sizeof(T) * tableStruct[1]) * row) + (sizeof(T) *
		column); //The position of the file pointer

	//Positioning the pointer
	fileStream.seekp(pointerPos, std::ios::beg);

	//Converting data to bytes
	char* byteBuffer = reinterpret_cast<char*>(data);

	//Writing data to file
	fileStream.write(byteBuffer, sizeof(T) * dataCount);

	//Closing the file after use
	fileStream.close();
}

/**********************Operators****************/
template<typename T> void Table<T>::operator=(const Table& table)
{
	//Getting the address for the file stream
	this->fileAddr = table.fileAddr;

	//Copying the table structure info
	this->tableStruct[0] = table.tableStruct[0];
	this->tableStruct[1] = table.tableStruct[1];
}