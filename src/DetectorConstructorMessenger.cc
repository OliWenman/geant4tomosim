#include "DetectorConstructionMessenger.hh"
#include "DetectorConstruction.hh"
#include "TrackerSD.hh"

#include "G4SystemOfUnits.hh"

#include "G4SystemOfUnits.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"

//DetectorConstructionMessenger::DetectorConstructionMessenger(DetectorConstruction* Detector):ConstructDet(Detector)
DetectorConstructionMessenger::DetectorConstructionMessenger(DetectorConstruction* Detector): G4UImessenger(), ConstructDet(Detector)
{	
	G4cout << G4endl << "DetectorConstructionMessenger has been created" << G4endl;
	//WORLD
	//Directory
	WorldDirectory = new G4UIdirectory("/world/");
	WorldDirectory -> SetGuidance("Commands to control the world variables. ");	

	WorldSize_Cmd = new G4UIcmdWith3VectorAndUnit("/world/size", this);
	WorldSize_Cmd -> SetGuidance("Set the world size, x, y and z. ");
	WorldSize_Cmd -> SetParameterName("X","Y","Z",true,true);
	WorldSize_Cmd -> SetUnitCandidates("mm cm m ");
	WorldSize_Cmd -> SetDefaultUnit("m");
	WorldSize_Cmd -> SetDefaultValue(G4ThreeVector(0.9*m, 0.9*m, 0.9*m));
	
//-----------------------------------------------------------------------------------------------------
	
	//DETECTOR
	//Directory
	DetectorDirectory = new G4UIdirectory("/detector/");
	DetectorDirectory -> SetGuidance("Commands to control the detector variables. ");

	//Number in y direction command
	NoDetectorsY_Cmd = new G4UIcmdWithAnInteger("/detector/numberY", this);
	NoDetectorsY_Cmd -> SetDefaultValue(100);
	NoDetectorsY_Cmd -> SetGuidance("Pick the number of detectors you would like to have.  ");

	//Number in z direction command
	NoDetectorsZ_Cmd = new G4UIcmdWithAnInteger("/detector/numberZ", this);
	NoDetectorsZ_Cmd -> SetDefaultValue(100);
	NoDetectorsZ_Cmd -> SetGuidance("Pick the number of detectors you would like to have.  ");

	//Detector Size command
	DetectorSize_Cmd = new G4UIcmdWith3VectorAndUnit("/detector/size", this);
	DetectorSize_Cmd -> SetGuidance("Set the detector size, x, y and z. ");
	DetectorSize_Cmd -> SetParameterName("X","Y","Z",true,true);
	DetectorSize_Cmd -> SetUnitCandidates("mm cm m ");
	DetectorSize_Cmd -> SetDefaultUnit("m");
	DetectorSize_Cmd -> SetDefaultValue(G4ThreeVector(0.001*m, 0.005*m, 0.005*m));		

	DetectorMaterial_Cmd = new G4UIcmdWithAString("/detector/material", this);
	DetectorMaterial_Cmd -> SetGuidance("Set the material of the detector ");
	DetectorMaterial_Cmd -> SetDefaultValue("G4_SODIUM_IODIDE");

//-----------------------------------------------------------------------------------------------------
	//TARGET
	//Directory
	TargetDirectory = new G4UIdirectory("/Target/");
	TargetDirectory -> SetGuidance("Commands to control the detector variables. ");
	
	TargetPosition_Cmd = new G4UIcmdWith3VectorAndUnit("/Target/position", this);
	TargetPosition_Cmd -> SetGuidance("Set the target position, x, y and z. ");
	TargetPosition_Cmd -> SetParameterName("X","Y","Z",true,true);
	TargetPosition_Cmd -> SetUnitCandidates("mm cm m ");
	TargetPosition_Cmd -> SetDefaultUnit("m");
	TargetPosition_Cmd -> SetDefaultValue(G4ThreeVector(0.0*m, 0.0*m, 0.0*m));	

	TargetMaterial_Cmd = new G4UIcmdWithAString("/Target/material", this);
	TargetMaterial_Cmd -> SetGuidance("Set the material of the target ");
	TargetMaterial_Cmd -> SetDefaultValue("G4_Al");

//------------------------------------------------------------------------------------------------------
	//Data Manipulation
	DataDirectory = new G4UIdirectory("/data/");
	DataDirectory -> SetGuidance("Commands to control the data variables. ");

	NoBins_Cmd = new G4UIcmdWithAnInteger("/data/bins", this);
	NoBins_Cmd -> SetDefaultValue(5);
	NoBins_Cmd -> SetGuidance("Pick the number of bins you would like the energy to be sorted into.  ");

	NoImages_Cmd = new G4UIcmdWithAnInteger("/data/NumberOfImages", this);
	NoImages_Cmd -> SetDefaultValue(2);
	NoImages_Cmd -> SetGuidance("Pick the number of images you would to have taken ");

	NoPhotons_Cmd = new G4UIcmdWithAnInteger("/data/NumberOfPhotons", this);
	NoPhotons_Cmd -> SetDefaultValue(1000);
	NoPhotons_Cmd -> SetGuidance("Pick the number of images you would to have taken ");

	Visualization_Cmd = new G4UIcmdWithAString("/data/Visualization", this);
	Visualization_Cmd -> SetGuidance("Choose if you want visualization ");
	Visualization_Cmd -> SetDefaultValue("true");
	
}

DetectorConstructionMessenger::~DetectorConstructionMessenger()
{
	delete WorldDirectory;	
	delete WorldSize_Cmd;

	delete DetectorDirectory;
	delete NoDetectorsY_Cmd;
	delete NoDetectorsZ_Cmd;
	delete DetectorSize_Cmd;
	delete DetectorMaterial_Cmd;

	delete TargetDirectory;
	delete TargetPosition_Cmd;
	delete TargetMaterial_Cmd;

	delete DataDirectory;
	delete NoBins_Cmd;
	delete NoImages_Cmd;
	delete NoPhotons_Cmd;
	delete Visualization_Cmd;

	G4cout << G4endl << "DetectorConstructionMessenger has been deleted "<< G4endl;
}

void DetectorConstructionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if( command == WorldSize_Cmd )
	{
		ConstructDet -> SetWorldSize(WorldSize_Cmd -> GetNew3VectorValue(newValue));
		G4cout << G4endl << "DetectorConstruction -> SetWorldSize command detected "<< G4endl;
	}
	else if( command == NoDetectorsY_Cmd )
  	{ 			
		ConstructDet -> SetNoDetectorsY(NoDetectorsY_Cmd -> GetNewIntValue(newValue));
		G4cout << G4endl << "DetectorConstruction -> SetNoDetectorsY command detected "<< G4endl;
	}
	else if ( command == NoDetectorsZ_Cmd )
	{
		ConstructDet -> SetNoDetectorsZ(NoDetectorsZ_Cmd -> GetNewIntValue(newValue));
		G4cout << G4endl << "DetectorConstruction -> SetNoDetectorsZ command detected "<< G4endl;
	}
	else if( command == DetectorSize_Cmd )
	{
		ConstructDet -> SetDetectorSize(DetectorSize_Cmd -> GetNew3VectorValue(newValue));
		G4cout << G4endl << "DetectorConstruction -> SetDetectorSize command detected "<< G4endl;
	}
	else if( command == DetectorMaterial_Cmd )
	{
		ConstructDet -> SetDetectorMaterial(newValue);
		G4cout << G4endl << "DetectorConstruction -> SetDetectorMaterial command detected "<< G4endl;
	}
	else if( command == TargetPosition_Cmd )
	{
		ConstructDet -> SetTargetPosition(TargetPosition_Cmd -> GetNew3VectorValue(newValue));	
		G4cout << G4endl << "DetectorConstruction -> SetTargetPosition command detected "<< G4endl;
	}
	else if(command == TargetMaterial_Cmd )
	{
		ConstructDet -> SetTargetMaterial(newValue);	
		G4cout << G4endl << "DetectorConstruction -> SetTargetMaterial command detected "<< G4endl;
	}
	else if( command == NoBins_Cmd )
	{
		ConstructDet -> SetNoBins(NoBins_Cmd -> GetNewIntValue(newValue));	
		G4cout << G4endl << "DetectorConstruction -> SetNoBins command detected "<< G4endl;
	}
	else if( command == NoImages_Cmd )
	{
		ConstructDet -> SetNoImages(NoImages_Cmd -> GetNewIntValue(newValue));	
		G4cout << G4endl << "DetectorConstruction -> SetNoImages command detected "<< G4endl;
	}
	else if( command == NoPhotons_Cmd )
	{
		ConstructDet -> SetNoPhotons(NoPhotons_Cmd -> GetNewIntValue(newValue));	
		G4cout << G4endl << "DetectorConstruction -> SetNoPhotons command detected "<< G4endl;
	}
	else if( command == Visualization_Cmd )
	{
		ConstructDet -> SetVisualization(newValue);	
		G4cout << G4endl << "DetectorConstruction -> SetVisualization command detected "<< G4endl;
	}
}
