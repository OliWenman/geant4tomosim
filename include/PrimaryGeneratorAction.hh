#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4Timer.hh"
#include "globals.hh"
#include <vector>

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
		void ReadOutInfo(G4String SaveFilePath);

		//Set methods
		inline void SetParticleEnergy(G4double value ){energyCmd = value;}
		inline void SetBeamWidthY(G4double value){BeamWidthY_Cmd = value;} 
		inline void SetBeamHeightZ(G4double value){BeamHeightZ_Cmd = value;} 
		void SetValues(int nBins, double Position);

		void SetBeamCheck(bool value){BeamCheck = value;}
		void SetBeamData(bool value){BeamData = value;}
		void SetFluoreFM(bool value){FluoreFM = value;}
		void SetNumberOfEvents(unsigned long long int value, int TotalImages){NumberOfEvents = value; NumberOfRuns = TotalImages;}
		void ResetEvents(int nImage){CurrentEvent = 0; ImageProgressCheck = TotalProgressCheck = -1; CurrentImage = nImage;}

		//Get methods
		//G4ParticleGun* GetParticleGun() {return ParticleGun;}
		G4double GetBeamWidthY() {return BeamWidthY_Cmd;}
		G4double GetBeamHeightZ() {return BeamHeightZ_Cmd;}
		G4double GetMaxEnergy(){return eMax;}

		std::vector<int> GetBeamEnergyFreq(){return BeamEnergyFreq;}

  	private:
		//Pointer to G4ParticleGun
    		//G4ParticleGun* ParticleGun;
		G4GeneralParticleSource* ParticleGun;

		G4ParticleDefinition *gamma;

		Data* data;

		G4Timer Timer;

		//Pointer to PrimaryGeneratorActionMessenger
		PrimaryGeneratorActionMessenger* gunMessenger;
		
		//Declaring data variables
		G4double energyCmd;
		G4double BeamWidthY_Cmd;
		G4double BeamHeightZ_Cmd;

		G4double eMax;
		G4double Bins;

		//std::vector<double> BeamEnergyBins;
		std::vector<int> BeamEnergyFreq;

		bool BeamCheck;
		bool BeamData;
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

		int TotalProgress;
		int TotalProgressCheck;

		double remainingTime;
		bool timeCheck;
};

#endif
