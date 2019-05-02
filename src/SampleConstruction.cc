#include "SampleConstruction.hh"
#include "SampleConstructionMessenger.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

SampleConstruction::SampleConstruction()
{
    messenger = new SampleConstructionMessenger(this);
    removeplacement = false;
    masterplacementSet = false;
    
    radius = 0;
    lastfullrotation = 0;
}

SampleConstruction::~SampleConstruction()
{
    delete messenger;
    
    for (SampleCollection::iterator it = samplelist.begin() ; it != samplelist.end(); ++it)
    {
        delete (*it);
    }
    samplelist.clear();
}

#include "G4PhysicalVolumeStore.hh"

void SampleConstruction::Construct(bool darkflatfields)
{
    //Loops through the samples, build the G4VSolids, G4LogicalVolumes and G4PVPlacements (the sample will decide within
    //each method if it needs to or not)
    for (int i = 0 ; i < samplelist.size() ; i++)
    {
        SampleDescription* sample = samplelist[i];
        
        sample->BuildSolid();
        sample->BuildLogicVolume();
        if (removeplacement) {sample->MotherBoxUpdated(); removeplacement = false;}
        bool sampleplaced = sample->BuildPlacement(darkflatfields);
    
        //Set the master placement as the first placement. All other samples
        //will rotate around this sample
        if (sampleplaced && !masterplacementSet) 
        {
            sample->SetMaster(true); 
            masterplacementSet = true;
            mastername = sample->GetName();
            mastercentre = sample->GetInitialPosition();
        }
    }
    G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

#include "G4PhysicalVolumeStore.hh"

void SampleConstruction::ApplyTransforms(double fullrotation, double position)
{
    //Work out the change in angle
    double deltaTheta = fullrotation - lastfullrotation;
    lastfullrotation = fullrotation;

    //Loop through the samples and apply the placement transforms needed to them
    for (int i = 0 ; i < samplelist.size() ; i++)
    {
        SampleDescription* sample = samplelist[i];
        
        //Use the master position as the centre for the other placements      
        sample->ApplyTransforms(deltaTheta, position, radius, mastercentre);   
    }
}

#include "Boolean_Sample.hh"
#include "CommandStatus.hh"

int SampleConstruction::AddToSampleList(std::string     name, 
                                        std::string     type, 
                                        double_vector1D dimensions)
{
    //Check if a sample containing that name already exists
    for (int i = 0 ; i < samplelist.size() ; i++)
    {
        if(samplelist[i]->GetName() == name)
        {
            G4cout << "\nWARNING: Cannot create sample \"" << name << "\" because it already exists. Ignoring command... " << G4endl;
            return fParameterAlreadyExists;
        }
    }

    SampleDescription* sample;
    
    if (type == "union" || type == "subtract")
    { 
        sample = new Boolean_Sample(name,
                                    type,
                                    dimensions,
                                    samplelist.size());
    }
    else
    {
        sample = new SampleDescription(name,
                                       type,
                                       dimensions,
                                       samplelist.size());
    }
    
    samplelist.push_back(sample);
    
    return 0;
}

int SampleConstruction::CloneSample(std::string name, std::string clone_name)
{

}

SampleDescription* SampleConstruction::FindSample(std::string name)
{
    SampleDescription* sample;

    for (int i = 0 ; i < samplelist.size() ; i++)
    {
        sample = samplelist[i];
        
        if (sample->GetName() == name)
        {
            return sample;
        }
    }
    
    return NULL;
}

Boolean_Sample* SampleConstruction::FindSample_Boolean(std::string name)
{
    for (int i = 0 ; i < samplelist.size() ; i++)
    {
        SampleDescription* sample = samplelist[i];
        
        if (sample->GetName() == name)
        {   
            Boolean_Sample* boolsample = static_cast<Boolean_Sample*> (sample);
        
            return boolsample;
        }
    }
    
    return NULL;
}

void SampleConstruction::Reset()
{
    for (SampleCollection::iterator it = samplelist.begin() ; it != samplelist.end(); ++it)
    {
        delete (*it);
    }
    samplelist.clear();
}


