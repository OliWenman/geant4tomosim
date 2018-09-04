#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"
#include "PhysicsList.hh"
#include "StackingAction.hh"
#include "Data.hh"
#include "globals.hh"

#include "G4Timer.hh"

#include "G4RunManager.hh"

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
  	G4RunManager* runManager = new G4RunManager();

	//Initialize visualization
  	G4VisManager* visManager = new G4VisExecutive;
	visManager -> Initialize();

	//Create an instance of the classes
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

    	//interactive mode
	UImanager -> ApplyCommand("/control/execute settings.mac");
	
	if (data -> GetVisualization() == "true")
	{
		UImanager -> ApplyCommand("/control/execute MyVis.mac");
		G4cout << G4endl << "GRAPHICS SYSTEM ENABLED: Will increase computational time." << G4endl << G4endl;
	}
	
	//Save variables to needed classes
	G4int Image = 0;
	//data -> SetNumberOfPhotons(DC -> GetNoPhotons());
	DC -> SetNoImages(data -> GetNoImages());

	//Start the simulation timer
	G4Timer FullTime;
	FullTime.Start();

	for (Image; Image < data -> GetNoImages(); Image++)
	{
		G4Timer LoopTimer;
		LoopTimer.Start();
		data -> SetCurrentImage(Image);
		DC -> SetCurrentImage(Image);
		
		G4cout << G4endl << "================================================================================"
		       << G4endl << "                           PROCESSING IMAGE " <<  Image+1
	               << G4endl << "================================================================================" << G4endl;
		
		runManager -> BeamOn(data -> GetNoPhotons());
		G4RunManager::GetRunManager()->ReinitializeGeometry();
		//G4RunManager::GetRunManager()->GeometryHasBeenModified();
		LoopTimer.Stop();
		G4cout << G4endl << "Run time [s] : " << LoopTimer << G4endl; 
	}
	
	FullTime.Stop();

	data -> SetSimulationTime(FullTime.GetRealElapsed());

	G4cout << G4endl << "================================================================================"
	       << G4endl << "                      The simulation is complete! "
	       << G4endl << "             Total simulation time : "<< FullTime
	       << G4endl << "================================================================================" << G4endl;
	

	if (data -> GetTextFileCmd() == "true")
		{data -> WriteToTextFile();}
	else 
		{G4cout << G4endl << "Data was not written to a text file" << G4endl;}

	ui -> SessionStart();   	

	G4cout << G4endl << "Deleting ui";
	delete ui;
	G4cout << G4endl << "Deleting visManager" << G4endl;
  	delete visManager;
	G4cout << G4endl << "Deleting runManager " << G4endl;
  	delete runManager;
	delete data;

	return 0;
}

//-------------------------------------------------------------------------------------

/*
#ifdef G4VIS_USE
	#include "G4VisExecutive.hh"
#endif
#ifdef G4UI_USE
	#include "G4UIExecutive.hh"
#endif

#ifdef G4VIS_USE
void visio(int argc, char* argv[])
{
		G4VisManager *visManager=new G4VisExecutive;
		visManager->Initialize();
#ifdef G4UI_USE
		G4UIExecutive *ui = new G4UIExecutive(argc, argv);
                G4UImanager *UImanager = G4UImanager::GetUIpointer();
		UImanager -> ApplyCommand("/control/execute init_vis.mac");
		UImanager->ApplyCommand("/control/execute vis.mac");     
		ui->SessionStart();
		delete ui;
#endif
		delete visManager;
}
#endif

int main(int argc, char* argv[])
{
	G4RunManager *runManager=new G4RunManager();

	//Creata an instance of the classes
	Data* data = new Data();
	DetectorConstruction* DC = new DetectorConstruction(data); 
	PhysicsList* PL = new PhysicsList(data); 	
	runManager -> SetUserInitialization(DC);
	runManager -> SetUserInitialization(PL);
  	runManager -> SetUserInitialization(new ActionInitialization(data, DC));

	// read the main mac file and execute the commands
	G4UImanager* UImanager = G4UImanager::GetUIpointer();
	G4String command = "/control/execute ";
	//UImanager->ApplyCommand(command+myInputData->inputData.generalData.StartFileInputData); 
	UImanager -> ApplyCommand("/hits/verbose 0");
	UImanager -> ApplyCommand("/process/em/verbose 0");

	UImanager -> ApplyCommand("/control/execute settings.mac");
	G4int Image = 0;
	data -> SetNumberOfPhotons(DC -> GetNoPhotons());
	data -> SetNumberOfImages(DC -> GetNoImages());

	G4Timer MyFullTime;
	MyFullTime.Start();	

	G4cout << G4endl << "Output test " << G4endl;

	visio(argc, argv);

	for (Image; Image < DC -> GetNoImages(); Image++)
	{
		G4cout << G4endl << "Output test 2 " << G4endl;
		data -> SetCurrentImage(Image);
		G4cout << G4endl << "Output test  3" << G4endl;
		DC -> SetCurrentImage(Image);
		G4cout << G4endl << "RUN ABOUT TO BEGIN: IMAGE " <<  Image+1 << G4endl;
		runManager -> BeamOn(DC -> GetNoPhotons());
	}
	MyFullTime.Stop();
	//loopElapsedTime=MyFullTime.GetUserElapsed();
	std::cout << "Simulation time [s] : "<< MyFullTime << '\n';
	std::cout <<'\n';
	
        delete runManager;
        return 0;
}*/

