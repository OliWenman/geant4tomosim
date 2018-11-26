#include "PrimaryGeneratorAction.hh"
#include "PrimaryGeneratorActionMessenger.hh"
#include "DetectorConstruction.hh"
#include "Data.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"

//Read/write to a file
#include <iomanip>
#include <fstream>

PrimaryGeneratorAction::PrimaryGeneratorAction(DetectorConstruction* DC_Object, Data* DataObject):G4VUserPrimaryGeneratorAction(), DC(DC_Object), data(DataObject)
{
	//Create a messenger for this class
  	gunMessenger = new PrimaryGeneratorActionMessenger(this, data);

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

void PrimaryGeneratorAction::ReadOutInfo(G4String SaveFilePath)
{
	G4cout << "\nBEAM INFORMATION\n"
	       << "\n- Energy of the monochomatic beam is: " << G4BestUnit(energyCmd, "Energy")
	       << "\n- Beam dimensions: " << BeamWidthY_Cmd << " x " << G4BestUnit(BeamHeightZ_Cmd, "Length") << G4endl;

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

	outdata << "\nBEAM INFORMATION\n"
	        << "\n- Energy of the monochomatic beam is: " << G4BestUnit(energyCmd, "Energy")
	        << "\n- Beam dimensions: " << BeamWidthY_Cmd << " x " << G4BestUnit(BeamHeightZ_Cmd, "Length") << "\n";

	outdata.close();
}
