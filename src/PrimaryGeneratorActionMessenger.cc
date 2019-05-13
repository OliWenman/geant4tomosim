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
	
	usefastGunCmd = new G4UIcmdWithABool("/beam/gps", this);
	usefastGunCmd -> SetGuidance("Choose which gun to use"
	                             "\nfalse = fast basic particle gun (use '/beam/' commands after)"
	                             "\ntrue = advanced Geant4 GeneralParticleSource (use '/gps/' commands after)"); 	

    //Energy and particles of the beam
	monoEnergyCmd = new G4UIcmdWithADoubleAndUnit("/beam/energy/mono", this);
	monoEnergyCmd -> SetGuidance("Set the kinetic energy. ");
  	monoEnergyCmd -> SetUnitCandidates("eV keV MeV GeV TeV");
	monoEnergyCmd -> SetParameterName("E", true);
	monoEnergyCmd -> SetRange("E > 0");
	
	maxEnergyBinCmd = new G4UIcmdWithADoubleAndUnit("/beam/maxenergy", this);
	maxEnergyBinCmd -> SetGuidance("Set the maximum energy that the data will record");
	
    polizationCmd = new G4UIcmdWith3VectorAndUnit("/beam/polarization", this);
    polizationCmd -> SetGuidance("");
    
    particleCmd = new G4UIcmdWithAString("/beam/particle", this);
	particleCmd -> SetGuidance("Set type of particle you want the beam to use");
	
    //Position, direction and placement of the beam
	beamHalfXCmd = new G4UIcmdWithADoubleAndUnit("/beam/pos/halfx", this);
	beamHalfXCmd -> SetGuidance("Set the width you would like the beam to be. ");
	beamHalfXCmd -> SetUnitCandidates("nm um mm cm m");
	beamHalfXCmd -> SetParameterName("Width", true);
	beamHalfXCmd -> SetRange("Width > 0");

	beamHalfYCmd = new G4UIcmdWithADoubleAndUnit("/beam/pos/halfy", this);
	beamHalfYCmd -> SetGuidance("Set the height you would like the beam to be. ");
	beamHalfYCmd -> SetUnitCandidates("nm um mm cm m");
	beamHalfYCmd -> SetParameterName("Height", true);
	beamHalfYCmd -> SetRange("Height > 0");
    
    centreCoordinates = new G4UIcmdWith3VectorAndUnit("/beam/pos/centre", this);
    centreCoordinates -> SetGuidance("Place the centre of the beam");
    centreCoordinates -> SetUnitCandidates("nm um mm cm m km");
    centreCoordinates -> SetParameterName("x", "y", "z", false, false);
    
    momentumDirection = new G4UIcmdWith3Vector("/beam/momentum", this);
    momentumDirection -> SetGuidance ("Set the direction of the beam");
    momentumDirection -> SetParameterName("px", "py", "pz", false, false);
    momentumDirection -> SetRange("px <= 1 && px >= -1 && py <=1 && py >= -1 && py >= -1 && pz <= 1 && pz >= -1");
    
    numberofbins = new G4UIcmdWithAnInteger("/beam/bins", this);
    
    auto_setupbeam = new G4UIcmdWithABool("/beam/pos/auto", this);
          
}

PrimaryGeneratorActionMessenger::~PrimaryGeneratorActionMessenger()
{
	delete BeamDirectory;
	
	delete usefastGunCmd;

	delete monoEnergyCmd;

	delete beamHalfXCmd;
	delete beamHalfYCmd;
	
	delete particleCmd;
	delete polizationCmd;
	delete maxEnergyBinCmd;
	
	delete centreCoordinates;
	
	delete momentumDirection;
	
	delete numberofbins;
	
	delete auto_setupbeam;
}

void PrimaryGeneratorActionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
    if (command == usefastGunCmd)
    {
        PGAction->GetBeam()->UseFastGun(!usefastGunCmd->GetNewBoolValue(newValue));
    } 
	else if( command == monoEnergyCmd )
  	{ 
		monoEnergyCmd -> GetNewUnitValue(newValue);
		PGAction->GetBeam()->SetMonoEnergy(monoEnergyCmd -> GetNewDoubleValue(newValue)); 
	}
	else if( command == beamHalfXCmd)
  	{ 
		beamHalfXCmd -> GetNewUnitValue(newValue);
		PGAction->GetBeam()->SetHalfX(beamHalfXCmd -> GetNewDoubleValue(newValue)); 
	}
	else if( command == beamHalfYCmd)
  	{ 
		beamHalfYCmd -> GetNewUnitValue(newValue);
		PGAction->GetBeam()->SetHalfX(beamHalfYCmd -> GetNewDoubleValue(newValue)); 
	}
	else if( command == particleCmd)
	{
	    PGAction->GetBeam()->SetParticle(newValue);
	}
	else if( command == polizationCmd)
	{
	    //PGAction -> SetPolization(polizationCmd -> GetNew3VectorValue(newValue));
	} 
	else if(command == maxEnergyBinCmd)
	{
		PGAction -> SetMaxEnergyBinCmd(maxEnergyBinCmd -> GetNewDoubleValue(newValue)); 
	}
	else if (command == centreCoordinates)
	{
	    PGAction -> GetBeam()-> SetCentreCoordinates(centreCoordinates->GetNew3VectorValue(newValue));
	}
	else if (command == momentumDirection)
	{
	    PGAction->GetBeam()->SetMomentumDirection(momentumDirection->GetNew3VectorValue(newValue));
	}   
	else if (command == numberofbins)
	{
	    PGAction->SetNumberOfBins(numberofbins->GetNewIntValue(newValue));
	}
	else if( command == auto_setupbeam)
	{
	    PGAction->auto_posbeam = auto_setupbeam->GetNewBoolValue(newValue);
	}
}
