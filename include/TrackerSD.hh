#ifndef TrackerSD_h
#define TrackerSD_h 1

#include "G4VSensitiveDetector.hh"
#include "G4UImessenger.hh"
#include "TrackerHit.hh"
#include "Data.hh"

#include <vector>

class G4Step;
class G4HCofThisEvent;
class Data;
class G4Run;

/// Tracker sensitive detector class
///
/// The hits are accounted in hits in ProcessHits() function which is called
/// by Geant4 kernel at each step. A hit is created with each step with non zero 
/// energy deposit.

class TrackerSD : public G4VSensitiveDetector
{
  	public:
		TrackerSD(const G4String& name, const G4String& hitsCollectionName, G4int NumDetectorsY, G4int NumDetectorsZ);
    		virtual ~TrackerSD();

		//Get methods
		G4int GetNoDetectorsY(){return NoDetectorsY_Cmd;}
		G4int GetNoDetectorsZ(){return NoDetectorsZ_Cmd;}

		//Set methods
		void SetNoDetectorsY(G4int value){NoDetectorsY_Cmd = value;}
		void SetNoDetectorsZ(G4int value){NoDetectorsZ_Cmd = value;}
  
    		//Methods
    		virtual void   Initialize(G4HCofThisEvent* hitCollection);
    		virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
		virtual void   EndOfEvent(G4HCofThisEvent* hitCollection);
    		//virtual void   EndOfEvent(const G4Run* aRun, G4HCofThisEvent* hitCollection);

  	private:

		//Pointers to different classes
		TrackerHitsCollection* fHitsCollection;
		Data* data;

		//Data variables 
		G4int NoDetectorsY_Cmd;
		G4int NoDetectorsZ_Cmd;
};

#endif
