#include "DetectorConstruction.hh"
#include "DetectorConstructionMessenger.hh"
#include "TrackerSD.hh"
#include "Data.hh"
#include "TargetConstruction.hh"

#include <math.h>	//Needed for sin and cos

#include "G4NistManager.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"

#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4SubtractionSolid.hh"

#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4UserLimits.hh"
#include "G4RunManager.hh"
#include "G4RotationMatrix.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

DetectorConstruction::DetectorConstruction(Data* DataObject):G4VUserDetectorConstruction(), data(DataObject)
{ 	G4cout << G4endl << "DetectorConstruction has been created ";

	//Create a messenger for this class
  	detectorMessenger = new DetectorConstructionMessenger(this);	
	TC = new TargetConstruction(data->GetNoImages());
	
}

DetectorConstruction::~DetectorConstruction()
{	
	delete detectorMessenger; 
	delete TC;
	G4cout << G4endl << "DetectorConstruction has been deleted " << G4endl;
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{  
	/*G4GeometryManager::GetInstance()->OpenGeometry();
        G4PhysicalVolumeStore::GetInstance()->Clean();
        G4LogicalVolumeStore::GetInstance()->Clean();
	G4SolidStore::GetInstance()->Clean();*/

	//World Geometry
	G4double WorldSizeX = WorldSize_Cmd.x();
	G4double WorldSizeY = WorldSize_Cmd.y();
	G4double WorldSizeZ = WorldSize_Cmd.z();

	if (data -> GetCurrentImage() == 0)
	{
		WorldSizeX = WorldSize_Cmd.x() + DetectorSize_Cmd.x();
		SetWorldSize(G4ThreeVector(WorldSizeX,WorldSize_Cmd.y(),WorldSize_Cmd.z()));
		TC -> SetNoImages(data ->GetNoImages());
		TC -> SetVisualization(data->GetVisualization());
	}

	if (WorldSizeY < DetectorSize_Cmd.y() * GetNoDetectorsY())
	{
		WorldSizeY = DetectorSize_Cmd.y() * GetNoDetectorsY();
		G4cout << G4endl << "================================================================================"
		       << G4endl << "         WARNING: Detectors are outside world volume in Y direction. "
		       << G4endl << "         World volume adjusted to fit them, needs to be at least " << G4BestUnit(WorldSizeY*2, "Length")
	               << G4endl << "================================================================================" << G4endl;
	}

	if (WorldSizeZ < DetectorSize_Cmd.z()* GetNoDetectorsZ())
	{	
		WorldSizeZ = DetectorSize_Cmd.z() * GetNoDetectorsZ();
		G4cout << G4endl << "================================================================================"
		       << G4endl << "         WARNING: Detectors are outside world volume in Z direction. "
		       << G4endl << "         World volume adjusted to fit them, needs to be at least " << G4BestUnit(WorldSizeZ*2, "Length")
	               << G4endl << "================================================================================" << G4endl;
	}

	//WORLD
	G4Box* solidWorld = new G4Box("World", WorldSizeX, WorldSizeY, WorldSizeZ);
	
	G4Material* WorldMaterial = FindMaterial("G4_AIR");
	G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, WorldMaterial, "World");

	//Create the world physical volume. The world is the only
	//physical volume with no mother volume.
	G4VPhysicalVolume* physWorld = new G4PVPlacement(0,            //no rotation
							 G4ThreeVector(),       //at (0,0,0)
							 logicWorld,            //its logical volume
							 "World",               //its name
							 0,                     //its mother  volume
							 false,                 //no boolean operation
							 0,                     //copy number
							 true);		//overlaps checking                     

	// Visualization attributes
	Visualization(logicWorld, G4Colour::White());

	TC->SetCurrentImage(data->GetCurrentImage());
	TC->Construct(logicWorld);

	SetUpDetectors(DetectorSize_Cmd, NoDetectorsY_Cmd, NoDetectorsZ_Cmd, GetDetectorMaterial(), logicWorld);
		
	return physWorld;

	G4cout << G4endl << "The world has been created succesfully ";  
}

void DetectorConstruction::SetUpDetectors(G4ThreeVector DetectorSize, G4int NoDetectorsY, G4int NoDetectorsZ, G4String Material, G4LogicalVolume* logicMotherBox)
{
	//Variables
	G4int CopyNo = 0;
	G4int TotalNoDetectors = NoDetectorsY * NoDetectorsZ;	
	G4double DetectorSizeX = DetectorSize.x();
	G4double DetectorSizeY = DetectorSize.y();
	G4double DetectorSizeZ = DetectorSize.z();
	G4ThreeVector WorldSize = GetWorldSize();
	G4double WorldSizeX = WorldSize.x();

	G4double DetectorPosX = -WorldSizeX + DetectorSizeX * 2;

	//G4Box* solid_Detector = new G4Box("DetectorGeometry", DetectorSizeX ,  DetectorSizeY ,  DetectorSizeZ );
	G4Box* solid_Detector = new G4Box("DetectorGeometry", DetectorSize.x() ,  DetectorSize.y() , DetectorSize.z() );	

	//Create the detector logical volume by assigning the material of the detectors 
	G4Material* DetectorMaterial = FindMaterial(Material);
		
	G4LogicalVolume* logic_Detector = new G4LogicalVolume(solid_Detector,DetectorMaterial, "LogicDetector");

	G4double MaxLengthPositionY = NoDetectorsY*DetectorSizeY;
	G4double MaxLengthPositionZ = NoDetectorsZ*DetectorSizeZ;

	G4double StartingPositionY = -NoDetectorsY * DetectorSizeY + DetectorSizeY;
	G4double StartingPositionZ = -NoDetectorsZ * DetectorSizeZ + DetectorSizeZ;

	for (G4double DetectorPosY = StartingPositionY; DetectorPosY <= MaxLengthPositionY; DetectorPosY = DetectorPosY+(DetectorSizeY*2))
	{	
		for (G4double DetectorPosZ = StartingPositionZ; DetectorPosZ <= MaxLengthPositionZ; DetectorPosZ = DetectorPosZ+(DetectorSizeZ*2))
		{
			G4VPhysicalVolume* phys_Detector = new G4PVPlacement(0,           //no rotation
							  	     	G4ThreeVector(DetectorPosX, DetectorPosY, DetectorPosZ),      
							  	     	logic_Detector,           //its logical volume
							  	     	"Detector",               //its name
						          	    	 logicMotherBox,            //its mother  volume
							  	    	 false,                 //no boolean operation
						          	   	  CopyNo,                     //copy number
							  	    	 false);	//overlaps checking    
			CopyNo++;
		}
	}   

	//Sets a max step length in the tracker region, with G4StepLimiter
  	G4double maxStep = 0.1*mm;
  	fStepLimit = new G4UserLimits(maxStep);
  	logic_Detector -> SetUserLimits(fStepLimit);

	//Visualization attributes
	Visualization(logic_Detector, G4Colour::Cyan());
	
	/*//Sensitive detectors
  	G4String trackerChamberSDname = "TrackerChamberSD";
  	aTrackerSD = new TrackerSD(trackerChamberSDname, "TrackerHitsCollection", NoDetectorsY, NoDetectorsZ, data);
  	G4SDManager::GetSDMpointer() -> AddNewDetector(aTrackerSD);
  	//Setting aTrackerSD to all logical volumes with the same name as "LogicDetector".
  	SetSensitiveDetector("LogicDetector", aTrackerSD, true);*/
	
	AttachSensitiveDetector(logic_Detector);

	G4cout << "The detectors have been created succesfully " << G4endl;  

}

G4Material* DetectorConstruction::FindMaterial(G4String MaterialName)
{
	//Obtain pointer to NIST material manager
	G4NistManager* nist = G4NistManager::Instance();
	//Build materials 
	G4Material* Material = nist -> FindOrBuildMaterial(MaterialName);
	return Material;
}

void DetectorConstruction::AttachSensitiveDetector(G4LogicalVolume* volume) 
{
  	if (!volume) 
		{return;}                  // Avoid unnecessary work

  	// Check if sensitive detector has already been created
	//const G4String& SDname = CDMSDetectorTypes::GetName("TrackerChamberSD");
	const G4String trackerChamberSDname = "TrackerChamberSD";
 	G4SDManager* SDmanager = G4SDManager::GetSDMpointer();
  	G4VSensitiveDetector* theSD = SDmanager->FindSensitiveDetector(trackerChamberSDname, false);

  	if (!theSD) 
	{
      		G4cout << "Creating the Sensitive Detector"  << G4endl;

    		aTrackerSD = new TrackerSD(trackerChamberSDname, "TrackerHitsCollection", GetNoDetectorsY(), GetNoDetectorsZ(), data, GetDetectorEfficiency());
		SDmanager->AddNewDetector(aTrackerSD);	// Store SD if built	
		//volume -> SetSensitiveDetector(aTrackerSD);
		
	}
	//SDmanager->AddNewDetector(aTrackerSD);	// Store SD if built	
	volume -> SetSensitiveDetector(aTrackerSD);

	//SetSensitiveDetector("LogicDetector", aTrackerSD, true);
	//SDmanager->AddNewDetector(aTrackerSD);	// Store SD if built

	//SetSensitiveDetector("LogicDetector", aTrackerSD, true);
	//SDmanager->AddNewDetector(aTrackerSD);	// Store SD if built	
 
	/*if (!volume) return;                  // Avoid unnecessary work

  	// Check if sensitive detector has already been created
  	//const G4String& SDname = CDMSDetectorTypes::GetName(detectorType);
  	G4SDManager* SDmanager = G4SDManager::GetSDMpointer();
  	G4VSensitiveDetector* theSD = SDmanager->FindSensitiveDetector("TrackerChamberSD", false);

  	if (!theSD) 
	{
    		//theSD = BuildSensitiveDetector();
		aTrackerSD = new TrackerSD("TrackerChamberSD", "TrackerHitsCollection", GetNoDetectorsY(), GetNoDetectorsZ(), data, GetDetectorEfficiency());
		SetSensitiveDetector("LogicDetector", aTrackerSD, true);
    		if (theSD) SDmanager->AddNewDetector(theSD);    // Store SD if built
  	}*/

}

void DetectorConstruction::Visualization(G4LogicalVolume* LV, G4Colour Colour)
{
	if (data -> GetVisualization() == true)
	{
		G4VisAttributes* ObjectColour = new G4VisAttributes(G4Colour(Colour));	//Cyan
  		LV -> SetVisAttributes(ObjectColour);
	}
}

