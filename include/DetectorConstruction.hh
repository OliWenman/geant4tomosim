#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4UserLimits.hh"
#include "G4UImessenger.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class DetectorConstructionMessenger;

//Detector construction class to define materials and geometry.

class DetectorConstruction : public G4VUserDetectorConstruction
{
  	public:
    		DetectorConstruction();
    		virtual ~DetectorConstruction();

    		virtual G4VPhysicalVolume* Construct();

		//Set methods
		inline void SetWorldSize(G4ThreeVector value){WorldSize_Cmd = value;}

		inline void SetNoDetectorsY(G4int value){NoDetectorsY_Cmd = value;} 
		inline void SetNoDetectorsZ(G4int value){NoDetectorsZ_Cmd = value;}

		inline void SetDetectorSize(G4ThreeVector value){DetectorSize_Cmd = value;}

		//Get methods
		G4ThreeVector GetWorldSize(){return WorldSize_Cmd;}

		G4int GetNoDetectorsY(){return NoDetectorsY_Cmd;}
		G4int GetNoDetectorsZ(){return NoDetectorsZ_Cmd;}

		G4ThreeVector GetDetectorSize(){return DetectorSize_Cmd;}

  	protected:
		//Pointer to user step limits
		G4UserLimits* fStepLimit;            

		//Pointer to DetectorConstructionMessenger
		DetectorConstructionMessenger* detectorMessenger;

		//DATA
		//World variables
		G4ThreeVector WorldSize_Cmd;

		//Detector variables
		G4int NoDetectorsY_Cmd;
		G4int NoDetectorsZ_Cmd;

		G4ThreeVector DetectorSize_Cmd;
};

#endif
