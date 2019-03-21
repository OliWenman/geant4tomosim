#include "PrimaryGeneratorActionMessenger.hh"
#include "PrimaryGeneratorAction.hh"

#include "G4SystemOfUnits.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWith3Vector.hh"
#include "ParticleBeam.hh"

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

	BeamHalfXCmd = new G4UIcmdWithADoubleAndUnit("/beam/pos/halfx", this);
	BeamHalfXCmd -> SetGuidance("Set the width you would like the beam to be. ");
	BeamHalfXCmd -> SetUnitCandidates("nm um mm cm m");
	BeamHalfXCmd -> SetParameterName("Width", true);
	BeamHalfXCmd -> SetRange("Width > 0");

	BeamHalfYCmd = new G4UIcmdWithADoubleAndUnit("/beam/pos/halfy", this);
	BeamHalfYCmd -> SetGuidance("Set the height you would like the beam to be. ");
	BeamHalfYCmd -> SetUnitCandidates("nm um mm cm m");
	BeamHalfYCmd -> SetParameterName("Height", true);
	BeamHalfYCmd -> SetRange("Height > 0");
	
	particleCmd = new G4UIcmdWithAString("/beam/particle", this);
	particleCmd -> SetGuidance("Set type of particle you want the beam to use");
	
	MaxEnergyBinCmd = new G4UIcmdWithADoubleAndUnit("/beam/MaxEnergy", this);
	MaxEnergyBinCmd -> SetGuidance("Set the maximum energy bin");
	
    SetPolizationCmd = new G4UIcmdWith3VectorAndUnit("/beam/polarization", this);
    SetPolizationCmd -> SetGuidance("");
    
    autoPlacement = new G4UIcmdWithABool("/beam/autoposition", this);
    autoPlacement -> SetGuidance("Automatically place the beam at the edge of the world in the -x direction");
    
    centreCoordinates = new G4UIcmdWith3VectorAndUnit("/beam/pos/centre", this);
    centreCoordinates -> SetGuidance("Place the centre of the beam");
    centreCoordinates -> SetUnitCandidates("nm um mm cm m km");
    centreCoordinates -> SetParameterName("x", "y", "z", false, false);
    
    momentumDirection = new G4UIcmdWith3Vector("/beam/momentum", this);
    momentumDirection -> SetGuidance ("Set the direction of the beam");
    momentumDirection -> SetParameterName("px", "py", "pz", false, false);
    momentumDirection -> SetRange("px <= 1 && px >= -1 && py <=1 && py >= -1 && py >= -1 && pz <= 1 && pz >= -1");
}

PrimaryGeneratorActionMessenger::~PrimaryGeneratorActionMessenger()
{
	delete BeamDirectory;

	delete EnergyCmd;
	delete EnergyDisTypeCmd;
	delete EnergySigmaCmd;

	delete BeamHalfXCmd;
	delete BeamHalfYCmd;
	
	delete particleCmd;
	delete SetPolizationCmd;
	delete MaxEnergyBinCmd;
	
	delete autoPlacement;
	delete centreCoordinates;
	
	delete momentumDirection;
}

void PrimaryGeneratorActionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if( command == EnergyCmd )
  	{ 
		EnergyCmd -> GetNewUnitValue(newValue);
		PGAction->GetBeam()->SetBeamMonoEnergy(EnergyCmd -> GetNewDoubleValue(newValue)); 
	}
	else if( command == EnergyDisTypeCmd)
	{
        //PGAction -> SetEnergyDistType(newValue);
	}
	else if( command == EnergySigmaCmd)
	{
        //PGAction -> SetEnergySigma(EnergySigmaCmd -> GetNewDoubleValue(newValue));
	}
	else if( command == BeamHalfXCmd)
  	{ 
		BeamHalfXCmd -> GetNewUnitValue(newValue);
		PGAction->GetBeam()->SetHalfX(BeamHalfXCmd -> GetNewDoubleValue(newValue)); 
	}
	else if( command == BeamHalfYCmd)
  	{ 
		BeamHalfYCmd -> GetNewUnitValue(newValue);
		PGAction->GetBeam()->SetHalfX(BeamHalfYCmd -> GetNewDoubleValue(newValue)); 
	}
	else if( command == particleCmd)
	{
	    PGAction->GetBeam()->SetParticle(newValue);
	}
	else if( command == SetPolizationCmd)
	{
	    //PGAction -> SetPolization(SetPolizationCmd -> GetNew3VectorValue(newValue));
	} 
	else if(command == MaxEnergyBinCmd)
	{
		PGAction -> SetMaxEnergyBinCmd(MaxEnergyBinCmd -> GetNewDoubleValue(newValue)); 
	}
	else if (command == autoPlacement)
	{
	    PGAction -> SetAutoBeamPlacement(autoPlacement->GetNewBoolValue(newValue));
	}
	else if (command == centreCoordinates)
	{
	    PGAction -> GetBeam()-> SetCentreCoordinates(centreCoordinates->GetNew3VectorValue(newValue));
	}
	else if (command == momentumDirection)
	{
	    PGAction ->GetBeam()->SetMomentumDirection(momentumDirection->GetNew3VectorValue(newValue));
	}   
}
