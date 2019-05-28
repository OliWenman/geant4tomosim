//Own classes
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

PrimaryGeneratorAction::PrimaryGeneratorAction():G4VUserPrimaryGeneratorAction(), progress(), 
                                                                                  beam(0), 
                                                                                  gunMessenger(0)
{
	//Create a messenger for this class
  	gunMessenger = new PrimaryGeneratorActionMessenger(this);
	beam = new ParticleBeam();
	
	//Set the max energy value (in keV)
    eMax = 175.;
    auto_posbeam = true;
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
	delete gunMessenger;
	delete beam;
}

#include "G4SolidStore.hh"
#include "G4Box.hh"

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{    
    if(currentevent == 0)
	{   
	    //Cleans the beam energy data at the start of each run    
	    memset(&beamintensity[0], 
	           0, 
	           sizeof(beamintensity[0]) * bins);
	    G4cout << std::flush;
	    if (currentrun == 1)
	    {
	        progress.Timer.Start();
	        
	        //Automatically set the beam dimensions to be the same as the absorption detector dimensions if option is on
	        //and place it at the end of the world volume
	        if (!beam->CheckIfFan() && auto_posbeam)
	        {
	            const G4Box* absdet = dynamic_cast<const G4Box*> (G4SolidStore::GetInstance()->GetSolid("Absorptioncontainer"));
	        
	            if (absdet)
	            {
	                beam->SetHalfX(absdet->GetZHalfLength());
	                beam->SetHalfY(absdet->GetYHalfLength());
	            }   
	            
	            const G4Box* world = dynamic_cast<const G4Box*> (G4SolidStore::GetInstance()->GetSolid("World"));
	        
	            if (world)
	            {
	                beam->DoAutoSourcePlacement(-world->GetXHalfLength());
	            }
	        }
	    }
    }
    particleposition = beam->FireParticle(event);
    //Save beam energy data    	
    int bin = floor(beam->GetEnergyOfEvent()*1000/(eMax/bins)) -1;
    if (bin > beamintensity.size()) {bin = beamintensity.size();}

    ++beamintensity[bin];

    ++currentevent;
    progress.PrintProgress(currentevent, currentrun);
} 

void PrimaryGeneratorAction::ReadOutInfo(SettingsLog& log)
{	
	G4String particle = beam -> GetParticleName();
	bool myGun = beam -> UseFastGun();
	G4String gun;
	if (myGun == true) {gun = "G4ParticleGun";}
	else               {gun = "G4GeneralParticleSource";}
	bool randomPolarization = beam -> PolarizationRandom();
	
	PrintToEndOfTerminal(log, '-');
	log << "BEAM INFORMATION:"
	       "\n- Gun type: " << gun
	    << "\n- Particles: " << particle
	    << "\n- Polization: ";
	                          if (randomPolarization) {log << "random";}
	                          else                    {log << beam -> GetPolarization();} 
	if (beam->UseFastGun())
	{
	    log << "\n- Mono energy: " << G4BestUnit(beam->GetMonoEnergy(), "Energy");
	}
	else 
	{
	    G4String enetype = beam->GetAdvaGun()->GetCurrentSource()->GetEneDist()->GetEnergyDisType();
	    log << "\n- Energy distrubition type: " << enetype;
	    
	    if (enetype == "Gauss" || enetype == "Mono")
	    {
	        log << "\n- Mono energy: " << G4BestUnit(beam->GetMonoEnergy(), "Energy");
	        if (enetype == "Gauss")
	        {
	            log << "\n- Sigma energy: " << G4BestUnit(beam->GetAdvaGun()->GetCurrentSource()->GetEneDist()->GetSE(), "Energy");
	        }
	    }
	}
	log << "\n- Momentum: " << beam -> GetMomentum(); 
	log << "\n- Centre: " << G4BestUnit(beam -> GetCentreCoordinates(), "Length");
	
	if (beam->UseFastGun())
	{
	    log << "\n- Halfx = " << G4BestUnit(beam -> GetHalfX(), "Length")
	        << "\n- Halfy = " << G4BestUnit(beam -> GetHalfY(), "Length");
	}
	else
	{
	    if (!beam->CheckIfFan())
	    {
	        log << "\n- Halfx = " << G4BestUnit(beam -> GetHalfX(), "Length")
	            << "\n- Halfy = " << G4BestUnit(beam -> GetHalfY(), "Length");
	    }
	} 
}

void PrimaryGeneratorAction::SetupData()
{  	
   	int_vector1D    ibeamintensity(bins, 0);
   	double_vector1D ienergy(bins, 0);
   	
    int ene = 0;
	for (int i = 0 ; i < bins ; i++)
	{
	    ++ene;
		ienergy[i] = (eMax/bins)*ene;
	}

    beamintensity = ibeamintensity;	
	energy        = ienergy;
}
