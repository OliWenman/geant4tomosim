#ifndef Simulation_h
#define Simulation_h 1

#include <vector>
#include "globals.hh"
#include <string>

class SimulationMessenger;
class Data;
class DefineMaterials;
class DetectorConstruction;
class PhysicsList;
class PrimaryGeneratorAction;
class StackingAction;

class G4RunManager;
class G4VisManager;
class G4UImanager;

class Simulation
{
	public:
		Simulation();
		~Simulation(); 

		//Methods
		//To be called from Python
		void pyOutputOptions(bool FFF, bool FFM, bool BM);
		void pyDataPaths(G4String settingsPath, G4String geometryPath, G4String h5OutputPath);
		void pyInitialise(int nDetectorsY, int nDetectorsZ, std::vector<double> DetectorDimensions, int nBins);
		std::vector<int> pyRun(unsigned long long int TotalParticles, double dTheta, int Image, int NumberOfImages, std::string Mode);
		std::vector<int> pyRunTest(unsigned long long int TotalParticles, int nImages, double rotation_angles, int CurrentImage, int nDarkFlatFields);

		//Python or C++
		std::vector<int> GetLastImage();
		std::vector<double> GetEnergyBins();//
		std::vector<int> GetEnergyFreq();//

		std::vector<int> GetBeamEnergyFreq();
		std::vector<std::vector<std::vector<int> > > GetFullMapping();
		int GetNumberCalibrations();
		void SetSavingTime(double Time);
		
		void SetSaveLogPath(std::string path){SaveLogPath = path;}

		void SetSeed(long int value){seedCmd = value;}
		void SetVerboseLevel(int value){verboseLevel = value;}
		void SetWriteToText(bool value){WriteToTextCmd = value;}

	private:
		//It's own private functions to be called by the public functions
		void Setup();
		void OutInfo(int verbose);
		void BeamOn(unsigned long long int nParticles);
		void Visualisation();
		unsigned long long int LimitGraphics(unsigned long long int nParticlesint, int nImage, std::string Mode);
		
		//Pointers to different classes
		SimulationMessenger* simMessenger;
		G4RunManager* runManager;
		G4VisManager* visManager;
		G4UImanager* UImanager;
		Data* data;
		DefineMaterials* materials;
		DetectorConstruction* DC;
		PhysicsList* PL;
		PrimaryGeneratorAction* PGA;
		StackingAction* particleManager;

		bool Reset;
		bool Ready;

		long int seedCmd;
		bool WriteToTextCmd;

		G4String SaveLogPath;
		G4String PathToScripts;
		
		int verboseLevel;
};

#endif

