#include "PrimaryGeneratorAction.hh"
#include "PrimaryGeneratorActionMessenger.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"

#include "Randomize.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction():G4VUserPrimaryGeneratorAction()
{
	//Create a messenger for this class
  	gunMessenger = new PrimaryGeneratorActionMessenger(this);

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
	G4ParticleDefinition* particleDefinition = G4ParticleTable::GetParticleTable()->FindParticle("gamma");
	ParticleGun->SetParticleDefinition(particleDefinition);
  	ParticleGun->SetParticleMomentumDirection(G4ThreeVector(-1.,0.,0.));	
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
	G4double x0 = ParticleX_Cmd;
	G4double y0 = BeamWidthY_Cmd * (G4UniformRand()-0.5);
  	G4double z0 = BeamHeightZ_Cmd * (G4UniformRand()-0.5);

	ParticleGun->SetParticleEnergy(energyCmd);
  
  	ParticleGun->SetParticlePosition(G4ThreeVector(x0,y0, z0));

  	ParticleGun->GeneratePrimaryVertex(anEvent);
}	

