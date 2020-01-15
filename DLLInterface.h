#pragma once
/**************************Preprocessor Directives****************/
#include <stdio.h>
#include "DjBot.h"

/**************************Variables****************/
DjBot bot;

/**************************Functions****************/
extern "C"
{
	_declspec(dllexport) void Initialize(int numOfSamples, float exploratoryProbability,
		float* sampleDurations, bool newSampleSet); /*Initializes the bot*/
	
	_declspec(dllexport) void GetTrack(int* trackBuffer, float songDuration = 0.0f,
		int numOfSamples = 0); 
	/*Gets the track from the bot and stores in buffer*/

	_declspec(dllexport) void GiveFeedBack(float rating); /*Gives the user's feedback
						rating to the bot*/

	_declspec(dllexport) void Quit(); /*The quitting protocol*/

	_declspec(dllexport) int TotalTracksCreated(); /*Returns the total number of tracks 
	created*/
}
