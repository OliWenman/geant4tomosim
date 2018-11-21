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

		//Functions to be called
		void Initialize();
		void Visualisation();
		void RunSimulation();
		void PythonRun(int Image, int NumberOfImages, double dTheta);
		void KillSimulation();
		void ResetSimulation();

		std::vector<int> GetLastImage();

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

