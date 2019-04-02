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
#include "Data.hh"

#include "MyVectors.hh"

class PrimaryGeneratorActionMessenger;
class Data;
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
    	PrimaryGeneratorAction(Data *data);    
    	~PrimaryGeneratorAction();
    	
   		void GeneratePrimaries(G4Event* );
   		void SetupData();
		void ReadOutInfo(SettingsLog& log);
		
		int_vector1D GetBeamIntensity() {return beamintensity;}
		double_vector1D GetBeamEnergy(){return energy;}
		
		G4int GetBeamEnergyByteTypeSize(){return sizeof(beamintensity[0]);}

		void SetMaxEnergyBinCmd(G4double value){eMax = value*1000.; data -> SetMaxEnergy(eMax);}
		void SetFluoreFM(bool value){FluoreFM = value;}		
		void SetNumberOfEvents(unsigned long long int value, int TotalImages){NumberOfEvents = value; 
		                                                                      NumberOfRuns = TotalImages; 
		                                                                      progress.Setup(value, TotalImages);}
		void SetNumberOfBins(int value){Bins = value;}
		void ResetEvents(int nImage){CurrentEvent = 0; 
		                             CurrentImage = nImage; 
		                             progress.ResetEvents();}
		
		void SetSavingTime(double value){progress.SetSavingTime(value);}
		void SetProgressBar(bool value){ShowProgressBar = value;}
		
		G4double GetMaxEnergy(){return eMax;}
		
		ParticleBeam* GetBeam(){return beam;}
		
		void SetAutoBeamPlacement(bool value)    {beam -> SetAutoSourcePlacement(value);}
		void DoAutoBeamPlacement(G4double value) {beam -> DoAutoSourcePlacement(value);}

        const G4ThreeVector GetParticlePosition() const {return particleposition;}
        
        double GetBeamHalfx() const {return beam->GetHalfX();}
        double GetBeamHalfy() const {return beam->GetHalfY();}
        int    GetNumberOfBins() const {return Bins;}

  	private:

		Data                            *data;
		ParticleBeam                    *beam;
		PrimaryGeneratorActionMessenger *gunMessenger;
        ProgressTracker                  progress;
        
        int_vector1D    beamintensity;
        double_vector1D energy;

		G4double eMax;
		G4double Bins;

		bool FluoreFM;
        
		int CurrentEvent;
		unsigned long long int NumberOfEvents;
		int CurrentImage;
		int NumberOfRuns;
		
		bool ShowProgressBar;
		
		G4ThreeVector particleposition;
};

#endif
