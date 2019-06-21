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

//Inherited method from SampleDescription
//Override the method to build the boolean versions
void Boolean_Sample::BuildSolid()
{
    if (construct_solid)
    {
        //Delete the solid if it already exists
        if(solid)
        {
            delete solid;
            solid = 0;
        }   
        
        //Build the correct type of boolean solid
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
            throw std::invalid_argument("\nWARNING: Boolean_Sample::BuildSolid() has an unrecogonised type \"" + type);
        }
        
        //Solid has been constructed
        construct_solid = false;
    }
        
}

#include "Exceptions.hh"
void Boolean_Sample::BuildSubtractSolid()
{
    //Create a rotation matrix for the rotation of the second object in relative to the second object
	G4RotationMatrix* rotmatrix = new G4RotationMatrix();
	rotmatrix->rotateX(inside_rot.x());
	rotmatrix->rotateY(inside_rot.y());
	rotmatrix->rotateZ(inside_rot.z());	

	//Find the correct solids with the name of the objects inputted
	G4VSolid* outersolid = G4SolidStore::GetInstance() -> GetSolid(componentname1, true); 
	G4VSolid* innersolid = G4SolidStore::GetInstance() -> GetSolid(componentname2, true); 
	
	if (outersolid && innersolid)
	{
	    //Create the new solid from 
        solid = new G4SubtractionSolid(name, 
						               outersolid, 
						               innersolid, 
						               rotmatrix, 
						               inside_pos);
    }
    else
    {   
        throw parameterNotFound();
    }
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


