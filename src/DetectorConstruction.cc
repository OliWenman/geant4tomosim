#include "DetectorConstruction.hh"
#include "DetectorConstructionMessenger.hh"
#include "TrackerSD.hh"
#include "Data.hh"

#include "G4NistManager.hh"
#include "G4SDManager.hh"

#include "G4Tubs.hh"
#include "G4Box.hh"

#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"
#include "G4SystemOfUnits.hh"
#include "G4UserLimits.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

DetectorConstruction::DetectorConstruction(Data* DataObject):G4VUserDetectorConstruction(), data(DataObject)
{ 
	G4cout << G4endl << "DetectorConstruction has been created ";

	//Create a messenger for this class
  	detectorMessenger = new DetectorConstructionMessenger(this);	
}

DetectorConstruction::~DetectorConstruction()
{
	delete detectorMessenger;
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{  
	//World Geometry
	SetWorldSize(WorldSize_Cmd);
	G4double WorldSizeX = WorldSize_Cmd.x();
	G4double WorldSizeY = WorldSize_Cmd.y();
	G4double WorldSizeZ = WorldSize_Cmd.z();

	//WORLD
	G4Box* solidWorld = new G4Box("World", WorldSizeX, WorldSizeY, WorldSizeZ);

	//Fill the world with air
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
  	G4VisAttributes* World_Colour = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0));	//White
  	logicWorld -> SetVisAttributes(World_Colour);

	SetUpTarget(TargetPosition_Cmd, "G4_Al", logicWorld);

	SetUpDetectors(DetectorSize_Cmd, NoDetectorsY_Cmd, NoDetectorsZ_Cmd, "G4_Pb", logicWorld);

	return physWorld;

	G4cout << G4endl << "The world has been created succesfully ";  
}

void DetectorConstruction::SetUpTarget(G4ThreeVector TargetPosition, G4String Material, G4LogicalVolume* logicMotherBox)
{
	G4ThreeVector WorldSize = GetWorldSize();
	G4double WorldSizeZ = WorldSize.z();
	
	//Target geometry
	G4double innerRadius = 1*cm;
	G4double outerRadius = 1.50*cm;
	G4double hz = WorldSizeZ/2;
	G4double startAngle = 0.*deg;
	G4double spanningAngle = 360.*deg;

	//Target Position
	G4double TargetPositionX = TargetPosition.x();
	G4double TargetPositionY = TargetPosition.y();
	G4double TargetPositionZ = TargetPosition.z();

	//TARGET
   	G4Tubs* TargetTube = new G4Tubs("TargetGeometry",
					 innerRadius,
                  			 outerRadius,
                  			 hz,
                  			 startAngle,
                  			 spanningAngle);

	//Fill the target with its material
	G4Material* TargetMaterial = FindMaterial(Material);
	G4LogicalVolume* logicTarget = new G4LogicalVolume(TargetTube, TargetMaterial, "Al_Target");


	//Create the target physical volume
	G4VPhysicalVolume* physTarget= new G4PVPlacement(0,            //no rotation
							 G4ThreeVector(TargetPositionX, TargetPositionY, TargetPositionZ),       
							 logicTarget,           //its logical volume
							 "Target",               //its name
							 logicMotherBox,                     //its mother  volume
							 false,                 //no boolean operation
							 0,                     //copy number
							 true);		//overlaps checking                     

	//Visualization attributes
  	G4VisAttributes* Target_Colour = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0));	//White
  	logicTarget -> SetVisAttributes(Target_Colour);
	
	G4cout << G4endl << "The target has been created succesfully ";  
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
	//Intial detector position
	G4double iDetectorPosX = -WorldSizeX + DetectorSizeX * 2;
	G4double iDetectorPosY = -NoDetectorsY * DetectorSizeY;
	G4double iDetectorPosZ = -NoDetectorsZ * DetectorSizeZ;
	//Detector Position
	G4double DetectorPosX = -WorldSizeX + DetectorSizeX * 2;
	G4double DetectorPosY = -NoDetectorsY * DetectorSizeY;
	G4double DetectorPosZ = -NoDetectorsZ * DetectorSizeZ;
	
	G4Box* solid_Detector = new G4Box("DetectorGeometry", DetectorSizeX ,  DetectorSizeY ,  DetectorSizeZ );

	//Create the detector logical volume by assigning the material of the detectors 
	G4Material* DetectorMaterial = FindMaterial(Material);
	G4LogicalVolume* logic_Detector = new G4LogicalVolume(solid_Detector,DetectorMaterial, "LogicDetector");

	//Create the detector physical volume by placing it in the "logicWorld" logical volume.
	for (CopyNo; CopyNo < TotalNoDetectors; CopyNo++)
	{
		if (DetectorPosY >= -iDetectorPosY)
		{
			DetectorPosY = iDetectorPosY;
			DetectorPosZ = DetectorPosZ + DetectorSizeZ * 2;
		}
		DetectorPosY = DetectorPosY + 2*DetectorSizeY;
		G4VPhysicalVolume* phys_Detector = new G4PVPlacement(0,           //no rotation
							  	     G4ThreeVector(DetectorPosX, DetectorPosY, DetectorPosZ),      
							  	     logic_Detector,           //its logical volume
							  	     "Detector",               //its name
						          	     logicMotherBox,            //its mother  volume
							  	     false,                 //no boolean operation
						          	     CopyNo,                     //copy number
							  	     false);	//overlaps checking    
	}    
	
	//Sensitive detectors
  	G4String trackerChamberSDname = "TrackerChamberSD";
  	TrackerSD* aTrackerSD = new TrackerSD(trackerChamberSDname, "TrackerHitsCollection", NoDetectorsY, NoDetectorsZ, NoBins_Cmd, data);
  	G4SDManager::GetSDMpointer() -> AddNewDetector(aTrackerSD);
  	//Setting aTrackerSD to all logical volumes with the same name as "LogicDetector".
  	SetSensitiveDetector("LogicDetector", aTrackerSD, true);

	//Sets a max step length in the tracker region, with G4StepLimiter
  	G4double maxStep = 0.001*mm*DetectorSizeX;
  	fStepLimit = new G4UserLimits(maxStep);
  	logic_Detector -> SetUserLimits(fStepLimit);

	//Visualization attributes
  	G4VisAttributes* Detector_Colour = new G4VisAttributes(G4Colour(0.0,1.0,1.0));	//Cyan
  	logic_Detector -> SetVisAttributes(Detector_Colour);

	G4cout << "The detectors have been created succesfully " << G4endl;  
}

G4Material* DetectorConstruction::FindMaterial(G4String MaterialName)
{
	//Obtain pointer to NIST material manager
	G4NistManager* nist = G4NistManager::Instance();
		
	//Build materials 
	G4Material* Material = nist->FindOrBuildMaterial(MaterialName);

	return Material;
}
	

