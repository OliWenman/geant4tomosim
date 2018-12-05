//Own classes
#include "DetectorConstruction.hh"
#include "DetectorConstructionMessenger.hh"
#include "TomographySD.hh"
#include "TomographyVSD.hh"
#include "FluorescenceSD.hh"
#include "Data.hh"
#include "TargetConstruction.hh"

//Material database
#include "G4NistManager.hh"

//SensitiveDetector manager
#include "G4SDManager.hh"

//Solid shapes
#include "G4Box.hh"

//Logic volume
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

//Physical volume
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"

//Efficient geometry for the millions of detectors
#include "G4PVParameterised.hh"
#include "G4PhantomParameterisation.hh"

//Particle filters
#include "G4VSDFilter.hh"
#include "G4SDParticleFilter.hh"

//Geant4 units
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

//Read/write to a file
#include <iomanip>
#include <fstream>

#include "G4StepLimiter.hh"
#include "G4UserLimits.hh"

DetectorConstruction::DetectorConstruction(Data* DataObject):G4VUserDetectorConstruction(), data(DataObject)
{ 	
	//Create a messenger for this class
  	detectorMessenger = new DetectorConstructionMessenger(this);	
	TC = new TargetConstruction();

	nImage = 0;
	WorldMaterial_Cmd = "G4_AIR";
}

DetectorConstruction::~DetectorConstruction()
{	
	delete detectorMessenger; 
	delete TC;
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{  
	//Setup needed paramenters and variables only on the first image
	if (nImage == 0)
	{
		TC -> SetVisualization(Visualization_Cmd);

		//Checks to see if the detectors are outside the world geometry and displays a warning message
		if (WorldSize_Cmd.y() < DetectorSize_Cmd.y() * NoDetectorsY_Cmd)
		{
			G4cout << "\n////////////////////////////////////////////////////////////////////////////////"
			       << "\n         WARNING: Detectors are outside world volume in Y direction. "
		  	       << "\n         Needs to be at most " << G4BestUnit(WorldSize_Cmd.y()*2, "Length") << ". Currently equal to " << G4BestUnit(DetectorSize_Cmd.y()*NoDetectorsY_Cmd, "Length")
	              	       << "\n////////////////////////////////////////////////////////////////////////////////" << G4endl;
		}

		if (WorldSize_Cmd.z() < DetectorSize_Cmd.z() * NoDetectorsZ_Cmd)
		{	
			G4cout << "\n////////////////////////////////////////////////////////////////////////////////"
		       	       << "\n         WARNING: Detectors are outside world volume in Z direction. "
		       	       << "\n         Needs to be at most " << G4BestUnit(WorldSize_Cmd.z()*2, "Length") << ". Currently equal to " << G4BestUnit(DetectorSize_Cmd.z()*NoDetectorsZ_Cmd, "Length")
	                       << "\n////////////////////////////////////////////////////////////////////////////////" << G4endl;
		}

		//Create an instance of the world geometry
		solidWorld = new G4Box("World", WorldSize_Cmd.x()+DetectorSize_Cmd.x(), WorldSize_Cmd.y(), WorldSize_Cmd.z());

		//Creates the dimensions for the detector
		param = new G4PhantomParameterisation();
		SolidDetectors();	

		FluorDetPos_Cmd = G4ThreeVector(0, WorldSize_Cmd.y()*0.95, 0);
	}

	//Create an instance of the logical volume and find the material
	logicWorld = new G4LogicalVolume(solidWorld, FindMaterial(WorldMaterial_Cmd), "World");

	//Visualization attributes
	Visualization(logicWorld, G4Colour::White());

	//Create the world physical volume. The world is the only physical volume with no mother volume.
	G4VPhysicalVolume* physWorld = new G4PVPlacement(0,            //no rotation
							 G4ThreeVector(),       //at (0,0,0)
							 logicWorld,            //its logical volume
							 "World",               //its name
							 0,                     //its mother  volume
							 false,                 //no boolean operation
							 0,                     //copy number
							 true);		//overlaps checking                     

	//Creates the logic and physical volumes for the detectors each run
	LVDetectors();
	PVDetectors(logicWorld);

	//Construct the target geometry
	TC->Construct(logicWorld);
	
	++nImage;

	//Return the world 
	return physWorld;

}

void DetectorConstruction::SolidDetectors()
{
//-----------------------------------------------------------------
	//Tomography detectors	

	G4int NoDetectorsX = 1;

	G4double HalfDetectorSizeX = DetectorSize_Cmd.x();
	G4double HalfDetectorSizeY = DetectorSize_Cmd.y();
	G4double HalfDetectorSizeZ = DetectorSize_Cmd.z();

	//Create the phantom container for the detectors to go into
	SolidContainer = new G4Box("PhantomContainer",
			           NoDetectorsX*HalfDetectorSizeX,
			           NoDetectorsY_Cmd*HalfDetectorSizeY, 
			           NoDetectorsZ_Cmd*HalfDetectorSizeZ);

	//Create the dimensiosn of the phantom boxes to go inside the container
	SolidPhantomBoxes = new G4Box("PhantomBox",
				      HalfDetectorSizeX,
				      HalfDetectorSizeY,
				      HalfDetectorSizeZ);

	//Voxel dimensions in the three dimensions
	param->SetVoxelDimensions(HalfDetectorSizeX, HalfDetectorSizeY, HalfDetectorSizeZ);

	//Number of voxels in the three dimensions
	param->SetNoVoxel(NoDetectorsX, NoDetectorsY_Cmd, NoDetectorsZ_Cmd);

	//Assure that the voxels are completely filling the container volume
	param->CheckVoxelsFillContainer(SolidContainer->GetXHalfLength(),
                                        SolidContainer->GetYHalfLength(),
                                        SolidContainer->GetZHalfLength());

//------------------------------------------------------------------
	//FLUORESCENCE DETECTOR
	if (FluorescenceDet == true)
	{
		SolidFluoDet = new G4Box("FluorescenceDetector",
			         	 FluorDetSize_Cmd.x(),
					 FluorDetSize_Cmd.y(),
					 FluorDetSize_Cmd.z());
	}
}

void DetectorConstruction::LVDetectors()
{
//-------------------------------------------------------------------
	//TOMOGRAPHY DETECTORS

	//Pick the material for the detectors based on if the detectors are 100% efficient or not
	G4Material* DetMaterial;
	if (DetectorEfficiency_Cmd == false)
	{	
		DetMaterial = FindMaterial(DetectorMaterial_Cmd);
	}
	else if (DetectorEfficiency_Cmd == true)
	{	
		DetMaterial = FindMaterial("G4_Galactic");
	}

	//Creates the logical volume for the phantom container	
	container_logic = new G4LogicalVolume(SolidContainer, 
					      DetMaterial, 
					      "LVPhantomContainer");

	//The parameterised volume which uses this parameterisation is placed in the container logical volume
	PhantomBoxes_logic = new G4LogicalVolume(SolidPhantomBoxes,
           				         DetMaterial,        // material is not relevant here...
           				         "LVPhantomBox");

	//Vector of materials of the voxels
	std::vector < G4Material* > theMaterials;
	theMaterials.push_back(DetMaterial);
	param -> SetMaterials(theMaterials);

	//Make the detectors sensitive to hits
	AttachSensitiveDetector(PhantomBoxes_logic, "TomographyDetector");

	Visualization(container_logic, G4Colour::Red());
	Visualization(PhantomBoxes_logic, G4Colour::Cyan());

//------------------------------------------------------------------
	//FLUORESCENCE DETECTOR
	if (FluorescenceDet == true)
	{
		FluoDet_logic = new G4LogicalVolume(SolidFluoDet,
					    	    FindMaterial("G4_Galactic"),
				                    "LVFluorescenceDetector");

		//Make the detectors sensitive to hits
		AttachSensitiveDetector(FluoDet_logic, "FluorescenceDetector");
		
		Visualization(FluoDet_logic, G4Colour::Cyan());	
	}
}

void DetectorConstruction::PVDetectors(G4LogicalVolume* logicMotherBox)
{
	G4double DetectorPosX = WorldSize_Cmd.x();
	G4int NumberOfVoxels = NoDetectorsY_Cmd * NoDetectorsZ_Cmd;

	G4VPhysicalVolume* container_phys = new G4PVPlacement(0,                  // rotation
            						      G4ThreeVector(DetectorPosX,0,0),                   // translation
           						      container_logic,            // logical volume
            						      "PhantomContainer",    // name
            						      logicMotherBox,           // mother volume
            						      false,                 // No op. bool.
           						      0,			//copy number
							      false);                    //overlap checking

	//Build the phantom container
	param -> BuildContainerSolid(container_phys);

	//
	G4PVParameterised* PhantomBoxes_phys = new G4PVParameterised("PhantomBoxes",               // name
                       						     PhantomBoxes_logic,           // logical volume
                        					     container_logic,              // mother volume
           							     kUndefined,                  // optimisation hint
                       						     NumberOfVoxels, 		// number of voxels
                       						     param);                  // parameterisation

	//Gives warning messages when set to 1?
	PhantomBoxes_phys->SetRegularStructureId(1);

//------------------------------------------------------------------
	//FLUORESCENCE DETECTOR	
	if (FluorescenceDet == true)
	{
		G4VPhysicalVolume* phyFluoDetector = new G4PVPlacement(0,
							       	       FluorDetPos_Cmd,
							               FluoDet_logic,
							               "Fluorecense detector",
							               logicMotherBox,
							               false,
							               0,
							               false);
	}
}

G4Material* DetectorConstruction::FindMaterial(G4String MaterialName)
{
	//Obtain pointer to NIST material manager
	//Build materials 
	return G4NistManager::Instance() -> FindOrBuildMaterial(MaterialName);;
}

void DetectorConstruction::AttachSensitiveDetector(G4LogicalVolume* volume, G4String TypeDetector) 
{
	// Avoid unnecessary work
  	if (!volume) 
		{return;}                  

  	// Check if sensitive detector has already been created
 	G4SDManager* SDmanager = G4SDManager::GetSDMpointer();
  	G4VSensitiveDetector* theSD = SDmanager->FindSensitiveDetector(TypeDetector, false);

  	if (!theSD) 
	{
		//If the sensitive detector hasn't already been created, create one
		if (Visualization_Cmd == true && TypeDetector == "TomographyDetector")
		{
			//Create a visual detector
			tomoVSD = new TomographyVSD("TomographyDetector", "TrackerHitsCollection", NoDetectorsY_Cmd, NoDetectorsZ_Cmd, data, DetectorEfficiency_Cmd);
			SDmanager->AddNewDetector(tomoVSD);	// Store SD if built	

			G4SDParticleFilter* gammaFilter = new G4SDParticleFilter("GammaFilter", "gamma");
			tomoVSD -> SetFilter(gammaFilter);
		}
		else if (TypeDetector == "TomographyDetector")
		{
			//Create a detector optimised for perfomance
    			tomoSD = new TomographySD("TomographyDetector", data, DetectorEfficiency_Cmd);
			SDmanager->AddNewDetector(tomoSD);	// Store SD if built	

			G4SDParticleFilter* gammaFilter = new G4SDParticleFilter("GammaFilter", "gamma");
			tomoSD -> SetFilter(gammaFilter);
		}	
		else if (TypeDetector == "FluorescenceDetector")
		{
			fluorescenceSD = new FluorescenceSD("FluorescenceDetector", data, DetectorEfficiency_Cmd);
			SDmanager->AddNewDetector(fluorescenceSD);

			G4SDParticleFilter* gammaFilter = new G4SDParticleFilter("GammaFilter", "gamma");
			fluorescenceSD -> SetFilter(gammaFilter);
		}
	}

	//Add the correct sensitive detector to the logical volume
	if (Visualization_Cmd == true && TypeDetector == "TomographyDetector")
		{volume -> SetSensitiveDetector(tomoVSD);}
	else if (TypeDetector == "TomographyDetector")
		{volume -> SetSensitiveDetector(tomoSD);}
	else if (TypeDetector == "FluorescenceDetector")
		{volume -> SetSensitiveDetector(fluorescenceSD);}
}

void DetectorConstruction::Visualization(G4LogicalVolume* LV, G4Colour Colour)
{
	//Set a visualization setting only if the setting is turned on
	if (Visualization_Cmd == true)
	{
		G4VisAttributes* ObjectColour = new G4VisAttributes(G4Colour(Colour));	
  		LV -> SetVisAttributes(ObjectColour);
	}
}

void DetectorConstruction::RelayToTC(int NumberOfImages, double TotalAngle)
{
	TC -> SetFullRotationAngle(TotalAngle);
	TC -> SetTotalImages(NumberOfImages);
}

void DetectorConstruction::ReadOutInfo(G4String SaveFilePath)
{
	TC -> ReadOutInfo();

	G4ThreeVector FullDetDimensions = G4ThreeVector(DetectorSize_Cmd.x()*1, DetectorSize_Cmd.y()*NoDetectorsY_Cmd, DetectorSize_Cmd.z()*NoDetectorsZ_Cmd);

	G4cout << "\nWORLD SETUP: \n"
	       << "\n- World dimensions: " << G4BestUnit(WorldSize_Cmd, "Length")
	       << "\n- World Material: " << WorldMaterial_Cmd << "\n"

	       << "\nTHE DETECTOR SETUP: \n"

	       << "\nTransmission detectors: "
	       << "\n- Number of detectors: " << NoDetectorsY_Cmd << " x " << NoDetectorsZ_Cmd << " = " << NoDetectorsY_Cmd * NoDetectorsZ_Cmd
	       << "\n- Individual detector dimensions: " << G4BestUnit(DetectorSize_Cmd, "Length")
	       << "\n- Full detector dimensions: " << G4BestUnit(FullDetDimensions, "Length") << G4endl;

	if (FluorescenceDet == true)
	{
		G4cout << "\nFluorescence detector: "
		       << "\n- Dimensions: " << G4BestUnit(FluorDetSize_Cmd, "Length")
		       << "\n- Position: " << G4BestUnit(FluorDetPos_Cmd, "Length") 
		       << "\n- Number of bins: " << data -> GetNoBins() << G4endl;
	}
	if (DetectorEfficiency_Cmd == false)
	{	
		G4cout << "\n- Detector material: " << DetectorMaterial_Cmd << G4endl;
	}
	else
	{	
		G4cout << "\n- Detectors are assumed to be 100%\ efficient " << G4endl;
	}
	
	//Creation of the writing to data file stream
	std::ofstream outdata; 

	G4String SettingsName = "OutputLog.txt";

	//Open the file within the path set
	outdata.open(SaveFilePath+SettingsName); 
   	
	//Output error if can't open file
	if( !outdata ) 
	{ 	std::cerr << "\nError: " << SettingsName << " file could not be opened from DetectorConstruction.\n" << std::endl;
      		exit(1);
   	}

	outdata << "\nWORLD SETUP: \n"
	        << "\n- World dimensions: " << G4BestUnit(WorldSize_Cmd, "Length")
	        << "\n- World Material: " << WorldMaterial_Cmd << "\n"

	        << "\nTHE DETECTOR SETUP:\n"

		<< "\nTransmission detectors: "
	        << "\n- Number of detectors: " << NoDetectorsY_Cmd << " x " << NoDetectorsZ_Cmd << " = " << NoDetectorsY_Cmd * NoDetectorsZ_Cmd
	        << "\n- Individual detector dimensions: " << G4BestUnit(DetectorSize_Cmd, "Length")
		<< "\n- Full detector dimensions: " << G4BestUnit(FullDetDimensions, "Length");

	if (FluorescenceDet == true)
	{
		outdata << "\nFluorescence detector: "
		        << "\n- Dimensions: " << G4BestUnit(FluorDetSize_Cmd, "Length")
		        << "\n- Position: " << G4BestUnit(FluorDetPos_Cmd, "Length") 
		        << "\n- Number of bins: " << data -> GetNoBins() << std::endl;
	}
	if (DetectorEfficiency_Cmd == false)
	{	
		outdata << "\n- Detector material: " << DetectorMaterial_Cmd << "\n";
	}
	else
	{	
		outdata << "\n- Detectors are assumed to be 100%\ efficient \n";
	}

	outdata.close();
}

