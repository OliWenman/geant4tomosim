#include "AbsorptionSD.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4Track.hh"

AbsorptionSD::AbsorptionSD() : G4VSensitiveDetector("AbsorptionDetecotor"), fHitsCollection(0)
{

}

void AbsorptionSD::InitialiseData(int xpix, int ypix)
{
    if (absorptionData.empty())
    {
        xpixels = xpix;
        ypixels = ypix;
    
        int initialValue = 0;
    
        int_vector1D temp(xpixels*ypixels, initialValue);
        absorptionData = temp;
        
        G4cout << "\nAbsorption data is empty, creating a " << xpixels << " x " << ypixels << " array" << G4endl;
    }
    else
    {
        memset(&absorptionData[0], 0, 
		       sizeof(absorptionData[0]) * xpixels * ypixels);
    }
}

void AbsorptionSD::FreeMemory()
{
    if (!absorptionData.empty())
    {
        absorptionData.clear();
        absorptionData.shrink_to_fit();
    }
    
    if (absorptionData.empty())
    {
        G4cout << "\nworked" << G4endl;
    }
    else
    {
        G4cout << "\nDidn't work" << G4endl;
    }
}

AbsorptionSD::~AbsorptionSD() {}

void AbsorptionSD::Initialize(G4HCofThisEvent* hce)
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

G4bool AbsorptionSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{  
    G4int pixel = aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber();

    ++absorptionData[pixel];
    
   /*
    
    //G4cout << n << ")ABSORPTION HIT " << G4endl;
    //++n;
	G4int nDetector = aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber();
	//Save the detector hits to the data class
	data -> SaveAbsorption(nDetector);
	*/
	if(graphicsOn)
	{
	    //Create the TrackerHit class object to record hits
  	    TrackerHit* newHit = new TrackerHit();

	    //Save all the information about the particle that hit the detector
  	    newHit -> SetChamberNb(pixel);
	    newHit -> SetEdep(aStep->GetPreStepPoint()->GetKineticEnergy());
	    newHit -> SetPos (aStep->GetPreStepPoint()->GetPosition());
	
	    fHitsCollection -> insert( newHit );
	}
    
  	return true;
}
