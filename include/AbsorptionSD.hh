#ifndef AbsorptionSD_h
#define AbsorptionSD_h 1

#include "G4VSensitiveDetector.hh"
#include "TrackerHit.hh"
#include "MyVectors.hh"

class G4Step;
class G4HCofThisEvent;

//Tracker sensitive detector class.
//The hits are accounted in hits in ProcessHits() function which is called
//by Geant4 kernel at each step. A hit is created with each step with non zero 
//energy deposit.

class AbsorptionSD : public G4VSensitiveDetector
{
  	public:
  	    AbsorptionSD();
    	~AbsorptionSD();
  
    	//Methods
    	virtual void   Initialize(G4HCofThisEvent* hitCollection);
    	virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);

        void InitialiseData(int xpix, int ypix);
        void FreeMemory();

        bool GraphicsActive()        {return graphicsOn;}
        void SetGraphics(bool value) {graphicsOn = value; collectionName.insert("AbsorptionHitsCollection");} 
        
        //Data
        int_vector1D GetData() {return absorptionData;}
    
  	private:

		//Pointers to different classes
		TrackerHitsCollection* fHitsCollection;
		
		int_vector1D absorptionData;
		
		int xpixels;
		int ypixels;
		
		int n;
		bool graphicsOn;
};

#endif
