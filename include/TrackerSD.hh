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
		TrackerSD(const G4String& name, const G4String& hitsCollectionName, G4int NumDetectorsY, G4int NumDetectorsZ, Data* data, G4bool DetectorEfficiency);
    		virtual ~TrackerSD();

		//Get methods
		G4bool GetDetectorEfficiency(){return DetectorEfficiency_Cmd;}

		G4int GetNoBins(){return NoBins;}

		//Set methods
		void SetNoBins(G4int value){NoBins = value;}
		void SetDetectorEfficiency(G4bool value){DetectorEfficiency_Cmd = value;}
		void SetEnergyOption(G4bool value){EnergyOption_Cmd = value;}
		void SetVisualization(G4bool value){Visualization_Cmd = value;}
  
    		//Methods
    		virtual void   Initialize(G4HCofThisEvent* hitCollection);
    		virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
		virtual void   EndOfEvent(G4HCofThisEvent* hitCollection);

		G4bool DetectorEfficiency_Cmd;
		G4bool EnergyOption_Cmd;
		G4bool Visualization_Cmd;

  	private:

		//Pointers to different classes
		TrackerHitsCollection* fHitsCollection;
		Data* data;

		//Data variables 

	G4int NoBins;
};

#endif
