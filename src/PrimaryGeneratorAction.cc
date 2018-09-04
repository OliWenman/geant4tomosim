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

PrimaryGeneratorAction::PrimaryGeneratorAction(DetectorConstruction* DC_Object, Data* DataObject):G4VUserPrimaryGeneratorAction(), DC(DC_Object), data(DataObject)
{
	G4cout << G4endl << "PrimaryGeneratorAction has been created ";

	//Create a messenger for this class
  	gunMessenger = new PrimaryGeneratorActionMessenger(this, data);

	//Set the number of particles for each event
	G4int nofParticles = 1;
  	ParticleGun = new G4ParticleGun(nofParticles);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
	//Free up memory
  	delete ParticleGun;
	delete gunMessenger;
	G4cout << G4endl << "PrimaryGeneratorAction has been deleted " << G4endl;
}

void PrimaryGeneratorAction::SetDefaultKinematic()
{
	//Setup which particle is used and its starting conidiions
	
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

	gamma = particleTable -> FindParticle("gamma");
	electron = particleTable -> FindParticle("e-");
	positron = particleTable -> FindParticle("e+");

	ParticleGun -> SetParticleDefinition(gamma);
  	ParticleGun -> SetParticleMomentumDirection(G4ThreeVector(-1.,0.,0.));	
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
	SetDefaultKinematic();
	//Allow the particles to be fired randomly within the beam width
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
