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

class PrimaryGeneratorActionMessenger;
class Data;
class ProgressTracker;

class G4Event;
class G4ParticleDefination;
class G4SPSPosDistribution;
class G4SPSAngDistribution;
class G4SPSEneDistribution;
class G4GeneralParticleSource;

//It defines a single particle which hits the detectors perpendicular to the input face

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  	public:
    	PrimaryGeneratorAction(Data *data);    
    	~PrimaryGeneratorAction();
    	
   		void GeneratePrimaries(G4Event* );
   		void SetupGun(G4String GunType, G4double monoEnergy, G4double sigmaEnegy);
		void ReadOutInfo(SettingsLog& log);

		//Set methods
		void SetParticleEnergy(G4double value ){energyCmd = value;}
		void SetEnergyDistType(G4String value ){EnergyDistTypeCmd = value;}
		void SetEnergySigma(G4double value ){EnergySigmaCmd = value;}
		void SetMaxEnergyBinCmd(G4double value){eMax = value*1000.;}
		
		void SetBeamWidthY(G4double value){BeamWidthY_Cmd = value;} 
		void SetBeamHeightZ(G4double value){BeamHeightZ_Cmd = value;} 
		void SetValues(int nBins, double Position);

		void SetFluoreFM(bool value){FluoreFM = value;}
		
		void SetNumberOfEvents(unsigned long long int value, int TotalImages){NumberOfEvents = value; 
		                                                                      NumberOfRuns = TotalImages; 
		                                                                      progress.Setup(value, TotalImages);}
		void ResetEvents(int nImage){CurrentEvent = 0; 
		                             CurrentImage = nImage; 
		                             progress.ResetEvents();}
		
		void SetSavingTime(double value){progress.SetSavingTime(value);}
		void SetProgressBar(bool value){ShowProgressBar = value;}
		
		void SetParticleType(G4String type);
		void SetPolization(G4ThreeVector value){polization = value; randPolization = false;}

		//Get methods
		G4double GetBeamWidthY() {return BeamWidthY_Cmd;}
		G4double GetBeamHeightZ() {return BeamHeightZ_Cmd;}
		G4double GetMaxEnergy(){return eMax;}

		std::vector<int> GetBeamEnergy(){return beamEnergy;}
		G4int GetBeamEnergyByteTypeSize(){return sizeof(beamEnergy[0]);}

  	private:
  	
  	    void SetupFastParticleGun(G4double monoEnergy);
  	    void SetupParticleGun(G4String GunType, G4double monoEnergy, G4double sigmaEnergy);
  	    G4ThreeVector RandomPolarization();
  	
		//Pointers to the particle source
    	G4ParticleGun* fastParticleGun;
		G4GeneralParticleSource* ParticleGun;
		G4ParticleDefinition *particle;

		Data* data;
		ProgressTracker progress;

		//Pointer to PrimaryGeneratorActionMessenger
		PrimaryGeneratorActionMessenger* gunMessenger;
		
		//Declaring data variables
		G4double energyCmd;
		G4String EnergyDistTypeCmd;
    	G4double EnergySigmaCmd;
    	bool gunExists;
		bool randPolization;
		G4ThreeVector polization;
		
		G4double BeamWidthY_Cmd;
		G4double BeamHeightZ_Cmd;
    	G4double StartingPosition;

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
