//Own classes
#include "DetectorConstruction.hh"
#include "DetectorConstructionMessenger.hh"
#include "AbsorptionDetector.hh"
#include "FluorescenceDetector.hh"
#include "DiffractionDetector.hh"
#include "Data.hh"
#include "TargetConstruction.hh"
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

DetectorConstruction::DetectorConstruction(Data* DataObject):G4VUserDetectorConstruction(), data(DataObject)
{ 	
	//Create a messenger for this class
  	detectorMessenger = new DetectorConstructionMessenger(this);	
	TC = new TargetConstruction();
	absDetector = new AbsorptionDetector();
	fluorescenceDetector = new FluorescenceDetector();
	diffractionDetector = new DiffractionDetector();

	nImage = 0;
	WorldMaterial_Cmd = "G4_AIR";
}

DetectorConstruction::~DetectorConstruction()
{	
	//Delete the messenger and Target Construct class
	delete detectorMessenger; 
	delete TC;
	
	delete absDetector;
	delete fluorescenceDetector;
	delete diffractionDetector;
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{  
	//Setup needed paramenters and variables only on the first image
	if (nImage == 0)
	{
		TC -> SetVisualization(Visualization_Cmd);

		//Create an instance of the world geometry
		solidWorld = new G4Box("World", WorldSize_Cmd.x(), WorldSize_Cmd.y(), WorldSize_Cmd.z());

		//Creates the dimensions for the detector
		absDetector -> CreateVolumes();
        fluorescenceDetector -> CreateVolumes();
        diffractionDetector -> SetNumberOfxPixels(absDetector->GetNumberOfxPixels());
        diffractionDetector -> SetNumberOfyPixels(absDetector->GetNumberOfyPixels());
        diffractionDetector -> SetHalfDimensions(absDetector->GetG4VectHalfDimensions());
        diffractionDetector ->CreateVolumes();
	}
	
	G4Material* material = FindMaterial(WorldMaterial_Cmd);
	
	G4MaterialPropertiesTable* MPT = material -> GetMaterialPropertiesTable();
	
	if (!MPT)
	{
	    MPT = new G4MaterialPropertiesTable();
	    
	    double energy[] = {0.2*keV};
	    double rindex[] = {1};
	    int size = 1;
	    
	    MPT -> AddProperty("RINDEX", energy, rindex, size);
	    
	    material -> SetMaterialPropertiesTable(MPT);
	}
	
	//Create an instance of the logical volume and find the material
	logicWorld = new G4LogicalVolume(solidWorld, material, "World");
	  
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
	absDetector -> AddProperties(data, Visualization_Cmd);
    fluorescenceDetector -> AddProperties(data, Visualization_Cmd);
    diffractionDetector -> AddProperties(data, Visualization_Cmd);
	
	absDetector -> PlaceDetectors(logicWorld, G4ThreeVector(WorldSize_Cmd.x(),0,0) );
	
	if (data->GetFullMapping_Option() || data->GetFluorescence_Option())
	{
	    fluorescenceDetector -> PlaceDetectors(logicWorld, G4ThreeVector(0, absDetector->GetG4VectHalfDimensions().y()*1.10, 0));
	}
	if (data->DoFullMappingDiffraction())
	{
	    double defualtPosX = WorldSize_Cmd.x()-absDetector -> GetG4VectHalfDimensions().x()*2;
	    double defualtPosY = absDetector -> GetG4VectHalfDimensions().y()*2;
	    double defualtPosZ = 0;
	
	    diffractionDetector -> PlaceDetectors(logicWorld, G4ThreeVector(defualtPosX, defualtPosY, defualtPosZ));
	}
	
	TC -> Construct(logicWorld);

	++nImage; 

	//Return the world 
	return physWorld;
}

G4Material* DetectorConstruction::FindMaterial(G4String MaterialName)
{
	//Obtain pointer to NIST material manager
	//Build materials 
	return G4NistManager::Instance() -> FindOrBuildMaterial(MaterialName);;
}

void DetectorConstruction::Visualization(G4LogicalVolume* LV, G4Colour Colour)
{
    G4VisAttributes* ObjectColour = new G4VisAttributes(Colour);	
  	LV -> SetVisAttributes(ObjectColour);
}

void DetectorConstruction::RelayToTC(int NumberOfImages, double TotalAngle )
{
	TC -> SetFullRotationAngle(TotalAngle);
	TC -> SetTotalImages(NumberOfImages);
}

void DetectorConstruction::ReadOutInfo(SettingsLog& log)
{
	TC -> ReadOutInfo();

	PrintToEndOfTerminal(log, '-');
	log << "WORLD SETUP:"
	       "\n- World dimensions: " << G4BestUnit(WorldSize_Cmd, "Length")
	    << "\n- World Material: " << WorldMaterial_Cmd;
	    
	absDetector -> ReadOutInfo(log);
	fluorescenceDetector -> ReadOutInfo(log);
    /*
           "\n--------------------------------------------------------------------"
	       "\nTHE DETECTOR SETUP: \n"

	       "\nTransmission detectors: "
	       "\n- Number of detectors: " << NoDetectorsY_Cmd << " x " << NoDetectorsZ_Cmd << " = " << NoDetectorsY_Cmd * NoDetectorsZ_Cmd
	    << "\n- Individual half detector dimensions: " << G4BestUnit(DetectorSize_Cmd, "Length")
	    << "\n- Half detector dimensions: " << G4BestUnit(FullDetDimensions, "Length") << G4endl;

	if (FluorescenceDet == true){
	
		log << "\nFluorescence detector: "
		       "\n- Dimensions: " << G4BestUnit(FluorDetSize_Cmd, "Length")
		    << "\n- Position: " << G4BestUnit(FluorDetPos_Cmd, "Length") 
	        << "\n- Number of bins: " << data -> GetNoBins() << G4endl;
	}
	if (DetectorEfficiency_Cmd == false){	
		log << "\n- Detector material: " << DetectorMaterial_Cmd << G4endl;
	}
	else{	
		log << "\n- Detectors are assumed to be 100%\ efficient " << G4endl;
	}
	*/
}

