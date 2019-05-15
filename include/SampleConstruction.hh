/*    
SampleConstruction works by saving inputs via commands from its messenger class into 
SampleDescription. Each instance of SampleDescription contains information on how to 
build a sample as well as the G4VSolids, G4LogicalVolume and G4VPhysicalVolume pointers
that are used to build geometry. Each SampleDescription instance is stored in a container 
called samplelist. SampleConstruction is used to manage this list of samples and invokes 
the method of constructing and updating them for each run.

Notes: Attempted to change the method to construct the G4VSolids, G4LogicalVolumes and 
G4VPhysicalVolumes directly from commands but was complicated by the fact that the world's 
G4LogicalVolumewould have to be constructed instantly in the constructer of DetectorConstruction
and modifiedwith commands. This was complicated when this would lead to segmentation faults if 
changing the world's material in the G4LogicalVolume as modifying a G4LogicalVolume's material is 
not directly supported within Geant4. Therefore best solution is to create an instance of the Geant4 
classes when the users variables are known and not attempted to modify them later. If modification 
is required, then the G4 classes can be deleted and constructed again with different variables

Author: Oliver Wenman
*/

#ifndef SampleConstruction_h
#define SampleConstruction_h 1

#include "G4ThreeVector.hh"
#include "SampleDescription.hh"
#include "Boolean_Sample.hh"
#include "MyVectors.hh"

typedef std::vector<SampleDescription*>  SampleCollection;

/*
Group together samples to make it easier to move a sample that is made up of lots of 
different materials.
*/
struct SampleGroup
{
    std::string      name;
    SampleCollection group;
    G4ThreeVector    position;
    G4ThreeVector    rotation;
};

typedef std::vector<SampleGroup*> SampleGroupCollection; 

class SampleConstructionMessenger;

class SampleConstruction
{
	public:
		SampleConstruction();
		~SampleConstruction();
		
		void Construct(bool darkflatfields);
		void ApplyTransforms(double        deltaTheta, 
                             double        deltaZ);
                             
		void RemovePlacement(bool val) {removeplacement = val;}
		void Reset();
		
		int AddToSampleList(std::string name, std::string, double_vector1D);
		int CloneSample(std::string name, std::string name_to_clone);
		
		int GroupTogetherSamples(std::string groupname, string_vector1D samplenames);
		
		SampleDescription* FindSample(std::string name);
		Boolean_Sample*    FindSample_Boolean(std::string name);
		
		void SetLastRotation(double value){lastrotation = value;}
		
		//Set commands from messenger
		void SetRadiusOffSet       (double r)        {radius = r;}
		void SetTiltAngleX         (double anglex)   {tiltangleX = anglex;}
		void SetTiltAngleY         (double angley)   {tiltangleY = angley;}
		void SetTiltCentre         (G4ThreeVector c) {tiltcentre = c;}
		void SetCheckForAllOverLaps(bool value)      {checkforoverlaps = value;}
		
    private:
        
        SampleConstructionMessenger* messenger;
        SampleCollection samplelist;   
        std::vector<SampleGroup> samplegroups;
        SampleDescription* master;  

        bool removeplacement;
        double lastrotation;   
        
        bool masterplacementSet;
        std::string mastername;
        G4ThreeVector mastercentre;
        
        double radius;
        bool   checkforoverlaps;
        double tiltangleX;
        double tiltangleY;
        G4ThreeVector tiltcentre;
};

#endif
