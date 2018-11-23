#ifndef VisTrackerSD_h
#define VisTrackerSD_h 1

#include "G4VSensitiveDetector.hh"
#include "G4UImessenger.hh"
#include "TrackerHit.hh"

#include <vector>

class G4Step;
class G4HCofThisEvent;
class Data;

//Tracker sensitive detector class.
//The hits are accounted in hits in ProcessHits() function which is called
//by Geant4 kernel at each step. A hit is created with each step with non zero 
//energy deposit.

class VisTrackerSD : public G4VSensitiveDetector
{
  	public:
		VisTrackerSD(const G4String& name, const G4String& hitsCollectionName, G4int NumDetectorsY, G4int NumDetectorsZ, Data* data,  const G4bool DetEfficiency, const G4bool EOption);
    		virtual ~VisTrackerSD();

		//Set methods
		void SetDetectorEfficiency(G4bool value){DetectorEfficiency = value;}
		void SetEnergyOption(G4bool value){EnergyOption = value;}
  
    		//Methods
    		virtual void   Initialize(G4HCofThisEvent* hitCollection);
    		virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
		virtual void   EndOfEvent(G4HCofThisEvent* hitCollection);

		G4bool DetectorEfficiency;
		G4bool EnergyOption;

  	private:

		//Pointers to different classes
		TrackerHitsCollection* fHitsCollection;
		Data* data;
};

#endif
