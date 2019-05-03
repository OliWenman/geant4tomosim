#include "ProgressTracker.hh"
#include <iostream>     
#include <iomanip>      

ProgressTracker::ProgressTracker(): Timer()
{
    totalparticles = 0;
	totalprojections = 0;
	
	savingtime = 0;

	totalprogress = 0;
	totalprogressCheck = projectioncheck = -1;

	remainingTime = 0;
	
	singleline = false;
	print      = true; 
	graphicsOn = false;
	
	dprogress = 5;
}

ProgressTracker::~ProgressTracker()
{

}

void ProgressTracker::PrintProgress(int CurrentEvent, int currentprojection)
{
    //Checks if information needs to be printed
    if (print)
    {
        //Prints only at the start of the simulation if info on single line	    
	    if (currentprojection == 1 && CurrentEvent == 1)
	    {
	        if (singleline)
	        {	
	            G4cout << "\n\n\n\n" << std::flush;
	        }
	        else
	        {
	            G4cout << "\n" << std::flush;
	        }
        }
	    //Works out the percentage of how many events it has completed for this projection
        int projectionprogress = CurrentEvent*100./totalparticles;
    
	    //Only calculates the percentage if the image number has changed so not wasting time printing same number
    	if (projectionprogress != projectioncheck)
	    {	
		    //Calculates the total progress of the simulation				
		    totalprogress = 100.*(double(totalparticles*(currentprojection -1) + CurrentEvent)/(totalparticles*totalprojections));
		    
		    projectioncheck = projectionprogress;
    
            //Checks if the user wants the information printed on a single line or not
            if (singleline)
            {
		        //Prints above one line and over rides it
		        G4cout << "\033[2A" "\033[K" "\r" "Projection " << currentprojection << " of " << totalprojections << ": " << std::setw(3) << projectionprogress << "%\ complete ";
		    
		        //If the projection has finished, indicate it's saving the data
		        if      (projectionprogress == 100) {G4cout << "- Saving data...\n";}
		        else if (projectionprogress == 0)   {G4cout << "                \n";}
		        else                           {G4cout << "\n";}
    
                //Print the total progress and estimated time if it has changed
		        if(totalprogress != totalprogressCheck)
		        {	
		            G4cout << "\033[K" "\rTotal progress: " << std::setw(3) << totalprogress << "\%"; ProgressBar(totalprogress); G4cout << "\n" "\033[40C";
		    	    G4cout << "\rEstimated time remaining: ";
		    	    double etime = EstimatedTime(totalprogress, currentprojection);
		    	    if (etime == -1)
		    	    {
		    	        G4cout << "calculating...";
		    	    }
		    	    else
		    	    {
		    	        PrintTime(etime);
		    	    }
		    	    
		    	    totalprogressCheck = totalprogress;
		        }
		        else {G4cout << "\n" "\033[40C";}
	        }
	        else
	        {
	            //Prints the information on a new line. Useful if extra information needs to be printed during simulation
	            if(totalprogress != totalprogressCheck && projectionprogress % dprogress == 0)
		        {	
		            G4cout << currentprojection << " of " << totalprojections << ": " << std::setw(3) << projectionprogress << "\%";
		            G4cout << ", total progress: " << std::setw(3) << totalprogress << "%\ complete. ";
		            G4cout << "Estimated time left "; 
		            double etime = EstimatedTime(totalprogress, currentprojection);
		            if (etime == -1)
		    	    {
		    	        G4cout << "calculating...";
		    	    }
		    	    else
		    	    {
		    	        PrintTime(etime);
		    	    }
		            
		            totalprogressCheck = totalprogress;
		            
		            G4cout << "\n";
		        }
	        }
	        
	        G4cout << std::flush;
	    }
	}
}

void ProgressTracker::ProgressBar(int Percent)
{
	int intervals = 50;
	int dProgress = 102/intervals;

	G4cout << " (";
	for (int nbar = 0; nbar < intervals ; ++nbar)
	{
		if (nbar*dProgress < Percent) {G4cout << "|";}
		else                          {G4cout << " ";}
	}
	G4cout << ") ";
}

double ProgressTracker::EstimatedTime(int Percent, int currentprojection)
{
    //If the percent is 0, return -1 as will cause an error due to dividing by 0
    if (Percent != 0)
    {
		Timer.Stop();	
		
		float currentTime = Timer.GetRealElapsed();
		
		//Calculate the estimated remaining time left for the simulation
		remainingTime = ((currentTime*(100./Percent)) - currentTime) + (savingtime * (totalprojections - currentprojection));
	
	    return remainingTime;
    }
    else
    {
        return -1;
    }
}

void ProgressTracker::PrintTime(double time)
{
	//Prints out the sustiable units for the estimated time 
	if (time > 60)
	{
		if(time > 60*60) {G4cout << std::setw(4) << std::setprecision(3) << time/(60*60) << " hours    ";}
		else             {G4cout << std::setw(4) << std::setprecision(2) << time/60 << " minutes  ";}
	}
	else {G4cout << std::setw(4) << std::setprecision(2) << int(time) << " s        ";}
}
