/*
A class that builds the absorption detectors used in a simulation. 
It will build the detectors G4VSolids, G4LogicalVolume and G4PhysicalVolume 
as well as contain the pointer to the sensitive detector class
AbsorptionSD, which determines what happens when a particle is detected.

Author: Oliver Jack Wenman
*/

#include "AbsorptionDetector.hh"
#include "AbsorptionDetectorMessenger.hh"
#include "AbsorptionSD.hh"

#include "G4Box.hh"
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

AbsorptionDetector::AbsorptionDetector() : container(0), cells(0),
                                           container_logic(0), cells_logic(0),
                                           container_placement(0), cells_placement(0)
{
    //Create the messenger class to control the setup via macrofiles, AbsorptionDetectorMessenger
    messenger = new AbsorptionDetectorMessenger(this);
    
    //Needed to make millions of volumes of same material
    param     = new G4PhantomParameterisation();
    
    //Create the sensitive detector and add it to Geant4's SDmanager
 	G4SDManager* SDmanager = G4SDManager::GetSDMpointer();
	absorSD = new AbsorptionSD();
	SDmanager->AddNewDetector(absorSD);	
}

AbsorptionDetector::~AbsorptionDetector()
{
    delete messenger;
     
    //All other pointers are deleted by respected Geant4 manager class when
    //Simulated is deleted
}

//Create the G4VSolids of the detector 
void AbsorptionDetector::CreateVolumes()
{
    //Checks to see if it exists and deletes it if it does.
    //Allows for rebuilding of geometry correctly.
    if (container) {delete container; container = 0;}

	//Create the container for the detectors to go into
	container = new G4Box("Absorptioncontainer",
			              halfdimensions.x(),
			              halfdimensions.y(), 
			              halfdimensions.z());

    //Checks to see if it exists and deletes it if it does.
    //Allows for rebuilding of geometry correctly.
    if (cells) {delete cells; container = 0;}

	//Create the pixel cell boxes to go inside the container
	cells = new G4Box("AbsorptionCell",
				       halfdimensions.x()/1,
				       halfdimensions.y()/xpixels,
				       halfdimensions.z()/ypixels);

	//Tell the param class (G4PhantomParameterisation created in AbsorptionDetector's constructor) the dimensions 
	//of the pixel cell
	param->SetVoxelDimensions(halfdimensions.x()/1, halfdimensions.y()/xpixels, halfdimensions.z()/ypixels);
	
	//Number of voxels in the three dimensions
	param->SetNoVoxel(1, xpixels, ypixels);
	
	//Assure that the voxels are completely filling the container volume
	param->CheckVoxelsFillContainer(container->GetXHalfLength(),
                                    container->GetYHalfLength(),
                                    container->GetZHalfLength());
}

//Create the G4LogicalVolume
void AbsorptionDetector::AddProperties(G4bool graphicsOn)
{
    //Tell the sensitivedetector that graphics are on.
	//Therefore will visualize a hit
	absorSD->SetGraphics(graphicsOn);

	//Pick the material for the detectors based on if the detectors are 100% efficient
	G4Material* material = G4NistManager::Instance() -> FindOrBuildMaterial("G4_Galactic");
	
	//Check if a G4MaterialPropertiesTable exists for the material chosen.
	//Needed if opticalphotons used so they interact with particle
	G4MaterialPropertiesTable* mpt = material -> GetMaterialPropertiesTable();
	if (!mpt)
	{
	    //Create dummy array and add it as a property
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
                   					     
    //Add visualisation attribute 					      
    G4VisAttributes* red = new G4VisAttributes(G4Colour::Red());	
  	container_logic -> SetVisAttributes(red);

    //The parameterised volume which uses this parameterisation is placed in the container logical volume
    if (cells_logic) {delete cells_logic;}
	cells_logic = new G4LogicalVolume(cells,
                   				      material,        // material is not relevant here...
                   				      "AbsorptionCells");

    //Add visualisation attribute 					  
    G4VisAttributes* cyan = new G4VisAttributes(G4Colour::Cyan);	
  	cells_logic -> SetVisAttributes(cyan);
	
	// Check if sensitive detector has already been created
 	G4SDManager* SDmanager = G4SDManager::GetSDMpointer();

	//G4SDParticleFilter* gammaFilter = new G4SDParticleFilter("GammaFilter", "gamma");
	//tomoVSD -> SetFilter(gammaFilter);

	//Add the sensitive detector to the logical volume
	cells_logic -> SetSensitiveDetector(absorSD);
	
	//Vector of materials of the voxels
	std::vector < G4Material* > theMaterials;
	theMaterials.push_back(material);
	param -> SetMaterials(theMaterials);
}

//Creates the G4PVPlacement for the detector (placing it in the world volume)
void AbsorptionDetector::PlaceDetectors(G4LogicalVolume* motherbox, G4ThreeVector position)
{
	G4int n_pixels = xpixels * ypixels;
	
	//Place the detector volume
    if (container_placement) {delete container_placement; container_placement = 0;}
	container_placement = new G4PVPlacement(0,                          // rotation
            						        position,                   // translation
           						            container_logic,            // logical volume
            						        "AbsorptionContainer",      // name
            						        motherbox,                  // mother volume
            						        false,                      // No op. bool.
           						            0,			                // copy number
							                false);                     // overlap checking

	//Build the phantom container
	param -> BuildContainerSolid(container_placement);

    //Build the pixels
    if (cells_placement) {delete cells_placement; cells_placement = 0;}
	cells_placement = new G4PVParameterised("AbsorptionCells",     // name
                       						cells_logic,           // logical volume
                        					container_logic,       // mother volume
           							        kUndefined,            // optimisation hint
                       						n_pixels, 	           // number of voxels
                       						param);                // parameterisation

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
