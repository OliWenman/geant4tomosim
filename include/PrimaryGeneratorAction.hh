#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4Timer.hh"
#include "globals.hh"
#include <vector>
#include "SettingsLog.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

class Data;
class G4Event;
class G4ParticleDefination;
class PrimaryGeneratorActionMessenger;
class G4SPSPosDistribution;
class G4SPSAngDistribution;
class G4SPSEneDistribution;
class G4Timer;
class G4GeneralParticleSource;

//It defines a single particle which hits the detectors perpendicular to the input face

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  	public:
    	PrimaryGeneratorAction(Data *data);    
    	~PrimaryGeneratorAction();
   		void GeneratePrimaries(G4Event* );

		//Set methods
		inline void SetParticleEnergy(G4double value ){energyCmd = value;}
		inline void SetEnergyDistType(G4String value ){EnergyDistTypeCmd = value;}
		inline void SetEnergySigma(G4double value ){EnergySigmaCmd = value;}
		
		inline void SetBeamWidthY(G4double value){BeamWidthY_Cmd = value;} 
		inline void SetBeamHeightZ(G4double value){BeamHeightZ_Cmd = value;} 
		void SetValues(int nBins, double Position);

		void SetFluoreFM(bool value){FluoreFM = value;}
		void SetNumberOfEvents(unsigned long long int value, int TotalImages){NumberOfEvents = value; NumberOfRuns = TotalImages; }//SavingTime = 0;}
		void ResetEvents(int nImage){CurrentEvent = 0; ImageProgressCheck = TotalProgressCheck = -1; CurrentImage = nImage;}
		
		void SetSavingTime(double value){SavingTime = value;}
		
		void SetProgressBar(bool value){ShowProgressBar = value;}
		
		void SetParticleType(G4String type);

		//Get methods
		G4double GetBeamWidthY() {return BeamWidthY_Cmd;}
		G4double GetBeamHeightZ() {return BeamHeightZ_Cmd;}
		G4double GetMaxEnergy(){return eMax;}

		std::vector<int> GetBeamEnergy(){return beamEnergy;}
		
		void ReadOutInfo(SettingsLog& log);
		
		void SetupGun(G4String GunType, G4double monoEnergy, G4double sigmaEnegy);

  	private:
  	
  	    void SetupFastParticleGun(G4double monoEnergy);
  	    void SetupParticleGun(G4String GunType, G4double monoEnergy, G4double sigmaEnergy);
  	
		//Pointers to the particle source
    	G4ParticleGun* fastParticleGun;
		G4GeneralParticleSource* ParticleGun;
		G4ParticleDefinition *particle;

		Data* data;
		G4Timer Timer;

		//Pointer to PrimaryGeneratorActionMessenger
		PrimaryGeneratorActionMessenger* gunMessenger;
		
		//Declaring data variables
		G4double energyCmd;
		G4String EnergyDistTypeCmd;
    	G4double EnergySigmaCmd;
    	bool gunExists;
		
		G4double BeamWidthY_Cmd;
		G4double BeamHeightZ_Cmd;
    	G4double StartingPosition;

		G4double eMax;
		G4double Bins;

		std::vector<int> beamEnergy;

		bool FluoreFM;

		//Functions to do with the progress of the simulation
		void PrintProgress();
		void ProgressBar(int Percent);
		void EstimatedTime(int Percent);
		void PrintTime(double time);

		int CurrentEvent;
		unsigned long long int NumberOfEvents;
		int CurrentImage;
		int NumberOfRuns;
		int ImageProgressCheck;
		double SavingTime;

		int TotalProgress;
		int TotalProgressCheck;

		double remainingTime;
		
		bool ShowProgressBar;
};

#endif
