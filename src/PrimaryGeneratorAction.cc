//Own classes
#include "Data.hh"
#include "PrimaryGeneratorAction.hh"
#include "PrimaryGeneratorActionMessenger.hh"
#include "G4GeneralParticleSource.hh"
//Output to console/write to file
#include "SettingsLog.hh"

//Used to create an event to fire the particles
#include "G4Event.hh"
#include "G4ParticleGun.hh"

//Used to define particles
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

//Geant4 Units
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4TwoVector.hh"

//Randomize position of the beam
#include "Randomize.hh"

//To access GPS varabiles
#include "G4SPSPosDistribution.hh"
#include "G4SPSAngDistribution.hh"
#include "G4SPSEneDistribution.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction(Data* DataObject):G4VUserPrimaryGeneratorAction(), data(DataObject), Timer()//, ProgressTracker()
{
	//Create a messenger for this class
  	gunMessenger = new PrimaryGeneratorActionMessenger(this);

	BeamCheck = false;
	BeamData = false;
	timeCheck = false;
	
	ShowProgressBar = true;
	
	//Defualt values
	EnergyDistTypeCmd = "Mono";
	energyCmd = 30*keV;
	EnergySigmaCmd = 0*keV;
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  	//delete ParticleGun;
	delete gunMessenger;
	delete ParticleGun;
	delete fastParticleGun;

	//G4RunManager will delete gamma
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    //Prints only at the start of the simulation
	if(CurrentEvent == 1 && CurrentImage == 1)
	{	Timer.Start();	
		
		if (SimMode == "Calibrating")
		{
		    G4cout << "\n================================================================================"
		              "\n                               Calibrating..."
	                  "\n================================================================================\n\n\n\n";
		}
		else if (SimMode == "Simulating")
		{
		    G4cout << "\n================================================================================"
		              "\n                            SIMULATION RUNNING..."
	                  "\n================================================================================\n\n\n\n";
	     }
	}

    if (EnergyDistTypeCmd == "Mono")
    {
        //Allow the particles to be fired randomly within the beam width
      	G4double y0 = BeamHeightZ_Cmd*2 * (G4UniformRand()-0.5);
      	G4double z0 = BeamWidthY_Cmd*2 * (G4UniformRand()-0.5);

        //Set the ParticleGun conditions
        fastParticleGun -> SetParticlePosition(G4ThreeVector(StartingPosition, y0, z0));
        	
        //Generate the particle in the event
      	fastParticleGun -> GeneratePrimaryVertex(anEvent);
      	
      	if (FluoreFM == true && SimMode == "Simulating")
        		data -> SetParticlePosition(G4ThreeVector(StartingPosition, y0, z0));
    }
	else
    {
        //Generate the particle in the event
        ParticleGun -> GeneratePrimaryVertex(anEvent);
         
        if (FluoreFM == true && SimMode == "Simulating")
       		data -> SetParticlePosition(ParticleGun -> GetParticlePosition());
    }

	if (CurrentImage == 1 && BeamData == true && SimMode == "Simulating")
	{	//Save beam energy data for the first image
       	int bin;
        	
        if (EnergyDistTypeCmd == "Mono")
             bin = floor(energyCmd*1000/(eMax/Bins)) -1;
        else
             bin = floor(ParticleGun -> GetParticleEnergy()*1000/(eMax/Bins)) - 1;
		
		if (bin < 0)
       		bin = 0;
        else if (bin > BeamEnergyFreq.size())
            bin = BeamEnergyFreq.size();

		++BeamEnergyFreq[bin];
	}
    
    ++CurrentEvent;
    
    if (ShowProgressBar == true && CurrentEvent > 1)
        PrintProgress();
} 

void PrimaryGeneratorAction::ReadOutInfo(G4String SaveFilePath)
{
	G4ThreeVector BeamDimensions = G4ThreeVector(0, BeamWidthY_Cmd, BeamHeightZ_Cmd);

    std::ofstream SaveToFile;
    
    //Open the file within the path set
	SaveToFile.open(SaveFilePath, std::fstream::app); 
   	
	//Output error if can't open file
	if( !SaveToFile ) 
	{ 	std::cerr << "\nError: " << SaveFilePath << " file could not be opened from PrimaryGeneratorAction.\n" << std::endl;
      	exit(1);
   	}
    
    SettingsLog log(SaveToFile, G4cout);

	log << "\n--------------------------------------------------------------------"
	       "\nBEAM INFORMATION: \n"
		      
           "\n- Beam type: " << EnergyDistTypeCmd;
    if (EnergyDistTypeCmd == "Gauss")
    {
        log << "\n- Mean energy: " << G4BestUnit(energyCmd, "Energy")
            << "\n- Sigma of the distrbution: " << G4BestUnit(EnergySigmaCmd, "Energy");
    }
    else
        log << "\n- Energy of the monochomatic beam: " << G4BestUnit(energyCmd, "Energy");
	       
	log << "\n- Half beam dimensions: " << G4BestUnit(BeamDimensions, "Length") << G4endl;

    SaveToFile.close();
    
}

void PrimaryGeneratorAction::SetValues(int nBins, double Position)
{
    //Setup which particle is used and its starting conidiions
   gamma = G4ParticleTable::GetParticleTable() -> FindParticle("gamma");
   
   //A list of available options for a user to pick from
   std::vector<G4String> EDistTypeOptions = {"Mono", "Mono(GPS)", "Gauss"};

    //If option picked is "Mono"
    if (EnergyDistTypeCmd == EDistTypeOptions[0])
    {
        fastParticleGun = new G4ParticleGun(1);
        
        fastParticleGun -> SetParticleDefinition(gamma);
        
        fastParticleGun -> SetParticleMomentumDirection(G4ThreeVector(1, 0, 0));
        
        fastParticleGun -> SetParticleEnergy(energyCmd);
        
        EnergySigmaCmd = 0*keV;
        
        StartingPosition = -Position;
    }
    //If option picked is "Mono(GPS)" or "Gauss"
    else if (EnergyDistTypeCmd == EDistTypeOptions[1] || EnergyDistTypeCmd == EDistTypeOptions[2] )
    {
        ParticleGun = new G4GeneralParticleSource();

        ParticleGun -> SetParticleDefinition(gamma);

        //Automatically set the gun position to the end of the world volume
        ParticleGun -> GetCurrentSource() -> GetPosDist() -> SetCentreCoords(G4ThreeVector(-Position, 0, 0));
        ParticleGun -> GetCurrentSource() -> GetPosDist() -> SetHalfX(BeamWidthY_Cmd);
        ParticleGun -> GetCurrentSource() -> GetPosDist() -> SetHalfY(BeamHeightZ_Cmd);
        //Automatically set the correct rotation
        ParticleGun -> GetCurrentSource() -> GetPosDist() -> SetPosRot1(G4ThreeVector(0, 0, 1));
        ParticleGun -> GetCurrentSource() -> GetPosDist() -> SetPosRot2(G4ThreeVector(0, 1, 0));
        //Automatically select the shape and type
        ParticleGun -> GetCurrentSource() -> GetPosDist() -> SetPosDisType("Plane");
        ParticleGun -> GetCurrentSource() -> GetPosDist() -> SetPosDisShape("Square");

        //Automatically set the gun momentum in the positive x direction
        ParticleGun -> GetCurrentSource() -> GetAngDist() -> SetParticleMomentumDirection(G4ThreeVector(1, 0, 0));

        if (EnergyDistTypeCmd == EDistTypeOptions[1])
        {
            ParticleGun -> GetCurrentSource() -> GetEneDist() -> SetEnergyDisType("Mono");
            EnergySigmaCmd = 0*keV;
        }
        else 
            ParticleGun -> GetCurrentSource() -> GetEneDist() -> SetEnergyDisType("Gauss");
        
        ParticleGun -> GetCurrentSource() -> GetEneDist() -> SetMonoEnergy(energyCmd);
        ParticleGun -> GetCurrentSource() -> GetEneDist() -> SetBeamSigmaInE(EnergySigmaCmd);
    }
    else
    {
        G4cout << "\nYou have not selected an available option for the command \"/beam/energy/type\". You can pick from: \n";
        for (int ele = 0; ele < EDistTypeOptions.size(); ele++)
            G4cout << EDistTypeOptions[ele] << " ";
        G4cout << G4endl;
        exit(0);
    }
    
    	//Set the max energy value (in keV)
    eMax = (energyCmd + EnergySigmaCmd)*2*1000;

    std::vector<int> iBeamEnergyFreq(nBins, 0);
   	BeamEnergyFreq = iBeamEnergyFreq;

    Bins = nBins;
}

//========================================================================================================================
//Functions to do with printing the progress of the simulation

void PrimaryGeneratorAction::PrintProgress()
{
	//Works out the percentage of how many events it has completed
	int ImageProgress = (double(CurrentEvent)/NumberOfEvents)*100;

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
		    G4cout << "- Saving data...\n";
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

void PrimaryGeneratorAction::ProgressBar(int Percent)
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

void PrimaryGeneratorAction::EstimatedTime(int Percent)
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

void PrimaryGeneratorAction::PrintTime(double time)
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
