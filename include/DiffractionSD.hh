#ifndef DiffractionSD_h
#define DiffractionSD_h 1

#include "G4VSensitiveDetector.hh"
#include "TrackerHit.hh"

class G4Step;
class G4HCofThisEvent;
class Data;

//Tracker sensitive detector class.
//The hits are accounted in hits in ProcessHits() function which is called
//by Geant4 kernel at each step. A hit is created with each step with non zero 
//energy deposit.

class DiffractionSD : public G4VSensitiveDetector
{
  	public:
		DiffractionSD(Data* data,  bool graphics);
    	~DiffractionSD();
  
    	//Methods
    	virtual void   Initialize(G4HCofThisEvent* hitCollection);
    	virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);

		G4bool GraphicsOn;

  	private:

		//Pointers to different classes
		TrackerHitsCollection* fHitsCollection;
		Data* data;
		int n;
};

#endif
