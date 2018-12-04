#ifndef Simulation_h
#define Simulation_h 1

#include <vector>

class SimulationMessenger;
class Data;
class Input;
class DetectorConstruction;
class PhysicsList;
class PrimaryGeneratorAction;

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
		void pyInitialise(int nDetectorsY, int nDetectorsZ, std::vector<double> DetectorDimensions, int nBins);
		std::vector<int> pyRun(unsigned long long int TotalParticles, int Image, int NumberOfImages, double dTheta);

		//To be called from C++		
		void Initialize();
		void RunSimulation();

		//Python or C++
		std::vector<int> GetLastImage();
		std::vector<std::vector<int> > GetLastEnergyData();
		std::vector<double> GetEnergyBins();//
		std::vector<int> GetEnergyFreq();//

		void SetSeed(long int value){seedCmd = value;}
		void SetWriteToText(bool value){WriteToTextCmd = value;}

	private:
		//It's own private functions to be called by the public functions
		void Setup();
		void BeamOn(unsigned long long int nParticles);
		void CompletionTime(double LoopTimer, int Image, int NoImages);
		void Visualisation();
		
		//Pointers to different classes
		SimulationMessenger* simMessenger;
		G4RunManager* runManager;
		G4VisManager* visManager;
		G4UImanager* UImanager;
		Data* data;
		Input* input;
		DetectorConstruction* DC;
		PhysicsList* PL;
		PrimaryGeneratorAction* PGA;

		bool Reset;
		bool Ready;

		long int seedCmd;
		bool WriteToTextCmd;
};

#endif

