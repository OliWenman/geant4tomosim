#include "PrimaryGeneratorActionMessenger.hh"
#include "PrimaryGeneratorAction.hh"
#include "Data.hh"

#include "G4SystemOfUnits.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh"

PrimaryGeneratorActionMessenger::PrimaryGeneratorActionMessenger(PrimaryGeneratorAction *Gun, Data* DataObject): PGAction(Gun), data(DataObject)
{	
	gunDirectory = new G4UIdirectory("/gun/");
	gunDirectory -> SetGuidance("Commands to control PrimaryGenenatorAction class");

	energyCmd = new G4UIcmdWithADoubleAndUnit("/gun/setEnergy", this);
	energyCmd -> SetGuidance("Set the kinetic energy. ");
  	energyCmd -> SetUnitCandidates("eV keV MeV GeV TeV");
	energyCmd -> SetParameterName("E", true);
	energyCmd -> SetRange("E > 0");

	BeamWidthY_Cmd = new G4UIcmdWithADoubleAndUnit("/gun/setBeamWidth", this);
	BeamWidthY_Cmd -> SetGuidance("Set the width you would like the beam to be. ");
	BeamWidthY_Cmd -> SetUnitCandidates("nm um mm cm m");
	BeamWidthY_Cmd -> SetParameterName("Width", true);
	BeamWidthY_Cmd -> SetRange("Width > 0");

	BeamHeightZ_Cmd = new G4UIcmdWithADoubleAndUnit("/gun/setBeamHeight", this);
	BeamHeightZ_Cmd -> SetGuidance("Set the height you would like the beam to be. ");
	BeamHeightZ_Cmd -> SetUnitCandidates("nm um mm cm m");
	BeamHeightZ_Cmd -> SetParameterName("Height", true);
	BeamHeightZ_Cmd -> SetRange("Height > 0");
}

PrimaryGeneratorActionMessenger::~PrimaryGeneratorActionMessenger()
{
	delete gunDirectory;

	delete energyCmd;

	delete BeamWidthY_Cmd;
	delete BeamHeightZ_Cmd;
}

void PrimaryGeneratorActionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if( command == energyCmd )
  	{ 
		energyCmd -> GetNewUnitValue(newValue);
		PGAction -> SetParticleEnergy(energyCmd -> GetNewDoubleValue(newValue)); 
		data -> SetMaxEnergy(energyCmd -> GetNewDoubleValue(newValue));
	}
	else if( command == BeamWidthY_Cmd )
  	{ 
		BeamWidthY_Cmd -> GetNewUnitValue(newValue);
		PGAction -> SetBeamWidthY(BeamWidthY_Cmd -> GetNewDoubleValue(newValue)); 
	}
	else if( command == BeamHeightZ_Cmd )
  	{ 
		BeamHeightZ_Cmd -> GetNewUnitValue(newValue);
		PGAction -> SetBeamHeightZ(BeamHeightZ_Cmd -> GetNewDoubleValue(newValue)); 
	}
}

