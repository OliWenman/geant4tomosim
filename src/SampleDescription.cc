#include "SampleDescription.hh"
#include <math.h> 
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

SampleDescription::SampleDescription() {}

SampleDescription::SampleDescription(std::string     _name, 
                                     std::string     _type, 
                                     double_vector1D _dim,
                                     int             _id   )
{
    solid     = 0;
    logic     = 0;
    placement = 0;

    name       = _name;
    type       = _type;
    dimensions = _dim;
    id         = _id;
    
    iposition  = G4ThreeVector(0, 0, 0);
    irotation  = G4ThreeVector(0, 0, 0);
    motherbox  = "World";
    material   = "EMPTY";
    
    construct_solid     = true;
    construct_logic     = true;
    construct_placement = true;
    
    master = false;
    checkforoverlaps = false;
}

#include "G4SolidStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"

SampleDescription::~SampleDescription()
{   
    /*G4VSolid*          solidptr     = G4SolidStore::GetInstance()->GetSolid(name, false);
    G4LogicalVolume*   logicptr     = G4LogicalVolumeStore::GetInstance()->GetVolume(name, false);
    G4VPhysicalVolume* placementptr = G4PhysicalVolumeStore::GetInstance()->GetVolume(name, false);
    
    G4cout << "\n--------------------------------------------------";
    */
    /*if (placementptr) {delete placementptr; G4cout << "\ndeleted placement " << name; } 
    if (logicptr)     {delete logicptr;     G4cout << "\ndeleted logic " << name;}
    if (solidptr)     {delete solidptr;     G4cout << "\ndeleted solid " << name << G4endl;}*/
    
    //if (placement) {delete placement; G4cout << "\ndeleted placement " << name; } 
    //if (logic)     {delete logic;     G4cout << "\ndeleted logic " << name;}
    //if (solid)     {delete solid;     G4cout << "\ndeleted solid " << name << G4endl;}

    delete placement;
    delete logic;
    delete solid;
}

void SampleDescription::PrintDescription()
{
    G4cout << G4endl;
    G4cout << "\n---------------------------------------------------"
           << "\nname : " << name
           << "\ntype : " << type
           << "\nid   : " << id
           << "\nipos : " << iposition
           << "\nirot : " << irotation;
}

//Different shapes for G4VSolid
#include "G4VSolid.hh"

void SampleDescription::BuildSolid()
{
    if (construct_solid)
    {
        if(solid)
        {
            delete solid;
            solid = 0;
        }
        
        if (type == "box")
        {
            BuildBox();
        }
        else if (type == "sphere")
        {
            BuildSphere();
        }
        else if (type == "cylinder")
        {
            BuildCylinder();
        }
        else if (type == "trapezoid")
        {
            BuildTrapezoid();
        }
        else if (type == "ellipsoid")
        {
            BuildEllipsoid();
        }
        else 
        {
            G4cout << "\nWARNING: SampleDescription::BuildSolid() has an unrecogonised type \"" << type << "\". Ignoring build" << G4endl;
        }
        
        construct_solid = false;
    }
}

//Material database
#include "G4NistManager.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include "G4LogicalVolume.hh" 

void SampleDescription::BuildLogicVolume()
{
    if (construct_logic)
    {
        construct_logic = false;
    
        if(logic)
        {
            delete logic;
            logic = 0;
        }

        if (material != "EMPTY")
        {                  
            G4Material* materialptr = G4NistManager::Instance() -> FindOrBuildMaterial(material);
            if (!materialptr)
            {
                G4cout << "\nWARNING: couldn't find material \"" << material << "\" for G4VSolid \"" << name << "\" " 
                       << "\nLogic volume creation ignored. " << G4endl;
                return;
            }   
         
            logic = new G4LogicalVolume(solid, materialptr, name);
        }
        
        if      (colour == "blue" ) {logic->SetVisAttributes(G4Colour::Blue()); }
        else if (colour == "red"  ) {logic->SetVisAttributes(G4Colour::Red());  }
        else if (colour == "green") {logic->SetVisAttributes(G4Colour::Green());}
        else if (colour == "cyan" ) {logic->SetVisAttributes(G4Colour::Cyan()); }
    }
}
       
//Find logicalvolume of motherbox
#include "G4LogicalVolumeStore.hh"

#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4RotationMatrix.hh"

bool SampleDescription::BuildPlacement(bool          darkflatfields, 
                                       double        tiltangleX, 
                                       double        tiltangleY, 
                                       G4ThreeVector tiltcentre, 
                                       bool          checkforoverlaps)
{
    if (darkflatfields)
    {
        if (placement)
        {
            delete placement;
            placement = 0;
            
            construct_placement = true;
        }
    }
    else
    {
        if(construct_placement)
        {
            if(placement)
            {
                delete placement;
                placement = 0;
            }
            if (logic)
            {
                G4LogicalVolume* motherbox_logic = G4LogicalVolumeStore::GetInstance()->GetVolume(motherbox, true);
            
                //The order of rotation matters
                G4RotationMatrix* rotmatrix = new G4RotationMatrix();
                
                //Adjust the rotations for any tilt to the sample
                irotation = G4ThreeVector(irotation.x() + tiltangleX,
                                          irotation.y() + tiltangleY,
                                          irotation.z());          
                
                //Rotate the axis of rotation for the sample first
                rotmatrix->rotateZ(irotation.z());
                
                //Rotate the other axes for the right orientation
                rotmatrix->rotateX(irotation.x());
                rotmatrix->rotateY(irotation.y());
                
                //Calculate the adjusted positions for z and y for tilt angle x
                double newZ = tiltcentre.z() + std::cos(tiltangleX)*(iposition.z()) - std::sin(tiltangleX)*(iposition.y());
                double newY = tiltcentre.y() + std::sin(tiltangleX)*(iposition.z()) + std::cos(tiltangleX)*(iposition.y());
                
                iposition = G4ThreeVector(iposition.x(), newY, newZ);
                
                //Calculate the adjusted positions for z and x for tilt angle y
                newZ = tiltcentre.z() + std::cos(-tiltangleY)*(iposition.z()) - std::sin(-tiltangleY)*(iposition.x());
                double newX = tiltcentre.x() + std::sin(-tiltangleY)*(iposition.z()) + std::cos(-tiltangleY)*(iposition.x());
                
                iposition = G4ThreeVector(newX, iposition.y(), newZ);
        
                placement = new G4PVPlacement(rotmatrix,            
				    			 	          iposition,  
				    			  	          logic,              //its logical volume
				    			  	          name,               //its name
				    			  	          motherbox_logic,    //its mother  volume
				    			 	          false,              //no boolean operation
				    			 	          id,                 //copy number
				    			 	          checkforoverlaps);  //overlaps checking   	
            
                construct_placement = false;
                
                return true;
            }
        }
    }
    return false;
}

void SampleDescription::ApplyTransforms(double        deltaTheta, 
                                        double        zpos, 
                                        double        radius,
                                        G4ThreeVector centre)
{
    if (placement)
    {
        //Apply the rotations to the sample
        G4RotationMatrix* rotmatrix = placement->GetRotation();
     
        //Undo the rotations for Y and X axes to get the original axes of rotation of the object
        rotmatrix->rotateY(-irotation.y());
        rotmatrix->rotateX(-irotation.x());
        
        //Rotate along the original axis of rotation by delta theta
        rotmatrix->rotateZ(deltaTheta);
        
        //Rotate the axes to give back the correct orientation of the sample
        rotmatrix->rotateX(irotation.x());
        rotmatrix->rotateY(irotation.y());
        
//-----------------------------------------------------------------------------------     
        //Apply the translations to the sample
        G4ThreeVector newpos;   
        
        G4ThreeVector currentpos = placement->GetTranslation();       
        if (deltaTheta == 0) 
        {
            currentpos.setX(currentpos.x() + radius);
        }
        
        if (master)
        {
            if (radius != 0)
		    {
                double newX = iposition.x() + (currentpos.x() - iposition.x()) * std::cos(-deltaTheta) - (currentpos.y() - iposition.y()) * std::sin(-deltaTheta);
                double newY = iposition.y() + (currentpos.x() - iposition.x()) * std::sin(-deltaTheta) + (currentpos.y() - iposition.y()) * std::cos(-deltaTheta);
			    newpos = G4ThreeVector(newX, newY, iposition.z() + zpos);   
		    }
		    else 
			{
                newpos = placement->GetTranslation();	
                newpos.setZ(iposition.z() + zpos);	
			}
	    }
	    else
	    {   
		    double newX = centre.x() + (currentpos.x() - centre.x()) * std::cos(-deltaTheta) - (currentpos.y() - centre.y()) * std::sin(-deltaTheta);
            double newY = centre.y() + (currentpos.x() - centre.x()) * std::sin(-deltaTheta) + (currentpos.y() - centre.y()) * std::cos(-deltaTheta);
		    
		    newpos = G4ThreeVector(newX, newY, iposition.z() + zpos);
        }   
        
        placement->SetTranslation(newpos); 
    }
}

#include "G4Box.hh"

void SampleDescription::BuildBox()
{
    if (dimensions.size() != 3)
    {
        G4cout << "\nWARNING: SampleDescription::BuildBox() doesn't have the correct number of dimensions."
                  "\nShould have 3 dimensions. It has " << dimensions.size() << G4endl;
    }

    //Get the dimensions for the cube
    G4double halfxlength = dimensions[0];
    G4double halfylength = dimensions[1];
    G4double halfzlength = dimensions[2];

    //Create a G4VSolid of the box
	solid = new G4Box(name, halfxlength, halfylength, halfzlength);
}

#include "G4Sphere.hh"

void SampleDescription::BuildSphere()
{
    if (dimensions.size() != 6)
    {
        G4cout << "\nWARNING: SampleDescription::BuildSphere() doesn't have the correct number of dimensions."
                  "\nShould have 6 dimensions. It has " << dimensions.size() << G4endl;
    }

    //Get the dimensions of the sphere
	G4double innerRadius   = dimensions[0];
	G4double outerRadius   = dimensions[1];
	G4double startingPhi   = dimensions[2];
	G4double endPhi        = dimensions[3];
	G4double startingTheta = dimensions[4];
	G4double endTheta      = dimensions[5]; 

	//Create a G4VSolid of the sphere
	solid = new G4Sphere(name, 
				     	 innerRadius,
				     	 outerRadius,
				     	 startingPhi,
				     	 endPhi,
				     	 startingTheta,
				     	 endTheta);
}

#include "G4Tubs.hh"

void SampleDescription::BuildCylinder()
{
    if (dimensions.size() != 5)
    {
        G4cout << "\nWARNING: SampleDescription::BuildCylinder() doesn't have the correct number of dimensions."
                  "\nShould have 5 dimensions. It has " << dimensions.size() << G4endl;
    }

    //Get the dim of the cylinder
	G4double innerRadius   = dimensions[0];
	G4double outerRadius   = dimensions[1];
	G4double height        = dimensions[2];
	G4double startAngle    = dimensions[3];
	G4double spanningAngle = dimensions[4];
		
	//Create a G4VSolid of the cylinder
   	solid = new G4Tubs(name,
				 	   innerRadius,
                       outerRadius,
                  	   height,
                  	   startAngle,
                  	   spanningAngle);
}

#include "G4Trd.hh"

void SampleDescription::BuildTrapezoid()
{
    if (dimensions.size() != 5)
    {
        G4cout << "\nWARNING: SampleDescription::BuildTrapezoid() doesn't have the correct number of dimensions."
                  "\nShould have 5 dimensions. It has " << dimensions.size() << G4endl;
    }

    G4double dx1 = dimensions[0];
    G4double dx2 = dimensions[1];
    G4double dy1 = dimensions[2];
    G4double dy2 = dimensions[3];
    G4double dz  = dimensions[4];

	solid = new G4Trd(name,
				      dx1,
          			  dx2,
            	      dy1,
            		  dy2,
            		  dz);
}

#include "G4Ellipsoid.hh"

void SampleDescription::BuildEllipsoid()
{
    if (dimensions.size() != 5)
    {
        G4cout << "\nWARNING: SampleDescription::BuildEllipsoid() doesn't have the correct number of dimensions."
                  "\nShould have 5 dimensions. It has " << dimensions.size() << G4endl;
    }

    G4double pxSemiAxis  = dimensions[0];
    G4double pySemiAxis  = dimensions[1];
    G4double pzSemiAxis  = dimensions[2];
    G4double pzBottomCut = dimensions[3];
    G4double pzTopCut    = dimensions[4];

	solid = new G4Ellipsoid(name,
                    		pxSemiAxis,
                    		pySemiAxis,
                    		pzSemiAxis,
                    		pzBottomCut,
                    		pzTopCut);
}
         

