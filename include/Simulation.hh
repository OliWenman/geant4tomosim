#ifndef Simulation_h
#define Simulation_h 1

#include <vector>

class Data;
class Input;
class DetectorConstruction;
class PhysicsList;

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
		void pyInitialise(int nDetectorsY, int nDetectorsZ);
		std::vector<int> pyRun(unsigned long long int TotalParticles, int Image, int NumberOfImages, double dTheta);

		//To be called from C++		
		void Initialize();
		void RunSimulation();

		//Python or C++
		std::vector<int> GetLastImage();
		void Visualisation();

	private:
		//It's own private functions to be called by the public functions
		void Setup();
		void SaveDataToFile();
		void BeamOn(unsigned long long int nParticles);
		void CompletionTime(double LoopTimer, int Image, int NoImages);
		
		//Pointers to different classes
		G4RunManager* runManager;
		G4VisManager* visManager;
		G4UImanager* UImanager;
		Data* data;
		Input* input;
		DetectorConstruction* DC;
		PhysicsList* PL;

		bool Reset;
		bool Ready;
};

#endif

