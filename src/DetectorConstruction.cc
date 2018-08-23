#include "DetectorConstruction.hh"
#include "DetectorConstructionMessenger.hh"
#include "TrackerSD.hh"

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
#include "G4Run.hh"

DetectorConstruction::DetectorConstruction():G4VUserDetectorConstruction()
{ 
	G4cout << G4endl << "DetectorConstruction has been created "<< G4endl;

	//Create a messenger for this class
  	detectorMessenger = new DetectorConstructionMessenger(this);	
}

DetectorConstruction::~DetectorConstruction()
{
	delete detectorMessenger;
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{  
	//Obtain pointer to NIST material manager
	G4NistManager* nist = G4NistManager::Instance();

//--------------------------------------------------------------------------------
		
	//Build materials for world and target
	G4Material* air = nist->FindOrBuildMaterial("G4_AIR");
	G4Material* Pb =  nist->FindOrBuildMaterial("G4_Pb");
	G4Material* Al =  nist->FindOrBuildMaterial("G4_Al");

//--------------------------------------------------------------------------------
	
	//GEOMETRY SIZE

	//World Geometry
	G4double WorldSizeX = WorldSize_Cmd.x();
	G4double WorldSizeY = WorldSize_Cmd.y();
	G4double WorldSizeZ = WorldSize_Cmd.z();

	//Target geometry
	G4double innerRadius = 1*cm;
	G4double outerRadius = 1.50*cm;
	G4double hz = WorldSizeZ/2;
	G4double startAngle = 0.*deg;
	G4double spanningAngle = 360.*deg;

	//Detector geometry
	G4double DetectorSizeX = DetectorSize_Cmd.x();
	G4double DetectorSizeY = DetectorSize_Cmd.y();
	G4double DetectorSizeZ = DetectorSize_Cmd.z();

//---------------------------------------------------------------------------------

	//GEOMETRY POSISTION

	//Target Position
	G4double TargetPositionX = TargetPosition_Cmd.x();
	G4double TargetPositionY = TargetPosition_Cmd.y();
	G4double TargetPositionZ = TargetPosition_Cmd.z();

//--------------------------------------------------------------------------------	
	
	//VARIABLES	

	//Detectors
	G4int CopyNo = 0;
	G4int NumDetectorsY = GetNoDetectorsY();
	G4int NumDetectorsZ = GetNoDetectorsZ();

	G4int TotalNumDetectors = NumDetectorsY * NumDetectorsZ;
	G4String filterName = "gammaFilter";
	G4String particleName = "gamma";

//--------------------------------------------------------------------------------
	//WORLD
	
	G4Box* solidWorld = new G4Box("World",WorldSizeX, WorldSizeY, WorldSizeZ);

	//Fill the world with air
	G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, air, "World");

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

//--------------------------------------------------------------------------------
	//TARGET

   	G4Tubs* TargetTube = new G4Tubs("TargetGeometry",
					 innerRadius,
                  			 outerRadius,
                  			 hz,
                  			 startAngle,
                  			 spanningAngle);

	//Fill the world with air
	G4LogicalVolume* logicTarget = new G4LogicalVolume(TargetTube, Al, "Al_Target");


	//Create the world physical volume. The world is the only
	//physical volume with no mother volume.
	G4VPhysicalVolume* physTarget= new G4PVPlacement(0,            //no rotation
							 G4ThreeVector(TargetPositionX, TargetPositionY, TargetPositionZ),       
							 logicTarget,           //its logical volume
							 "Target",               //its name
							 logicWorld,                     //its mother  volume
							 false,                 //no boolean operation
							 0,                     //copy number
							 true);		//overlaps checking                     

	// Visualization attributes
  	G4VisAttributes* Target_Colour = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0));	//White
  	logicTarget -> SetVisAttributes(Target_Colour);

//--------------------------------------------------------------------------------
	//DETECTORS

	G4Box* solid_Detector = new G4Box("DetectorGeometry", DetectorSizeX ,  DetectorSizeY ,  DetectorSizeZ );

	//Create the Detector logical volume by
	//assigning the material of the target 
	G4LogicalVolume* logic_Detector = new G4LogicalVolume(solid_Detector, Pb, "LogicDetector");

	//Create the Detector physical volume by placing it in the
	//"logicWorld" logical volume.

	//Intial Detector Position
	G4double IntDetectorPosX = -WorldSizeX + DetectorSizeX * 2;
	G4double IntDetectorPosY = -NumDetectorsY * DetectorSizeY;
	G4double IntDetectorPosZ = -NumDetectorsZ * DetectorSizeZ;

	//Detector Position
	G4double DetectorPosX = -WorldSizeX + DetectorSizeX * 2;
	G4double DetectorPosY = -NumDetectorsY * DetectorSizeY;
	G4double DetectorPosZ = -NumDetectorsZ * DetectorSizeZ;

	for (CopyNo; CopyNo < TotalNumDetectors; CopyNo++)
	{
		if (DetectorPosY >= -IntDetectorPosY)
		{
			DetectorPosY = IntDetectorPosY;
			DetectorPosZ = DetectorPosZ + DetectorSizeZ * 2;
		}
			
		DetectorPosY = DetectorPosY + 2*DetectorSizeY;
	
		G4VPhysicalVolume* phys_Detector = new G4PVPlacement(0,           //no rotation
							  	     G4ThreeVector(DetectorPosX, DetectorPosY, DetectorPosZ),      
							  	     logic_Detector,           //its logical volume
							  	     "Detector",               //its name
						          	     logicWorld,            //its mother  volume
							  	     false,                 //no boolean operation
						          	     CopyNo,                     //copy number
							  	     false);	//overlaps checking    
	}    

	//G4int NoBins = GetNoBins();
	G4cout << G4endl << "NoBins " << NoBins_Cmd << G4endl; 

	// Sensitive detectors
  	G4String trackerChamberSDname = "TrackerChamberSD";
  	TrackerSD* aTrackerSD = new TrackerSD(trackerChamberSDname, "TrackerHitsCollection", NumDetectorsY, NumDetectorsZ, NoBins_Cmd);

	//aTrackerSD -> SetNoDetectorsY(NumDetectorsY);
	//aTrackerSD -> SetNoDetectorsZ(NumDetectorsZ);
  	G4SDManager::GetSDMpointer() -> AddNewDetector(aTrackerSD);
  	// Setting aTrackerSD to all logical volumes with the same name 
  	// of "myTarget".
  	SetSensitiveDetector("LogicDetector", aTrackerSD, true);

	// Sets a max step length in the tracker region, with G4StepLimiter
  	G4double maxStep = 0.001*mm*DetectorSizeX;
  	fStepLimit = new G4UserLimits(maxStep);
  	logic_Detector -> SetUserLimits(fStepLimit);

	// Visualization attributes
  	G4VisAttributes* Detector_Colour = new G4VisAttributes(G4Colour(0.0,1.0,1.0));	//Cyan
  	logic_Detector -> SetVisAttributes(Detector_Colour);
  	
//--------------------------------------------------------------------------------

	return physWorld;
}




