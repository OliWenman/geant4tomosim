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
	BeamDirectory = new G4UIdirectory("/beam/");
	BeamDirectory -> SetGuidance("Commands to control PrimaryGenenatorAction class");

	EnergyCmd = new G4UIcmdWithADoubleAndUnit("/beam/energy/mono", this);
	EnergyCmd -> SetGuidance("Set the kinetic energy. ");
  	EnergyCmd -> SetUnitCandidates("eV keV MeV GeV TeV");
	EnergyCmd -> SetParameterName("E", true);
	EnergyCmd -> SetRange("E > 0");
	
	EnergyDisTypeCmd = new G4UIcmdWithAString("/beam/energy/type", this);
	EnergyDisTypeCmd -> SetGuidance("Set the type of distrubition you want for the energy of the beam. Mono or Gauss, Mono will use G4ParticleGun, Gauss will use G4GeneralParticleSource(GPS)");
	
	EnergySigmaCmd = new G4UIcmdWithADoubleAndUnit("/beam/energy/sigma", this);
	EnergySigmaCmd -> SetGuidance("Pick a sigma value for a guassian distrbution");
	EnergySigmaCmd -> SetUnitCandidates("eV keV MeV GeV TeV");
	EnergySigmaCmd -> SetParameterName("sE", true);
	EnergySigmaCmd -> SetRange("sE >= 0");

	BeamHalfXCmd = new G4UIcmdWithADoubleAndUnit("/beam/position/halfx", this);
	BeamHalfXCmd -> SetGuidance("Set the width you would like the beam to be. ");
	BeamHalfXCmd -> SetUnitCandidates("nm um mm cm m");
	BeamHalfXCmd -> SetParameterName("Width", true);
	BeamHalfXCmd -> SetRange("Width > 0");

	BeamHalfYCmd = new G4UIcmdWithADoubleAndUnit("/beam/position/halfy", this);
	BeamHalfYCmd -> SetGuidance("Set the height you would like the beam to be. ");
	BeamHalfYCmd -> SetUnitCandidates("nm um mm cm m");
	BeamHalfYCmd -> SetParameterName("Height", true);
	BeamHalfYCmd -> SetRange("Height > 0");
}

PrimaryGeneratorActionMessenger::~PrimaryGeneratorActionMessenger()
{
	delete BeamDirectory;

	delete EnergyCmd;
	delete EnergyDisTypeCmd;
	delete EnergySigmaCmd;

	delete BeamHalfXCmd;
	delete BeamHalfYCmd;
}

void PrimaryGeneratorActionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if( command == EnergyCmd )
  	{ 
		EnergyCmd -> GetNewUnitValue(newValue);
		PGAction -> SetParticleEnergy(EnergyCmd -> GetNewDoubleValue(newValue)); 
	}
	else if( command == EnergyDisTypeCmd)
	{
        	PGAction -> SetEnergyDistType(newValue);
	}
	else if( command == EnergySigmaCmd)
	{
        	PGAction -> SetEnergySigma(EnergySigmaCmd -> GetNewDoubleValue(newValue));
	}
	else if( command == BeamHalfXCmd)
  	{ 
		BeamHalfXCmd -> GetNewUnitValue(newValue);
		PGAction -> SetBeamWidthY(BeamHalfXCmd -> GetNewDoubleValue(newValue)); 
	}
	else if( command == BeamHalfYCmd)
  	{ 
		BeamHalfYCmd -> GetNewUnitValue(newValue);
		PGAction -> SetBeamHeightZ(BeamHalfYCmd -> GetNewDoubleValue(newValue)); 
	}
}