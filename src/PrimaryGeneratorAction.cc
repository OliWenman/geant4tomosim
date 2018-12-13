//Own classes
#include "Data.hh"
#include "PrimaryGeneratorAction.hh"
#include "PrimaryGeneratorActionMessenger.hh"
#include "G4GeneralParticleSource.hh"

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

#include "G4SPSPosDistribution.hh"
#include "G4SPSAngDistribution.hh"
#include "G4SPSEneDistribution.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction(Data* DataObject):G4VUserPrimaryGeneratorAction(), data(DataObject)
{
	//Create a messenger for this class
  	gunMessenger = new PrimaryGeneratorActionMessenger(this);

	//Set the number of particles for each event
  	//ParticleGun = new G4ParticleGun(1);
	ParticleGun = new G4GeneralParticleSource();

	BeamCheck = false;
	BeamData = false;

	//Setup which particle is used and its starting conidiions
	gamma = G4ParticleTable::GetParticleTable() -> FindParticle("gamma");

	ParticleGun -> SetParticleDefinition(gamma);
  	//ParticleGun -> SetParticleMomentumDirection(G4ThreeVector(1, 0, 0));	
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  	delete ParticleGun;
	delete gunMessenger;

	//G4RunManager will delete gamma
}

void PrimaryGeneratorAction::PrintProgress()
{
	++CurrentEvent;

	//Works out the percentage of how many events it has completed
	int ImageProgress = (double(CurrentEvent)/NumberOfEvents)*100;

	//Prints only at the start of the simulation
	if(CurrentEvent == 1 && CurrentImage == 1)
	{	G4cout << "\n================================================================================"
		          "\n                            SIMULATION RUNNING..."
	                  "\n================================================================================\n\n" << G4endl;
	}

	//Only prints the percentage if the image number has changed
	if (ImageProgress != NumberCheck)
	{	//Calculates the total progress of the simulation
		int FullProgress = double(CurrentImage - 1)/NumberOfRuns*100;
		TotalProgress = FullProgress + (double(ImageProgress)/100 * (1./NumberOfRuns)*100);

		//Prints above one line and over rides it
		G4cout << "\033[1A" "\033[K" "\rImage " << CurrentImage << ": " << ImageProgress << "%\ complete"  "\n\rTotal progress: " << TotalProgress << "\%" << std::flush;
		NumberCheck = ImageProgress;
	}

	//Corrects the end perecentage to 100% once simulation is complete and outputs a space
	if(CurrentEvent == NumberOfEvents && CurrentImage == NumberOfRuns)
		G4cout << "\033[1A" "\033[K" "\rImage " << CurrentImage << ": " << "100%\ complete"  "\n\rTotal progress: 100\%" << G4endl;

}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
	//Generate the particle in the event
  	ParticleGun -> GeneratePrimaryVertex(anEvent);
	
	if (FluoreFM == true)
		data -> SetParticlePosition(ParticleGun -> GetParticlePosition());

	if (BeamCheck == false && BeamData == true)
	{	//Create a graph of the beam energy, only for the first image
		int bins = floor(ParticleGun -> GetParticleEnergy()*1000/(eMax/Bins));
		if (bins >= Bins)
			bins = Bins - 1;

		++BeamEnergyFreq[bins];
	}

	PrintProgress();
} 

void PrimaryGeneratorAction::ReadOutInfo(G4String SaveFilePath)
{
	G4ThreeVector BeamDimensions = G4ThreeVector(0, BeamWidthY_Cmd, BeamHeightZ_Cmd);

	G4cout << "\nBEAM INFORMATION: \n"
	       << "\n- Energy of the monochomatic beam is: " << G4BestUnit(energyCmd, "Energy")
	       << "\n- Beam dimensions: " << G4BestUnit(BeamDimensions, "Length") << G4endl;

	//Creation of the writing to data file stream
	std::fstream outdata; 

	//Open the file within the path set
	outdata.open(SaveFilePath, std::fstream::app); 
   	
	//Output error if can't open file
	if( !outdata ) 
	{ 	std::cerr << "\nError: " << SaveFilePath << " file could not be opened from PrimaryGeneratorAction.\n" << std::endl;
      		exit(1);
   	}

	outdata << "\nBEAM INFORMATION: \n"
	        << "\n- Energy of the monochomatic beam is: " << G4BestUnit(energyCmd, "Energy")
	        << "\n- Beam dimensions: " << BeamWidthY_Cmd << " x " << G4BestUnit(BeamHeightZ_Cmd, "Length") << "\n";

	outdata.close();
}

void PrimaryGeneratorAction::SetValues(int nBins, double Position)
{
	//Automatically set the gun position to the end of the world volume
	ParticleGun -> GetCurrentSource() -> GetPosDist() -> SetCentreCoords(G4ThreeVector(-Position, 0, 0));

	//Automatically set the gun momentum in the positive x direction
	ParticleGun -> GetCurrentSource() -> GetAngDist() -> SetParticleMomentumDirection(G4ThreeVector(1, 0, 0));

	//Find the energy sigma value for the distrubition (in keV)
	G4double eSigma = ParticleGun -> GetCurrentSource() -> GetEneDist() -> GetSE() * 1000;

	//Find the main energy value (in keV)
	G4double eMono = ParticleGun -> GetCurrentSource() -> GetEneDist() -> GetMonoEnergy() * 1000;

	//Set the max energy value (in keV)
	eMax = (eMono + eSigma) * 2;

	std::vector<int> iBeamEnergyFreq(nBins, 0);
	BeamEnergyFreq = iBeamEnergyFreq;

	Bins = nBins;
}
