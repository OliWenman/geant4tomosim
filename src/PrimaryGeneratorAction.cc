#include "PrimaryGeneratorAction.hh"
#include "PrimaryGeneratorActionMessenger.hh"
#include "DetectorConstruction.hh"
#include "Data.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"

#include "Randomize.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction(DetectorConstruction* DC_Object):G4VUserPrimaryGeneratorAction(), DC(DC_Object)
{
	G4cout << G4endl << "PrimaryGeneratorAction has been created ";

	//Create a messenger for this class
  	gunMessenger = new PrimaryGeneratorActionMessenger(this);

	//Set the number of particles for each event
	G4int nofParticles = 1;
  	ParticleGun = new G4ParticleGun(nofParticles);

	SetDefaultKinematic();
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
	//Free up memory
  	delete ParticleGun;
	delete gunMessenger;
}

void PrimaryGeneratorAction::SetDefaultKinematic()
{
	//Setup which particle is used and its starting conidiions
	G4ParticleDefinition* particleDefinition = G4ParticleTable::GetParticleTable() -> FindParticle("gamma");
	ParticleGun -> SetParticleDefinition(particleDefinition);
  	ParticleGun -> SetParticleMomentumDirection(G4ThreeVector(-1.,0.,0.));	
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
	//Allow the particles to be fired randomly within the beam width
	//G4double x0 = ParticleX_Cmd;
	G4ThreeVector WorldSize = DC -> GetWorldSize();
	G4double x0 = WorldSize.x();
	G4double y0 = BeamWidthY_Cmd * (G4UniformRand()-0.5);
  	G4double z0 = BeamHeightZ_Cmd * (G4UniformRand()-0.5);

	//Apply the commands for starting energy and positon 
	ParticleGun -> SetParticleEnergy(energyCmd);

  	ParticleGun->SetParticlePosition(G4ThreeVector(x0,y0, z0));

	//Generate the particle in the event
  	ParticleGun -> GeneratePrimaryVertex(anEvent);
}	

