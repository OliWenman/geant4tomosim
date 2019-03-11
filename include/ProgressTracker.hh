#ifndef ProgressTracker_h
#define ProgressTracker_h 1

#include <iostream>
#include "G4Timer.hh"
#include "globals.hh"

class ProgressTracker
{
    public:
        ProgressTracker();
        ~ProgressTracker();

        G4Timer Timer;  
    
        //Functions to do with the progress of the simulation
        void Setup(unsigned long long int nEvents, int nRuns){NumberOfEvents = nEvents; NumberOfRuns = nRuns;}
	    void PrintProgress(int CurrentEvent, int CurrentImage);
        void ResetEvents(){ImageProgressCheck = TotalProgressCheck = -1;}
        void SetSavingTime(double time){SavingTime = time;}
      
    private:
        void ProgressBar(int Percent);
	    void EstimatedTime(int Percent, int CurrentImage);
	    void PrintTime(double time);
        
		unsigned long long int NumberOfEvents;
		int NumberOfRuns;
		int ImageProgressCheck;
		double SavingTime;

		int TotalProgress;
		int TotalProgressCheck;

		double remainingTime;
		
		bool ShowProgressBar;
};

#endif

