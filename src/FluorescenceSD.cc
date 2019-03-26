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

FluorescenceSD::~FluorescenceSD()
{
    if (!fullfieldfluorescence.empty()) {fullfieldfluorescence.clear();}
}

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
	G4double energy = aStep->GetPreStepPoint()->GetKineticEnergy();

    if (!fullfieldfluorescence.empty())
    {
        //int bin = floor(E*1000/(MaxE/NoBins_Cmd) -1);
        //if (bin > NoBins_Cmd - 1) {bin = NoBins_Cmd -1;}

	    //++fullfieldFluorescenceData[bin];
    }
	if (RecordFullMapping){data -> SaveFullMapping(energy);}
	if (RecordFullField)  {data -> SaveFluorescence(energy);}
	
	if(GraphicsOn)
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

void FluorescenceSD::SetupFullField(int nBins)
{
    fullfieldfluorescence.assign(nBins, 0);
}

void FluorescenceSD::ResetData()
{
    if (!fullfieldfluorescence.empty()) {std::fill(fullfieldfluorescence.begin(), fullfieldfluorescence.end(), 0);}
}

//void FluorescenceSD::
