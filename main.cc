#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"
#include "PhysicsList.hh"
#include "StackingAction.hh"
#include "Data.hh"
#include "globals.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

int main(int argc,char** argv)
{
  	//Detect interactive mode (if no arguments) and define UI session
  	G4UIExecutive* ui = 0;
  	
	if ( argc == 1 ) 
	{
    		ui = new G4UIExecutive(argc, argv);
  	}
  
  	//Construct the default run manager
	#ifdef G4MULTITHREADED
  		G4MTRunManager* runManager = new G4MTRunManager;
	#else
  		G4RunManager* runManager = new G4RunManager();
	#endif
  
  	//Initialize visualization
  	G4VisManager* visManager = new G4VisExecutive;
	visManager -> Initialize();

	//Creata an instance of the classes
	Data* data = new Data();
	DetectorConstruction* DC = new DetectorConstruction(data); 
	PhysicsList* PL = new PhysicsList(data); 	
	runManager -> SetUserInitialization(DC);
	runManager -> SetUserInitialization(PL);
  	runManager -> SetUserInitialization(new ActionInitialization(data, DC));


	//Get the pointer to the User Interface manager, set all print info to 0 during events by default
  	G4UImanager* UImanager = G4UImanager::GetUIpointer();
	UImanager -> ApplyCommand("/tracking/verbose 0");	//Gives information about particle
	UImanager -> ApplyCommand("/control/verbose 0");	
	UImanager -> ApplyCommand("/hits/verbose 0");
	UImanager -> ApplyCommand("/process/em/verbose 0");

  	//Process macro files or start UI session
  	if ( ! ui ) 
	{ 
    		// batch mode
    		G4String command = "/control/execute ";
    		G4String fileName = argv[1];
    		UImanager -> ApplyCommand(command+fileName);
  	}
  	else 
	{ 
    		// interactive mode
		UImanager -> ApplyCommand("/control/execute settings.mac");
		UImanager -> ApplyCommand("/control/execute init_vis.mac");
		UImanager -> ApplyCommand("/control/execute vis.mac");
		
		
		G4int Image = 1;
		for (Image; Image <= DC -> GetNoImages(); Image++)
		{
			DC -> SetCurrentImage(Image);
			G4cout << G4endl << "RUN ABOUT TO BEGIN: IMAGE " <<  Image << G4endl;
			runManager -> BeamOn(DC -> GetNoPhotons());
			G4RunManager::GetRunManager()->ReinitializeGeometry();
			//UImanager -> ApplyCommand("/control/execute start.in");
		}
		ui -> SessionStart();
   		delete ui;
  	}
	
  	delete visManager;
  	delete runManager;

	return 0;
}

