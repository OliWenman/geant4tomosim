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
		VisTrackerSD(const G4String& name, const G4String& hitsCollectionName, G4int NumDetectorsY, G4int NumDetectorsZ, Data* data, G4bool DetectorEfficiency);
    		virtual ~VisTrackerSD();

		//Get methods
		G4int GetNoDetectorsY(){return NoDetectorsY_Cmd;}
		G4int GetNoDetectorsZ(){return NoDetectorsZ_Cmd;}
		G4bool GetDetectorEfficiency(){return DetectorEfficiency_Cmd;}

		G4int GetNoBins(){return NoBins;}

		//Set methods
		void SetNoDetectorsY(G4int value){NoDetectorsY_Cmd = value;}
		void SetNoDetectorsZ(G4int value){NoDetectorsZ_Cmd = value;}
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
		G4int NoDetectorsY_Cmd;
		G4int NoDetectorsZ_Cmd;

		G4int NoBins;
};

#endif
