#include "FluorescenceSD.hh"
#include "Data.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"

#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"

FluorescenceSD::FluorescenceSD(Data* DataObject, bool graphics) : G4VSensitiveDetector("FluorescenceDetector"), fHitsCollection(NULL), data(DataObject)
{
	RecordFullField = data -> GetFluorescence_Option();
	RecordFullMapping = data -> GetFullMapping_Option();
	
	GraphicsOn = graphics;
	if(GraphicsOn){collectionName.insert("FluorescenceHitsCollection");}
}

FluorescenceSD::~FluorescenceSD(){}

void FluorescenceSD::Initialize(G4HCofThisEvent* hce)
{	
    if(GraphicsOn)
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
	G4double PhotonEnergy = aStep->GetPreStepPoint()->GetKineticEnergy();

	if (RecordFullMapping){data -> SaveFullMapping(PhotonEnergy);}
	if (RecordFullField)  {data -> SaveFluorescence(PhotonEnergy);}
	
	if(GraphicsOn)
	{
	    //Create the TrackerHit class object to record hits
  	    TrackerHit* newHit = new TrackerHit();

	    //Save all the information about the particle that hit the detector
  	    newHit -> SetChamberNb(0);
	    newHit -> SetEdep(PhotonEnergy);
	    newHit -> SetPos (aStep->GetPreStepPoint()->GetPosition());
	
	    fHitsCollection -> insert( newHit );
	}

	return true;
}
