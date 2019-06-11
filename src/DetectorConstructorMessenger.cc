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

	setworld_dimensions = new G4UIcmdWith3VectorAndUnit("/world/halfdimensions", this);
	setworld_dimensions -> SetGuidance("Set the world half dimensions ");
	setworld_dimensions -> SetParameterName("worldX","worldY","worldZ",true,true);
	setworld_dimensions -> SetUnitCandidates("mm cm m um ");
	setworld_dimensions -> SetRange("worldX > 0 || worldY > 0 || worldZ > 0");

	setworld_material = new G4UIcmdWithAString("/world/material", this);
	setworld_material -> SetGuidance("Set the world's material");

	Visualization_Cmd = new G4UIcmdWithABool("/world/visualization", this);
	Visualization_Cmd -> SetGuidance("Set if you would like the ouput of a .HepRep file to be visualized later ");
	Visualization_Cmd -> SetDefaultValue(false);	
	
}

DetectorConstructionMessenger::~DetectorConstructionMessenger()
{
	delete WorldDirectory;	
	delete setworld_dimensions;
	delete setworld_material;
	delete Visualization_Cmd;
}

void DetectorConstructionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if( command == setworld_dimensions )
	{
		G4ThreeVector dimensions = setworld_dimensions -> GetNew3VectorValue(newValue);
		ConstructDet -> SetWorldDimensions(dimensions);
	}
	if( command == setworld_material )
	{
		ConstructDet -> SetWorldMaterial(newValue);//SetWorldMaterial(newValue);
	}
	else if( command == Visualization_Cmd )
	{
		ConstructDet -> SetVisualization(Visualization_Cmd -> GetNewBoolValue(newValue));	
	}
}
