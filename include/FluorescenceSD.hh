/*
Sensitive Detector class for FluorescenceDetector. Can record full-mapping
and full-field fluorescence. 

Author: Oliver Jack Wenman
*/

#ifndef FluorescenceSD_h
#define FluorescenceSD_h 1

#include "G4VSensitiveDetector.hh"
#include "TrackerHit.hh"
#include "MyVectors.hh"

class G4Step;
class G4HCofThisEvent;

class FluorescenceSD : public G4VSensitiveDetector
{
  	public:
  	    FluorescenceSD();
    	~FluorescenceSD();
/*
        The ProcessHits method is a function inherited from G4VSensitiveDetector. It returns
        true or false when a particle enters the volume. Assuming the detectors are 100%
        efficient, the detector will always process the hit of the particle.
*/
    	G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* hisotry);
    	void   Initialize(G4HCofThisEvent* hitCollection);
		
		void InitialiseData(bool clean_data);
		void FreeMemory();
		
		//Set methods
		void RecordFullMapping(bool value) {fullmappingOn = value;}
		void RecordFullField  (bool value) {fullfieldOn = value;}
		
		void SetGraphics    (bool value) {graphicsOn = value;}
		void SetMaxEnergy   (double value)  {maxenergy  = value;}
		void SetNumberOfBins(int value)  {nbins      = value;}
		
		//Get methods
		bool FullMappingActive() {return fullmappingOn;}
		bool FullFieldActive()   {return fullfieldOn;}
		
		int GetNumberOfBins()    {return nbins;}
				
		//Return the data
		int_vector1D    GetFullField()   {return fullfieldfluorescence;}
		int_vector3D    GetFullMapping() {return fullmappingfluorescence;}
		double_vector1D GetEnergyBins()  {return energybins;}

  	private:
  	    //Pointers to different classes
		TrackerHitsCollection* fHitsCollection;
  	
  	    //Data
		int_vector1D    fullfieldfluorescence;
		int_vector3D    fullmappingfluorescence;
  	    double_vector1D energybins;
  	
  	    //Variables to build the data
		int           nbins;
		double        maxenergy;
		double        absorb_xpixels;
		double        absorb_ypixels;
		G4ThreeVector absorb_halfdimensions;

        //Options to turn on
		bool fullfieldOn;
		bool fullmappingOn;
		bool graphicsOn;
		
		int primary;
		int secondary;
};

#endif
