#include "AbsorptionDetector.hh"
#include "AbsorptionDetectorMessenger.hh"
#include "AbsorptionSD.hh"
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
#include "SettingsLog.hh"

#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include "PlacementTest.hh"

AbsorptionDetector::AbsorptionDetector()
{
    ADMessenger = new AbsorptionDetectorMessenger(this);
    absorSD = NULL;
    param = new G4PhantomParameterisation();
    
    // Check if sensitive detector has already been created
 	G4SDManager* SDmanager = G4SDManager::GetSDMpointer();

	//Create a visual detector
	absorSD = new AbsorptionSD();
	SDmanager->AddNewDetector(absorSD);	// Store SD if built	
}

AbsorptionDetector::~AbsorptionDetector()
{
    delete ADMessenger;
}
    
void AbsorptionDetector::CreateVolumes()
{
	//Create the phantom container for the detectors to go into
	DetectorContainer = new G4Box("DetectorContainer",
			                       halfdimensions.x(),
			                       halfdimensions.y(), 
			                       halfdimensions.z());

	//Create the dimensiosn of the phantom boxes to go inside the container
	DetectorCells = new G4Box("AbsorptionCell",
				               halfdimensions.x()/1,
				               halfdimensions.y()/rows,
				               halfdimensions.z()/columns);

	//Voxel dimensions in the three dimensions
	param->SetVoxelDimensions(halfdimensions.x()/1, halfdimensions.y()/rows, halfdimensions.z()/columns);
	//Number of voxels in the three dimensions
	param->SetNoVoxel(1, rows, columns);
	//Assure that the voxels are completely filling the container volume
	param->CheckVoxelsFillContainer(DetectorContainer->GetXHalfLength(),
                                    DetectorContainer->GetYHalfLength(),
                                    DetectorContainer->GetZHalfLength());
}

void AbsorptionDetector::AddProperties(G4bool GraphicsOn)
{
	//Pick the material for the detectors based on if the detectors are 100% efficient or not
	G4Material* Material = G4NistManager::Instance() -> FindOrBuildMaterial("G4_Galactic");
	
	G4MaterialPropertiesTable* MPT = Material -> GetMaterialPropertiesTable();
	if (!MPT)
	{
	    MPT = new G4MaterialPropertiesTable();
	    
	    double energy[] = {0.2*keV};
	    double rindex[] = {1};
	    int size = 1;
	    
	    MPT -> AddProperty("RINDEX", energy, rindex, size);
	    
	    Material -> SetMaterialPropertiesTable(MPT);
	}

	//Creates the logical volume for the phantom container	
	DetectorContainerLV = new G4LogicalVolume( DetectorContainer, 
                					           Material, 
                					          "AbsorptionContainerLV");
                   					      					      
    G4VisAttributes* Red = new G4VisAttributes(G4Colour::Red());	
  	DetectorContainerLV -> SetVisAttributes(Red);

	//The parameterised volume which uses this parameterisation is placed in the container logical volume
	DetectorCellsLV = new G4LogicalVolume( DetectorCells,
                   				           Material,        // material is not relevant here...
                   				          "AbsorptionCellsLV");

    G4VisAttributes* Cyan = new G4VisAttributes(G4Colour::Cyan);	
  	DetectorCellsLV -> SetVisAttributes(Cyan);
  	
  	if (rows*columns > 30*30)
    {
	    G4VisAttributes* VisAttributes = new G4VisAttributes();	
	    VisAttributes -> SetVisibility(false);
	    DetectorCellsLV -> SetVisAttributes(VisAttributes);
	    //DetectorContainerLV -> SetDaughtersInvisible(true);
    }
	
	// Check if sensitive detector has already been created
 	G4SDManager* SDmanager = G4SDManager::GetSDMpointer();

	//G4SDParticleFilter* gammaFilter = new G4SDParticleFilter("GammaFilter", "gamma");
	//tomoVSD -> SetFilter(gammaFilter);

	//Add the sensitive detector to the logical volume
	DetectorCellsLV -> SetSensitiveDetector(absorSD);
	
	//Vector of materials of the voxels
	std::vector < G4Material* > theMaterials;
	theMaterials.push_back(Material);
	param -> SetMaterials(theMaterials);
	
	absorSD->SetGraphics(GraphicsOn);
}

#include "G4PhysicalVolumeStore.hh"

void AbsorptionDetector::PlaceDetectors(G4LogicalVolume* MotherBox, G4ThreeVector position)
{
	G4int NumberOfVoxels = rows * columns;
	
	/*G4PhysicalVolumeStore* volumestore = G4PhysicalVolumeStore::GetInstance();
	G4VPhysicalVolume* check = volumestore->GetVolume("AbsorptionContainer", false);
	
	if (check)
	{
	    //delete check;
	    //check -> Set
	}*/

	G4VPhysicalVolume* container_phys = new PlacementTest(0,                  // rotation
            						                      position,                   // translation
           						                          DetectorContainerLV,            // logical volume
            						                      "AbsorptionContainer",    // name
            						                      MotherBox,           // mother volume
            						                      false,                 // No op. bool.
           						                          0,			//copy number
							                              false);                    //overlap checking

	//Build the phantom container
	param -> BuildContainerSolid(container_phys);

	//
	G4PVParameterised* PhantomBoxes_phys = new G4PVParameterised("AbsorptionCells",               // name
                       						                     DetectorCellsLV,           // logical volume
                        					                     DetectorContainerLV,              // mother volume
           							                             kUndefined,                  // optimisation hint
                       						                     NumberOfVoxels, 		// number of voxels
                       						                     param);                  // parameterisation

	//Gives warning messages when set to 1?
	PhantomBoxes_phys->SetRegularStructureId(1);
	
	absorSD->InitialiseData(rows, columns);
}

#include "PrintLines.hh"

void AbsorptionDetector::ReadOutInfo(SettingsLog& log)
{
    PrintToEndOfTerminal(log, '-');
	log << "ABSORPTION DETECTOR:"
		   "\n - Number of pixels: " << rows << " x " << columns << " = " << rows*columns <<
		   "\n - Pixel half dimensions: " << G4BestUnit(G4ThreeVector(halfdimensions.x(), halfdimensions.y()/rows, halfdimensions.z()/columns), "Length") <<
		   "\n - Detector half dimensions: " << G4BestUnit(halfdimensions, "Length");
		    
}
