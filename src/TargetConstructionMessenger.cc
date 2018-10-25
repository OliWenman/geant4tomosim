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

	//Directory
	TargetDirectory = new G4UIdirectory("/Target/");
	TargetDirectory -> SetGuidance("Commands to control the detector variables. ");

	//Command to keep track of what type of objects that are being placed
	TypeOfObjects_Cmd = new G4UIcmdWithAString("/Target/Object", this);
	TypeOfObjects_Cmd -> SetGuidance("Set the type of object your about to create");

	//Command to set the dimensions of a cube
	CubeDimensions_Cmd = new G4UIcmdWith3VectorAndUnit("/Target/Cube/Dimensions", this);
	CubeDimensions_Cmd -> SetGuidance("Set the dimensions of a cube for x, y and z. ");
	CubeDimensions_Cmd -> SetUnitCandidates("mm cm m ");
	CubeDimensions_Cmd -> SetDefaultUnit("mm");
	CubeDimensions_Cmd -> SetDefaultValue(G4ThreeVector(0.3*mm, 0.3*mm, 0.0*mm));
	
	//Command to set the position of an object
	TargetPosition_Cmd = new G4UIcmdWith3VectorAndUnit("/Target/position", this);
	TargetPosition_Cmd -> SetGuidance("Set the target position, x, y and z. ");
	TargetPosition_Cmd -> SetUnitCandidates("um mm cm m ");
	TargetPosition_Cmd -> SetDefaultUnit("m");
	TargetPosition_Cmd -> SetDefaultValue(G4ThreeVector(0.0*m, 0.0*m, 0.0*m));

	//Command to set the rotation of an object
	TargetRotation_Cmd = new G4UIcmdWith3VectorAndUnit("/Target/rotation", this);
	TargetRotation_Cmd -> SetGuidance("Set the starting rotation of the targert. ");
	TargetRotation_Cmd -> SetUnitCandidates("deg rad");
	TargetRotation_Cmd -> SetDefaultUnit("deg");
	TargetRotation_Cmd -> SetDefaultValue(G4ThreeVector(0.0*deg, 0.0*deg, 0.0*deg));

	//Command to set the material of an object
	TargetMaterial_Cmd = new G4UIcmdWithAString("/Target/material", this);
	TargetMaterial_Cmd -> SetGuidance("Set the material of the target ");
	TargetMaterial_Cmd -> SetDefaultValue("G4_Al");

	//Command to set the off set radius of an object when it rotates between projections
	OffSetRadius_Cmd = new G4UIcmdWithADoubleAndUnit("/Target/OffSet", this);
	OffSetRadius_Cmd -> SetGuidance("Set the off set of the poisition of the target with a radius");
	OffSetRadius_Cmd -> SetDefaultUnit("cm");
	OffSetRadius_Cmd -> SetDefaultValue(0.5*cm);

	//Command to set the centre of this rotation if there is an offset
	Centre_Cmd = new G4UIcmdWith3VectorAndUnit("/Target/centre", this);
	Centre_Cmd -> SetGuidance("Set the centre for the target to rotate round");
	Centre_Cmd -> SetUnitCandidates("um mm cm m ");
	Centre_Cmd -> SetDefaultUnit("m");
	Centre_Cmd -> SetDefaultValue(G4ThreeVector(0.0*m, 0.0*m, 0.0*m));


	NumberOfObjects_Cmd = new G4UIcmdWithAnInteger("/Target/NumberOfObjects",this);
	NumberOfObjects_Cmd -> SetGuidance("Set the number of objects");
	NumberOfObjects_Cmd -> SetDefaultValue(1);
}

TargetConstructionMessenger::~TargetConstructionMessenger()
{
	delete TargetDirectory;
	delete TypeOfObjects_Cmd;
	delete CubeDimensions_Cmd;
	delete TargetPosition_Cmd;
	delete TargetRotation_Cmd;
	delete TargetMaterial_Cmd;
	delete OffSetRadius_Cmd;
	delete Centre_Cmd;
	delete NumberOfObjects_Cmd;

	G4cout << G4endl << "TargetConstructionMessenger has been deleted ";
}

void TargetConstructionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if(command == TypeOfObjects_Cmd)
	{
		TC -> AddTypeOfObjects(newValue);	
		G4cout << "TargetConstruction -> AddObjectTypes command detected "<< G4endl;
	}
	else if(command == CubeDimensions_Cmd)
	{
		G4ThreeVector Dimensions = CubeDimensions_Cmd -> GetNew3VectorValue(newValue);
		TC -> AddCubeDimensionsArray(Dimensions);	
		G4cout << "TargetConstruction -> AddCubeDimensions command detected "<< G4endl;
	}
	else if(command == TargetPosition_Cmd)
	{
		G4ThreeVector Position = TargetPosition_Cmd -> GetNew3VectorValue(newValue);
		TC -> AddVectorPosition(Position);	
		G4cout << "TargetConstruction -> AddTargetPosition command detected "<< G4endl;
	}
	else if(command == TargetRotation_Cmd)
	{
		G4ThreeVector Rotation = TargetRotation_Cmd -> GetNew3VectorValue(newValue);
		TC -> AddVectorRotation(Rotation);	
		G4cout << "TargetConstruction -> AddTargetRotation command detected "<< G4endl;
	}
	else if(command == TargetMaterial_Cmd )
	{
		TC -> AddMaterial(newValue);	
		G4cout << "TargetConstruction -> AddTargetMaterial command detected "<< G4endl;
	}
	else if(command == OffSetRadius_Cmd )
	{
		OffSetRadius_Cmd -> GetNewUnitValue(newValue);
		TC -> SetOffSetRadius(OffSetRadius_Cmd -> GetNewDoubleValue(newValue));	
		G4cout << "TargetConstruction -> OffSetRadius_Cmd command detected "<< G4endl;
	}
	else if(command == Centre_Cmd)
	{
		G4ThreeVector Centre = Centre_Cmd -> GetNew3VectorValue(newValue);
		TC -> SetCentrePosition(Centre);	
		G4cout << "TargetConstruction -> SetRadius command detected "<< G4endl;
	}
	else if( command == NumberOfObjects_Cmd )
	{
		TC -> SetNumberOfObjects(NumberOfObjects_Cmd -> GetNewIntValue(newValue));	
		G4cout << "TargetConstruction -> SetNumberOfObjects command detected "<< G4endl;
	}
}
