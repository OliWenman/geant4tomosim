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

	MasterTargetMaterial_Cmd = new G4UIcmdWithAString("/Target/material", this);
	MasterTargetMaterial_Cmd -> SetGuidance("Set the material of the target ");
	MasterTargetMaterial_Cmd -> SetDefaultValue("G4_Al");

	MasterOffSetRadius_Cmd = new G4UIcmdWithADoubleAndUnit("/Target/OffSet", this);
	MasterOffSetRadius_Cmd -> SetGuidance("Set the off set of the poisition of the target with a radius");
	MasterOffSetRadius_Cmd -> SetDefaultUnit("cm");
	MasterOffSetRadius_Cmd -> SetDefaultValue(0.5*cm);

	NumberOfObjects_Cmd = new G4UIcmdWithAnInteger("/Target/NumberOfObjects",this);
	NumberOfObjects_Cmd -> SetGuidance("Set the number of objects");
	NumberOfObjects_Cmd -> SetDefaultValue(1);
	
}

TargetConstructionMessenger::~TargetConstructionMessenger()
{
	delete TargetDirectory;
	delete TargetPosition_Cmd;
	delete MasterTargetMaterial_Cmd;
	delete MasterOffSetRadius_Cmd;
	delete NumberOfObjects_Cmd;

	G4cout << G4endl << "TargetConstructionMessenger has been deleted "<< G4endl;
}

void TargetConstructionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if( command == TargetPosition_Cmd )
	{
		TC -> SetTargetPosition(TargetPosition_Cmd -> GetNew3VectorValue(newValue));	
		G4cout << "TargetConstruction -> SetTargetPosition command detected "<< G4endl;
	}
	else if(command == MasterTargetMaterial_Cmd )
	{
		TC -> SetMasterTargetMaterial(newValue);	
		G4cout << "TargetConstruction -> MasterSetTargetMaterial command detected "<< G4endl;
	}
	else if(command == MasterOffSetRadius_Cmd )
	{
		MasterOffSetRadius_Cmd -> GetNewUnitValue(newValue);
		TC -> SetMasterOffSetRadius(MasterOffSetRadius_Cmd -> GetNewDoubleValue(newValue));	
		G4cout << "TargetConstruction -> MasterOffSetRadius_Cmd command detected "<< G4endl;
	}
	else if( command == NumberOfObjects_Cmd )
	{
		TC -> SetNumberOfObjects(NumberOfObjects_Cmd -> GetNewIntValue(newValue));	
		G4cout << "TargetConstruction -> SetNumberOfObjects command detected "<< G4endl;
	}
}
