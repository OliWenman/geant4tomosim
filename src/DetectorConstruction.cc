#include "DetectorConstruction.hh"
#include "DetectorConstructionMessenger.hh"
#include "TrackerSD.hh"
#include "VisTrackerSD.hh"
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
#include "G4PVParameterised.hh"
#include "G4PhantomParameterisation.hh"
#include "G4PVReplica.hh"
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
  	detectorMessenger = new DetectorConstructionMessenger(this, data);	
	TC = new TargetConstruction();
}

DetectorConstruction::~DetectorConstruction()
{	
	delete detectorMessenger; 
	delete TC;
	G4cout << G4endl << "DetectorConstruction has been deleted ";
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{  
	CurrentImage = data -> GetCurrentImage();

	if (CurrentImage == 0)
	{
		TC -> SetNoImages(data->GetNoImages());
		TC -> SetVisualization(data->GetVisualization());
	}

	if (WorldSize_Cmd.y() < DetectorSize_Cmd.y() * GetNoDetectorsY())
	{
		G4cout << G4endl << "////////////////////////////////////////////////////////////////////////////////"
		       << G4endl << "         WARNING: Detectors are outside world volume in Y direction. "
		       << G4endl << "                          Needs to be at least " << G4BestUnit(WorldSize_Cmd.y()*2, "Length")
	               << G4endl << "////////////////////////////////////////////////////////////////////////////////" << G4endl;
	}

	if (WorldSize_Cmd.z() < DetectorSize_Cmd.z()* GetNoDetectorsZ())
	{	
		G4cout << G4endl << "////////////////////////////////////////////////////////////////////////////////"
		       << G4endl << "         WARNING: Detectors are outside world volume in Z direction. "
		       << G4endl << "                          Needs to be at least " << G4BestUnit(WorldSize_Cmd.z()*2, "Length")
	               << G4endl << "////////////////////////////////////////////////////////////////////////////////" << G4endl;
	}

	//WORLD
	G4Box* solidWorld = new G4Box("World", WorldSize_Cmd.x()+DetectorSize_Cmd.x(), WorldSize_Cmd.y(), WorldSize_Cmd.z());

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

	TC->SetCurrentImage(CurrentImage);
	TC->Construct(logicWorld);

	SetUpDetectors(DetectorSize_Cmd, NoDetectorsY_Cmd, NoDetectorsZ_Cmd, GetDetectorMaterial(), logicWorld);

	return physWorld;

	G4cout << G4endl << "The world has been created succesfully ";  
}

void DetectorConstruction::SetUpDetectors(G4ThreeVector HalfDetectorSize, G4int nDetectorsY, G4int nDetectorsZ, G4String Material, G4LogicalVolume* logicMotherBox)
{
	//Get the needed variablles
	G4double WorldSizeX = GetWorldSize().x();

	G4double DetectorPosX = -WorldSizeX;
	G4int nDetectorsX = 1;
	G4double HalfDetectorSizeX = HalfDetectorSize.x();
	G4double HalfDetectorSizeY = HalfDetectorSize.y();
	G4double HalfDetectorSizeZ = HalfDetectorSize.z();

	G4PhantomParameterisation* param = new G4PhantomParameterisation();

	//Voxel dimensions in the three dimensions
	param->SetVoxelDimensions(HalfDetectorSizeX, HalfDetectorSizeY, HalfDetectorSizeZ);

	//Number of voxels in the three dimensions
	param->SetNoVoxel( nDetectorsX, nDetectorsY, nDetectorsZ );

	//Vector of materials of the voxels
	std::vector < G4Material* > theMaterials;
	theMaterials.push_back(FindMaterial(Material));

	//Sets the materials of the voxels
	param -> SetMaterials(theMaterials);
	size_t* materialIDs = new size_t[nDetectorsX*nDetectorsY*nDetectorsZ];
	param -> SetMaterialIndices( materialIDs );

	//Create the phantom container for the detectors
	G4Box* container_solid = new G4Box("PhantomContainer",nDetectorsX*HalfDetectorSizeX,nDetectorsY*HalfDetectorSizeY,nDetectorsZ*HalfDetectorSizeZ);

	G4LogicalVolume* container_logic = new G4LogicalVolume(container_solid,
           						       FindMaterial(Material),        // material is not relevant here...
           						       "PhantomContainer");
	G4VPhysicalVolume* container_phys = new G4PVPlacement(0,                  // rotation
            						      G4ThreeVector(DetectorPosX,0,0),                   // translation
           						      container_logic,            // logical volume
            						      "PhantomContainer",    // name
            						      logicMotherBox,           // mother volume
            						      false,                 // No op. bool.
           						      0,			//copy number
							      false);                    //overlap checking
	param->BuildContainerSolid(container_phys);
	Visualization(container_logic, G4Colour::Red());

	// Assure that the voxels are completely filling the container volume
	param->CheckVoxelsFillContainer( container_solid->GetXHalfLength(),
                                         container_solid->GetYHalfLength(),
                                         container_solid->GetZHalfLength() );

	// The parameterised volume which uses this parameterisation is placed
	// in the container logical volume

	G4Box* PhantomBoxes_solid = new G4Box("PhantomBox",HalfDetectorSizeX,HalfDetectorSizeY,HalfDetectorSizeZ);
	
	G4LogicalVolume* PhantomBoxes_logic = new G4LogicalVolume(PhantomBoxes_solid,
           						          FindMaterial(Material),        // material is not relevant here...
           						          "PhantomBox");

	G4PVParameterised* PhantomBoxes_phys = new G4PVParameterised("PhantomBoxes",               // name
                       						     PhantomBoxes_logic,           // logical volume
                        					     container_logic,              // mother volume
           							     kXAxis,                  // optimisation hint
                       						     nDetectorsX*nDetectorsY*nDetectorsZ, // number of voxels
                       						     param);                  // parameterisation

	// Indicate that this physical volume is having a regular structure
	//
	//param->SetNoVoxel(nVoxelX, nVoxelY, nVoxelZ );
	PhantomBoxes_phys->SetRegularStructureId(1);

	AttachSensitiveDetector(PhantomBoxes_logic);

	//Visualization attributes
	Visualization(PhantomBoxes_logic, G4Colour::Cyan());
	
	G4cout << "The detectors have been created succesfully " << G4endl;  
}

G4Material* DetectorConstruction::FindMaterial(G4String MaterialName)
{
	//Obtain pointer to NIST material manager
	//Build materials 
	return G4NistManager::Instance() -> FindOrBuildMaterial(MaterialName);;
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
	
		if (data ->GetVisualization() == true)
		{
			VTrackerSD = new VisTrackerSD("TrackerChamberSD", "TrackerHitsCollection", GetNoDetectorsY(), GetNoDetectorsZ(), data, GetDetectorEfficiency());
			SDmanager->AddNewDetector(VTrackerSD);	// Store SD if built	
		}
		else
		{
    			aTrackerSD = new TrackerSD("TrackerChamberSD", "TrackerHitsCollection", GetNoDetectorsY(), GetNoDetectorsZ(), data, GetDetectorEfficiency());
			SDmanager->AddNewDetector(aTrackerSD);	// Store SD if built	
		}	
	}
	if (data->GetVisualization() == true)
		{volume -> SetSensitiveDetector(VTrackerSD);}
	else
		{volume -> SetSensitiveDetector(aTrackerSD);}
}

void DetectorConstruction::Visualization(G4LogicalVolume* LV, G4Colour Colour)
{
	if (data->GetVisualization() == true)
	{
		G4VisAttributes* ObjectColour = new G4VisAttributes(G4Colour(Colour));	//Cyan
  		LV -> SetVisAttributes(ObjectColour);
	}
}

