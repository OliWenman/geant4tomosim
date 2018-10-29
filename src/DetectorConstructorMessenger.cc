#include "DetectorConstructionMessenger.hh"
#include "DetectorConstruction.hh"
#include "Input.hh"

#include "G4SystemOfUnits.hh"

#include "G4SystemOfUnits.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"

#include "G4TwoVector.hh"
#include "G4Tokenizer.hh"

//DetectorConstructionMessenger::DetectorConstructionMessenger(DetectorConstruction* Detector):ConstructDet(Detector)
DetectorConstructionMessenger::DetectorConstructionMessenger(DetectorConstruction* Detector, Input* InputObject): G4UImessenger(), ConstructDet(Detector), input(InputObject)
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

	Visualization_Cmd = new G4UIcmdWithABool("/world/visualization", this);
	Visualization_Cmd -> SetGuidance("Set if you would like the ouput of a .HepRep file to be visualized later ");
	Visualization_Cmd -> SetDefaultValue("false");	
	
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

	DetectorEfficiency_Cmd = new G4UIcmdWithABool("/detector/MaxEfficiency", this);
	DetectorEfficiency_Cmd -> SetGuidance("Set the efficncy of the detectors to be 100%\ efficient or realistic ");
	DetectorEfficiency_Cmd -> SetDefaultValue("true");	

//-----------------------------------------------------------------------------------------------------
		
}

DetectorConstructionMessenger::~DetectorConstructionMessenger()
{
	delete WorldDirectory;	
	delete WorldSize_Cmd;
	delete Visualization_Cmd;

	delete DetectorDirectory;
	delete NoDetectorsY_Cmd;
	delete NoDetectorsZ_Cmd;
	delete DetectorSize_Cmd;
	delete DetectorMaterial_Cmd;
	delete DetectorEfficiency_Cmd;

	G4cout << G4endl << "DetectorConstructionMessenger has been deleted ";
}

void DetectorConstructionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if( command == WorldSize_Cmd )
	{
		G4ThreeVector Dimensions = WorldSize_Cmd -> GetNew3VectorValue(newValue);
		ConstructDet -> SetWorldSize(Dimensions/2);
		G4cout << "DetectorConstruction -> SetWorldSize command detected "<< G4endl;
		input -> SetBeamLength(Dimensions.x());
	}
	else if( command == Visualization_Cmd )
	{
		ConstructDet -> SetVisualization(Visualization_Cmd -> GetNewBoolValue(newValue));	
		G4cout << "DetectorConstruction -> SetVisualization command detected " << G4endl;
	}
	else if( command == NoDetectorsY_Cmd )
  	{ 			
		ConstructDet -> SetNoDetectorsY(NoDetectorsY_Cmd -> GetNewIntValue(newValue));
		G4cout << "DetectorConstruction -> SetNoDetectorsY command detected "<< G4endl;
		input -> SetNumberRows(NoDetectorsY_Cmd -> GetNewIntValue(newValue));	
	}
	else if ( command == NoDetectorsZ_Cmd )
	{
		ConstructDet -> SetNoDetectorsZ(NoDetectorsZ_Cmd -> GetNewIntValue(newValue));
		G4cout << "DetectorConstruction -> SetNoDetectorsZ command detected "<< G4endl;
		input -> SetNumberColumns(NoDetectorsZ_Cmd -> GetNewIntValue(newValue));
	}
	else if( command == DetectorSize_Cmd )
	{
		ConstructDet -> SetDetectorSize(DetectorSize_Cmd -> GetNew3VectorValue(newValue)/2);
		G4cout << "DetectorConstruction -> SetDetectorSize command detected "<< G4endl;
		input -> SetDetectorDimensions(DetectorSize_Cmd -> GetNew3VectorValue(newValue)/2);
	}
	else if( command == DetectorMaterial_Cmd )
	{
		ConstructDet -> SetDetectorMaterial(newValue);
		G4cout << "DetectorConstruction -> SetDetectorMaterial command detected "<< G4endl;
		input -> SetDetectorMaterial(newValue);
	}
	else if( command == DetectorEfficiency_Cmd )
	{
		ConstructDet -> SetDetectorEfficiency(DetectorEfficiency_Cmd -> GetNewBoolValue(newValue));	
		G4cout << "DetectorConstruction -> SetDetectorEfficiency command detected " << G4endl;
		input -> SetDetectorEfficiency(DetectorEfficiency_Cmd -> GetNewBoolValue(newValue));
	}
}
