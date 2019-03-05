#ifndef AbsorptionDetector_h
#define AbsorptionDetector_h 1

//My own classes
class Data;
class TomographySD;
class TomographyVSD;

//Solids, logic volume and physical volume for the geometry
class G4Box;
class G4LogicalVolume;
class G4VPhysicalVolume;

//For efficient geometry containing millions of solids
class G4PhantomParameterisation;

//Materials and colour classes
class G4Material;
class G4Colour;

class AbsorptionDetector
{
    public:
        AbsorptionDetector();
        ~AbsorptionDetector();
    
        void Create();
        void AddProperties();
        void PlaceDetectors();
        
    private:
        
    
};
#endif
