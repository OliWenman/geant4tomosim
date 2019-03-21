//Own classes
#include "Data.hh"
#include "PrimaryGeneratorAction.hh"
#include "PrimaryGeneratorActionMessenger.hh"
#include "G4GeneralParticleSource.hh"
#include "G4VPrimaryGenerator.hh"

//Output to console/write to file
#include "SettingsLog.hh"

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

//To access GPS varabiles
#include "G4SPSPosDistribution.hh"
#include "G4SPSAngDistribution.hh"
#include "G4SPSEneDistribution.hh"

#include "PrintLines.hh"
#include "ProgressTracker.hh"

#include "ParticleBeam.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction(Data* DataObject):G4VUserPrimaryGeneratorAction(), data(DataObject), progress(), beam(NULL), gunMessenger(NULL)
{
	//Create a messenger for this class
  	gunMessenger = new PrimaryGeneratorActionMessenger(this);
	beam = new ParticleBeam();
	
	ShowProgressBar = true;	
	
	//Set the max energy value (in keV)
    eMax = 175;
    data -> SetMaxEnergy(eMax);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
	delete gunMessenger;
	delete beam;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{    
    if(CurrentEvent == 1)
	{   
	    //Cleans the beam energy data at the start of each run    
	    memset(&beamEnergy[0], 0, sizeof(beamEnergy[0]) * Bins);
	    
	    if (CurrentImage == 1)
	    {
	        progress.Timer.Start();	
	    }
    }
    G4ThreeVector particleposition = beam->FireParticle(event);
    
    if (FluoreFM == true) {data -> SetParticlePosition(particleposition);}
    
    //Save beam energy data    	
    int bin = floor(beam->GetEnergyOfEvent()*1000/(eMax/Bins)) -1;
		
    //if (bin < 0)                      {bin = 0;}
    if (bin > beamEnergy.size()) {bin = beamEnergy.size();}

    //G4cout << "\nbin = " << bin << G4endl;

    ++beamEnergy[bin];
    
    if (ShowProgressBar == true && CurrentEvent >= 1) {progress.PrintProgress(CurrentEvent, CurrentImage);}
    
    ++CurrentEvent;
} 

void PrimaryGeneratorAction::ReadOutInfo(SettingsLog& log)
{	
	G4String particle = beam -> GetParticleName();
	/*bool myGun = beam -> UseFastGun();
	G4String gun;
	if (myGun == true) {gun = "G4ParticleGun";}
	else               {gun = "G4GeneralParticleSource";}*/
	bool randomPolarization = beam -> PolarizationRandom();
	
	PrintToEndOfTerminal(log, '-');
	log << "BEAM INFORMATION:"
	       //"\n- Gun type: " << gun
	    << "\n- Particles: " << particle
	    << "\n- Polization: ";
	                          if (randomPolarization) {log << "random";}
	                          else                    {log << beam -> GetPolarization();} 
	/*if (myGun)
	{
	    log << "\n- Halfx = " << G4BestUnit(beam -> GetHalfX(), "Length")
	        << "\n- Halfy = " << G4BestUnit(beam -> GetHalfY(), "Length");
	}*/ 
}

void PrimaryGeneratorAction::SetupGun(G4String GunType, G4double monoEnergy, G4double sigmaEnergy)
{  
   	if (GunType == "Mono")
   	{
   	    beam->UseFastGun(true);
   	    beam->SetBeamEnergy(monoEnergy, 0);
   	}
   	else
   	{
   	    beam->UseFastGun(false);
   	    beam->SetBeamEnergy(monoEnergy, sigmaEnergy);
   	}
   	
   	std::vector<int> ibeamEnergy(Bins, 0);
   	beamEnergy = ibeamEnergy;	
}
