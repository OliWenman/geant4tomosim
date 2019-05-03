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
  
    	virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* hisotry);
    	virtual void   Initialize(G4HCofThisEvent* hitCollection);
		
		void InitialiseData();
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
		int    nbins;
		double absorb_xpixels;
		double absorb_ypixels;
        double maxenergy;

        //Options to turn on
		bool fullfieldOn;
		bool fullmappingOn;
		bool graphicsOn;
};

#endif
