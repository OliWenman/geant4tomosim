#include "DetectorConstructionMessenger.hh"
#include "DetectorConstruction.hh"

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
	WorldSize_Cmd -> SetUnitCandidates("mm cm m um ");
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
	DetectorSize_Cmd -> SetUnitCandidates("mm cm m um ");
	DetectorSize_Cmd -> SetDefaultUnit("m");
	DetectorSize_Cmd -> SetDefaultValue(G4ThreeVector(0.001*m, 0.005*m, 0.005*m));		

	DetectorMaterial_Cmd = new G4UIcmdWithAString("/detector/material", this);
	DetectorMaterial_Cmd -> SetGuidance("Set the material of the detector ");
	DetectorMaterial_Cmd -> SetDefaultValue("G4_SODIUM_IODIDE");

//-----------------------------------------------------------------------------------------------------
		
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

	G4cout << G4endl << "DetectorConstructionMessenger has been deleted "<< G4endl;
}

void DetectorConstructionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if( command == WorldSize_Cmd )
	{
		ConstructDet -> SetWorldSize(WorldSize_Cmd -> GetNew3VectorValue(newValue)/2);
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
		ConstructDet -> SetDetectorSize(DetectorSize_Cmd -> GetNew3VectorValue(newValue)/2);
		G4cout << G4endl << "DetectorConstruction -> SetDetectorSize command detected "<< G4endl;
	}
	else if( command == DetectorMaterial_Cmd )
	{
		ConstructDet -> SetDetectorMaterial(newValue);
		G4cout << G4endl << "DetectorConstruction -> SetDetectorMaterial command detected "<< G4endl;
	}
}
