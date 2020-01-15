
/**************************Preprocessor Directives****************/
#include "pch.h"
#include <time.h>
#include "Misc.h"
#include "DjBot.h"

/**************************Constructors And Destructors****************/
DjBot::DjBot(int numOfSamples, float exploratoryProbability, float* sampleDurations, 
	bool newSampleSet) 
	: totalTracksCreated(0), currentTrackDuration(0.0f), LEARNING_RATE(0.4F),
	NEGATIVE_REWARD_THRESHOLD(0.45F)
{
	this->sampleCount = numOfSamples;
	this->exploratoryProbability = exploratoryProbability;

	this->sampleDurations = new float[numOfSamples];
	Misc::CopyArray<float>(sampleDurations, this->sampleDurations, numOfSamples);

	//Initializing the structure of the required table
	int* tableStruct = new int[2];
	tableStruct[0] = tableStruct[1] = sampleCount;
	
	//Initializing the suitability values for the samples' table
	if (newSampleSet) /*Checking if a new sample set is being used*/
	{
		float** compatibilityValues = new float* [sampleCount];
		for (int a = 0; a < sampleCount; ++a)
		{
			compatibilityValues[a] = new float[sampleCount];
			for (int b = 0; b < sampleCount; ++b)
			{
				compatibilityValues[a][b] = 0.5f;
			}
		}
		//Initializing the table used for storing the compatibility values with the given values
		compatibilityValuesTables = Table<float>("CompatibilityValues.bin", tableStruct,
			compatibilityValues);
	}
	else
	{
		//Initializing the table used for storing the compatibility values
		compatibilityValuesTables = Table<float>("CompatibilityValues.bin", tableStruct);

		//Loading the data from the previous training sessions
		LoadPreviousSessionData();
	}

	//Deallocating memory after use
	delete[] tableStruct;
}

DjBot::DjBot(const DjBot& bot) : LEARNING_RATE(0.4F), NEGATIVE_REWARD_THRESHOLD(0.45F)
{
	*this = bot;
}

DjBot::DjBot() : totalTracksCreated(0), currentTrackDuration(0.0f), LEARNING_RATE(0.4F),
NEGATIVE_REWARD_THRESHOLD(0.45f)
{
	this->sampleCount = 0;
	this->exploratoryProbability = 0.0f;
}

DjBot::~DjBot()
{
	if (totalTracksCreated > 0)
	{
		//Saving the current session data
		SaveCurrentSessionData();
	}
}

/**************************Functions****************/
void DjBot::LoadPreviousSessionData()
{
	std::ifstream file; //A stream to the initialization file
	file.open("Initialize.bin", std::ios::in | std::ios::binary); /*Opening stream to
	the initialization file*/
	
	if (!file.is_open()) //Checking if file has opened successfully
	{
		throw std::exception("Unable to open initialization file");
	}

	//Placing the file read pointer
	file.seekg(0, std::ios::beg);

	//Reading data from file
	char* byteBuffer = new char[sizeof(totalTracksCreated)];
	file.read(byteBuffer, sizeof(totalTracksCreated));

	totalTracksCreated = *(reinterpret_cast<int*>(byteBuffer));

	//Closing file after use
	file.close();

	//Deallocating memory after use
	delete[] byteBuffer;
}

void DjBot::AdjustExploratoryProbability()
{
	if ((totalTracksCreated % (sampleCount * 2) == 0) && totalTracksCreated > 0)
		exploratoryProbability /= 2.0f;
}

void DjBot::DetermineIdealSampleCombination(float* songDuration)
{
	//The exit condition for recursion
	if (currentTrackDuration >= *songDuration) /*Checking if a song
	of the required duration has been produced*/
	{
		return;
	}

	//Selecting the next sample
	track.push_back(ReturnNextSample());
	currentTrackDuration += sampleDurations[track[track.size() - 1]];

	//Searching for next sample
	DetermineIdealSampleCombination(songDuration);
}

void DjBot::DetermineIdealSampleCombination(int numberOfSamples)
{
	//The exit condition for recursion
	if (currentTrackDuration == numberOfSamples) /*Checking if track with the desired
	number of samples has been generated*/
	{ 
		return;
	}

	//Selecting the next sample
	track.push_back(ReturnNextSample());
	++currentTrackDuration;

	//Searching for the next sample
	DetermineIdealSampleCombination(numberOfSamples);
}

int DjBot::ReturnNextSample()
{
	//Determining policy
	if (Misc::Random(-1, 11) <= (exploratoryProbability * 10))
		return Misc::Random(-1, sampleCount - 1);

	//Returning the most greedy action
	return MostSuitableSample();

}

int DjBot::MostSuitableSample()
{
	int mostSuitableSample = -1;
	float maxCompatibility = 0.0f;
	for (int a = 0; a < sampleCount; ++a)
	{
		float* compatibilityVals = new float[sampleCount]; /*The compatibility of sample 
		'a' with other samples*/
		compatibilityValuesTables.RetrieveFromTable(&compatibilityVals, sampleCount, a);

		//Calculating the compatibility of sample 'a' with other samples in the track
		float compatibility = 0.0f;
		for (int b = 0; b < track.size(); ++b)
		{
			compatibility += (compatibilityVals[track[b]] / (float)(track.size() - b));
		}

		//Checking if this sample is the most compatible sample
		if (compatibility >= maxCompatibility)
		{
			mostSuitableSample = a;
			maxCompatibility = compatibility;
		}
	}

	return mostSuitableSample;
}

void DjBot::SaveCurrentSessionData()
{
	std::ofstream file; //A stream to the initialization file
	file.open("Initialize.bin", std::ios::out | std::ios::binary); /*Opening stream to
	the initialization file*/

	if (!file.is_open()) //Checking if file has opened successfully
	{
		/*Trying to create a new file assuming that the file did not open as it doesnt 
		exist*/
		file.open("Initialize.bin", std::ios::out | std::ios::binary | std::ios::trunc);

		//Checking if file was successfully created
		if (!file.is_open())
			throw std::exception("Unable to open initialization file");
	}

	//Placing the file write pointer
	file.seekp(0, std::ios::beg);

	//Writing the data
	char* byteBuffer = reinterpret_cast<char*>(&totalTracksCreated);
	file.write(byteBuffer, sizeof(totalTracksCreated));

	//Closing the file after use
	file.close();
}

/**************************Methods****************/
std::vector<int> DjBot::ReturnSampleOrder(float songDuration)
{
	//Updating the probability of exploratory policy
	AdjustExploratoryProbability();
	
	//Clearing the track
	track.clear();

	//Seeding the random number generator
	srand(time(NULL));

	//Randomly selecting the first sample
	track.push_back(Misc::Random(-1, sampleCount - 1));
	currentTrackDuration = sampleDurations[track[0]]; //Resetting the track duration

	//Creating the track
	DetermineIdealSampleCombination(&songDuration);

	//Updating the track counter
	++(this->totalTracksCreated);

	//Returning the newly created track
	return track;
}

std::vector<int> DjBot::ReturnSampleOrder(int numberOfSamples)
{
	//Updating the probability of exploratory policy
	AdjustExploratoryProbability();

	//Clearing the track
	track.clear();

	//Seeding the random number generator
	srand(time(NULL));

	//Randomly selecting the first sample
	track.push_back(Misc::Random(-1, sampleCount - 1));
	currentTrackDuration = 1;

	//Creating the track
	DetermineIdealSampleCombination(numberOfSamples);

	//Updating the track counter
	++(this->totalTracksCreated);

	//Returning the newly created track
	return track;

}

void DjBot::ReceiveFeedbackForLastTrack(float rating)
{
	//Adjusting the compatibility values
	for (int a = 0; a < track.size(); ++a)
	{
		float* oldCompatibilityValues = new float[sampleCount];
		
		compatibilityValuesTables.RetrieveFromTable(&oldCompatibilityValues, sampleCount,
			track[a]); //Getting the old compatibility values
		
		for (int b = 0; b < track.size(); ++b)
		{
			if (b != a)
			{
				float scaledRating = std::abs(rating - NEGATIVE_REWARD_THRESHOLD);
				float* newCompatibility = new float;
				if (rating > NEGATIVE_REWARD_THRESHOLD)
				{
					*newCompatibility = oldCompatibilityValues[track[b]] +
						(1.0f - oldCompatibilityValues[track[b]]) *
						((scaledRating / std::abs(a - b)) * LEARNING_RATE);
				}
				else
				{
					*newCompatibility = oldCompatibilityValues[track[b]] +
						(0.0f - oldCompatibilityValues[track[b]]) *
						((scaledRating / std::abs(a - b)) * LEARNING_RATE);
				}

				//Changing the compatibility value
				compatibilityValuesTables.WriteToTable(newCompatibility, 1, track[a], 
					track[b]);

				delete newCompatibility;
			}
		}

		delete[] oldCompatibilityValues;
	}
}

void DjBot::ReceiveFeedbackForTrack(std::vector<int>& track, float rating)
{
	//Adjusting the compatibility values
	for (int a = 0; a < track.size(); ++a)
	{
		float* oldCompatibilityValues = new float[sampleCount];

		compatibilityValuesTables.RetrieveFromTable(&oldCompatibilityValues, sampleCount,
			track[a]); //Getting the old compatibility values

		for (int b = 0; b < track.size(); ++b)
		{
			if (b != a)
			{
				float scaledRating = std::abs(rating - NEGATIVE_REWARD_THRESHOLD);
				float* newCompatibility = new float;
				if (rating > NEGATIVE_REWARD_THRESHOLD)
				{
					*newCompatibility = oldCompatibilityValues[track[b]] +
						(1.0f - oldCompatibilityValues[track[b]]) *
						((scaledRating / std::abs(a - b)) * LEARNING_RATE);
				}
				else
				{
					*newCompatibility = oldCompatibilityValues[track[b]] +
						(0.0f - oldCompatibilityValues[track[b]]) *
						((scaledRating / std::abs(a - b)) * LEARNING_RATE);
				}

				//Changing the compatibility value
				compatibilityValuesTables.WriteToTable(newCompatibility, 1, track[a],
					track[b]);

				delete newCompatibility;
			}
		}

		delete[] oldCompatibilityValues;
	}
}

int DjBot::SampleCount() const
{
	return sampleCount;
}

int DjBot::TotalTracksCreated() const
{
	return this->totalTracksCreated;
}

/**************************Operators****************/
void DjBot::operator=(const DjBot& bot)
{
	if (this->sampleCount != 0)
		delete[] this->sampleDurations;
	this->sampleDurations = new float[bot.sampleCount];
	Misc::CopyArray<float>(bot.sampleDurations, this->sampleDurations, bot.sampleCount);
	
	this->sampleCount = bot.sampleCount;
	this->compatibilityValuesTables = bot.compatibilityValuesTables;
	this->exploratoryProbability = bot.exploratoryProbability;
	this->track = bot.track;
	this->currentTrackDuration = bot.currentTrackDuration;
	this->totalTracksCreated = bot.totalTracksCreated;
}