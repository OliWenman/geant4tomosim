#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "ProgressTracker.hh"
#include "globals.hh"
#include "SettingsLog.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

#include "ParticleBeam.hh"

#include "MyVectors.hh"

class PrimaryGeneratorActionMessenger;
class ProgressTracker;

class G4Event;
class G4ParticleDefination;
class G4SPSPosDistribution;
class G4SPSAngDistribution;
class G4SPSEneDistribution;
class G4GeneralParticleSource;

class ParticleBeam;

//It defines a single particle which hits the detectors perpendicular to the input face

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  	public:
    	PrimaryGeneratorAction();    
    	~PrimaryGeneratorAction();
    	
   		void GeneratePrimaries(G4Event* );
   		void SetupData();
		void ReadOutInfo(SettingsLog& log);

        //Set functions
		void SetMaxEnergyBinCmd(G4double value){eMax = value*1000.;}
		void SetFluoreFM(bool value){fm_fluorescence = value;}		
		void SetNumberOfEvents(unsigned long long int value, int TotalImages){totalevents = value; 
		                                                                      totalruns = TotalImages; 
		                                                                      progress.Setup(value, TotalImages);}
		void SetNumberOfBins(int value){bins = value;}
		void ResetEvents(int n_projection){currentevent = 0; 
		                                   currentrun = n_projection; 
		                                   progress.ResetEvents();}
		
		void SetSavingTime(double value){progress.SetSavingTime(value);}
		
		void SetAutoBeamPlacement(bool value)    {beam -> SetAutoSourcePlacement(value);}
		void DoAutoBeamPlacement(G4double value) {beam -> DoAutoSourcePlacement(value);}

        //Get functions
        ParticleBeam*       GetBeam()            {return beam;}
        ProgressTracker&    GetProgressTracker() {return progress;}
        int_vector1D        GetBeamIntensity()   {return beamintensity;}
		double_vector1D     GetBeamEnergy()      {return energy;}
		
        const G4ThreeVector GetParticlePosition() const {return particleposition;}        
        const double        GetBeamHalfx() const {return beam->GetHalfX();}
        const double        GetBeamHalfy() const {return beam->GetHalfY();}
        const double        GetMaxEnergy(){return eMax;}
        const int           GetNumberOfBins() const {return bins;}
        const int           GetBeamEnergyByteTypeSize(){return sizeof(beamintensity[0]);}

  	private:
        //Data

        //Objects
        PrimaryGeneratorActionMessenger *gunMessenger;
		ParticleBeam                    *beam;
        ProgressTracker                  progress;
        
        //Vectors
        int_vector1D    beamintensity;
        double_vector1D energy;

        //Simulation variables
		double        eMax;
		double        bins;
		bool          fm_fluorescence;
        G4ThreeVector particleposition;
        
        //Progress
        unsigned long long int totalevents;
		int currentevent;
		int totalruns;
		int currentrun;
};

#endif
