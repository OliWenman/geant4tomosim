#include "SampleConstruction.hh"
#include "SampleConstructionMessenger.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

SampleConstruction::SampleConstruction()
{
    messenger = new SampleConstructionMessenger(this);
    removeplacement = false;
    masterplacementSet = false;
    
    radius = 0.*cm;
    lastrotation = 0;
    
    tiltangleX = 0.*deg;
    tiltangleY = 0.*deg;
    tiltcentre = G4ThreeVector(0., 0., 0.);
    
    checkforoverlaps = false;
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
        
        bool sampleplaced = sample->BuildPlacement(darkflatfields, 
                                                   tiltangleX, 
                                                   tiltangleY, 
                                                   tiltcentre, 
                                                   checkforoverlaps);
    
        //Set the master placement as the first placement. All other samples
        //will rotate around this sample along the z axis
        if (sampleplaced && !masterplacementSet) 
        {
            sample->SetMaster(true); 
            masterplacementSet = true;
            mastername = sample->GetName();
            mastercentre = sample->GetInitialPosition();
            master = samplelist[i];
        }
    }
    
    G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

#include "G4PhysicalVolumeStore.hh"

void SampleConstruction::ApplyTransforms(double rotation, double position)
{
    //Work out the change in angle
    double deltaTheta = rotation - lastrotation;
    lastrotation = rotation;
    //G4ThreeVector centre = CalculateCentre();

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

int SampleConstruction::GroupTogetherSamples(std::string groupname, string_vector1D samplenames)
{
    SampleGroup samplegroup;
    samplegroup.name = groupname;
    
    //Check if a group containing that name already exists
    for (int i = 0 ; i < samplegroups.size() ; i++)
    {
        if(samplegroups[i].name == groupname)
        {
            G4cout << "\nWARNING: Cannot create sample group\"" << groupname << "\" because it already exists. Ignoring command... " << G4endl;
            return fParameterAlreadyExists;
        }
    }

    //Loop through samples, grouping them
    for (int i = 0 ; i < samplenames.size() ; i++)
    {
        for (int n = 0 ; n < samplelist.size() ; n++)
        {
            if(samplelist[n]->GetName() == samplenames[i])
            {
                samplegroup.group.push_back(samplelist[n]);
            }
        }
    }
    samplegroups.push_back(samplegroup);
    
    return 0;
}

int SampleConstruction::CloneSample(std::string name, std::string name_to_clone)
{   
    /*
    //Check if a sample containing that name already exists
    for (int i = 0 ; i < samplelist.size() ; i++)
    {
        if(samplelist[i]->GetName() == name)
        {
            G4cout << "\nWARNING: Cannot create sample \"" << name << "\" because it already exists. Ignoring command... " << G4endl;
            return fParameterAlreadyExists;
        }
    }
    
    int n;
    
    //Find the sample to clone
    for (int i = 0 ; i < samplelist.size() ; i++)
    {
        if(samplelist[i]->GetName() == name_to_clone)
        {
            n = i;
            break;
        }
    }
    
    SampleDescription* clone = new SampleDescription();
    clone->PrintDescription();
    
    clone = samplelist[n];
    clone->PrintDescription();
    */
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

