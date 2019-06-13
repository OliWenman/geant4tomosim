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

#include "G4UIcommandTree.hh"
#include "G4UIcommand.hh"

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

//#include "G4MPImanager.hh"
//#include "G4MPIsession.hh"


Simulation::Simulation(int verb, bool interactive) : runManager(0), 
                                                     detectorManager(0), 
                                                     physicsManager(0), 
                                                     beamManager(0), 
                                                     visManager(0), 
                                                     particleManager(0), 
                                                     stepManager(0)//,
                                                     //g4MPI(0)
                                                     
{
    globalVerbose = verb;
    interactiveOn = interactive;
    
    simMessenger = new SimulationMessenger(this);	
	materials    = new DefineMaterials(); 

    //Create the objects needed for the G4RunManager class	
	runManager = new G4RunManager();
	
	visManager = new G4VisExecutive("quiet");
    
  	detectorManager = new DetectorConstruction(); 
  	runManager -> SetUserInitialization(detectorManager); 
  	
	physicsManager = new PhysicsList();              
	runManager -> SetUserInitialization(physicsManager);
	
	beamManager = new PrimaryGeneratorAction(); 
	runManager -> SetUserAction(beamManager);
	
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
	UImanager -> ApplyCommand("/process/verbose 0");
	UImanager -> ApplyCommand("/run/verbose 0");

    sampleconstruction = detectorManager->GetSampleConstruction();

    //Set the seed engine
	CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());
	
	//UImanager->GetTree()->List();
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

void Simulation::Execute_Macrolist_pyw(std::vector<std::string> macroFiles)
{
    int nFiles = macroFiles.size();
    
    for (int n = 0; n < nFiles ; n++)
    {    
        Execute_Macro_pyw(macroFiles[n]);
    }
    
    macrofiles = macroFiles;
}

void Simulation::Execute_Macro_pyw(std::string macro)
{
    //Read the macro file line by line and extract the commands from the file

     if (globalVerbose > 0)
     {
        PrintToEndOfTerminal('-');        
        G4cout << "Reading file '" << macro << "' " << G4endl;
     }

    std::ifstream readfile;
        
    //Try to open the macro file      
    readfile.open(macro);
    if (!readfile) 
    {
        G4cout << "\nERROR: Unable to open file " << macro << " to apply commands. " << G4endl;
        return; 
    }
      
    //Read the file
    std::string command;    
    int line = 0;
    while ((std::getline(readfile, command))) 
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
            Execute_Command_pyw (command);
            
        }
    }
    
    readfile.close();
    if (globalVerbose > 0) {G4cout << "done "<< G4endl;}
}

#include "G4UIcommandTree.hh"
#include "G4UIcommand.hh"
#include "strfunctions.hh"
#include "CommandStatus.hh"

void Simulation::Execute_Command_pyw (std::string command)
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
                
                    Execute_Command_pyw (reapplycommand);
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
                            throw std::runtime_error("Invalid command");
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
             throw std::runtime_error("Invalid command");
        }
    }
}

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4GeometryManager.hh"
#include "ProgressTracker.hh"

int Simulation::Run_Tomography_pyw(unsigned long long int n_particles, 
                                   std::vector<int>       imageinfo, 
                                   double                 rotation_angle,
                                   double                 zposition)
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
		beamManager->SetNumberOfEvents(n_particles, totalprojections);
	 
	    runManager->Initialize(); 
	    runManager->SetNumberOfEventsToBeStored (0);
	    physicsManager->ActivateUserPhysics();
	    
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
            beamManager->GetProgressTracker().print = false;
        }
	}
    
    //Construct the samples. If darkflatfields are on, then it will remove the samples G4VPhysicalVolume.
    //After it has been constructed, if the G4VPhysicalVolume still exists, apply the needed transformations
    //to the sample such as rotation and translation
    sampleconstruction->Construct(darkflatfields);
    sampleconstruction->ApplyTransforms(rotation_angle, zposition);

    //Prepare for next run. Check if energy or gun has changed 
    beamManager -> ResetEvents(n_projection + 1);   
    
    Initialise_dataSets(true);
    beamManager -> GetProgressTracker().rotationangle = rotation_angle;
     
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
        beamManager -> ResetEvents(1);
        sampleconstruction->SetLastRotation(0);
	}

	return 0;
}

int Simulation::Run_Projection_pyw (unsigned long long int n_particles,
                                    bool   flatfield,
                                    double rotation_angle,
                                    double zposition,
                                    bool   resetdata)
{

    //globalVerbose = 0;
    beamManager->GetProgressTracker().print = false;
    
    //Set the seed for this image by calling the rand function (next number in the sequence)
	CLHEP::HepRandom::setTheSeed(rand());
    	  	    	
    //Let the PrimaryGeneratorAction class know where to position the start of the beam
    beamManager -> SetNumberOfEvents(n_particles, 1);
	 
	runManager -> Initialize(); 
	//runManager -> SetNumberOfEventsToBeStored(0);
	physicsManager -> ActivateUserPhysics();
    
    //Construct the samples. If darkflatfields are on, then it will remove the samples G4VPhysicalVolume.
    //After it has been constructed, if the G4VPhysicalVolume still exists, apply the needed transformations
    //to the sample such as rotation and translation
    sampleconstruction->Construct(flatfield);
    sampleconstruction->ApplyTransforms(rotation_angle, zposition);

    //Prepare for next run. Check if energy or gun has changed 
    beamManager -> ResetEvents(1);   
    
    Initialise_dataSets(resetdata);
    beamManager -> GetProgressTracker().rotationangle = rotation_angle;
    
	//UImanager -> ApplyCommand("/process/em/verbose 0");
	//UImanager -> ApplyCommand("/process/verbose 0");
     
    //Beam on to start the simulation
    BeamOn(n_particles);

	sampleconstruction->ApplyTransforms(0, 0);
    sampleconstruction->SetLastRotation(0);

	return 0;
}    

void Simulation::Setup_Visualization_pyw(std::string path,
		                                 std::string filename)
{
    //Checks to see if visualization setting is turned on, if so a .heprep file will be outputted to be viewed in a HepRApp viewer
	if (detectorManager -> GetVisualization() == true)
	{	
	    if (globalVerbose > 0)
	    {
            PrintToEndOfTerminal('-');
            G4cout << "GRAPHICS TURNED ON!";
        }
        
        runManager->Initialize();
		visManager -> Initialize();
        
        //If a different path is used, create a folder to store the visualization data 
        path = path + "visualization_" + filename + "/";
        const char *path_char = path.c_str();
            
        //Check if the directory exists
        DIR* directory_exists = opendir(path_char);
            
        //If it doesn't exist, create it
        if (!directory_exists)
        {
            std::string mkdir = "mkdir " + path;
            const char *mkdir_char = mkdir.c_str();
            system(mkdir_char);
        }
		
		//Setup the vis manager
		UImanager -> ApplyCommand("/vis/open HepRepFile");
		UImanager -> ApplyCommand("/vis/heprep/setFileDir " + path);
		UImanager -> ApplyCommand("/vis/heprep/setFileName " + filename);
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
		
		UImanager -> ApplyCommand("/vis/scene/add/axes 0 0 0 1 cm");
		UImanager -> ApplyCommand("/vis/viewer/zoom 0.5 mm");
		
		UImanager -> ApplyCommand("/vis/scene/add/hits");
		UImanager -> ApplyCommand("/vis/scene/endOfEventAction accumulate");
		UImanager -> ApplyCommand("/vis/viewer/set/autoRefresh true");
		
		if (globalVerbose > 0) {G4cout << "\nSaving as " << path + filename + "N.heprep where N is an integer. "<< G4endl;}
			
		beamManager->GetProgressTracker().singleline = false;
		beamManager->GetProgressTracker().graphicsOn = true;
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
    if (detectorManager -> GetVisualization() == true && nParticles > 5000)
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
void Simulation::Print_SimulationInfo_pyw(std::string filepath, 
                                          unsigned long long int n_particles, 
                                          int totalprojections, 
                                          int nDarkFlatFields)
{   
    if (globalVerbose > 0)
    {
        std::ofstream SaveToFile;
    
        //Try to open the file 
        SaveToFile.open(filepath); 
        if( !SaveToFile ) { 	
            G4cout << "\nError: " << filepath << " file could not be opened from Simulation. " << "\n" << G4endl;
            exit(1);
        }
        
        //Create an instance of the log to output to terminal and file    
        SettingsLog log(SaveToFile);
        PrintInformation(log);

        double particleperpixel = n_particles/(detectorManager->GetAbsorptionDetector()->GetxPixels() * detectorManager->GetAbsorptionDetector()->GetyPixels());
	
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
            std::ifstream readfile;
        
            PrintToEndOfTerminal(log, '-');
            //Try to open the macro file      
            readfile.open(macrofiles[n]);
            if (!readfile) {
                G4cout << "\nERROR: Unable to open " << macrofiles[n] << " for log output. " << G4endl;
                exit(1); 
            }
            log << "Macro file " << n + 1 << ": " << macrofiles[n] << "\n" << G4endl; 
      
            //Read the file
            std::string Line;    
            while ((std::getline(readfile, Line))) {
                //if (Line.find('#') == std::string::npos)
                if(Line[1] != '/') {log << Line << G4endl;}
            }
            readfile.close();
        }
    
        if (globalVerbose >= 1) {log.terminalOn = true;}
        else                    {log.terminalOn = false;}
    
        //Log the info from other classes
        detectorManager -> ReadOutInfo(log);
	    beamManager -> ReadOutInfo(log);
	    physicsManager -> ReadOutInfo(log);
    
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
    
        int abs_xPixels = AbsorptionDetector_GetXPixels_pyw();
        int abs_yPixels = AbsorptionDetector_GetYPixels_pyw();
        int bins = FluorescenceDetector_GetNoEnergyBins_pyw();
    
        PrintToEndOfTerminal('-');
        G4cout << "DISK SPACE REQUIRED FOR DATA: ";
    
        //Absorption
        size_t absorpDataSize = sizeof(AbsorptionDetector_GetProjection_pyw()[0]);
        double absorpStorageSpace = abs_xPixels * abs_yPixels * absorpDataSize * projections;
        totalStorage += absorpStorageSpace;
        unit = GetStorageUnit(absorpStorageSpace);
        G4cout << "\n- Absorption: " << absorpStorageSpace << unit;
    
        //Beam energy
        size_t beam_ByteTypeSize = beamManager -> GetBeamEnergyByteTypeSize();
        double beamStorageSpace = bins * beam_ByteTypeSize * projections;
        totalStorage += beamStorageSpace;
        unit = GetStorageUnit(beamStorageSpace);
        G4cout << "\n- Beam energy: " << beamStorageSpace << unit;
    
        //Fluorecence
        if (FluorescenceDetector_FullMappingActive_pyw())
        {
            size_t FMfluoresenceDataSize = sizeof(FluorescenceDetector_GetEnergyBins_pyw()[0]);  
            double fluorescenceStorageSpace = absorpStorageSpace*FluorescenceDetector_GetNoEnergyBins_pyw();
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
                        return " pb";
                    }
                    storage = storage/tera;
                    return " tb";
                }
                storage = storage/giga;
                return " gb";
            }
            storage = storage/mega;
            return " mb";
        }
        storage = storage/kilo;
        return " kb";
    }
    
    storage = storage/byte;
    return " bytes";           
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

void Simulation::Initialise_dataSets(bool clean_data)
{
    detectorManager->GetAbsorptionDetector()->GetSensitiveDetector()->InitialiseData(clean_data);
    detectorManager->GetFluorescenceDetector()->GetSensitiveDetector()->InitialiseData(clean_data);
    beamManager->SetupData(clean_data);
}
