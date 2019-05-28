
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

AbsorptionDetector::AbsorptionDetector() : container(0), cells(0),
                                           container_logic(0), cells_logic(0),
                                           container_placement(0), cells_placement(0)
{
    messenger = new AbsorptionDetectorMessenger(this);
    param = new G4PhantomParameterisation();
    
    // Check if sensitive detector has already been created
 	G4SDManager* SDmanager = G4SDManager::GetSDMpointer();

	//Create a visual detector
	absorSD = new AbsorptionSD();
	SDmanager->AddNewDetector(absorSD);	// Store SD if built	
}

AbsorptionDetector::~AbsorptionDetector()
{
    delete messenger;
}
    
void AbsorptionDetector::CreateVolumes()
{
    if (container) {delete container; container = 0;}

	//Create the phantom container for the detectors to go into
	container = new G4Box("Absorptioncontainer",
			              halfdimensions.x(),
			              halfdimensions.y(), 
			              halfdimensions.z());

    if (cells) {delete cells; container = 0;}

	//Create the dimensiosn of the phantom boxes to go inside the container
	cells = new G4Box("AbsorptionCell",
				       halfdimensions.x()/1,
				       halfdimensions.y()/xpixels,
				       halfdimensions.z()/ypixels);

	//Voxel dimensions in the three dimensions
	param->SetVoxelDimensions(halfdimensions.x()/1, halfdimensions.y()/xpixels, halfdimensions.z()/ypixels);
	
	//Number of voxels in the three dimensions
	param->SetNoVoxel(1, xpixels, ypixels);
	
	//Assure that the voxels are completely filling the container volume
	param->CheckVoxelsFillContainer(container->GetXHalfLength(),
                                    container->GetYHalfLength(),
                                    container->GetZHalfLength());
}

void AbsorptionDetector::AddProperties(G4bool GraphicsOn)
{
	//Pick the material for the detectors based on if the detectors are 100% efficient or not
	G4Material* material = G4NistManager::Instance() -> FindOrBuildMaterial("G4_Galactic");
	
	G4MaterialPropertiesTable* mpt = material -> GetMaterialPropertiesTable();
	if (!mpt)
	{
	    mpt = new G4MaterialPropertiesTable();
	    
	    double energy[] = {0.2*keV};
	    double rindex[] = {1};
	    int size = 1;
	    
	    mpt -> AddProperty("RINDEX", energy, rindex, size);
	    
	    material -> SetMaterialPropertiesTable(mpt);
	}

    //Creates the logical volume for the phantom container	
    if (container_logic) {delete container_logic; container_logic = 0;}
	container_logic = new G4LogicalVolume(container, 
                					      material, 
                					      "AbsorptionContainer");
                   					      					      
    G4VisAttributes* Red = new G4VisAttributes(G4Colour::Red());	
  	container_logic -> SetVisAttributes(Red);

    //The parameterised volume which uses this parameterisation is placed in the container logical volume
    if (cells_logic) {delete cells_logic;}
	cells_logic = new G4LogicalVolume(cells,
                   				      material,        // material is not relevant here...
                   				      "AbsorptionCells");

    G4VisAttributes* Cyan = new G4VisAttributes(G4Colour::Cyan);	
  	cells_logic -> SetVisAttributes(Cyan);
  	
  	if (xpixels*ypixels > 30*30)
    {
	    G4VisAttributes* VisAttributes = new G4VisAttributes();	
	    VisAttributes -> SetVisibility(false);
	    cells_logic -> SetVisAttributes(VisAttributes);
	    //container_logic -> SetDaughtersInvisible(true);
    }
	
	// Check if sensitive detector has already been created
 	G4SDManager* SDmanager = G4SDManager::GetSDMpointer();

	//G4SDParticleFilter* gammaFilter = new G4SDParticleFilter("GammaFilter", "gamma");
	//tomoVSD -> SetFilter(gammaFilter);

	//Add the sensitive detector to the logical volume
	cells_logic -> SetSensitiveDetector(absorSD);
	absorSD->SetGraphics(GraphicsOn);
	
	//Vector of materials of the voxels
	std::vector < G4Material* > theMaterials;
	theMaterials.push_back(material);
	param -> SetMaterials(theMaterials);
	
	absorSD->SetGraphics(GraphicsOn);
}

#include "G4PhysicalVolumeStore.hh"

void AbsorptionDetector::PlaceDetectors(G4LogicalVolume* MotherBox, G4ThreeVector position)
{
	G4int NumberOfVoxels = xpixels * ypixels;
	
    if (container_placement) {delete container_placement; container_placement = 0;}
	container_placement = new G4PVPlacement(0,                          // rotation
            						        position,                   // translation
           						            container_logic,            // logical volume
            						        "AbsorptionContainer",      // name
            						        MotherBox,                  // mother volume
            						        false,                      // No op. bool.
           						            0,			                // copy number
							                false);                     // overlap checking

	//Build the phantom container
	param -> BuildContainerSolid(container_placement);

    if (cells_placement) {delete cells_placement; cells_placement = 0;}
	cells_placement = new G4PVParameterised("AbsorptionCells",     // name
                       						cells_logic,           // logical volume
                        					container_logic,       // mother volume
           							        kUndefined,            // optimisation hint
                       						NumberOfVoxels, 	   // number of voxels
                       						param);                // parameterisation

	//Gives warning messages when set to 1?
	cells_placement->SetRegularStructureId(1);
	absorSD->Set_xpixels(xpixels);
	absorSD->Set_ypixels(ypixels);
}

#include "PrintLines.hh"

void AbsorptionDetector::ReadOutInfo(SettingsLog& log)
{
    PrintToEndOfTerminal(log, '-');
	log << "ABSORPTION DETECTOR:"
		   "\n - Number of pixels: " << xpixels << " x " << ypixels << " = " << xpixels*ypixels <<
		   "\n - Pixel half dimensions: " << G4BestUnit(G4ThreeVector(halfdimensions.x(), halfdimensions.y()/xpixels, halfdimensions.z()/ypixels), "Length") <<
		   "\n - Detector half dimensions: " << G4BestUnit(halfdimensions, "Length");
		    
}
