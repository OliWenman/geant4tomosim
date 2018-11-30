#ifndef TrackerSD_h
#define TrackerSD_h 1

#include "G4VSensitiveDetector.hh"
#include "TrackerHit.hh"

class G4Step;
class G4HCofThisEvent;
class Data;

//Tracker sensitive detector class.
//The hits are accounted in hits in ProcessHits() function which is called
//by Geant4 kernel at each step. A hit is created with each step with non zero 
//energy deposit if the detectors are realistic, otherwise they record a hit when a photon enters
//its volume for 100% efficiency.

class TomographySD : public G4VSensitiveDetector
{
  	public:
		TomographySD(const G4String& name, Data* data, const G4bool DetectorEfficiency);
    		~TomographySD();
  
    		//Base Methods
    		G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* );

  	private:
		bool DetectorEfficiency;

		//Pointers to different classes
		Data* data;
};

#endif
