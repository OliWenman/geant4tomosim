//Own classes
#include "DetectorConstruction.hh"
#include "DetectorConstructionMessenger.hh"
#include "TomographySD.hh"
#include "TomographyVSD.hh"
#include "FluorescenceSD.hh"
#include "Data.hh"
#include "TargetConstruction.hh"
//Output to console/write to file
#include "SettingsLog.hh"

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

#include "G4StepLimiter.hh"
#include "G4UserLimits.hh"

#include "G4Element.hh"
#include "G4MaterialPropertiesTable.hh"

DetectorConstruction::DetectorConstruction(Data* DataObject):G4VUserDetectorConstruction(), data(DataObject)
{ 	
	//Create a messenger for this class
  	detectorMessenger = new DetectorConstructionMessenger(this);	
	TC = new TargetConstruction();

	nImage = 0;
	WorldMaterial_Cmd = "G4_AIR";
	
	WarningChecked = false;
	
	param = NULL;
	SolidContainer = NULL;
	SolidPhantomBoxes = NULL;
	container_logic = NULL;
	PhantomBoxes_logic = NULL;
	
	SolidFluoDet = NULL;
	FluoDet_logic = NULL;
	
	solidWorld = NULL;
	logicWorld = NULL;
}

DetectorConstruction::~DetectorConstruction()
{	
	//Delete the messenger and Target Construct class
	delete detectorMessenger; 
	delete TC;

	//Delete the transmission detectors
	if (param){delete param;}
	if (SolidContainer){delete SolidContainer;}
	if (SolidPhantomBoxes){delete SolidPhantomBoxes;}
	if (container_logic){delete container_logic;}
	if (PhantomBoxes_logic){delete PhantomBoxes_logic;}

	//Delete the fluorescence detectors
	if (SolidFluoDet){delete SolidFluoDet;}
	if (FluoDet_logic){delete FluoDet_logic;} 

	//Delete the wolrd
	if (solidWorld){delete solidWorld;}
	if (logicWorld){delete logicWorld;}
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

		//Set the position of the fluorescence detector a distance of 5% from the world boundary 
		FluorDetPos_Cmd = G4ThreeVector(0, WorldSize_Cmd.y()*0.95, 0);
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
	
	//G4cout << "\nmaterial -> GetName() = " << material -> GetName() << G4endl;
	
	//material -> GetMaterialPropertiesTable() -> DumpTable();
	
	//Create an instance of the logical volume and find the material
	logicWorld = new G4LogicalVolume(solidWorld, material, "World");
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//EXPERIMENTAL

    /*G4Material* MaterialFound = FindMaterial("TI64");
    
    if (MaterialFound)
    {
        const G4ElementVector* Material_elements = MaterialFound -> GetElementVector();
        const G4double* Material_fractional_mass = MaterialFound -> GetFractionVector(); 
        const int nElements = Material_elements -> size();
        
        G4cout << "\nMaterial: " << MaterialFound -> GetName()
               << "\nMade up of the following elements\n"; 
        for (int i = 0; i < nElements; i++)
        {
            G4Element const* element = Material_elements[0][i];
            G4String eleName = element -> GetName();
            G4double eleDensity = element -> GetZ();
            G4double f_mass = Material_fractional_mass[i];
           
            G4cout << "Element: " << eleName
                   << " -> mass: " << f_mass*100 << "%\ \n"
                   << " -> density: " << eleDensity;
        }
    }*/

    //Create an instance of the world geometry
	/*solidWorld = new G4Box("World", WorldSize_Cmd.x()+DetectorSize_Cmd.x(), WorldSize_Cmd.y(), WorldSize_Cmd.z());

    G4double a, z, density;
    G4int nelements;
    
    //Air
	G4Element* N = new G4Element("Nitrogen", "N", z=7 , a=14.01*g/mole);
    G4Element* O = new G4Element("Oxygen"  , "O", z=8 , a=16.00*g/mole);

    G4Material* air = new G4Material("air", density=1.29*mg/cm3, nelements=2);
    air->AddElement(N, 70.*perCent);
    air->AddElement(O, 30.*perCent);
    
    G4double photonEnergy[] =
            { 2.034*eV, 2.068*eV, 2.103*eV, 2.139*eV,
              2.177*eV, 2.216*eV, 2.256*eV, 2.298*eV,
              2.341*eV, 2.386*eV, 2.433*eV, 2.481*eV,
              2.532*eV, 2.585*eV, 2.640*eV, 2.697*eV,
              2.757*eV, 2.820*eV, 2.885*eV, 2.954*eV,
              3.026*eV, 3.102*eV, 3.181*eV, 3.265*eV,
              3.353*eV, 3.446*eV, 3.545*eV, 3.649*eV,
              3.760*eV, 3.877*eV, 4.002*eV, 4.136*eV };

    const G4int nEntries = sizeof(photonEnergy)/sizeof(G4double);
    
    G4double refractiveIndex2[] =
            { 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
              1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
              1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
              1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
              1.00, 1.00, 1.00, 1.00 };

    G4MaterialPropertiesTable* myMPT2 = new G4MaterialPropertiesTable();
    myMPT2->AddProperty("RINDEX", photonEnergy, refractiveIndex2, nEntries);

    //G4cout << "Air G4MaterialPropertiesTable" << G4endl;
    //myMPT2->DumpTable();

    air->SetMaterialPropertiesTable(myMPT2);
    
    logicWorld = new G4LogicalVolume(solidWorld, air, "World");
    
//---------------------------------------------------------------------------------------------------------------------------------------------	
	//Water
	G4Element* H = new G4Element("Hydrogen", "H", z=1 , a=1.01*g/mole);

    G4Material* water = new G4Material("water", density= 1.0*g/cm3, nelements=2);
    water->AddElement(H, 2);
    water->AddElement(O, 1);
            

    G4Box* testSample = new G4Box("test", 0.5*m, 0.5*m, 0.5*m);
    
    G4LogicalVolume* testSample_logic = new G4LogicalVolume(testSample, water, "test_logic");
    
    G4double refractiveIndex1[] =
            { 1.3435, 1.344,  1.3445, 1.345,  1.3455,
              1.346,  1.3465, 1.347,  1.3475, 1.348,
              1.3485, 1.3492, 1.35,   1.3505, 1.351,
              1.3518, 1.3522, 1.3530, 1.3535, 1.354,
              1.3545, 1.355,  1.3555, 1.356,  1.3568,
              1.3572, 1.358,  1.3585, 1.359,  1.3595,
              1.36,   1.3608};

    assert(sizeof(refractiveIndex1) == sizeof(photonEnergy));

    G4double absorption[] =
           {3.448*m,  4.082*m,  6.329*m,  9.174*m, 12.346*m, 13.889*m,
           15.152*m, 17.241*m, 18.868*m, 20.000*m, 26.316*m, 35.714*m,
           45.455*m, 47.619*m, 52.632*m, 52.632*m, 55.556*m, 52.632*m,
           52.632*m, 47.619*m, 45.455*m, 41.667*m, 37.037*m, 33.333*m,
           30.000*m, 28.500*m, 27.000*m, 24.500*m, 22.000*m, 19.500*m,
           17.500*m, 14.500*m };

    assert(sizeof(absorption) == sizeof(photonEnergy));
    
    G4MaterialPropertiesTable* myMPT1 = new G4MaterialPropertiesTable();

    myMPT1->AddProperty("RINDEX",       photonEnergy, refractiveIndex1,nEntries)
        ->SetSpline(true);
    myMPT1->AddProperty("ABSLENGTH",    photonEnergy, absorption,     nEntries)
        ->SetSpline(true);
        
    water->SetMaterialPropertiesTable(myMPT1);
    
    G4RotationMatrix* RotateObjectAngle = new G4RotationMatrix();
	RotateObjectAngle->rotateX(0);
	RotateObjectAngle->rotateY(0);
	RotateObjectAngle->rotateZ(10*deg);
    
    G4VPhysicalVolume* water_phys = new G4PVPlacement(RotateObjectAngle,                  // rotation
            						                      G4ThreeVector(0,0,0),                   // translation
           						                          testSample_logic,            // logical volume
            						                      "PhantomContainer",    // name
            						                      logicWorld,           // mother volume
            						                      false,                 // No op. bool.
           						                          0,			//copy number
							                              false);                    //overlap checking
    //G4cout << "Water G4MaterialPropertiesTable" << G4endl;
    //myMPT1->DumpTable();*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
    
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
	
	//Don't place the detectors if over the limit for visualization as will be used to test geometry placement
	if (Visualization_Cmd == true)
	{
	    int limit = 1000;    
	    
	    int nDetectors = NoDetectorsY_Cmd*NoDetectorsZ_Cmd;
	    
	    if (nDetectors > limit && WarningChecked == false)   
	    {   
	           G4cout << "\n////////////////////////////////////////////////////////////////////////////////\n"
	                     "\n                                 WARNING:"
	                     "\n                      TOO MANY DETECTORS TO VISUALISE!"
	                     "\n              The limit is " << limit << ". You attempted to place " << nDetectors << 
	                     ".\n                  Aborting the placement of the detectors.\n"
	                     "\n////////////////////////////////////////////////////////////////////////////////" << G4endl;
	                      
	            WarningChecked = true;
	    } 
	    else if (nDetectors < limit)
	        PVDetectors(logicWorld);
	}
	else 
	    PVDetectors(logicWorld);
	
	
	TC -> Construct(logicWorld);

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
	
	G4MaterialPropertiesTable* MPT = DetMaterial -> GetMaterialPropertiesTable();
	if (!MPT)
	{
	    MPT = new G4MaterialPropertiesTable();
	    
	    double energy[] = {0.2*keV};
	    double rindex[] = {1};
	    int size = 1;
	    
	    MPT -> AddProperty("RINDEX", energy, rindex, size);
	    
	    DetMaterial -> SetMaterialPropertiesTable(MPT);
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

    if (NoDetectorsY_Cmd > 30 || NoDetectorsZ_Cmd > 30)
    {
	    G4VisAttributes* VisAttributes = new G4VisAttributes();	
  		//LV -> SetVisAttributes(ObjectColour);
	    VisAttributes -> SetVisibility(false);
	    PhantomBoxes_logic -> SetVisAttributes(VisAttributes);
    }

//------------------------------------------------------------------
	//FLUORESCENCE DETECTOR
	if (FluorescenceDet == true)
	{
		FluoDet_logic = new G4LogicalVolume(SolidFluoDet, DetMaterial, "LVFluorescenceDetector");

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

			//G4SDParticleFilter* gammaFilter = new G4SDParticleFilter("GammaFilter", "gamma");
			//tomoVSD -> SetFilter(gammaFilter);
		}
		else if (TypeDetector == "TomographyDetector")
		{
			//Create a detector optimised for perfomance
    			tomoSD = new TomographySD("TomographyDetector", data, DetectorEfficiency_Cmd);
			SDmanager->AddNewDetector(tomoSD);	// Store SD if built	

			//G4SDParticleFilter* gammaFilter = new G4SDParticleFilter("GammaFilter", "gamma");
			//tomoSD -> SetFilter(gammaFilter);
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
    G4VisAttributes* ObjectColour = new G4VisAttributes(G4Colour(Colour));	
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

	G4ThreeVector FullDetDimensions = G4ThreeVector(DetectorSize_Cmd.x()*1, DetectorSize_Cmd.y()*NoDetectorsY_Cmd, DetectorSize_Cmd.z()*NoDetectorsZ_Cmd);

	log << "\n--------------------------------------------------------------------"
		   "\nWORLD SETUP: \n"
	       "\n- World dimensions: " << G4BestUnit(WorldSize_Cmd, "Length")
	    << "\n- World Material: " << WorldMaterial_Cmd << "\n"

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
	
}

TargetConstruction* DetectorConstruction::GetTargetConstruction()
{
    return TC;
}

