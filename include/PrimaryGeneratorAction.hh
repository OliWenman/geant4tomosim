#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "ProgressTracker.hh"
#include "globals.hh"
#include <vector>
#include "SettingsLog.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

#include "ParticleBeam.hh"

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
   		void SetupGun(G4String GunType, G4double monoEnergy, G4double sigmaEnegy);
		void ReadOutInfo(SettingsLog& log);

		void SetMaxEnergyBinCmd(G4double value){eMax = value*1000.;}
		void SetFluoreFM(bool value){FluoreFM = value;}		
		void SetNumberOfEvents(unsigned long long int value, int TotalImages){NumberOfEvents = value; 
		                                                                      NumberOfRuns = TotalImages; 
		                                                                      progress.Setup(value, TotalImages);}
		void ResetEvents(int nImage){CurrentEvent = 0; 
		                             CurrentImage = nImage; 
		                             progress.ResetEvents();}
		
		void SetSavingTime(double value){progress.SetSavingTime(value);}
		void SetProgressBar(bool value){ShowProgressBar = value;}
		
		G4double GetMaxEnergy(){return eMax;}
		std::vector<int> GetBeamEnergy(){return beamEnergy;}
		G4int GetBeamEnergyByteTypeSize(){return sizeof(beamEnergy[0]);}
		
		ParticleBeam* GetBeam(){return beam;}
		void SetBins(G4int value) {Bins = value;}
		
		void SetAutoBeamPlacement(bool value)    {beam -> SetAutoSourcePlacement(value);}
		void DoAutoBeamPlacement(G4double value) {beam -> DoAutoSourcePlacement(value);}

  	private:
  	
  	    void SetupFastParticleGun(G4double monoEnergy);
  	    void SetupParticleGun(G4String GunType, G4double monoEnergy, G4double sigmaEnergy);

		Data* data;
		ParticleBeam* beam;
		ProgressTracker progress;

		PrimaryGeneratorActionMessenger* gunMessenger;

		G4double eMax;
		G4double Bins;

		std::vector<int> beamEnergy;

		bool FluoreFM;
        
		int CurrentEvent;
		unsigned long long int NumberOfEvents;
		int CurrentImage;
		int NumberOfRuns;
		
		bool ShowProgressBar;
};

#endif
