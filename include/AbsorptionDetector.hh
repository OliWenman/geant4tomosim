/*
A class that builds the absorption detectors used in a simulation. 
It will build the detectors G4VSolids, G4LogicalVolume and G4PhysicalVolume 
as well as contain the pointer to the sensitive detector class
AbsorptionSD, which determines what happens when a particle is detected.

Author: Oliver Jack Wenman
*/

#ifndef AbsorptionDetector_h
#define AbsorptionDetector_h 1

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "SettingsLog.hh"
#include "MyVectors.hh"

//My own classes
class AbsorptionSD;
class AbsorptionDetectorMessenger;

//Solids, logic volume and physical volume for the geometry
class G4Box;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4PVParameterised;   

//For efficient geometry containing millions of solids
class G4PhantomParameterisation;

//Materials and colour classes
class G4Material;
class G4Colour;

class AbsorptionDetector
{
    public:
        AbsorptionDetector();
        ~AbsorptionDetector();
    
        void CreateVolumes();
        void AddProperties(G4bool GraphicsOn);
        void PlaceDetectors(G4LogicalVolume* MotherBox, G4ThreeVector position);
        
        void ReadOutInfo(SettingsLog& log);
        
        //SetFunctions
        void SetHalfDimensions(G4ThreeVector value){halfdimensions = value;}
        void SetxPixels(int value) {xpixels = value;}
        void SetyPixels(int value) {ypixels = value;}
        
        //Get functions
        int             GetxPixels()           {return xpixels;}
        int             GetyPixels()           {return ypixels;}     
        G4ThreeVector   GetHalfDimenions()     {return halfdimensions;}
        double_vector1D GetHalfDimensions()    {return {halfdimensions.x(),halfdimensions.y(),halfdimensions.z()};}
        G4ThreeVector   GetG4HalfDimensions()  {return halfdimensions;}
        AbsorptionSD*   GetSensitiveDetector() {return absorSD;}
    private:
        //Pointers
        AbsorptionDetectorMessenger *messenger;
        G4PhantomParameterisation   *param;
        
        G4Box                       *container;
        G4Box                       *cells;
        G4LogicalVolume             *container_logic;
		G4LogicalVolume             *cells_logic;
		G4VPhysicalVolume           *container_placement;
		G4PVParameterised           *cells_placement;
		
		AbsorptionSD                *absorSD;
    
        //Variables
		G4ThreeVector halfdimensions;
		G4String      materialName;
		G4bool        realDetector;
		
		int xpixels;
		int ypixels;
};
#endif
