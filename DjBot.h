#pragma once
/**************************Preprocessor Directives****************/
#include <vector>
#include "Table.h"
#include "Table.cpp"

/**************************Classes****************/
class DjBot
{
private:
	//Variables
	const float LEARNING_RATE; //The learning rate for the bot
	const float NEGATIVE_REWARD_THRESHOLD; /*The min. positive reward*/
	int sampleCount; //The num of samples that the dj has
	float* sampleDurations; /*The durations of the samples*/
	Table<float> compatibilityValuesTables; /*A Table for storing the compatibility values 
	of the samples*/
	float exploratoryProbability; /*The probability that the bot makes exploratory moves*/
	std::vector<int> track; /*The last track i.e sample combination*/
	int totalTracksCreated; /*The total number of tracks that the bot has created for the
	given sample set*/
	float currentTrackDuration; /*The duration of the current track*/

	//Functions
	void LoadPreviousSessionData(); /*Loads the data for given sample set from previous sessions*/
	void AdjustExploratoryProbability(); /*Adjust the exploratory probability to favour
	greedy policies more, at later stages*/
	void DetermineIdealSampleCombination(float* songDuration); /*Determines the best 
	combination of samples i.e track*/
	void DetermineIdealSampleCombination(int numberOfSamples); /*Determines the best 
	combination of samples i.e track*/
	int ReturnNextSample(); /*Returns the next sample for the song*/
	int MostSuitableSample(); /*Returns the sample having max compatibility*/
	void SaveCurrentSessionData(); /*Saves the initialization data for the current session*/

public:
	//Constructors And Destructors 
	DjBot(int numOfSamples, float exploratoryProbability, float* sampleDurations, 
		bool newSampleSet = false);
	DjBot(const DjBot& bot);
	DjBot();
	~DjBot();

	//Methods
	std::vector<int> ReturnSampleOrder(float songDuration); /*Returns the most suitable
	arrangement of the samples depending on the required duration of the song*/
	std::vector<int> ReturnSampleOrder(int numberOfSamples); /*Returns a track consisting
	of the given number of samples*/
	void ReceiveFeedbackForLastTrack(float rating); /*Receives the rating for the track
	and adjusts the compatibility values*/
	void ReceiveFeedbackForTrack(std::vector<int>& track, float rating); /*Receives 
	feedback for the given track and adjusts the compatibility values*/
	int SampleCount() const; //Returns the no. of samples the bot has to work with
	int TotalTracksCreated() const; /*Returns the total number of tracks created for the
	current sample set*/

	//Operators
	void operator=(const DjBot& bot);
};
