#include "DiffractionDetector.hh"
//#include "RefractionDetector.hh"
#include "DiffractionSD.hh"
#include "Data.hh"
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

DiffractionDetector::DiffractionDetector()
{
    //ADMessenger = new AbsorptionDetectorMessenger(this);
    diffraSD = NULL;
}

DiffractionDetector::~DiffractionDetector()
{
    //delete ADMessenger;
}
    
void DiffractionDetector::CreateVolumes()
{
    param = new G4PhantomParameterisation();

	//Create the phantom container for the detectors to go into
	DetectorContainer = new G4Box("DiffractionDetectorContainer",
			                       halfdimensions.x(),
			                       halfdimensions.y(), 
			                       halfdimensions.z());

	//Create the dimensiosn of the phantom boxes to go inside the container
	DetectorCells = new G4Box("DiffractionCell",
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

void DiffractionDetector::AddProperties(Data* data, G4bool GraphicsOn)
{
    materialName = "G4_Galactic";
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
                					          "DiffractionContainerLV");
                   					      					      
    G4VisAttributes* Red = new G4VisAttributes(G4Colour::Red());	
  	DetectorContainerLV -> SetVisAttributes(Red);

	//The parameterised volume which uses this parameterisation is placed in the container logical volume
	DetectorCellsLV = new G4LogicalVolume( DetectorCells,
                   				           Material,        // material is not relevant here...
                   				          "DiffractionCellsLV");

    G4VisAttributes* Cyan = new G4VisAttributes(G4Colour::Cyan);	
  	DetectorCellsLV -> SetVisAttributes(Cyan);
  	
  	/*if (rows*columns > 30*30)
    {
	    G4VisAttributes* VisAttributes = new G4VisAttributes();	
	    VisAttributes -> SetVisibility(false);
	    DetectorCellsLV -> SetVisAttributes(VisAttributes);
	    //DetectorContainerLV -> SetDaughtersInvisible(true);
    }*/
	
	// Check if sensitive detector has already been created
 	G4SDManager* SDmanager = G4SDManager::GetSDMpointer();

  	if (!diffraSD) 
	{
		//Create a visual detector
		diffraSD = new DiffractionSD(data, GraphicsOn);
		SDmanager->AddNewDetector(diffraSD);	// Store SD if built	

		//G4SDParticleFilter* gammaFilter = new G4SDParticleFilter("GammaFilter", "gamma");
		//tomoVSD -> SetFilter(gammaFilter);
	}

	//Add the sensitive detector to the logical volume
	DetectorCellsLV -> SetSensitiveDetector(diffraSD);
	
	//Vector of materials of the voxels
	std::vector < G4Material* > theMaterials;
	theMaterials.push_back(Material);
	param -> SetMaterials(theMaterials);
	
	//data -> SetHalfDetectorDimensions(halfdimensions);	 
}

void DiffractionDetector::PlaceDetectors(G4LogicalVolume* MotherBox, G4ThreeVector position)
{
	G4int NumberOfVoxels = rows * columns;

	G4VPhysicalVolume* container_phys = new G4PVPlacement(0,                  // rotation
            						                      position,                   // translation
           						                          DetectorContainerLV,            // logical volume
            						                      "DiffractionContainer",    // name
            						                      MotherBox,           // mother volume
            						                      false,                 // No op. bool.
           						                          0,			//copy number
							                              true);                    //overlap checking

	//Build the phantom container
	param -> BuildContainerSolid(container_phys);

	//
	G4PVParameterised* PhantomBoxes_phys = new G4PVParameterised("DiffractionCells",               // name
                       						                     DetectorCellsLV,           // logical volume
                        					                     DetectorContainerLV,              // mother volume
           							                             kUndefined,                  // optimisation hint
                       						                     NumberOfVoxels, 		// number of voxels
                       						                     param);                  // parameterisation

	//Gives warning messages when set to 1?
	PhantomBoxes_phys->SetRegularStructureId(1);
}

#include "PrintLines.hh"

void DiffractionDetector::ReadOutInfo(SettingsLog& log)
{
    /*PrintToEndOfTerminal(log, '-');
	log << "ABSORPTION DETECTOR:"
		   "\n - Number of pixels: " << rows << " x " << columns << " = " << rows*columns <<
		   "\n - Pixel half dimensions: " << G4BestUnit(G4ThreeVector(halfdimensions.x(), halfdimensions.y()/rows, halfdimensions.z()/columns), "Length") <<
		   "\n - Detector half dimensions: " << G4BestUnit(halfdimensions, "Length");
		    */
}
