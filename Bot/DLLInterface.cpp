/**************************Preprocessor Directives****************/
#include "pch.h"
#include "DLLInterface.h"

/**************************Functions****************/
void Initialize(int numOfSamples, float exploratoryProbability,
	float* sampleDurations, bool newSampleSet)
{
	//Initializing the bot
	bot = DjBot(numOfSamples, exploratoryProbability, sampleDurations, newSampleSet);
}

void GetTrack(int* trackBuffer, float songDuration, int numOfSamples)
{
	//Getting the track from bot in vector form
	std::vector<int> trackVec;
	if (numOfSamples == 0)
	{
		trackVec = bot.ReturnSampleOrder(songDuration);
	}
	else
	{
		trackVec = bot.ReturnSampleOrder(numOfSamples);
	}
	
	//Storing track data in provided buffer
	for (int a = 0; a < trackVec.size(); ++a)
	{
		trackBuffer[a] = trackVec[a];
	}
	if(songDuration != 0.0f)
		trackBuffer[trackVec.size()] = bot.SampleCount() + 1; /*Indicating end of track*/
}

void GiveFeedBack(float rating)
{
	bot.ReceiveFeedbackForLastTrack(rating / 10.0f);
}

void Quit()
{
	//Destroying the bot object
	bot.~DjBot();
}

int TotalTracksCreated()
{
	return bot.TotalTracksCreated();
}
