#include "FluorescenceSD.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"

#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"

#include "G4RunManager.hh"
#include "PrimaryGeneratorAction.hh"
#include "G4VUserPrimaryGeneratorAction.hh"

FluorescenceSD::FluorescenceSD(): G4VSensitiveDetector("FluorescenceDetector"), fHitsCollection(0)
{
    fullfieldOn   = true;
    fullmappingOn = true;
    graphicsOn    = false;
    primary       = 0;
    secondary     = 0;
}

FluorescenceSD::~FluorescenceSD()
{
    //if (!fullfieldfluorescence.empty()) {fullfieldfluorescence.clear();}
}

void FluorescenceSD::Initialize(G4HCofThisEvent* hce)
{	
    if(graphicsOn)
    {
  	    //Create hits collection object
  	    fHitsCollection = new TrackerHitsCollection(SensitiveDetectorName, collectionName[0]); 
  	    //Add this collection in hce
  	    G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  	    hce -> AddHitsCollection( hcID, fHitsCollection ); 
    }
}

#include "G4Electron.hh"
G4bool FluorescenceSD::ProcessHits(G4Step* aStep, G4TouchableHistory* histoy)
{
    G4String particle_name = aStep->GetTrack()->GetParticleDefinition()->GetParticleName();

    if (particle_name != "gamma" && particle_name != "opticalphoton"){return false;}

	G4double energy = aStep->GetPreStepPoint()->GetKineticEnergy();

    if (fullfieldOn)
    {
        int energybin = floor(energy*1000/(maxenergy/nbins) -1);
        if (energybin > nbins - 1) {energybin = nbins -1;}

	    ++fullfieldfluorescence[energybin];
    }
    if (fullmappingOn)
    {
        const PrimaryGeneratorAction* pga = dynamic_cast<const PrimaryGeneratorAction*> (G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());

        int energybin = floor(energy*1000/(maxenergy/nbins) -1);
    
        if (energybin > nbins - 1) {energybin = nbins -1;}
               
        G4ThreeVector ipos = pga->GetParticlePosition();
    
        //Get the needed dimenions and positions
        double particlepositionx     = ipos.y();
        double particlepositiony     = ipos.z();
        double halfdimensionx        = absorb_halfdimensions.y();
        double halfdimensiony        = absorb_halfdimensions.z();
        double pixelx_halfdimensions = halfdimensionx/absorb_xpixels;
        double pixely_halfdimensions = halfdimensiony/absorb_ypixels;
    
        int xbin = floor(particlepositionx/(pixelx_halfdimensions*2) + 0.5*absorb_xpixels);
	    int ybin = floor(particlepositiony/(pixely_halfdimensions*2) + 0.5*absorb_ypixels);
    
        ++fullmappingfluorescence[ybin][xbin][energybin];
        
        const G4Track* track = aStep->GetTrack();
        G4int trackID = track->GetTrackID();
        const G4VProcess* process = track->GetCreatorProcess();
        
        G4String process_name;
        
        if (process)
        {
            process_name = process->GetProcessName();
        }
        else
        {
            process_name = "PRIMARY";
            process_name = aStep->GetPreStepPoint()->GetProcessDefinedStep()->GetProcessName();
        }

        //G4double partEnergy = aStep->GetPreStepPoint()->GetKineticEnergy(); 
        //G4double secondEnergy = aStep->GetPostStepPoint()->GetKineticEnergy();
        
        //if (process_name == "phot" && trackID > 3)
        //{
        /*
        G4cout << "\nprocess = " << process_name << G4endl;
        G4cout << "trackid   = " << trackID << G4endl;
        G4cout << "particle  = " << particle_name << G4endl;
        G4cout << "energy    = " << energy << G4endl;
        //}
        */
        if (trackID == 3 && process_name == "phot")
        {
            ++primary;
        }
        else if (trackID > 3 && process_name == "phot")
        {
            ++secondary;
        }
        
    }
	
	if(graphicsOn)
	{
	    //Create the TrackerHit class object to record hits
  	    TrackerHit* newHit = new TrackerHit();

	    //Save all the information about the particle that hit the detector
  	    newHit -> SetChamberNb(0);
	    newHit -> SetEdep(energy);
	    newHit -> SetPos (aStep->GetPreStepPoint()->GetPosition());
	
	    fHitsCollection -> insert( newHit );
	}

	return true;
}

#include "DetectorConstruction.hh"
#include "AbsorptionDetector.hh"

void FluorescenceSD::InitialiseData()
{   
    int initialvalue = 0;

    if (fullfieldOn)
    {
        if (fullfieldfluorescence.empty())
        {
            int_vector1D temp(nbins, initialvalue);
            fullfieldfluorescence = temp;
            
            //G4cout << "\nfullfield fluorescence empty, created" << G4endl;
        }
        else
        {
            memset(&fullfieldfluorescence[0], 
                   0, 
			       sizeof(fullfieldfluorescence[0]) * nbins);
        }
    }
    
    if (fullmappingOn)
    {      
        if (fullmappingfluorescence.empty())
        {
            const DetectorConstruction* detectorconstruction = dynamic_cast<const DetectorConstruction*> (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
            
            const int xpixels = detectorconstruction->GetAbsorptionDetector()->GetxPixels();
            const int ypixels = detectorconstruction->GetAbsorptionDetector()->GetyPixels();
            
            int_vector3D temp (ypixels, int_vector2D
				              (xpixels, int_vector1D
				              (nbins)));
				              
            fullmappingfluorescence = temp;
            
            absorb_xpixels = xpixels;
            absorb_ypixels = ypixels;
            
            absorb_halfdimensions = detectorconstruction->GetAbsorptionDetector()->GetG4HalfDimensions();
            
            //G4cout << "\nfullmapping fluorescence empty, created " << xpixels << " x " << ypixels << " x " << nbins << G4endl;
        }
        else
        {
            std::fill(fullmappingfluorescence.begin(), fullmappingfluorescence.end(), 
		              int_vector2D (absorb_xpixels, 
		              int_vector1D (nbins)));
        }
    }
    
    if (!fullmappingfluorescence.empty() || !fullfieldfluorescence.empty())
    {
        if (energybins.empty())
        {
            double_vector1D temp (nbins, initialvalue);
            
            int energy = 0;
			for (int i = 0 ; i < nbins; i++)
			{
				++energy;
				temp[i] = (maxenergy/nbins)*energy;
			}
			energybins = temp;
        }
    }
}

void FluorescenceSD::FreeMemory()
{
    if (!fullfieldfluorescence.empty())
    {
        fullfieldfluorescence.clear();
        fullfieldfluorescence.shrink_to_fit();
    }
    if (!fullmappingfluorescence.empty())
    {
        fullmappingfluorescence.clear();
        fullmappingfluorescence.shrink_to_fit();
    }
}
