#ifndef FluorescenceSD_h
#define FluorescenceSD_h 1

#include "G4VSensitiveDetector.hh"

class G4Step;
class G4HCofThisEvent;
class Data;

class FluorescenceSD : public G4VSensitiveDetector
{
  	public:
		FluorescenceSD(const G4String& name, Data* data, const G4bool DetEfficiency);
    		~FluorescenceSD();
  
    		G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* );

		void SetFFF(bool value){fluoreFullField = value;}
		void SetFFM(bool value){fluoreFullMapping = value;}

  	private:
		bool DetectorEfficiency;
		int nBins;

		//Pointers to different classes
		Data* data;

		bool fluoreFullField;
		bool fluoreFullMapping;
		
		
};

#endif
