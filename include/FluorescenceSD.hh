#ifndef FluorescenceSD_h
#define FluorescenceSD_h 1

#include "G4VSensitiveDetector.hh"
#include "TrackerHit.hh"

class G4Step;
class G4HCofThisEvent;
class Data;

class FluorescenceSD : public G4VSensitiveDetector
{
  	public:
		FluorescenceSD(Data* data, bool graphics);
    	~FluorescenceSD();
  
    	virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* hisotry);
    	virtual void   Initialize(G4HCofThisEvent* hitCollection);

		void SetFFF(bool value){RecordFullField = value;}
		void SetFFM(bool value){RecordFullMapping = value;}
		
		bool FullField()  {return RecordFullField;}
		bool FullMapping(){return RecordFullMapping;}

  	private:
		bool DetectorEfficiency;
		int nBins;

		//Pointers to different classes
		Data* data;
		TrackerHitsCollection* fHitsCollection;

		bool RecordFullField;
		bool RecordFullMapping;
		bool GraphicsOn;
		
		
};

#endif
