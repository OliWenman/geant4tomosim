#include "FluorescenceSD.hh"
#include "Data.hh"

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

FluorescenceSD::FluorescenceSD (): G4VSensitiveDetector("FluorescenceDetector"), fHitsCollection(0)
{
    fullfieldOn   = true;
    fullmappingOn = true;
    graphicsOn    = false;
}

FluorescenceSD::FluorescenceSD(Data* DataObject, bool graphics) : G4VSensitiveDetector("FluorescenceDetector"), fHitsCollection(NULL), data(DataObject)
{
	fullfieldOn = data -> GetFluorescence_Option();
	fullmappingOn = data -> GetFullMapping_Option();
	
	graphicsOn = graphics;
	if(graphicsOn){collectionName.insert("FluorescenceHitsCollection");}
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

G4bool FluorescenceSD::ProcessHits(G4Step* aStep, G4TouchableHistory* histoy)
{
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
    
        int xbin = floor(ipos.y()/(pga->GetBeamHalfx()*2/absorb_xpixels) + 0.5*absorb_xpixels -1);
	    int ybin = floor(ipos.z()/(pga->GetBeamHalfy()*2/absorb_ypixels) + 0.5*absorb_ypixels -1);
    
        ++fullmappingfluorescence[xbin][ybin][energybin];
    }
	//if (fullmappingOn){data -> SaveFullMapping(energy);}
	//if (fullfieldOn)  {data -> SaveFluorescence(energy);}
	
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

void FluorescenceSD::InitialiseData()
{   
    int initialvalue = 0;

    if (fullfieldOn)
    {
        if (fullfieldfluorescence.empty())
        {
            int_vector1D temp(nbins, initialvalue);
            fullfieldfluorescence = temp;
            
            G4cout << "\nfullfield fluorescence empty, created" << G4endl;
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
            
            int xpixels = detectorconstruction->GetAbsorptionDetector_xpixels();
            int ypixels = detectorconstruction->GetAbsorptionDetector_ypixels();
            
            int_vector3D temp (xpixels, int_vector2D
				              (ypixels, int_vector1D
				              (nbins)));
				              
            fullmappingfluorescence = temp;
            
            absorb_xpixels = xpixels;
            absorb_ypixels = ypixels;
            
            G4cout << "\nfullmapping fluorescence empty, created " << xpixels << " x " << ypixels << " x " << nbins << G4endl;
        }
        else
        {
            std::fill(fullmappingfluorescence.begin(), fullmappingfluorescence.end(), 
		              int_vector2D (absorb_ypixels, 
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
