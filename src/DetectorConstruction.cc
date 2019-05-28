//Own classes
#include "DetectorConstruction.hh"
#include "DetectorConstructionMessenger.hh"
#include "AbsorptionDetector.hh"
#include "FluorescenceDetector.hh"
#include "DiffractionDetector.hh"
#include "SampleConstruction.hh"
//Output to console/write to file
#include "SettingsLog.hh"

//Material database
#include "G4NistManager.hh"

//Solid shapes
#include "G4Box.hh"

//Logic volume
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

//Physical volume
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"

//Particle filters
#include "G4VSDFilter.hh"
#include "G4SDParticleFilter.hh"

//Geant4 units
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

#include "G4StepLimiter.hh"
#include "G4UserLimits.hh"

#include "G4Element.hh"
#include "G4MaterialPropertiesTable.hh"

#include "PrintLines.hh"

#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4RegionStore.hh"
#include "G4GeometryManager.hh"

DetectorConstruction::DetectorConstruction() : G4VUserDetectorConstruction(), 
                                               solidWorld(0), 
                                               logicWorld(0), 
                                               physWorld(0),
                                               detectorMessenger(0),
                                               absDetector(0),
                                               fluorescenceDetector(0),
                                               diffractionDetector(0),
                                               sampleconstruction(0)
{ 	
	//Create a messenger for this class
  	detectorMessenger    = new DetectorConstructionMessenger(this);	
	absDetector          = new AbsorptionDetector();
	fluorescenceDetector = new FluorescenceDetector();
	//diffractionDetector  = new DiffractionDetector();
	
	sampleconstruction  = new SampleConstruction(); 
	
	//Defualt values
	world_material = "G4_AIR";
	world_dimensions = G4ThreeVector(20*cm, 10*cm, 2.1*cm);
	
	built = false;
}

DetectorConstruction::~DetectorConstruction()
{	
	//Delete the messenger and Target Construct class
	delete detectorMessenger; 
	
	delete absDetector;
	delete fluorescenceDetector;
	delete diffractionDetector;
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{  
    if (!built)
    {
        //Build these first so sample geometry can be built using commands
	    if (solidWorld) {delete solidWorld; solidWorld = 0;}
	    solidWorld = new G4Box("World", 
	                           world_dimensions.x(), 
	                           world_dimensions.y(), 
	                           world_dimensions.z());
    
		//TC -> SetVisualization(Visualization_Cmd);

		//Creates the dimenions of the detectors
		absDetector -> CreateVolumes();
        fluorescenceDetector -> CreateVolumes();
        /*
        diffractionDetector -> SetNumberOfxPixels(absDetector->GetNumberOfxPixels());
        diffractionDetector -> SetNumberOfyPixels(absDetector->GetNumberOfyPixels());
        diffractionDetector -> SetHalfDimensions(absDetector->GetG4VectHalfDimensions());
        diffractionDetector -> CreateVolumes();
	    */
	
	    G4Material* material = G4NistManager::Instance() -> FindOrBuildMaterial(world_material);
	    G4MaterialPropertiesTable* mpt = material -> GetMaterialPropertiesTable();
	
	    //Checks if the world material has a materials property table for refraction.
	    //If it doesn't give it a refractive index of 1 so refraction still works
	    if (!mpt)
	    {
	        mpt = new G4MaterialPropertiesTable();
	    
	        double energy[] = {0.2*keV};
	        double rindex[] = {1};
	        int size = 1;
	    
	        mpt -> AddProperty("RINDEX", energy, rindex, size);
	    
	        material -> SetMaterialPropertiesTable(mpt);
	    }
	    
	    if (logicWorld) {delete logicWorld; logicWorld = 0;}
	    logicWorld = new G4LogicalVolume(solidWorld, 
	                                     material, 
	                                    "World");
	
	    //Visualization attributes
	    G4VisAttributes* objectcolour = new G4VisAttributes(G4Colour::White());	
  	    logicWorld -> SetVisAttributes(objectcolour);
    
        //Create the world physical volume. The world is the only physical volume with no mother volume.
        if (physWorld) {delete physWorld; physWorld = 0;}
	    physWorld = new G4PVPlacement(0,                    //no rotation
							          G4ThreeVector(),      //translation (0,0,0)
							          logicWorld,           //its logical volume
							          "World",              //its name
							          0,                    //its mother  volume
							          false,                //no boolean operation
							          0,                    //copy number
							          true);		        //overlaps checking    

	    //Creates the logic and physical volumes for the detectors each run
	    absDetector -> AddProperties(Visualization_Cmd);
        fluorescenceDetector -> AddProperties(Visualization_Cmd);
        //diffractionDetector -> AddProperties(Visualization_Cmd);
	
	    absDetector -> PlaceDetectors(logicWorld, G4ThreeVector(solidWorld->GetXHalfLength(),0,0) );
	    fluorescenceDetector -> PlaceDetectors(logicWorld, G4ThreeVector(0, absDetector->GetG4HalfDimensions().y()*1.15, 0));
	
	    sampleconstruction->RemovePlacement(true);
	
	    //double defualtPosX = solidWorld->GetXHalfLength()-absDetector -> GetG4VectHalfDimensions().x()*2;
	    //double defualtPosY = absDetector -> GetG4VectHalfDimensions().y()*2;
	    //double defualtPosZ = 0;
	
        //diffractionDetector -> PlaceDetectors(logicWorld, G4ThreeVector(defualtPosX, defualtPosY, defualtPosZ));
	    
	    built = true;
	}

	//Return the world 
	return physWorld;
}

void DetectorConstruction::ReadOutInfo(SettingsLog& log)
{
	PrintToEndOfTerminal(log, '-');
	log << "WORLD SETUP:"
	       "\n- World dimensions: " << G4BestUnit(world_dimensions, "Length")
	    << "\n- World Material: " << world_material;
	    
	absDetector -> ReadOutInfo(log);
	fluorescenceDetector -> ReadOutInfo(log);
}

#include "G4RunManager.hh"
#include "G4GeometryManager.hh"
void DetectorConstruction::ModifyWorldVolume(G4ThreeVector newdimensions)
{
    //G4GeometryManager::GetInstance()->OpenGeometry();

    solidWorld->SetXHalfLength(newdimensions.x());
    solidWorld->SetYHalfLength(newdimensions.y());
    solidWorld->SetZHalfLength(newdimensions.z());   
    
    G4RunManager::GetRunManager()->GeometryHasBeenModified();
    //G4GeometryManager::GetInstance()->CloseGeometry();
}

void DetectorConstruction::ModifyWorldMaterial(G4String materialname)
{
    G4GeometryManager::GetInstance()->OpenGeometry();
    G4Material* material = G4NistManager::Instance()->FindOrBuildMaterial(materialname); 
    logicWorld -> UpdateMaterial(material);
    
    G4RunManager::GetRunManager()->GeometryHasBeenModified();

    G4GeometryManager::GetInstance()->CloseGeometry();
}

void DetectorConstruction::SetGraphics(bool graphics)
{
    
}
