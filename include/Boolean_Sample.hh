//Auther: Oliver Wenman

//A class that inherits from SampleDescription, Boolean_Sample is used to
//extend its ability to make more complicated boolean solids by joining
//and subtracting geometry away from other solids. Therefore it needs
//additional attributes and methods to know which solids to use as well as
//at what rotation and position to subtract the solids away from each other.

#ifndef Boolean_Sample_h
#define Boolean_Sample_h 1

#include "SampleDescription.hh"

class Boolean_Sample : public SampleDescription
{
    public:
        Boolean_Sample(std::string     _name, 
                       std::string     _type, 
                       double_vector1D _dim,
                       int             _id);
                          
        //~Boolean_Sample();
    
    
        void BuildSolid();
    
        void SetComponent1(G4String _comp1) {if (componentname1 != _comp1) {componentname1 = _comp1; construct_solid = construct_logic = construct_placement = true;}}
        void SetComponent2(G4String _comp2) {if (componentname2 != _comp2) {componentname2 = _comp2; construct_solid = construct_logic = construct_placement = true;}}
        
        void SetInsideRotation(G4ThreeVector _irot) {if (inside_rot != _irot) {inside_rot = _irot; construct_solid = construct_logic = construct_placement = true;}}
        void SetInsidePosition(G4ThreeVector _ipos) {if (inside_pos != _ipos) {inside_pos = _ipos; construct_solid = construct_logic = construct_placement = true;}}

    private:
        void BuildSubtractSolid();                            
        void BuildUnionSolid();
        
        G4String      componentname1;
        G4String      componentname2;
    
        G4ThreeVector inside_rot;
        G4ThreeVector inside_pos; 
};

#endif
