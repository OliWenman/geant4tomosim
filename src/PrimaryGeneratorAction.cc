#include "PrimaryGeneratorAction.hh"
#include "PrimaryGeneratorActionMessenger.hh"
#include "DetectorConstruction.hh"
#include "Input.hh"
#include "Data.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"

#include "Randomize.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction(DetectorConstruction* DC_Object, Input* InputObject, Data* DataObject):G4VUserPrimaryGeneratorAction(), DC(DC_Object), input(InputObject), data(DataObject)
{
	//Create a messenger for this class
  	gunMessenger = new PrimaryGeneratorActionMessenger(this, input, data);

	//Set the number of particles for each event
	G4int nofParticles = 1;
  	ParticleGun = new G4ParticleGun(nofParticles);
	
	SetDefaultKinematic();

	
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  	delete ParticleGun;
	delete gunMessenger;
}

void PrimaryGeneratorAction::SetDefaultKinematic()
{
	//Setup which particle is used and its starting conidiions
	gamma = G4ParticleTable::GetParticleTable() -> FindParticle("gamma");

	ParticleGun -> SetParticleDefinition(gamma);
  	ParticleGun -> SetParticleMomentumDirection(G4ThreeVector(1.,0.,0.));	
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
	const double WorldX = -DC -> GetWorldSize().x();

	//Allow the particles to be fired randomly within the beam width
	G4double y0 = BeamWidthY_Cmd * (G4UniformRand()-0.5);
  	G4double z0 = BeamHeightZ_Cmd * (G4UniformRand()-0.5);

	//Set the ParticleGun conditions
	ParticleGun -> SetParticleEnergy(energyCmd);
  	ParticleGun-> SetParticlePosition(G4ThreeVector(WorldX, y0, z0));

	//Generate the particle in the event
  	ParticleGun -> GeneratePrimaryVertex(anEvent);
} 
