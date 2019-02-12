//Own classes
#include "Simulation.hh"
#include "SimulationMessenger.hh"
#include "DetectorConstruction.hh"
#include "TargetConstruction.hh"
#include "FluorescenceSD.hh"
#include "PhysicsList.hh"
#include "PrimaryGeneratorAction.hh"
#include "StackingAction.hh"
#include "Data.hh"
#include "DefineMaterials.hh"
//Output to console/write to file
#include "SettingsLog.hh"

//Geant4 managers for running the simulation
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"

//Used for timing simulation and getting a random seed
#include "G4Timer.hh"
#include "time.h"
#include "Randomize.hh"

//Geant4 units
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4ThreeVector.hh"

#include <list>

Simulation::Simulation()
{	
	simMessenger = new SimulationMessenger(this);

	Reset = false;
	Ready = false;

	WriteToTextCmd = false;
	verboseLevel = 1;
	
	//SimMode = "Calibrating";

	G4cout << "\nWelcome to the tomography data simulation!\n"; 

	Setup();
}

Simulation::~Simulation()
{
	G4cout << "\nClosing simulation...";

	delete simMessenger;

	delete runManager;
	delete visManager;

	delete data;
	delete materials;

	G4cout << "\nSimulation closed! \n" << G4endl;
}

void Simulation::Setup()
{ 
	G4cout << "Setting up... \n";

	//Create an instance of the classes
	runManager = new G4RunManager();
	//runManager -> SetVerboseLevel(1);
	data = new Data();
	DC = new DetectorConstruction(data); 
	PL = new PhysicsList();
	materials = new DefineMaterials(); 
	visManager = 0;
	SaveLogPath = "./../Output/HDF5/";

	//Setup the Geant4 user and action intialization	
	runManager -> SetUserInitialization(DC);
	runManager -> SetUserInitialization(PL);

	PGA = new PrimaryGeneratorAction(data);
	runManager -> SetUserAction(PGA);
	
	particleManager = new StackingAction();
	runManager -> SetUserAction(particleManager);
	particleManager -> SetKillElectrons(true);

	//Get the pointer to the User Interface manager, set all print info to 0 during events by default
  	UImanager = G4UImanager::GetUIpointer();
	UImanager -> ApplyCommand("/tracking/verbose 0");	//Gives information about particle
	UImanager -> ApplyCommand("/control/verbose 0");	
	UImanager -> ApplyCommand("/hits/erbose 0");
	UImanager -> ApplyCommand("/process/em/verbose 0");

	CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());
}

void Simulation::pyOutputOptions(bool FFF, bool FFM, bool BE)
{
	//Tell the PrimaryGeneratorAction class what data to record
	PGA -> SetBeamData(BE);
	PGA -> SetFluoreFM(FFM);

	//Tell the Data class what data to record
	data -> SetFFF(FFF);
	data -> SetFFM(FFM);
	data -> SetBE(BE);

	//Tell the FluorescencSD class what data to record
	FluorescenceSD* fluorescenceDetector = DC -> GetFluoreDetector();
	
	//Checks to see if it exists first
	if(fluorescenceDetector)
	{	fluorescenceDetector -> SetFFF(FFF);
		fluorescenceDetector -> SetFFM(FFM);
	}
}

void Simulation::pyInitialise(int nDetectorsY, int nDetectorsZ, std::vector<double> DetDimensions, int nBins)
{
	PathToScripts = "./../scripts/";

	G4ThreeVector halfDimensions = G4ThreeVector(DetDimensions[0], DetDimensions[1], DetDimensions[2]);

	DC -> SetNoDetectorsY(nDetectorsY);
	DC -> SetNoDetectorsZ(nDetectorsZ);
	DC -> SetDetectorSize(G4ThreeVector(DetDimensions[0], DetDimensions[1], DetDimensions[2])); 

	if (nBins > 0)
		{DC -> SetFluorescenceDet(true);}
	else
		{DC -> SetFluorescenceDet(false);}

	data -> SetNoBins(nBins);
	data -> SetHalfDetectorDimensions(halfDimensions);	

    //Apply the commands from the macro files to fill the values
	G4cout << "\nReading Geometry.mac... ";
	
	//UImanager -> ApplyCommand("/control/verbose 2");
	UImanager -> ApplyCommand("/control/execute " + PathToScripts + "Geometry.mac");
	
    //UImanager -> ApplyCommand("/control/verbose 0");
	G4cout << "\nReading pySettings.mac..." << G4endl;
	UImanager -> ApplyCommand("/control/execute " + PathToScripts + "pySettings.mac");

    //Keeps the seed (broken)?
	if (seedCmd != 0)	
		{CLHEP::HepRandom::setTheSeed(seedCmd);}

	//Random seed
	else if (seedCmd == 0)	
	{
		//set random seed with system time
		seedCmd = time(NULL);
		CLHEP::HepRandom::setTheSeed(seedCmd);
	}

	//Tell the data class what the max energy is
	data -> SetMaxEnergy(PGA -> GetMaxEnergy());

    G4cout << "\nCommands successfully added\n"
	          "\nSimulation Ready!" << G4endl;

	Ready = true;
	
	runManager -> Initialize();
}

void Simulation::pyDataPaths(G4String settingsPath, G4String geometryPath, G4String h5OutputPath)
{
	G4cout << "\nsettingsPath = " << settingsPath;
}

std::vector<int> Simulation::pyRun(unsigned long long int TotalParticles, double TotalAngle, int Image, int NumberOfImages, std::string Mode)
{
    TotalParticles = LimitGraphics(TotalParticles, Image, Mode);

	//Checks to see if simulation is ready (if pyInitialise has been used before)
	if (Ready == true)
	{
        if(Image == 0)
		{		
		    DC -> RelayToTC(NumberOfImages, TotalAngle);
			PGA -> SetNumberOfEvents(TotalParticles, NumberOfImages);
			
			DC -> GetTargetConstruction() -> SetSimMode(Mode);
		    PGA -> SetSimMode(Mode);
			
		    if (Mode == "Calibrating")
	        {    
	            OutInfo(verboseLevel);
	     
                //Prints the time and date of the local time that the simulation started
			    time_t now = time(0);
			    //Convert now to tm struct for local timezone
			    tm* localtm = localtime(&now);

                Visualisation();

			    G4cout << "\n--------------------------------------------------------------------"
			              "\nStarting simulation... \n";
			          
			    G4cout << "\n" << asctime(localtm);
		    } 
		    else if (Mode == "Simulating")
		    {
		        //Open the file within the path set
		        std::ofstream SaveToFile;
                SaveToFile.open(SaveLogPath, std::fstream::app); 
   	
                //Output error if can't open file
                if( !SaveToFile ){ 	
                    std::cerr << "\nError: " << SaveLogPath << " file could not be opened from Simulation.\n" << std::endl;
              	    exit(1);
           	    }
    
                SettingsLog log(SaveToFile);

			    log << "\n--------------------------------------------------------------------"
			           "\nMETA DATA: \n"

			        << "\n- The seed used: " << seedCmd
			        << "\n- Number of projections being processed: " << NumberOfImages
                    << "\n- Number of photons per image: " << TotalParticles
                    << "\n- Number of particles per detector on average: " << TotalParticles/(DC -> GetNoDetectorsY() * DC -> GetNoDetectorsZ())
			        << "\n- Full rotation angle: " << G4BestUnit(TotalAngle, "Angle") << G4endl;
                    
                SaveToFile.close();
		    }
		    
		    if (TotalParticles > 0){
			    G4cout << "\n================================================================================"
		                  "\n                                 Geant4 info"
	                      "\n================================================================================" << G4endl;
	        }
		}

        PGA -> ResetEvents(Image + 1);

		//Creates the arrays for the data, wipes them after each image
		data -> SetUpData(DC -> GetNoDetectorsY(), DC -> GetNoDetectorsZ(), Image);
		
		if (NumberOfImages != 0 || TotalParticles != 0)
		{   //Beam on to start the simulation
		    BeamOn(TotalParticles);
		}
		
		//Prepare for next run that geometry has changed
		runManager -> ReinitializeGeometry();

		if (WriteToTextCmd == true)
			{data -> WriteToTextFile(Image);}

		if (Image == 0 && Mode == "Simulating")
			{PGA -> SetBeamCheck(true);}

		if (Image + 1 == NumberOfImages)
		{
		    if (Mode == "Simulating" && NumberOfImages != 0)
		    {
			    G4cout << "\n================================================================================"
	                      "\n                        The simulation is finished! "
	                      "\n================================================================================" << G4endl;
	            
	            TargetConstruction* TC = DC -> GetTargetConstruction();         
	            TC -> SetCurrentImage(0);
	            DC -> SetCurrentImage(0);
	            PGA -> ResetEvents(0);
	        }
	        else if (Mode == "Calibrating")
	        {
	            G4cout << "\n================================================================================"
	                      "\n                 Calibration finished, simulating sample... "
	                      "\n================================================================================" << G4endl;
	        }
		}
	
		return data -> GetHitData();
	}
	
	else if (Ready == false){
		G4cout << "\nSIMULATION IS NOT READY! Check the macro files and initialize the simulation first! \n";
	}
}

std::vector<int> Simulation::pyRunTest(unsigned long long int TotalParticles, int NumberOfImages, double rotation_angle, int Image, int nDarkFlatFields)
{   
    //runManager -> Initialize();

    G4String Mode;
    if (Image < NumberOfImages - nDarkFlatFields){
        Mode = "Simulating";
    }
    else if (Image >= NumberOfImages - nDarkFlatFields){
        Mode = "Calibrating";
    }
 
    TargetConstruction* TC = DC -> GetTargetConstruction();
    TC -> SetSimMode(Mode);
    TC -> SetRotationAngle(rotation_angle);
    
    //PGA -> SetParticleEnergy(energy);
       
	//Checks to see if simulation is ready (if pyInitialise has been used before)
	if (Ready == true)
	{
        if(Image == 0)
		{		
		    //Let the PrimaryGeneratorAction class know where to position the start of the beam
	        PGA-> SetValues(data -> GetNoBins(), DC -> GetWorldSize().x());
		
		    DC -> RelayToTC(NumberOfImages, rotation_angle);
			PGA -> SetNumberOfEvents(TotalParticles, NumberOfImages);
			
		    PGA -> SetSimMode(Mode);
			
	        OutInfo(verboseLevel);
	     
            //Prints the time and date of the local time that the simulation started
			time_t now = time(0);
			//Convert now to tm struct for local timezone
			tm* localtm = localtime(&now);

            Visualisation();
		 
		    //Open the file within the path set
		    std::ofstream SaveToFile;
            SaveToFile.open(SaveLogPath, std::fstream::app); 
   	
            //Output error if can't open file
            if( !SaveToFile ){ 	
                std::cerr << "\nError: " << SaveLogPath << " file could not be opened from Simulation.\n" << std::endl;
              	exit(1);
           	}
    
            SettingsLog log(SaveToFile);

			log << "\n--------------------------------------------------------------------"
			       "\nMETA DATA: \n"

			    << "\n- The seed used: " << seedCmd
			    << "\n- Total number of projections being processed: " << NumberOfImages
			    << "\n  - Dark fields: " << nDarkFlatFields
			    << "\n  - Sample: " << NumberOfImages - nDarkFlatFields
                << "\n- Number of photons per image: " << TotalParticles
                << "\n- Number of particles per detector on average: " << TotalParticles/(DC -> GetNoDetectorsY() * DC -> GetNoDetectorsZ()) << G4endl;
                    
            SaveToFile.close();
                
            G4cout << "\n--------------------------------------------------------------------"
			          "\nStarting simulation... \n";
			          
			G4cout << "\n" << asctime(localtm);
		    
		    if (TotalParticles > 0){
			    G4cout << "\n================================================================================"
		                  "\n                                 Geant4 info"
	                      "\n================================================================================" << G4endl;
	        }
		}

        //Prepare for next run that geometry has changed
		runManager -> ReinitializeGeometry();

        PGA -> ResetEvents(Image + 1);

		//Creates the arrays for the data, wipes them after each image
		data -> SetUpData(DC -> GetNoDetectorsY(), DC -> GetNoDetectorsZ(), Image);
		
	    //Beam on to start the simulation
	    BeamOn(TotalParticles);
		
		if (WriteToTextCmd == true){
		    data -> WriteToTextFile(Image);
		}
	    
	    if (Image == 0){
			PGA -> SetBeamCheck(true);}

		if (Image + 1 == NumberOfImages)
		{
	        G4cout << "\n================================================================================"
	                  "\n                        The simulation is finished! "
	                  "\n================================================================================" << G4endl;
	            
	        TargetConstruction* TC = DC -> GetTargetConstruction();         
	        TC -> SetCurrentImage(0);
	        DC -> SetCurrentImage(0);
	        PGA -> ResetEvents(0);
		}
	
		return data -> GetHitData();
	}
	
	else if (Ready == false){
		G4cout << "\nSIMULATION IS NOT READY! Check the macro files and initialize the simulation first! \n";
	}
}

void Simulation::Visualisation()
{
	//Checks to see if visualization setting is turned on, if so a .heprep file will be outputted to be viewed in a HepRApp viewer
	if (DC -> GetVisualization() == true)
	{	
		visManager = new G4VisExecutive();

		visManager -> Initialize();
		UImanager -> ApplyCommand("/control/execute ./../scripts/MyVis.mac");
		
		PGA -> SetProgressBar(false);
	}
}

//Functions to be wrapped by Cython
std::vector<int> Simulation::GetLastImage(){
	return data -> GetHitData();
}

std::vector<int> Simulation::GetEnergyFreq(){
	return data -> GetEnergyFreq();
}

std::vector<double> Simulation::GetEnergyBins(){
	return data -> GetEnergyBins();
}

std::vector<int> Simulation::GetBeamEnergyFreq(){
	return PGA -> GetBeamEnergyFreq();
}

std::vector<std::vector<std::vector<int> > > Simulation::GetFullMapping(){
	return data -> GetFullMapping();
}

int Simulation::GetNumberCalibrations(){
    TargetConstruction* TempTC = DC -> GetTargetConstruction();
    return TempTC -> GetCalibrationImages();
}

void Simulation::SetSavingTime(double Time){
    PGA -> SetSavingTime(Time);
}

//Private functions
void Simulation::BeamOn(unsigned long long int nParticles)
{
	//Max limit for an integer (for Geant4 BeamOn), must be unsigned long long int as well to compare it if input is above limit.
	unsigned long long int limit = 2147483647;

	//Checks to see if the input is above the limit
	if (nParticles > limit)
	{
		//Rounds up to the nearest number to do the correct number of loops
		int NumberOfLoops = std::ceil(nParticles/limit);

		for (int loop = 1 ; loop <= NumberOfLoops ; loop++)
		{
			if (nParticles > limit)
			{	//Beam on to start the simulation with the max number limit
				runManager -> BeamOn(limit);

				//Subtract from the true number of inputted particles until it is within the limit
				nParticles = nParticles - limit;
			}
			else 
			{	//Once within the limit, fire the remaining particles that are left 
				runManager -> BeamOn(nParticles);
			}	
		}
	}
	else 
	{	//If the inputted number of particles is within the limit, fire that number of particles
		runManager -> BeamOn(nParticles);
	}
}

unsigned long long int Simulation::LimitGraphics(unsigned long long int nParticles, int nImage, std::string Mode)
{
    if (DC -> GetVisualization() == true && nParticles > 5000)
    {   
        if (nImage == 0)
        {
            G4cout << "\n////////////////////////////////////////////////////////////////////////////////\n"
                      "\n         WARNING: " << nParticles << " PARTICLES IS TOO MANY TOO SIMULATE WITH GRAPHICS"
                      "\n                    Reducing the number of particles to 1\n"
                      "\n////////////////////////////////////////////////////////////////////////////////" << G4endl;;         
        }
        nParticles = 1;     
    }
    
    return nParticles;
}

void Simulation::OutInfo(int verbose)
{
    //UImanager -> ApplyCommand("/control/verbose 0");
	        
	std::string FileName = "SimulationLog.txt";
	SaveLogPath = SaveLogPath + FileName;
	     
	//Open the file within the path set   
    std::ofstream SaveToFile;
    SaveToFile.open(SaveLogPath); 
    if( !SaveToFile ) 
    { 	std::cerr << "\nError: " << SaveLogPath << " file could not be opened from Simulation. " << "\n" << std::endl;
        exit(1);
    }
    
    SettingsLog log(SaveToFile);
    
    //If the verbose has been set >= 2 output info to terminal. Will always output to textfile though
    if (verbose >= 2){
        log.terminalOn = true;
    }
    else{
        log.terminalOn = false;
    }
   
	log << "\n--------------------------------------------------------------------"
           "\nSETTINGS COMMANDS USED: \n" << G4endl;

    std::ifstream ReadFile;  
    G4String ScriptName = "pySettings.mac";
    ReadFile.open(PathToScripts+ScriptName);
    if (!ReadFile) 
    {
        G4cout << "\nERROR: Unable to open " << PathToScripts+ScriptName << " for log output. " << G4endl;
        exit(1); 
    }
             
    std::string Line;    
    while ((std::getline(ReadFile, Line))) 
    {
        //if (Line.find('#') == std::string::npos)
        if(Line[1] != '/'){
            log << Line << G4endl;}
    }
    ReadFile.close();
                
    log << "\n--------------------------------------------------------------------"            
        << "\nGEOMETRY COMMANDS USED: \n" << G4endl;
                
     ScriptName = "Geometry.mac";
     ReadFile.open(PathToScripts+ScriptName);
     if (!ReadFile) 
     {
         G4cout << "\nERROR: Unable to open " << PathToScripts+ScriptName << " for log output. " << G4endl;
         exit(1); 
     }
            
     while ((std::getline(ReadFile, Line))) 
     {
        //if (Line.find("#/") == std::string::npos)
        if(Line[1] != '/'){
            log << Line << G4endl;}        
     }                
     log << G4endl;

     ReadFile.close();
     
     //If the verbose has been set >= 1 output info to terminal. Will always output to textfile though
     if (verbose >= 1){
        log.terminalOn = true;
     }
     else {
        log.terminalOn = false;
     }
     
	 DC -> ReadOutInfo(log);
	 PGA -> ReadOutInfo(log);
	 PL -> ReadOutInfo(log);
	 
	 SaveToFile.close();
}
