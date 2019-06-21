#include "FluorescenceDetector.hh"
#include "FluorescenceDetectorMessenger.hh"
#include "FluorescenceSD.hh"

#include "G4Box.hh"

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
    //Create the messenger class to control the setup via macrofiles, FluorescenceDetectorMessenger
    messenger = new FluorescenceDetectorMessenger(this); 
    
 	G4SDManager* sd_manager = G4SDManager::GetSDMpointer();

    //Check if sensitive detector has already been created
  	if (!sensitiveDetector) 
	{
		sensitiveDetector = new FluorescenceSD();
		sd_manager->AddNewDetector(sensitiveDetector);	

		//G4SDParticleFilter* gammaFilter = new G4SDParticleFilter("GammaFilter", "gamma");
		//sensitiveDetector -> SetFilter(gammaFilter);
	}
    
}

FluorescenceDetector::~FluorescenceDetector()
{
    delete messenger;
}

//Create the G4VSolid for the detector
void FluorescenceDetector::CreateVolumes()
{
    detectorvolume = new G4Box("FluorescenceDetector",
			         	       halfdimensions.x(),
					           halfdimensions.y(),
					           halfdimensions.z());
}
void FluorescenceDetector::CreateVolumes(G4ThreeVector halfDimensions)
{
    detectorvolume = new G4Box("FluorescenceDetector",
			         	       halfDimensions.x(),
					           halfDimensions.y(),
					           halfDimensions.z());
					          
    halfdimensions = halfDimensions;
}

//Create the G4LogicalVolume for the detector
void FluorescenceDetector::AddProperties(G4bool graphicsOn)
{
    sensitiveDetector->SetGraphics(graphicsOn);

    //Pick the material for the detectors based on if the detectors are 100% efficient
	G4Material* material = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
	
	//Create the logic volume
	detector_logic = new G4LogicalVolume(detectorvolume, material, "FluorescenceDetector");
	
	//Set visualization colour
	G4VisAttributes* cyan = new G4VisAttributes(G4Colour::Cyan);	
  	detector_logic->SetVisAttributes(cyan);

	//Add the sensitive detector to the logical volume
	detector_logic->SetSensitiveDetector(sensitiveDetector);
}

//Create the G4PVPlacement
void FluorescenceDetector::PlaceDetectors(G4LogicalVolume* motherbox, G4ThreeVector _position)
{
    G4VPhysicalVolume* detectorPlacement = new G4PVPlacement(0,
							       	                         _position,
							                                 detector_logic,
							                                "FluorescenceDetector",
							                                 motherbox,
							                                 false,
							                                 0,
							                                 false);
	position = _position;         
}

void FluorescenceDetector::ReadOutInfo(SettingsLog& log)
{
    PrintToEndOfTerminal(log, '-');
	log	<< "FLUORESCENCE DETECTOR:"
		   "\n - Detector half dimensions: " << G4BestUnit(halfdimensions, "Length")
		<< "\n - ";
		    
}
