//Own classes
#include "Simulation.hh"
#include "SimulationMessenger.hh"
#include "SampleConstruction.hh"
#include "DetectorConstruction.hh"
#include "FluorescenceSD.hh"
#include "PhysicsList.hh"
#include "PrimaryGeneratorAction.hh"
#include "StackingAction.hh"
#include "SteppingAction.hh"
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
#include <cstdlib>
#include <ctime>
#include <sys/types.h>
#include <dirent.h>
#include <xraylib.h>
#include "PrintLines.hh"

Simulation::Simulation()
{

}

Simulation::Simulation(int verb, bool interactive) : runManager(0), DC(0), PL(0), PGA(0), visManager(0), particleManager(0), stepManager(0) 
{	
    globalVerbose = verb;
    interactiveOn = interactive;
    
    if (globalVerbose > 0)
    {
        G4cout << "\nWelcome to the tomography data simulation!"
                  "\nSetting up... " << G4endl;
    
        if (interactiveOn)
        {
            G4cout << "\nInteractivity is turned on. The simulation will be paused for inputs if there are any problems. ";
        }    
        else 
        {
            G4cout << "\nInteractivity is turned off. The simulation will be interupted if there are any problems.";
        }
    }
    
    SaveLogPath = "./../Output/HDF5/";	

    simMessenger = new SimulationMessenger(this);	
	materials    = new DefineMaterials(); 

    //Create the objects needed for the G4RunManager class	
	runManager = new G4RunManager();
	
  	DC = new DetectorConstruction(); 
  	runManager -> SetUserInitialization(DC); 
  	
	PL = new PhysicsList();              
	runManager -> SetUserInitialization(PL);
	
	PGA = new PrimaryGeneratorAction(); 
	runManager -> SetUserAction(PGA);
	
	particleManager = new StackingAction(); particleManager -> SetKillElectrons(true);
    runManager -> SetUserAction(particleManager);
    
	stepManager = new SteppingAction();
	runManager -> SetUserAction(stepManager);

	//Get the pointer to the User Interface manager, set all print info to 0 during events by default
  	UImanager = G4UImanager::GetUIpointer();
	UImanager -> ApplyCommand("/tracking/verbose 0");	//Gives information about particle
	UImanager -> ApplyCommand("/control/verbose 0");	
	UImanager -> ApplyCommand("/hits/verbose 0");
	UImanager -> ApplyCommand("/process/em/verbose 0");

    sampleconstruction = DC->GetSampleConstruction();

    //Set the seed engine
	CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());
	SetSeed(0);
}

Simulation::~Simulation()
{
    if (globalVerbose > 0) {G4cout << "\nClosing simulation..." << G4endl;}
	int showDeletion = 3;
	
	if(globalVerbose > showDeletion) {runManager -> SetVerboseLevel(showDeletion);}

    delete sampleconstruction; if(globalVerbose > showDeletion) {G4cout << "\nSampleConstrction deleted " << std::flush;}
	delete materials;          if(globalVerbose > showDeletion) {G4cout << "\nDefineMaterials deleted " << std::flush;}
	delete simMessenger;       if(globalVerbose > showDeletion) {G4cout << "\nSimulationMessenger deleted\n" << std::flush;}

    delete visManager; if(globalVerbose > showDeletion) {G4cout << "\nVisualizationManager deleted\n" << std::flush;}
    delete runManager; if(globalVerbose > showDeletion) {G4cout << "RunManager deleted" << std::flush;}
    
    delete CLHEP::HepRandom::getTheEngine();
	if (globalVerbose > 0) {G4cout << "\nSimulation closed! \n" << G4endl;}
}

#include "CommandStatus.hh"

void Simulation::addmacros_pywrapped(std::vector<std::string> macroFiles)
{
    int nFiles = macroFiles.size();
    
    for (int n = 0; n < nFiles ; n++)
    {    
        applymacrofile_pywrapped(macroFiles[n]);
    }
    
    macrofiles = macroFiles;
    
    G4cout << "\nCOMPLETE! " << G4endl;
}

void Simulation::applymacrofile_pywrapped(std::string macro)
{
    //Read the macro file line by line and extract the commands from the file

     if (globalVerbose > 0)
     {
        PrintToEndOfTerminal('-');        
        G4cout << "Reading file '" << macro << "' " << G4endl;
     }

    std::ifstream ReadFile;
        
    //Try to open the macro file      
    ReadFile.open(macro);
    if (!ReadFile) 
    {
        G4cout << "\nERROR: Unable to open " << macro << " to apply commands. " << G4endl;
        exit(1); 
    }
      
    G4cout << "\n";  
      
    //Read the file
    std::string command;    
    int line = 0;
    while ((std::getline(ReadFile, command))) 
    {
        ++line;
        // TAB-> ' ' conversion
        str_size nb = 0;
        while ((nb = command.find('\t', nb)) != G4String::npos) 
        {
            command.replace(nb, 1, " ");
        }
        
        //Skips empty lines
        if(command.size() == 0) continue;
        
        size_t foundhash = command.find('#');
            
        //If found '#' will remove it and everything after
        if(foundhash != std::string::npos)
        {
            command = command.substr(0, foundhash);
        }
            
        //Check if string isn't just spaces
        if (command.find_first_not_of(' ') != std::string::npos)
        {
            if (globalVerbose > 3) {G4cout << line << ")";}
            applycommand_pywrapped(command);
            
        }
    }
    
    ReadFile.close();
    if (globalVerbose > 0) {G4cout << "\n" << macro << " complete! "<< G4endl;}
}

#include "G4UIcommandTree.hh"
#include "G4UIcommand.hh"
#include "strfunctions.hh"

void Simulation::applycommand_pywrapped(std::string command)
{
    //Remove all spaces at the front of the parameters, at the back and any repeated spaces
    strfunctions::RemoveSpaces_repeated_front_back(command);

    //Apply the command
    G4int status = UImanager -> ApplyCommand(command);
    
    //Check the status of the command. If successful, continue with the simulation
    if (status == fCommandSucceeded)
    {
        if (globalVerbose > 3) {G4cout << command << " successful!" << G4endl;}
    }
    else
    {
        switch (status)
        {
            //Geant4 checks
            case fCommandNotFound:          G4cout << "ERROR: command \"" << command << "\" not found! " << G4endl;                     break;
            case fIllegalApplicationState:  G4cout << "ERROR: command \"" << command << "\" is applied at the wrong state! " << G4endl; break;
            case fParameterOutOfRange:      G4cout << "ERROR: command \"" << command << "\" parameter out of range! " << G4endl;        break;
            case fParameterUnreadable:      G4cout << "ERROR: command \"" << command << "\" parameter is unreadable! " << G4endl;       break;
            case fParameterOutOfCandidates: G4cout << "ERROR: command \"" << command << "\" parameter out of candidates! " << G4endl;   break;
            case fAliasNotFound:            G4cout << "ERROR: command \"" << command << "\" alias not found! " << G4endl;               break;
            //My checks
            case fNotEnoughParameters:      G4cout << "ERROR: command \"" << command << "\" incorrect parameters! " << G4endl;          break;
            case fIncorrectUnit:            G4cout << "ERROR: command \"" << command << "\" incorrect unit! " << G4endl;                break;
            case fParameterNotADouble:      G4cout << "ERROR: command \"" << command << "\" parameter not a double! " << G4endl;        break;
            case fParameterNotAInteger:     G4cout << "ERROR: command \"" << command << "\" parameter not a integer! " << G4endl;       break;
            case fFlagNotFound:             G4cout << "ERROR: command \"" << command << "\" flag not found! " << G4endl;                break;
            case fParameterNotFound:        G4cout << "ERROR: command \"" << command << "\" cannot find parameter! " << G4endl;         break;
            case fParameterAlreadyExists:   G4cout << "ERROR: command \"" << command << "\" parameter already exists! " << G4endl;      break;
            default:                        G4cout << "ERROR: command \"" << command << "\" error code : " << status << G4endl;         break;
        }
        
        //Remove the users input to get only the command path 
        auto commandpath = command.substr(0, command.find(' '));
        //Find the command pointer that corrosponds to the command path used        
        G4UIcommand* commandptr = UImanager->GetTree()->FindPath(commandpath.c_str());
        //If commandptr exists, print the guidance associated with that command to the user
        if (commandptr) {G4cout << "\nGuidance: \"" << commandptr->GetGuidanceLine(0) << "\"" << G4endl; }
        
        //If interactivtiy is on, give the user the chance to reapply the command
        if (interactiveOn)
        {
            bool breakLoop1 = false;
            G4cout << "\nWould you like to reapply the command?" "\n[y/n]" << G4endl;
        
            while (!breakLoop1)
            {
                std::string reapplycommand;
                getline(std::cin, reapplycommand);
        
                if (reapplycommand == "y")
                {
                    breakLoop1 = true;
                    G4cout << G4endl;
                    getline(std::cin, reapplycommand);
                
                    applycommand_pywrapped(reapplycommand);
                }
                else if (reapplycommand == "n")
                {
                    breakLoop1 = true;
                
                    bool breakLoop2 = false;
                    G4cout << "\nContinue with the simulation? ""\n[y/n]" << G4endl;
                
                    while (!breakLoop2)
                    {
                        std::string continueSimulation;
                        getline(std::cin, continueSimulation);
            
                        if (continueSimulation == "n")
                        {
                            breakLoop2 = true;
                            G4cout << "\nExiting..." << G4endl;
                            exit(0);
                        }
                        else if (continueSimulation == "y")
                        {
                            breakLoop2 = true;
                        }
                        else 
                        {
                            G4cout << "\nPlease type [y] or [n]. " << G4endl;
                        }
                    }
                }
                else 
                {
                    G4cout << "\nPlease type [y] or [n]. " << G4endl;
                }
            }
        }
        else
        {
            G4cout << "\nAborting simulation..." << G4endl;
            exit(0);
        }
    }
}

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4GeometryManager.hh"
#include "ProgressTracker.hh"

int Simulation::run_pywrapped(unsigned long long int n_particles, 
                              std::vector<int>       imageinfo, 
                              double                 rotation_angle)
{   
    //Get image parameters from the imageinfo
    int n_projection    = imageinfo[0];
    int nDarkFlatFields = imageinfo[1];
    int totalprojections  = imageinfo[2];
    
    //Set the seed for this image by calling the rand function (next number in the sequence)
	CLHEP::HepRandom::setTheSeed(rand());

    bool darkflatfields;
    if      (n_projection < totalprojections - nDarkFlatFields)  {darkflatfields = false;}
    else if (n_projection >= totalprojections - nDarkFlatFields) {darkflatfields = true;}
    
    if(n_projection == 0)
    {		  	    
	    int verbose;    
	    if (globalVerbose < 3){verbose = 3;}
	    else                  {verbose = globalVerbose - 3;}
		
		//Let the PrimaryGeneratorAction class know where to position the start of the beam
	    PGA -> DoAutoBeamPlacement(-DC->GetWorldSize().x());
		PGA -> SetNumberOfEvents(n_particles, totalprojections);
		
	    PL -> Fluorescence(); 
	 
	    runManager -> Initialize(); 
	    runManager -> SetNumberOfEventsToBeStored (0);
	    PL -> ActivateUserPhysics();
        Visualisation();
	    
	    if (globalVerbose > 0)
	    { 
            //Prints the time and date of the local time that the simulation started
		    time_t now = time(0);
		    //Convert now to tm struct for local timezone
		    tm* localtm = localtime(&now);
        
            PrintToEndOfTerminal('=');        
            G4cout << "STARTING SIMULATION... \n";
			          
	        G4cout << "\n" << asctime(localtm);
        }
        else 
        {
            PGA->GetProgressTracker().print = false;
        }
	}
    
    //Construct the samples. If darkflatfields are on, then it will remove the samples G4VPhysicalVolume.
    //After it has been constructed, if the G4VPhysicalVolume still exists, apply the needed transformations
    //to the sample such as rotation and translation
    sampleconstruction->Construct(darkflatfields);
    sampleconstruction->ApplyTransforms(rotation_angle, 0);

    //Prepare for next run. Check if energy or gun has changed 
    PGA -> ResetEvents(n_projection + 1);   
    
    Initialise_dataSets();
     
    //Beam on to start the simulation
    BeamOn(n_particles);

	if (n_projection + 1 == totalprojections)
	{
	    if (globalVerbose > 0)
	    {
            G4cout << "\n================================================================================"
                      "\n                        The simulation is finished! "
                      "\n================================================================================" << G4endl;
	    }
	    
	    //Reset needed variables for next run
        PGA -> ResetEvents(0);
        sampleconstruction->SetLastFullRotation(0);
	}

	return 0;
}

void Simulation::Visualisation()
{
	//Checks to see if visualization setting is turned on, if so a .heprep file will be outputted to be viewed in a HepRApp viewer
	if (DC -> GetVisualization() == true)
	{	
	    if (globalVerbose > 0)
	    {
            PrintToEndOfTerminal('-');
            G4cout << "GRAPHICS TURNED ON!";
        }
        
		visManager = new G4VisExecutive("quiet");
		visManager -> Initialize();
        //Make the name of the file the same as the logname, remove the "_log.txt" at the end and replace with _vis
        std::string visFileName = FileName.substr (0, FileName.length() - 8) + "_vis";    
        
        //Set the filepath to save it, if the defualt is used, save in the Visualization folder
        std::string filePath;
        std::string defaultPath = "./../Output/HDF5/";
        if (SaveLogPath == defaultPath){
            filePath = "./../Output/Visualization/";
        }
        else {
            //If a different path is used, create a folder to store the visualization data 
            filePath = SaveLogPath + "Visualization/";
            const char *filepath_char = filePath.c_str();
            
            //Check if the directory exists
            DIR* Directory_exists = opendir(filepath_char);
            
            //If it doesn't exist, create it
            if (!Directory_exists){
                std::string mkdir = "mkdir " + filePath;
                const char *mkdir_char = mkdir.c_str();
                system(mkdir_char);
            }
        }
		
		//Setup the vis manager
		UImanager -> ApplyCommand("/vis/open HepRepFile");
		UImanager -> ApplyCommand("/vis/heprep/setFileDir " + filePath);
		UImanager -> ApplyCommand("/vis/heprep/setFileName " + visFileName);
		UImanager -> ApplyCommand("/vis/viewer/set/autoRefresh false");
		//UImanager -> ApplyCommand("/vis/verbose errors");
		UImanager -> ApplyCommand("/vis/viewer/flush");
		UImanager -> ApplyCommand("/vis/drawVolume");
		
		//Set the draw options
		UImanager -> ApplyCommand("/vis/viewer/set/style wireframe");
		UImanager -> ApplyCommand("/vis/viewer/set/auxiliaryEdge true");
		UImanager -> ApplyCommand("/vis/viewer/set/lineSegmentsPerCircle 100");
		UImanager -> ApplyCommand("/vis/scene/add/trajectories smooth");
		UImanager -> ApplyCommand("/vis/modeling/trajectories/create/drawByCharge");
		UImanager -> ApplyCommand("/vis/modeling/trajectories/drawByCharge-0/default/setDrawStepPts true");
		UImanager -> ApplyCommand("/vis/modeling/trajectories/drawByCharge-0/default/setStepPtsSize 2");
		
		UImanager -> ApplyCommand("/vis/scene/add/axes 0 0 0 1 mm");
		UImanager -> ApplyCommand("/vis/viewer/zoom 0.5 mm");
		
		UImanager -> ApplyCommand("/vis/scene/add/hits");
		UImanager -> ApplyCommand("/vis/scene/endOfEventAction accumulate");
		UImanager -> ApplyCommand("/vis/viewer/set/autoRefresh true");
		
		if (globalVerbose > 0) {G4cout << "\nSaving as " << filePath + visFileName + "N.heprep where N is an integer "<< G4endl;}
			
		PGA->GetProgressTracker().singleline = false;
		PGA->GetProgressTracker().graphicsOn = true;
	}
}

//Function to run the simulation, will repeat simulation if over max limit
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

//Function to limit the number of particles if graphics is turned on
unsigned long long int Simulation::LimitGraphics(unsigned long long int nParticles, int nImage, std::string Mode)
{
    if (DC -> GetVisualization() == true && nParticles > 5000)
    {   
        int limit = 100;
        nParticles = limit;     
            
        if (nImage == 0)
        {
            G4cout << "\n////////////////////////////////////////////////////////////////////////////////\n"
                      "\n         WARNING: " << nParticles << " PARTICLES IS TOO MANY TOO SIMULATE WITH GRAPHICS"
                      "\n                    Reducing the number of particles to " << limit << "\n"
                      "\n////////////////////////////////////////////////////////////////////////////////" << G4endl;;         
        }
    }
    
    return nParticles;
}

//Function to log the inforimation about the simulation. Outputs to terminal depending on verbose level. Always outputs to _log.txt file
void Simulation::printinfo_pywrapped(unsigned long long int n_particles, int totalprojections, int nDarkFlatFields)
{   
    if (globalVerbose > 0)
    {
        std::ofstream SaveToFile;
    
        //Try to open the file 
        SaveToFile.open(SaveLogPath + FileName); 
        if( !SaveToFile ) { 	
            G4cout << "\nError: " << SaveLogPath + FileName << " file could not be opened from Simulation. " << "\n" << G4endl;
            exit(1);
        }
        
        //Create an instance of the log to output to terminal and file    
        SettingsLog log(SaveToFile);
        PrintInformation(log);

        double particleperpixel = n_particles/(DC->GetAbsorptionDetector()->GetNumberOfxPixels() * DC->GetAbsorptionDetector()->GetNumberOfyPixels());
	
	    PrintToEndOfTerminal(log, '-');
	    log << "META DATA: "
               "\n- The seed used: " << seedCmd
            << "\n- Total number of projections being processed: " << totalprojections
	        << "\n  - Dark fields: " << nDarkFlatFields
	        << "\n  - Sample: " << totalprojections - nDarkFlatFields
            << "\n- Number of photons per image: " << n_particles
            << "\n- Number of particles per pixel on average: " << particleperpixel;
	 
	    SaveToFile.close();
	    
	    CalculateStorageSpace(totalprojections);
    }
}

void Simulation::PrintInformation(SettingsLog log)
{       
    if (globalVerbose > 0)
    {
        log.terminalOn = false;
    
        //Loop through the macro files
        int nFiles = macrofiles.size();
        for (int n = 0 ; n < nFiles ; n++)
        {      
            std::ifstream ReadFile;
        
            PrintToEndOfTerminal(log, '-');
            //Try to open the macro file      
            ReadFile.open(macrofiles[n]);
            if (!ReadFile) {
                G4cout << "\nERROR: Unable to open " << macrofiles[n] << " for log output. " << G4endl;
                exit(1); 
            }
            log << "Macro file " << n + 1 << ": " << macrofiles[n] << "\n" << G4endl; 
      
            //Read the file
            std::string Line;    
            while ((std::getline(ReadFile, Line))) {
                //if (Line.find('#') == std::string::npos)
                if(Line[1] != '/') {log << Line << G4endl;}
            }
            ReadFile.close();
        }
    
        if (globalVerbose >= 1) {log.terminalOn = true;}
        else                    {log.terminalOn = false;}
    
        //Log the info from other classes
        DC -> ReadOutInfo(log);
	    PGA -> ReadOutInfo(log);
	    PL -> ReadOutInfo(log);
    
    }
}

#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4RegionStore.hh"
#include "G4GeometryManager.hh"

void Simulation::CleanGeometry()
{
    //Clean old geometry if any
    G4GeometryManager::GetInstance()->OpenGeometry();
    sampleconstruction->Reset();
    
	G4PhysicalVolumeStore::GetInstance()->Clean(); 
    G4LogicalVolumeStore::GetInstance()->Clean(); 
    G4RegionStore::GetInstance()->Clean();
    G4GeometryManager::GetInstance()->CloseGeometry();
    
    //The geometry needs to be Reinitialised for the next run
	runManager -> ReinitializeGeometry();
}

void Simulation::CalculateStorageSpace(int projections)
{
    if (globalVerbose > 0)
    {
        double totalStorage = 0;
        std::string unit;
    
        int abs_xPixels = getNumAbsXpixels_pywrapped();
        int abs_yPixels = getNumAbsYpixels_pywrapped();
        int bins = getNumFluorbins_pywrapped();
    
        PrintToEndOfTerminal('-');
        G4cout << "DISK SPACE REQUIRED FOR DATA: ";
    
        //Absorption
        size_t absorpDataSize = sizeof(getAbsorption_pywrapped()[0]);
        double absorpStorageSpace = abs_xPixels * abs_yPixels * absorpDataSize * projections;
        totalStorage += absorpStorageSpace;
        unit = GetStorageUnit(absorpStorageSpace);
        G4cout << "\n- Absorption: " << absorpStorageSpace << unit;
    
        //Beam energy
        size_t beam_ByteTypeSize = PGA -> GetBeamEnergyByteTypeSize();
        double beamStorageSpace = bins * beam_ByteTypeSize * projections;
        totalStorage += beamStorageSpace;
        unit = GetStorageUnit(beamStorageSpace);
        G4cout << "\n- Beam energy: " << beamStorageSpace << unit;
    
        //Fluorecence
        if (fluorFMactive_pywrapped())
        {
            size_t FMfluoresenceDataSize = sizeof(getFluoreEneBins_pywrapped()[0]);  
            double fluorescenceStorageSpace = absorpStorageSpace*getNumFluorbins_pywrapped();
            totalStorage += fluorescenceStorageSpace;
            unit = GetStorageUnit(fluorescenceStorageSpace);              
            G4cout << "\n- Full mapping fluorescence: " << fluorescenceStorageSpace << unit;
        }
    
        /*//Diffraction
        if (data->DoFullMappingDiffraction() )
        {
            size_t DiffractionDataSize = data -> GetDiffractionSizeType();
            double diffractionStorageSpace  = double(abs_xPixels) * double(abs_yPixels) *abs_xPixels * abs_yPixels * DiffractionDataSize * projections;
            totalStorage += diffractionStorageSpace;
            unit = GetStorageUnit(diffractionStorageSpace);   
            G4cout << "\n- Full mapping diffraction: " << diffractionStorageSpace << unit;
        }*/
    
        unit = GetStorageUnit(totalStorage);
        G4cout << "\n- Total: " << totalStorage << unit << std::flush;
    
        PrintToEndOfTerminal('-');
    }
}

std::string Simulation::GetStorageUnit(double &storage)
{
    double peta = 1.e15;
    double tera = 1.e12;
    double giga = 1.e9;
    double mega = 1.e6;
    double kilo = 1.e3;
    double byte = 1.;

    if (storage < kilo)
    {
        if (storage > kilo)
        {
            if (storage > mega)
            {
                if (storage > giga)
                {
                    if(storage > tera)
                    { 
                        if(storage > peta)
                        {
                            storage = storage/peta;
                            return " PB";
                        }
                        storage = storage/tera;
                        return " TB";
                    }
                    storage = storage/giga;
                    return " GB";
                }
                storage = storage/mega;
                return " MB";
            }
            storage = storage/kilo;
            return " KB";
        }
        storage = storage/byte;
        return " Bytes";
    }              
}

void Simulation::SetSeed(long int seedinput)
{
    seedCmd = seedinput;
    
    if (seedinput == 0)
    {
        //Set random seed sequence with system time
        seedCmd = time(0);
        srand(seedCmd);	 
        randseed = true;
    }
    else
    {       
        //Set the seed sequence using inputted seed
        srand(seedCmd);	 
        randseed = false;
    }   
}

void Simulation::Initialise_dataSets()
{
    if (globalVerbose > 2) {G4cout << "\nInitialising data... " << G4endl;}
    DC->GetAbsorptionDetector()->GetSensitiveDetector()->InitialiseData();
    DC->GetFluorescenceDetector()->GetSensitiveDetector()->InitialiseData();
    PGA->SetupData();
}
