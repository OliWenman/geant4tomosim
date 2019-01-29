#include "ProgressTracker.hh"
#include <iostream>     
#include <iomanip>      

ProgressTracker::ProgressTracker(unsigned long long int nEvents, int nRuns): Timer()
{
    NumberOfEvents = nEvents;
	NumberOfRuns = nRuns;

    CurrentEvent = 0;
	CurrentImage = 0;
	
	SavingTime = 0;

	TotalProgress = 0;
	TotalProgressCheck = ImageProgressCheck = -1;

	remainingTime = 0;
	timeCheck = false;
}

ProgressTracker::~ProgressTracker()
{

}

//Functions to do with the progress of the simulation
void ProgressTracker::PrintProgress(G4String Mode)
{
    ++CurrentEvent;

	//Works out the percentage of how many events it has completed
	int ImageProgress = (double(CurrentEvent)/NumberOfEvents)*100;

	//Prints only at the start of the simulation
	if(CurrentEvent == 1 && CurrentImage == 1)
	{	Timer.Start();	
		
		if (Mode == "Calibrating")
		{
		    G4cout << "\n================================================================================"
		              "\n                               Calibrating..."
	                  "\n================================================================================\n\n\n\n";
		}
		else if (Mode == "Simulating")
		{
		    G4cout << "\n================================================================================"
		              "\n                            SIMULATION RUNNING..."
	                  "\n================================================================================\n\n\n\n";
	     }
	}

	//Only prints the percentage if the image number has changed
	if (ImageProgress != ImageProgressCheck)
	{	
		//Calculates the total progress of the simulationEnergySigmaCmd
		int FullProgress = double(CurrentImage - 1)/NumberOfRuns*100;
		TotalProgress = FullProgress + (double(ImageProgress)/100 * (1./NumberOfRuns)*100);
		
		ImageProgressCheck = ImageProgress;

		//Prints above one line and over rides it
		G4cout << "\033[2A" "\033[K" "\rImage " << CurrentImage << ": " << std::setw(3) << ImageProgress << "%\ complete ";
		if (ImageProgress == 100)
		    G4cout << "| Saving data...\n";
		else if (ImageProgress == 0)
		    G4cout << "                \n";
		else 
		    G4cout << "\n";

		if(TotalProgress != TotalProgressCheck)
		{	G4cout << "\033[K" "\rTotal progress: " << std::setw(3) << TotalProgress << "\%"; ProgressBar(TotalProgress); G4cout << "\n" "\033[40C";
			EstimatedTime(TotalProgress);
		}

		G4cout << std::flush;
	}

	//Corrects the end perecentage to 100% once simulation is complete and outputs a space
	if(CurrentEvent == NumberOfEvents && CurrentImage == NumberOfRuns)
	{	G4cout << "\033[2A" "\033[K" "\rImage " << CurrentImage << ": " << "100%\ complete                  \n"  
                                              "\rTotal progress: 100\%"; ProgressBar(100); G4cout << "\n";
		EstimatedTime(100);
		G4cout << G4endl;
		Timer.Stop();
	}
}
void ProgressTracker::ProgressBar(int Percent)
{
    int intervals = 50;
	int dProgress = 100/intervals;

	G4cout << " (";
	for (int nbar = -1; nbar < intervals ; ++nbar)
	{
		if (nbar*dProgress < Percent)
			G4cout << "|";
		else
			G4cout << " ";
	}
	G4cout << ") ";
}
void ProgressTracker::EstimatedTime(int Percent)
{
    int interval = 2;

	if (Percent % interval == 0 && Percent != 0)
	{
		if (timeCheck == false){
			Timer.Stop();	

			remainingTime = ((Timer.GetRealElapsed()*(100./Percent)) - Timer.GetRealElapsed()) + (SavingTime * (NumberOfRuns - CurrentImage));
	
			PrintTime(remainingTime);

			timeCheck = true;}

	}
	else if (Percent % interval != 0 && Percent != 0)
	{	
		timeCheck = false;
	} 

	else
	{
		if (Percent < interval)
			G4cout << "\rEstimated time remaining: calculating... ";
	}
}

void ProgressTracker::PrintTime(double time)
{
    G4cout <<  "\rEstimated time remaining: ";

	//Prints out the sustiable units for the estimated time 
	if (time > 60)
	{
		if(time > 60*60)
			G4cout << std::setw(4) << std::setprecision(3) << time/(60*60) << " hours    ";
		else
			G4cout << std::setw(4) << std::setprecision(2) << time/60 << " minutes  ";
	}
	else
		G4cout << std::setw(4) << std::setprecision(2) << int(time) << " s        ";
}
