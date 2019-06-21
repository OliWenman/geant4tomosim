#include "AbsorptionSD.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4Track.hh"

AbsorptionSD::AbsorptionSD() : G4VSensitiveDetector("AbsorptionDetecotor"), fHitsCollection(0)
{
    collectionName.insert("AbsorptionHitsCollection");
}

void AbsorptionSD::InitialiseData(bool cleandata)
{
    //Create the dataset if it doesn't exist
    if (absorptionData.empty() || xpixels*ypixels != absorptionData.size())
    {
        int initialValue = 0;
    
        int_vector1D temp(xpixels*ypixels, initialValue);
        absorptionData = temp;
        
        //G4cout << "\nAbsorption data is empty, creating a " << xpixels << " x " << ypixels << " array" << G4endl;
    }
    //Wipe the data clean back to zero if the option is on
    else if (cleandata)
    {
        memset(&absorptionData[0], 0, 
		       sizeof(absorptionData[0]) * xpixels * ypixels);
    }
}

//Set the vector size back to zero
void AbsorptionSD::FreeMemory()
{
    if (!absorptionData.empty())
    {
        absorptionData.clear();
        absorptionData.shrink_to_fit();
    }
}

AbsorptionSD::~AbsorptionSD() {}

//Inherited method from G4VSensitiveDetector
void AbsorptionSD::Initialize(G4HCofThisEvent* hce)
{	
    //If the graphics are on, make a hits collection to display the hit
    if(graphicsOn)
    {
  	    //Create hits collection object
  	    fHitsCollection = new TrackerHitsCollection(SensitiveDetectorName, collectionName[0]); 
  	    //Add this collection in hce
  	    G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  	    hce -> AddHitsCollection( hcID, fHitsCollection ); 
    }
}

//Inherited methods from G4VSensitiveDetector
G4bool AbsorptionSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{  
    //Get the pixel number that the particle entered.
    G4int pixel = aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber();

    //+1 to the hit for the pixel
    ++absorptionData[pixel];
    
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
