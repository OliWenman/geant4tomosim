#include "DetectorConstructionMessenger.hh"
#include "DetectorConstruction.hh"

#include "G4SystemOfUnits.hh"

#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"

#include "G4TwoVector.hh"
#include "G4Tokenizer.hh"

DetectorConstructionMessenger::DetectorConstructionMessenger(DetectorConstruction* Detector): G4UImessenger(), ConstructDet(Detector)
{	
	//WORLD
	//Directory
	WorldDirectory = new G4UIdirectory("/world/");
	WorldDirectory -> SetGuidance("Commands to control the world variables. ");	

	WorldSize_Cmd = new G4UIcmdWith3VectorAndUnit("/world/size", this);
	WorldSize_Cmd -> SetGuidance("Set the world size for the x, y and z dimensions ");
	WorldSize_Cmd -> SetParameterName("worldX","worldY","worldZ",true,true);
	WorldSize_Cmd -> SetUnitCandidates("mm cm m um ");
	WorldSize_Cmd -> SetRange("worldX > 0 || worldY > 0 || worldZ > 0");

	WorldMaterial_Cmd = new G4UIcmdWithAString("/world/material", this);
	WorldMaterial_Cmd -> SetGuidance("Set the world's material");

	Visualization_Cmd = new G4UIcmdWithABool("/world/visualization", this);
	Visualization_Cmd -> SetGuidance("Set if you would like the ouput of a .HepRep file to be visualized later ");
	Visualization_Cmd -> SetDefaultValue(false);	
	
}

DetectorConstructionMessenger::~DetectorConstructionMessenger()
{
	delete WorldDirectory;	
	delete WorldSize_Cmd;
	delete WorldMaterial_Cmd;
	delete Visualization_Cmd;
}

void DetectorConstructionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if( command == WorldSize_Cmd )
	{
		G4ThreeVector Dimensions = WorldSize_Cmd -> GetNew3VectorValue(newValue);
		ConstructDet -> SetWorldSize(Dimensions/2);
	}
	if( command == WorldMaterial_Cmd )
	{
		ConstructDet -> SetWorldMaterial(newValue);
	}
	else if( command == Visualization_Cmd )
	{
		ConstructDet -> SetVisualization(Visualization_Cmd -> GetNewBoolValue(newValue));	
	}
}
