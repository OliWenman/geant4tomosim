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
class SteppingAction;

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
		void pyOutputOptions(bool FFF, bool FFM);
		void pySetupDetectors(int nDetectorsY, int nDetectorsZ, std::vector<double> DetectorDimensions, int nBins);
		void pyAddMacros(std::vector<std::string> macroFiles);
        std::vector<int> pyRun(unsigned long long int TotalParticles, std::vector<int> ImageInfo, double rotation_angles, std::vector<double> gunEnergy, G4String gunType);

		//Python or C++
		std::vector<int> GetLastImage();
		std::vector<double> GetEnergyBins();
		std::vector<int> GetFluorescence();

		std::vector<int> GetBeamEnergy();
		std::vector<std::vector<std::vector<int> > > GetFullMapping();
		void SetSavingTime(double Time);
		
		void SetSaveLogPath(std::string path, std::string fileName){SaveLogPath = path; FileName = fileName;}

		void SetSeed(long int value){seedCmd = value;}
		void SetVerboseLevel(int value){verboseLevel = value;}

        //It's own private functions to be called by the public functions
		void OutInfo(unsigned long long int TotalParticles, int NumberOfImages, int nDarkFlatFields);

	private:
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
		SteppingAction* stepManager;

		bool Reset;
		bool Ready;

		long int seedCmd;

		std::string SaveLogPath;
		std::string FileName;
		
		int verboseLevel;
		
		std::vector<std::string> macrofiles;
};

#endif

