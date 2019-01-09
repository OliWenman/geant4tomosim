#include "PrimaryGeneratorActionMessenger.hh"
#include "PrimaryGeneratorAction.hh"

#include "G4SystemOfUnits.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh"

PrimaryGeneratorActionMessenger::PrimaryGeneratorActionMessenger(PrimaryGeneratorAction *Gun): PGAction(Gun)
{	
	gunDirectory = new G4UIdirectory("/beam/");
	gunDirectory -> SetGuidance("Commands to control PrimaryGenenatorAction class");

	energyCmd = new G4UIcmdWithADoubleAndUnit("/beam/energy/mono", this);
	energyCmd -> SetGuidance("Set the kinetic energy. ");
  	energyCmd -> SetUnitCandidates("eV keV MeV GeV TeV");
	energyCmd -> SetParameterName("E", true);
	energyCmd -> SetRange("E > 0");
	
	EnergyDisTypeCmd = new G4UIcmdWithAString("/beam/energy/type", this);
	EnergyDisTypeCmd -> SetGuidance("Set the type of distrubition you want for the energy of the beam. Mono or Gauss, Mono will use G4ParticleGun, Gauss will use G4GeneralParticleSource(GPS)");
	
	EnergySigmaCmd = new G4UIcmdWithADoubleAndUnit("/beam/energy/sigma", this);
	EnergySigmaCmd -> SetGuidance("Pick a sigma value for a guassian distrbution");
	EnergySigmaCmd -> SetUnitCandidates("eV keV MeV GeV TeV");
	EnergySigmaCmd -> SetParameterName("sE", true);
	EnergySigmaCmd -> SetRange("sE >= 0");

	BeamWidthY_Cmd = new G4UIcmdWithADoubleAndUnit("/beam/position/halfx", this);
	BeamWidthY_Cmd -> SetGuidance("Set the width you would like the beam to be. ");
	BeamWidthY_Cmd -> SetUnitCandidates("nm um mm cm m");
	BeamWidthY_Cmd -> SetParameterName("Width", true);
	BeamWidthY_Cmd -> SetRange("Width > 0");

	BeamHeightZ_Cmd = new G4UIcmdWithADoubleAndUnit("/beam/position/halfy", this);
	BeamHeightZ_Cmd -> SetGuidance("Set the height you would like the beam to be. ");
	BeamHeightZ_Cmd -> SetUnitCandidates("nm um mm cm m");
	BeamHeightZ_Cmd -> SetParameterName("Height", true);
	BeamHeightZ_Cmd -> SetRange("Height > 0");
}

PrimaryGeneratorActionMessenger::~PrimaryGeneratorActionMessenger()
{
	delete gunDirectory;

	delete energyCmd;
	delete EnergyDisTypeCmd;
	delete EnergySigmaCmd;

	delete BeamWidthY_Cmd;
	delete BeamHeightZ_Cmd;
}

void PrimaryGeneratorActionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if( command == energyCmd )
  	{ 
		energyCmd -> GetNewUnitValue(newValue);
		PGAction -> SetParticleEnergy(energyCmd -> GetNewDoubleValue(newValue)); 
	}
	else if( command == EnergyDisTypeCmd)
	{
        	PGAction -> SetEnergyDistType(newValue);
	}
	else if( command == EnergySigmaCmd)
	{
        	PGAction -> SetEnergySigma(EnergySigmaCmd -> GetNewDoubleValue(newValue));
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

