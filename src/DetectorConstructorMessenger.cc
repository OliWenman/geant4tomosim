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

DetectorConstructionMessenger::DetectorConstructionMessenger(DetectorConstruction* Detector, Input* InputObject): G4UImessenger(), ConstructDet(Detector), input(InputObject)
{	
	//WORLD
	//Directory
	WorldDirectory = new G4UIdirectory("/world/");
	WorldDirectory -> SetGuidance("Commands to control the world variables. ");	

	WorldSize_Cmd = new G4UIcmdWith3VectorAndUnit("/world/size", this);
	WorldSize_Cmd -> SetGuidance("Set the world size for the x, y and z dimensions ");
	WorldSize_Cmd -> SetParameterName("worldX","worldY","worldZ",true,true);
	WorldSize_Cmd -> SetUnitCandidates("mm cm m um ");
	WorldSize_Cmd -> SetDefaultUnit("m");
	WorldSize_Cmd -> SetDefaultValue(G4ThreeVector(0.9*m, 0.9*m, 0.9*m));
	WorldSize_Cmd -> SetRange("worldX > 0 || worldY > 0 || worldZ > 0");

	Visualization_Cmd = new G4UIcmdWithABool("/world/visualization", this);
	Visualization_Cmd -> SetGuidance("Set if you would like the ouput of a .HepRep file to be visualized later ");
	Visualization_Cmd -> SetDefaultValue(false);	
	
//-----------------------------------------------------------------------------------------------------
	
	//DETECTOR
	//Directory
	DetectorDirectory = new G4UIdirectory("/detector/");
	DetectorDirectory -> SetGuidance("Commands to control the detector variables. ");

	//Number in y direction command
	NoDetectorsY_Cmd = new G4UIcmdWithAnInteger("/detector/numberY", this);
	NoDetectorsY_Cmd -> SetGuidance("Pick the number of detectors you would like to have. ");
	NoDetectorsY_Cmd -> SetParameterName("nDetY", true);
	NoDetectorsY_Cmd -> SetRange("nDetY > 0");
	NoDetectorsY_Cmd -> SetDefaultValue(100);
	
	//Number in z direction command
	NoDetectorsZ_Cmd = new G4UIcmdWithAnInteger("/detector/numberZ", this);
	NoDetectorsZ_Cmd -> SetParameterName("nDetZ", true, true);
	NoDetectorsZ_Cmd -> SetRange("nDetZ > 0");
	NoDetectorsZ_Cmd -> SetDefaultValue(100);
	NoDetectorsZ_Cmd -> SetGuidance("Pick the number of detectors you would like to have.  ");

	//Detector Size command
	DetectorSize_Cmd = new G4UIcmdWith3VectorAndUnit("/detector/size", this);
	DetectorSize_Cmd -> SetGuidance("Set the detector size, x, y and z. ");
	DetectorSize_Cmd -> SetParameterName("X","Y","Z",true,true);
	DetectorSize_Cmd -> SetUnitCandidates("mm cm m um ");
	DetectorSize_Cmd -> SetRange("X > 0 || Y > 0 || Z > 0");
	DetectorSize_Cmd -> SetDefaultUnit("m");
	DetectorSize_Cmd -> SetDefaultValue(G4ThreeVector(0.001*m, 0.005*m, 0.005*m));		

	DetectorMaterial_Cmd = new G4UIcmdWithAString("/detector/material", this);
	DetectorMaterial_Cmd -> SetGuidance("Set the material of the detector ");
	DetectorMaterial_Cmd -> SetDefaultValue("G4_SODIUM_IODIDE");

	DetectorEfficiency_Cmd = new G4UIcmdWithABool("/detector/MaxEfficiency", this);
	DetectorEfficiency_Cmd -> SetGuidance("Set the efficncy of the detectors to be 100%\ efficient or realistic ");
	DetectorEfficiency_Cmd -> SetDefaultValue(true);		
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
}

void DetectorConstructionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if( command == WorldSize_Cmd )
	{
		G4ThreeVector Dimensions = WorldSize_Cmd -> GetNew3VectorValue(newValue);
		ConstructDet -> SetWorldSize(Dimensions/2);
		input -> SetBeamLength(Dimensions.x());
	}
	else if( command == Visualization_Cmd )
	{
		ConstructDet -> SetVisualization(Visualization_Cmd -> GetNewBoolValue(newValue));	
	}
	else if( command == NoDetectorsY_Cmd )
  	{ 		
		G4int nDetectorsY = NoDetectorsY_Cmd -> GetNewIntValue(newValue);	
		ConstructDet -> SetNoDetectorsY(nDetectorsY);
		input -> SetNumberRows(nDetectorsY);	
	}
	else if ( command == NoDetectorsZ_Cmd )
	{
		G4int nDetectorsZ = NoDetectorsZ_Cmd -> GetNewIntValue(newValue);	
		ConstructDet -> SetNoDetectorsZ(nDetectorsZ);
		input -> SetNumberColumns(nDetectorsZ);
	}
	else if( command == DetectorSize_Cmd )
	{
		ConstructDet -> SetDetectorSize(DetectorSize_Cmd -> GetNew3VectorValue(newValue)/2);
		input -> SetDetectorDimensions(DetectorSize_Cmd -> GetNew3VectorValue(newValue)/2);
	}
	else if( command == DetectorMaterial_Cmd )
	{
		ConstructDet -> SetDetectorMaterial(newValue);
		input -> SetDetectorMaterial(newValue);
	}
	else if( command == DetectorEfficiency_Cmd )
	{
		ConstructDet -> SetDetectorEfficiency(DetectorEfficiency_Cmd -> GetNewBoolValue(newValue));	
		input -> SetDetectorEfficiency(DetectorEfficiency_Cmd -> GetNewBoolValue(newValue));
	}
}
