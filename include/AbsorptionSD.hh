/*
A class assigned to a AbsorptionDetectors G4LogicalVolume. Inherits 
from Geant4's G4SenstiveDetector class which is used to determines
the process taken when a particle is detected.

Each pixel is assigned a copy number, data is stored as a 1D vector.
Converted to 2D on python side via numpy.shape method.

Author: Oliver Jack Wenman
*/

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

        void InitialiseData();
        void FreeMemory();

        bool GraphicsActive()        {return graphicsOn;}
        void SetGraphics(bool value) {graphicsOn = value;} 
        
        //Data
        int_vector1D GetData() {return absorptionData;}
        
        //Set
        void Set_xpixels(int value){xpixels = value;}
        void Set_ypixels(int value){ypixels = value;}
    
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
