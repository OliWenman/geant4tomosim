#include "TrackerSD.hh"
#include "Data.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"

TrackerSD::TrackerSD(const G4String& name, const G4String& hitsCollectionName, G4int NumDetectorsY, G4int NumDetectorsZ, Data* DataObject, G4bool DetectorEfficiency) 
          : G4VSensitiveDetector(name), fHitsCollection(NULL), data(DataObject)
{
	G4cout << G4endl << "TrackerSD has been created "<< G4endl;

	//Setup the data only at the begining of simulation
	if (data -> GetCurrentImage() == 0)
	{			
		//Save the variables into its own class
		SetDetectorEfficiency(DetectorEfficiency);
		data -> SetDetectorEfficiency(DetectorEfficiency);

		data -> SetUpHitData(NumDetectorsZ, NumDetectorsY);
		data -> SetUpEnergyData();
	}
	
	collectionName.insert(hitsCollectionName);
}

TrackerSD::~TrackerSD() {G4cout << G4endl << "TrackerSD has been deleted ";}

void TrackerSD::Initialize(G4HCofThisEvent* hce)
{	
	if (data ->GetVisualization() == true || verboseLevel > 1)
	{
  		//Create hits collection object
  		fHitsCollection = new TrackerHitsCollection(SensitiveDetectorName, collectionName[0]); 

  		//Add this collection in hce
  		G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  		hce -> AddHitsCollection( hcID, fHitsCollection ); 
	}
}

G4bool TrackerSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{  
	G4double edep;
	if (GetDetectorEfficiency() == false)
	{
  		// energy deposit
  		edep = aStep->GetTotalEnergyDeposit();

  		if (edep==0.) return false;
	}
	else 
		{edep = aStep-> GetTrack()-> GetTotalEnergy();}

	//Create the TrackerHit class object to record hits
  	TrackerHit* newHit = new TrackerHit();

	if (fHitsCollection)
	{
		//Save all the information about the particle that hit the detector
  		//newHit -> SetTrackID  (aStep->Gep->GetetTrack()->GetTrackID());
		//newHit -> SetParticle( aStTrack()->GetDefinition() );
  		newHit -> SetChamberNb(aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber());
		newHit -> SetEdep(edep);
	
		//Needed for visualization of hits, only need it if it's turned on
	
		newHit -> SetPos (aStep->GetPostStepPoint()->GetPosition());
	
		fHitsCollection -> insert( newHit );
	}
	else 
		{delete newHit;}
	//Save the detector hits to the data class
	data -> SaveHitData(aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber());
	data-> SaveEnergyData(aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber(), edep);

  	return true;
}
 
void TrackerSD::EndOfEvent(G4HCofThisEvent*)
{ 
	if (verboseLevel > 0)
	{	
  		const G4Event* evt = G4RunManager::GetRunManager() -> GetCurrentEvent();

		if ( verboseLevel == 1 ) 
			{ G4cout << G4endl << "Event " << evt -> GetEventID();}
		
  		else if ( verboseLevel > 1)
		{
			G4int nofHits = fHitsCollection -> entries();

			G4cout << G4endl << "Event " << evt -> GetEventID();
			if (nofHits == 0)
				{G4cout << ": No hit ";}
			if (nofHits > 0)
			{	//If there's a hit, print it if the verbose setting is greater than 1
				for (G4int i = 0; i < nofHits; i++ ) 
					{(*fHitsCollection)[i] -> Print();}
			
			}
  		}
	}
}



//149.56s 10,000,000 photons
//146.61s 10,000,000 photons without data class
//144.72s
//151.34s without deleting newHit

