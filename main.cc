#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"
#include "PhysicsList.hh"
#include "StackingAction.hh"
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
  	// Detect interactive mode (if no arguments) and define UI session
  	G4UIExecutive* ui = 0;
  	if ( argc == 1 ) 
	{
    		ui = new G4UIExecutive(argc, argv);
  	}
  
  	// Construct the default run manager
	#ifdef G4MULTITHREADED
  		G4MTRunManager* runManager = new G4MTRunManager;
	#else
  		G4RunManager* runManager = new G4RunManager;
	#endif
  
  	// Initialize visualization
  	G4VisManager* visManager = new G4VisExecutive;
  	// G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  	// G4VisManager* visManager = new G4VisExecutive("Quiet");
  	visManager->Initialize();
	//G4cout << "**********************************Seed is: " << seed << G4endl; 

  	// Get the pointer to the User Interface manager
  	G4UImanager* UImanager = G4UImanager::GetUIpointer();
	UImanager->ApplyCommand("/tracking/verbose 1");	

	// Set mandatory initialization classes
  	// Detector construction
  	runManager->SetUserInitialization(new DetectorConstruction());

	// Physics list
	runManager -> SetUserInitialization(new PhysicsList());
   
//--------------------------------------------------------------------------------------

  	// User action initialization
  	runManager->SetUserInitialization(new ActionInitialization());

  	// Process macro or start UI session
  	//
  	if ( ! ui ) 
	{ 
    		// batch mode
    		G4String command = "/control/execute ";
    		G4String fileName = argv[1];
    		UImanager->ApplyCommand(command+fileName);
  	}
  	else 
	{ 
    		// interactive mode
		UImanager->ApplyCommand("/control/execute settings.mac");
		UImanager->ApplyCommand("/control/execute init_vis.mac");
		UImanager->ApplyCommand("/control/execute vis.mac");
		UImanager->ApplyCommand("/control/execute start.in");
		


    		ui->SessionStart();
   		delete ui;
  	}
	

  	// Job termination
  	// Free the store: user actions, physics_list and detector_description are
  	// owned and deleted by the run manager, so they should not be deleted 
  	// in the main() program !
  
  	delete visManager;
  	delete runManager;
}

