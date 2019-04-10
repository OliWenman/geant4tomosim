#include "Boolean_Sample.hh"

#include "G4RotationMatrix.hh"
#include "G4SolidStore.hh"
#include "G4SubtractionSolid.hh"

Boolean_Sample:: Boolean_Sample(std::string     _name, 
                                std::string     _type, 
                                double_vector1D _dim,
                                int             _id) 
                                
               : SampleDescription(_name, _type, _dim, _id)
{

}

void Boolean_Sample::BuildSolid()
{
    if (construct_solid)
    {
        if(solid)
        {
            delete solid;
            solid = 0;
        }   
        
        if (type == "subtract")
        {
            BuildSubtractSolid();
        }
        else if (type == "union")
        {
            BuildUnionSolid();
        }
        else 
        {
            G4cout << "\nWARNING: Boolean_Sample::BuildSolid() has an unrecogonised type \"" << type << "\". Ignoring build" << G4endl;
        }
        
        construct_solid = false;
    }
        
}

void Boolean_Sample::BuildSubtractSolid()
{
   // G4cout << "\n building SubtractSolid" << name <<G4endl;

    //Create a rotation matrix for the rotation and give it the correct units using the dictionary created in the TCMessenger
	G4RotationMatrix* rotmatrix = new G4RotationMatrix();
	rotmatrix->rotateX(inside_rot.x());
	rotmatrix->rotateY(inside_rot.y());
	rotmatrix->rotateZ(inside_rot.z());	

	//Find the correct solids with the name of the objects inputted
	G4VSolid* outersolid = G4SolidStore::GetInstance() -> GetSolid(componentname1, true); 
	G4VSolid* innersolid = G4SolidStore::GetInstance() -> GetSolid(componentname2, true); 
	
	//G4cout << "\nComponenent name 1 = " << componentname1 << G4endl;
	//G4cout << "\nComponenent name 2 = " << componentname2 << G4endl;

	//Create the new solid from 
	solid = new G4SubtractionSolid(name, 
							       outersolid, 
							       innersolid, 
							       rotmatrix, 
							       inside_pos);
    //G4cout << "\n Finished SubtractSolid" << name << G4endl;
}

#include "G4UnionSolid.hh"

void Boolean_Sample::BuildUnionSolid()
{
    //Create a rotation matrix for the rotation and give it the correct units using the dictionary created in the TCMessenger
	G4RotationMatrix* rotmatrix = new G4RotationMatrix();
	rotmatrix->rotateX(inside_rot.x());
	rotmatrix->rotateY(inside_rot.y());
	rotmatrix->rotateZ(inside_rot.z());	

	//Find the correct solids with the name of the objects inputted
	G4VSolid* outersolid = G4SolidStore::GetInstance() -> GetSolid(componentname1, true); 
	G4VSolid* innersolid = G4SolidStore::GetInstance() -> GetSolid(componentname2, true); 

	//Create the new solid from 
	solid = new G4UnionSolid(name, 
							 outersolid, 
							 innersolid, 
					         rotmatrix, 
				             inside_pos);
}


