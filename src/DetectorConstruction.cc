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
	G4cout << G4endl << "DetectorConstruction has been deleted ";
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{  
	if (data -> GetCurrentImage() == 0)
	{
		TC -> SetNoImages(data ->GetNoImages());
		TC -> SetVisualization(data->GetVisualization());
		data -> SetDetectorMaterial(GetDetectorMaterial());
		data -> SetDetectorDimensions(DetectorSize_Cmd);
	}

	if (WorldSize_Cmd.y() < DetectorSize_Cmd.y() * GetNoDetectorsY())
	{
		G4double WorldSizeY = DetectorSize_Cmd.y() * GetNoDetectorsY();
		G4cout << G4endl << "================================================================================"
		       << G4endl << "         WARNING: Detectors are outside world volume in Y direction. "
		       << G4endl << "         World volume adjusted to fit them, needs to be at least " << G4BestUnit(WorldSizeY*2, "Length")
	               << G4endl << "================================================================================" << G4endl;
	}

	if (WorldSize_Cmd.z() < DetectorSize_Cmd.z()* GetNoDetectorsZ())
	{	
		G4double WorldSizeZ = DetectorSize_Cmd.z() * GetNoDetectorsZ();
		G4cout << G4endl << "================================================================================"
		       << G4endl << "         WARNING: Detectors are outside world volume in Z direction. "
		       << G4endl << "         World volume adjusted to fit them, needs to be at least " << G4BestUnit(WorldSizeZ*2, "Length")
	               << G4endl << "================================================================================" << G4endl;
	}

	//WORLD
	G4Box* solidWorld = new G4Box("World", WorldSize_Cmd.x(), WorldSize_Cmd.y(), WorldSize_Cmd.z());
	
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
	G4ThreeVector WorldSize = GetWorldSize();
	
	//G4Box* solid_Detector = new G4Box("DetectorGeometry", DetectorSizeX ,  DetectorSizeY ,  DetectorSizeZ );
	G4Box* solid_Detector = new G4Box("DetectorGeometry", DetectorSize.x() ,  DetectorSize.y() , DetectorSize.z() );	

	//Create the detector logical volume by assigning the material of the detectors 
	G4LogicalVolume* logic_Detector = new G4LogicalVolume(solid_Detector,FindMaterial(Material), "LogicDetector");

	G4double MaxLengthPositionY = NoDetectorsY*DetectorSize.y();
	G4double MaxLengthPositionZ = NoDetectorsZ*DetectorSize.z();

	G4double StartingPositionY = -NoDetectorsY * DetectorSize.y() + DetectorSize.y();
	G4double StartingPositionZ = -NoDetectorsZ * DetectorSize.z() + DetectorSize.z();

	G4double DetectorPosX = -WorldSize.x() + DetectorSize.x() ;

	for (G4double DetectorPosY = StartingPositionY; DetectorPosY <= MaxLengthPositionY; DetectorPosY = DetectorPosY+(DetectorSize.y()*2))
	{	
		for (G4double DetectorPosZ = StartingPositionZ; DetectorPosZ <= MaxLengthPositionZ; DetectorPosZ = DetectorPosZ+(DetectorSize.z()*2))
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

	//Visualization attributes
	Visualization(logic_Detector, G4Colour::Cyan());
	
	AttachSensitiveDetector(logic_Detector);

	//Sets a max step length in the tracker region, with G4StepLimiter
  	//G4double maxStep = 0.1*mm;
  	//fStepLimit = new G4UserLimits(maxStep);
  	//logic_Detector -> SetUserLimits(fStepLimit);

	G4cout << "The detectors have been created succesfully " << G4endl;  

}

G4Material* DetectorConstruction::FindMaterial(G4String MaterialName)
{
	G4Material* Material;
	//Obtain pointer to NIST material manager
	G4NistManager* nist = G4NistManager::Instance();
	//Build materials 
	return Material = nist -> FindOrBuildMaterial(MaterialName);
	
}

void DetectorConstruction::AttachSensitiveDetector(G4LogicalVolume* volume) 
{
  	if (!volume) 
		{return;}                  // Avoid unnecessary work

  	// Check if sensitive detector has already been created
 	G4SDManager* SDmanager = G4SDManager::GetSDMpointer();
  	G4VSensitiveDetector* theSD = SDmanager->FindSensitiveDetector("TrackerChamberSD", false);

  	if (!theSD) 
	{
      		G4cout << "Creating the Sensitive Detector"  << G4endl;

    		aTrackerSD = new TrackerSD("TrackerChamberSD", "TrackerHitsCollection", GetNoDetectorsY(), GetNoDetectorsZ(), data, GetDetectorEfficiency());
		SDmanager->AddNewDetector(aTrackerSD);	// Store SD if built		
	}	
	volume -> SetSensitiveDetector(aTrackerSD);
}

void DetectorConstruction::Visualization(G4LogicalVolume* LV, G4Colour Colour)
{
	if (data -> GetVisualization() == true)
	{
		G4VisAttributes* ObjectColour = new G4VisAttributes(G4Colour(Colour));	//Cyan
  		LV -> SetVisAttributes(ObjectColour);
	}
}

