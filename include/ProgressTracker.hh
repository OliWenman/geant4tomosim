#ifndef ProgressTracker_h
#define ProgressTracker_h 1

#include <iostream>
#include "G4Timer.hh"
#include "globals.hh"

struct ProgressTracker
{
    ProgressTracker(unsigned long long int nEvents, int nRuns);
    ~ProgressTracker();
    
    G4Timer Timer;
    
    //Functions to do with the progress of the simulation
	void PrintProgress(G4String Mode);
	void ProgressBar(int Percent);
	void EstimatedTime(int Percent);
	void PrintTime(double time);

	int CurrentEvent;
	unsigned long long int NumberOfEvents;
	int CurrentImage;
	int NumberOfRuns;
	int ImageProgressCheck;
	double SavingTime;

	int TotalProgress;
	int TotalProgressCheck;

	double remainingTime;
	bool timeCheck;
};

#endif

