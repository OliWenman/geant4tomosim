#ifndef TrackerSD_h
#define TrackerSD_h 1

#include "G4VSensitiveDetector.hh"
#include "G4UImessenger.hh"
#include "TrackerHit.hh"
//#include "Data.hh"

#include <vector>

class G4Step;
class G4HCofThisEvent;
class Data;

//Tracker sensitive detector class.
//The hits are accounted in hits in ProcessHits() function which is called
//by Geant4 kernel at each step. A hit is created with each step with non zero 
//energy deposit.

class TrackerSD : public G4VSensitiveDetector
{
  	public:
		TrackerSD(const G4String& name, const G4String& hitsCollectionName, G4int NumDetectorsY, G4int NumDetectorsZ, Data* data, const G4bool DetectorEfficiency, const G4bool EnergyDataOption);
    		~TrackerSD();
  
    		//Methods
    		//virtual void   Initialize(G4HCofThisEvent* hitCollection);
    		G4bool ProcessHits(G4Step* step, G4TouchableHistory* );
		void EndOfEvent(G4HCofThisEvent* hitCollection);

		G4bool Visualization_Cmd;

  	private:
		bool DetectorEfficiency;
		bool EnergyDataOption;

		//Pointers to different classes
		TrackerHitsCollection* fHitsCollection;
		Data* data;
};

#endif
