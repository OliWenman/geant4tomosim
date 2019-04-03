#include "FluorescenceDetector.hh"
#include "FluorescenceDetectorMessenger.hh"
#include "FluorescenceSD.hh"

//Solid shapes
#include "G4Box.hh"

//Efficient geometry for the millions of detectors
#include "G4PVParameterised.hh"
#include "G4PhantomParameterisation.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4SDManager.hh"

#include "G4PVPlacement.hh"

#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include "PrintLines.hh"

#include "G4SDParticleFilter.hh"

FluorescenceDetector::FluorescenceDetector(): sensitiveDetector(NULL)
{
    FDMessenger = new FluorescenceDetectorMessenger(this); 
    
    // Check if sensitive detector has already been created
 	G4SDManager* SDmanager = G4SDManager::GetSDMpointer();

  	if (!sensitiveDetector) 
	{
		//Create a visual detector
		sensitiveDetector = new FluorescenceSD();
		SDmanager->AddNewDetector(sensitiveDetector);	// Store SD if built	

		//G4SDParticleFilter* gammaFilter = new G4SDParticleFilter("GammaFilter", "gamma");
		//sensitiveDetector -> SetFilter(gammaFilter);
	}
    
}

FluorescenceDetector::~FluorescenceDetector()
{
    delete FDMessenger;
}

void FluorescenceDetector::CreateVolumes()
{
    DetectorVolume = new G4Box("FluorescenceDetector",
			         	       halfdimensions.x(),
					           halfdimensions.y(),
					           halfdimensions.z());
}

void FluorescenceDetector::CreateVolumes(G4ThreeVector halfDimensions)
{
    DetectorVolume = new G4Box("FluorescenceDetector",
			         	       halfDimensions.x(),
					           halfDimensions.y(),
					           halfDimensions.z());
					          
    halfdimensions = halfDimensions;
}

void FluorescenceDetector::AddProperties(G4bool GraphicsOn)
{
	G4Material* Material = G4NistManager::Instance() -> FindOrBuildMaterial("G4_Galactic");
	
	DetectorLV = new G4LogicalVolume(DetectorVolume, Material, "FluorescenceDetectorLV");
	
	G4VisAttributes* Cyan = new G4VisAttributes(G4Colour::Cyan);	
  	DetectorLV -> SetVisAttributes(Cyan);

	//Add the sensitive detector to the logical volume
	DetectorLV -> SetSensitiveDetector(sensitiveDetector);
}

void FluorescenceDetector::PlaceDetectors(G4LogicalVolume* MotherBox, G4ThreeVector Position)
{
    G4VPhysicalVolume* DetectorPlacement = new G4PVPlacement(0,
							       	                         Position,
							                                 DetectorLV,
							                                "Fluorecense detector",
							                                 MotherBox,
							                                 false,
							                                 0,
							                                 false);
	position = Position; 
	
	sensitiveDetector->InitialiseData();						          
}

void FluorescenceDetector::ReadOutInfo(SettingsLog& log)
{
    //log << "\n--------------------------------------------------------------------"
    PrintToEndOfTerminal(log, '-');
	log	<< "FLUORESCENCE DETECTOR:"
		   "\n - Detector half dimensions: " << G4BestUnit(halfdimensions, "Length")
		<< "\n - ";
		    
}
