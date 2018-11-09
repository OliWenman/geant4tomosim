#ifndef Simulation_h
#define Simulation_h 1

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
		void KillSimulation();

	private:
		//It's own private functions to be called by the public functions
		void SaveDataToFile(Data*);
		void BeamOn(G4RunManager* , unsigned long long int nParticles);
		void CompletionTime(double LoopTimer, int Image, int NoImages);
		void Setup();
		
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

