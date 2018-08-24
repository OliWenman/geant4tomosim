#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4UserLimits.hh"
#include "G4UImessenger.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class DetectorConstructionMessenger;
class Data;

//Detector construction class to define materials and geometry.

class DetectorConstruction : public G4VUserDetectorConstruction
{
  	public:
    		DetectorConstruction(Data* data);
    		virtual ~DetectorConstruction();

    		virtual G4VPhysicalVolume* Construct();

		//virtual void SetUpWorld();
		//virtual void SetUpTarget();
		//virtual void SetUpDetector();
		

		//Set methods
		inline void SetWorldSize(G4ThreeVector value){WorldSize_Cmd = value;}

		inline void SetNoDetectorsY(G4int value){NoDetectorsY_Cmd = value;} 
		inline void SetNoDetectorsZ(G4int value){NoDetectorsZ_Cmd = value;}

		inline void SetDetectorSize(G4ThreeVector value){DetectorSize_Cmd = value;}

		inline void SetTargetPosition(G4ThreeVector value){TargetPosition_Cmd = value;}

		inline void SetNoBins(G4int value){NoBins_Cmd = value;}

		//Get methods
		G4ThreeVector GetWorldSize(){return WorldSize_Cmd;}

		G4int GetNoDetectorsY(){return NoDetectorsY_Cmd;}
		G4int GetNoDetectorsZ(){return NoDetectorsZ_Cmd;}

		G4ThreeVector GetDetectorSize(){return DetectorSize_Cmd;}

		G4ThreeVector GetTargetPosition(){return TargetPosition_Cmd;}

		G4int GetNoBins(){return NoBins_Cmd;}


  	protected:
		//Pointer to user step limits
		G4UserLimits* fStepLimit;            

		//Pointer to DetectorConstructionMessenger
		DetectorConstructionMessenger* detectorMessenger;
		
		//Pointer to the Data class
		Data* data;

		//DATA
		//World variables
		G4ThreeVector WorldSize_Cmd;

		//Detector variables
		G4int NoDetectorsY_Cmd;
		G4int NoDetectorsZ_Cmd;

		G4ThreeVector DetectorSize_Cmd;

		//Target variables
		G4ThreeVector TargetPosition_Cmd;

		//Data manipulation varaibles
		G4int NoBins_Cmd;
};

#endif
