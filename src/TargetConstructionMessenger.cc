#include "TargetConstructionMessenger.hh"
#include "TargetConstruction.hh"

#include "G4SystemOfUnits.hh"

#include "G4SystemOfUnits.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"

TargetConstructionMessenger::TargetConstructionMessenger(TargetConstruction* Target): G4UImessenger(), TC(Target)
{	
	G4cout << G4endl << "TargetConstructionMessenger has been created" << G4endl;

	Count = 0;

	//TARGET
	//Directory
	TargetDirectory = new G4UIdirectory("/Target/");
	TargetDirectory -> SetGuidance("Commands to control the detector variables. ");

	CubeDimensions_Cmd = new G4UIcmdWith3VectorAndUnit("/Target/Cube/Dimensions", this);
	CubeDimensions_Cmd -> SetGuidance("Set the dimensions of a cube for x, y and z. ");
	CubeDimensions_Cmd -> SetUnitCandidates("mm cm m ");
	CubeDimensions_Cmd -> SetDefaultUnit("mm");
	CubeDimensions_Cmd -> SetDefaultValue(G4ThreeVector(0.3*mm, 0.3*mm, 0.0*mm));
	
	TargetPosition_Cmd = new G4UIcmdWith3VectorAndUnit("/Target/position", this);
	TargetPosition_Cmd -> SetGuidance("Set the target position, x, y and z. ");
	TargetPosition_Cmd -> SetUnitCandidates("um mm cm m ");
	TargetPosition_Cmd -> SetDefaultUnit("m");
	TargetPosition_Cmd -> SetDefaultValue(G4ThreeVector(0.0*m, 0.0*m, 0.0*m));

	TargetRotation_Cmd = new G4UIcmdWith3VectorAndUnit("/Target/rotation", this);
	TargetRotation_Cmd -> SetGuidance("Set the starting rotation of the targert. ");
	TargetRotation_Cmd -> SetUnitCandidates("deg rad");
	TargetRotation_Cmd -> SetDefaultUnit("deg");
	TargetRotation_Cmd -> SetDefaultValue(G4ThreeVector(0.0*deg, 0.0*deg, 0.0*deg));

	TargetMaterial_Cmd = new G4UIcmdWithAString("/Target/material", this);
	TargetMaterial_Cmd -> SetGuidance("Set the material of the target ");
	TargetMaterial_Cmd -> SetDefaultValue("G4_Al");

	OffSetRadius_Cmd = new G4UIcmdWithADoubleAndUnit("/Target/OffSet", this);
	OffSetRadius_Cmd -> SetGuidance("Set the off set of the poisition of the target with a radius");
	OffSetRadius_Cmd -> SetDefaultUnit("cm");
	OffSetRadius_Cmd -> SetDefaultValue(0.5*cm);

	NumberOfObjects_Cmd = new G4UIcmdWithAnInteger("/Target/NumberOfObjects",this);
	NumberOfObjects_Cmd -> SetGuidance("Set the number of objects");
	NumberOfObjects_Cmd -> SetDefaultValue(1);
}

TargetConstructionMessenger::~TargetConstructionMessenger()
{
	delete TargetDirectory;
	delete CubeDimensions_Cmd;
	delete TargetPosition_Cmd;
	delete TargetRotation_Cmd;
	delete TargetMaterial_Cmd;
	delete OffSetRadius_Cmd;
	delete NumberOfObjects_Cmd;

	G4cout << G4endl << "TargetConstructionMessenger has been deleted ";
}

void TargetConstructionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if(command == CubeDimensions_Cmd)
	{
		G4cout <<G4endl<< "Count = " << Count;
		G4ThreeVector Dimensions = CubeDimensions_Cmd -> GetNew3VectorValue(newValue);
		TC -> SetCubeDimensions(0, Dimensions);	
		G4cout << "TargetConstruction -> SetCubeDimensions command detected "<< G4endl;
	}
	else if(command == TargetPosition_Cmd)
	{
		G4ThreeVector Position = TargetPosition_Cmd -> GetNew3VectorValue(newValue);
		TC -> AddVectorPosition(0, Position);	
		G4cout << "TargetConstruction -> SetTargetPosition command detected "<< G4endl;
	}
	else if(command == TargetRotation_Cmd)
	{
		G4ThreeVector Rotation = TargetRotation_Cmd -> GetNew3VectorValue(newValue);
		TC -> SetVectorRotation(0, Rotation);	
		G4cout << "TargetConstruction -> SetTargetRotation command detected "<< G4endl;
	}
	else if(command == TargetMaterial_Cmd )
	{
		TC -> SetMaterial(0, newValue);	
		G4cout << "TargetConstruction -> SetTargetMaterial command detected "<< G4endl;
	}
	else if(command == OffSetRadius_Cmd )
	{
		OffSetRadius_Cmd -> GetNewUnitValue(newValue);
		TC -> SetOffSetRadius(OffSetRadius_Cmd -> GetNewDoubleValue(newValue));	
		G4cout << "TargetConstruction -> OffSetRadius_Cmd command detected "<< G4endl;
	}
	else if( command == NumberOfObjects_Cmd )
	{
		TC -> SetNumberOfObjects(NumberOfObjects_Cmd -> GetNewIntValue(newValue));	
		G4cout << "TargetConstruction -> SetNumberOfObjects command detected "<< G4endl;
	}
}
