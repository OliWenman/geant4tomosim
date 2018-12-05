//Own classes
#include "PrimaryGeneratorAction.hh"
#include "PrimaryGeneratorActionMessenger.hh"

//Used to create an event to fire the particles
#include "G4Event.hh"
#include "G4ParticleGun.hh"

//Used to define particles
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

//Geant4 Units
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4TwoVector.hh"

//Randomize position of the beam
#include "Randomize.hh"

//Read/write to a file
#include <iomanip>
#include <fstream>

PrimaryGeneratorAction::PrimaryGeneratorAction():G4VUserPrimaryGeneratorAction()
{
	//Create a messenger for this class
  	gunMessenger = new PrimaryGeneratorActionMessenger(this);

	//Set the number of particles for each event
	G4int nofParticles = 1;
  	ParticleGun = new G4ParticleGun(nofParticles);

	SetDefaultKinematic();
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  	delete ParticleGun;
	delete gunMessenger;

	//G4RunmManager will delete gamma
}

void PrimaryGeneratorAction::SetDefaultKinematic()
{
	//Setup which particle is used and its starting conidiions
	gamma = G4ParticleTable::GetParticleTable() -> FindParticle("gamma");

	ParticleGun -> SetParticleDefinition(gamma);
  	ParticleGun -> SetParticleMomentumDirection(G4ThreeVector(1, 0, 0));	
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
	//Allow the particles to be fired randomly within the beam width
	G4double y0 = BeamWidthY_Cmd * (G4UniformRand()-0.5);
  	G4double z0 = BeamHeightZ_Cmd * (G4UniformRand()-0.5);

	//Set the ParticleGun conditions
	ParticleGun -> SetParticleEnergy(energyCmd);
	ParticleGun -> SetParticlePosition(G4ThreeVector(-WorldLength, y0, z0));

	//Generate the particle in the event
  	ParticleGun -> GeneratePrimaryVertex(anEvent);
} 

void PrimaryGeneratorAction::ReadOutInfo(G4String SaveFilePath)
{
	G4ThreeVector BeamDimensions = G4ThreeVector(WorldLength, BeamWidthY_Cmd, BeamHeightZ_Cmd);

	G4cout << "\nBEAM INFORMATION: \n"
	       << "\n- Energy of the monochomatic beam is: " << G4BestUnit(energyCmd, "Energy")
	       << "\n- Beam dimensions: " << G4BestUnit(BeamDimensions, "Length") << G4endl;

	//Creation of the writing to data file stream
	std::fstream outdata; 

	G4String SettingsName = "OutputLog.txt";

	//Open the file within the path set
	outdata.open(SaveFilePath+SettingsName, std::fstream::app); 
   	
	//Output error if can't open file
	if( !outdata ) 
	{ 	std::cerr << "\nError: " << SettingsName << " file could not be opened from PrimaryGeneratorAction.\n" << std::endl;
      		exit(1);
   	}

	outdata << "\nBEAM INFORMATION: \n"
	        << "\n- Energy of the monochomatic beam is: " << G4BestUnit(energyCmd, "Energy")
	        << "\n- Beam dimensions: " << BeamWidthY_Cmd << " x " << G4BestUnit(BeamHeightZ_Cmd, "Length") << "\n";

	outdata.close();
}
