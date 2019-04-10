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
    
    for (int i = 0 ; i < samplelist.size() ; i++)
    {
        delete samplelist[i];
    }
    samplelist.clear();
    samplelist.shrink_to_fit();
}

#include "G4PhysicalVolumeStore.hh"

void SampleConstruction::Construct(bool darkflatfields)
{

    for (int i = 0 ; i < samplelist.size() ; i++)
    {
        SampleDescription* sample = samplelist[i];
        
        sample->BuildSolid();
        sample->BuildLogicVolume();
        if (removeplacement) {sample->MotherBoxUpdated(); removeplacement = false;}
        bool sampleplaced = sample->BuildPlacement(darkflatfields);
    
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
    double deltaTheta = fullrotation - lastfullrotation;
    lastfullrotation = fullrotation;

    for (int i = 0 ; i < samplelist.size() ; i++)
    {
        SampleDescription* sample = samplelist[i];
        
        //Use the master position as the centre for the other placements      
        sample->ApplyTransforms(deltaTheta, position, radius, mastercentre);   
    }
}

#include "Boolean_Sample.hh"

void SampleConstruction::AddToSampleList(std::string     name, 
                                         std::string     type, 
                                         double_vector1D dimensions)
{
    //Check if a sample containing that name already exists
    for (int i = 0 ; i < samplelist.size() ; i++)
    {
        if(samplelist[i]->GetName() == name)
        {
            G4cout << "\nWARNING: Cannot create sample \"" << name << "\" because it already exists. Ignoring command... " << G4endl;
            return;
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
    return sample;
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
    
    G4cout << "\nWARNING: Couldn't find sample \"" << name << "\". Returning NULL pointer..." << G4endl;
}


