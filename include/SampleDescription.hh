//Author: Oliver Wenman    

//SampleDescription stores all the variables needed to build the G4VSolids, G4LogicalVolumes,
//and G4VPhysicalVolumes. It works by only building the G4 classes if it has the appropiate 
//variables to do so. If one of the G4 classes is modified via commands, this class has the 
//ability to update the material for instance in G4LogicalVolume by rebuilding it.

//Allows for very flexible sample construction and modifying mid simulation

#ifndef SampleDescription_h 
#define SampleDescription_h 1

#include "MyVectors.hh"
#include <string>
#include "G4ThreeVector.hh"

class G4VSolid;
class G4LogicalVolume;
class G4VPhysicalVolume;

class SampleDescription
{
    public:
        SampleDescription();
        SampleDescription(std::string     _name, 
                          std::string     _type, 
                          double_vector1D _dim,
                          int             _id);
                          
        virtual ~SampleDescription();
        
        //Build the samples G4VSolid, G4LogicalVolume and G4PVPlacement
        virtual void BuildSolid();
        void BuildLogicVolume();
        bool BuildPlacement(bool darkflatfields);
        void ApplyTransforms(double        deltaTheta, 
                             double        deltaZ, 
                             double        radius,
                             G4ThreeVector centre);
                                        
        void RemovePhysicalVolume() {delete placement; placement = 0; construct_placement = true;}
        void MotherBoxUpdated(){construct_placement = true;}
        
        virtual void PrintDescription();
        
        //Set methods via messenger - checks if value has changed. If it has it knowns to reconstruct the G4 class associated with it
        void SetMaterial (std::string _material) {if (material  != _material)  {material  = _material;  construct_logic = construct_placement = true;}}
        void SetColour   (std::string col)       {if (colour    != col)        {colour = col;           construct_logic = construct_placement = true;}}
        void SetMotherBox(std::string _motherbox){if (motherbox != _motherbox) {motherbox = _motherbox; construct_placement = true;}}
        void SetPosition (G4ThreeVector _ipos)   {if (iposition != _ipos)      {iposition = _ipos;      construct_placement = true;}}
        void SetRotation (G4ThreeVector _irot)   {if (irotation != _irot)      {irotation = _irot;      construct_placement = true;}}
        void SetCheckForOverlaps(bool val)       {if (checkforoverlaps != val) {checkforoverlaps = val;  construct_placement = true;}}
        
        void SetMaster(bool value) {master = value;}
        
        //Get functions
        std::string   GetName()            const {return name;}
        G4ThreeVector GetInitialPosition() const {return iposition;}
               
    private:
        //Build methods for G4VSolid
        void BuildBox();
        void BuildSphere();
        void BuildCylinder();
        void BuildTrapezoid();
        void BuildEllipsoid();
         
    protected:
    
        //Atrributes set via constructor
        std::string     name;
        std::string     type;
        double_vector1D dimensions;
        int             id;
        bool            master;
        
        //Set via set methods
        std::string   material;
        std::string   colour;
        std::string   motherbox;
        G4ThreeVector iposition;
        G4ThreeVector irotation;
        bool          checkforoverlaps;
    
        //Checks to see if it needs to be rebuilt
        bool construct_solid;
        bool construct_logic;
        bool construct_placement;
    
        //Pointers to the Geant4 classes that create the geometry
        G4VSolid          *solid;
        G4LogicalVolume   *logic;
        G4VPhysicalVolume *placement;
};

#endif
