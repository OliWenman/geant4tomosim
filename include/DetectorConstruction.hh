#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4ThreeVector.hh"
#include "SettingsLog.hh"
#include "AbsorptionDetector.hh"
#include "FluorescenceDetector.hh"
#include "DiffractionDetector.hh"
#include "G4Box.hh"
//My own classes
class DetectorConstructionMessenger;
class AbsorptionDetector;
class FluorescenceDetector;
class DiffractionDetector;
class SampleConstruction;

//Solids, logic volume and physical volume for the geometry
class G4Box;
class G4LogicalVolume;
class G4VPhysicalVolume;

//Materials and colour classes
class G4Material;
class G4Colour;

//Detector construction class to define materials and geometry.
class DetectorConstruction : public G4VUserDetectorConstruction
{
  	public:
        DetectorConstruction();
    	~DetectorConstruction();

		//Base class method
    	G4VPhysicalVolume* Construct();
		
		void ModifyWorldVolume(G4ThreeVector value);
		void ModifyWorldMaterial(G4String materialname);
		void SetGraphics(bool graphics);
		void ReadOutInfo(SettingsLog& log);
		
		//Set methods
		void SetVisualization(G4bool value){Visualization_Cmd = value;}	
		void SetWorldDimensions(G4ThreeVector dim) { if(world_dimensions != dim) {world_dimensions = dim; built = false;}} 
        void SetWorldMaterial  (std::string   mat) { if(world_material != mat)   {world_material   = mat; built = false;}}

		//Get methods
		G4ThreeVector    GetWorldSize() const {return world_dimensions;}
		G4bool           GetVisualization()   {return Visualization_Cmd;}
		
		//Get pointers
		AbsorptionDetector   *GetAbsorptionDetector()   const {return absDetector;}
		FluorescenceDetector *GetFluorescenceDetector() const {return fluorescenceDetector;}
		DiffractionDetector  *GetDiffractionDetector()  const {return diffractionDetector;}
		SampleConstruction   *GetSampleConstruction()   const {return sampleconstruction;}
        G4LogicalVolume      *GetWorldLV()              const {return logicWorld;}
        
  	private:
		//Pointers to my own classes 
		DetectorConstructionMessenger  *detectorMessenger;
        AbsorptionDetector             *absDetector;
		FluorescenceDetector           *fluorescenceDetector;		
		DiffractionDetector            *diffractionDetector; 	
		SampleConstruction             *sampleconstruction;

		//World pointers
		G4Box             *solidWorld;
		G4LogicalVolume   *logicWorld;
		G4VPhysicalVolume *physWorld; 
		
		G4ThreeVector world_dimensions;
		std::string   world_material;
		
		bool Visualization_Cmd;
        bool built;
};

#endif
