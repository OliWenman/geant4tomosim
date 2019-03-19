#ifndef Simulation_h
#define Simulation_h 1

#include <vector>
#include "globals.hh"
#include <string>
#include "SettingsLog.hh"
#include "Data.hh"
#include "AbsorptionDetector.hh"
#include "FluorescenceDetector.hh"
#include "FluorescenceSD.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"


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
		Simulation(int verb);
		~Simulation(); 

		//Methods
		//To be called from Python
		void pyOutputOptions(bool FFF, bool FFM);
		void pySetupDetectors(int nDetectorsY, int nDetectorsZ, std::vector<double> DetectorDimensions, int nBins);
		void pyAddMacros(std::vector<std::string> macroFiles);
        std::vector<int> pyRun(unsigned long long int TotalParticles, std::vector<int> ImageInfo, double rotation_angles, std::vector<double> gunEnergy, G4String gunType);

        //It's own private functions to be called by the public functions
		void PrintInfo(unsigned long long int TotalParticles, int NumberOfImages, int nDarkFlatFields);
		void PrintInfo(int verbose);
		
		void CalculateStorageSpace(int projections);

        //GET FUNCTIONS
		//Functions to return data to Python
		std::vector<int> GetLastImage()                               {return data -> GetHitData();}
		std::vector<double> GetEnergyBins()                           {return data -> GetEnergyBins();}
		std::vector<int> GetBeamEnergy()                              {return PGA -> GetBeamEnergy();}
		std::vector<int> GetFluorescence()                            {return data -> GetFluorescence();}
		std::vector<std::vector<std::vector<int> > > GetFullMapping() {return data -> GetFullMapping();}
		std::vector<std::vector<int> > GetDiffractionData()           {return data -> GetDiffractionData();}
		
		int GetNumberOfBins()                                     {return data -> GetNumberOfBins();}
		int GetNumberOfxPixels()                                  {return DC->GetAbsorptionDetector()->GetNumberOfxPixels();}
		int GetNumberOfyPixels()                                  {return DC->GetAbsorptionDetector()->GetNumberOfyPixels();}
		std::vector<double> GetAbsorptionDetectorHalfDimensions() {return DC->GetAbsorptionDetector()->GetHalfDimensions();}
		bool FullMappingFluorescence()                            {return data->GetFullMapping_Option();}//DC->GetFluorescenceDetector()->GetSensitiveDetector()->FullMapping();}
		bool FullFieldFluorescence()                              {return data->GetFluorescence_Option();}//DC->GetFluorescenceDetector()->GetSensitiveDetector()->FullField();}
		
		//SET FUNCTIONS
		void SetSaveLogPath(std::string path, std::string fileName) {SaveLogPath = path; FileName = fileName;}
		void SetSeed(long int value)                                {seedCmd = value;}
		void SetSavingTime(double Time)                             {PGA -> SetSavingTime(Time);}
		void SetVerboseLevel(int value)                             {verboseLevel = value;}
		void SetGlobalVerboseLevel(int value)                       {globalVerbose = value;}

	private:
	    std::string GetStorageUnit(double &storage); 
	    void Setup();
		void BeamOn(unsigned long long int nParticles);
		void Visualisation();
		unsigned long long int LimitGraphics(unsigned long long int nParticlesint, int nImage, std::string Mode);
		void PrintInformation(SettingsLog log);
		
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
		int globalVerbose;
		
		std::vector<std::string> macrofiles;
};

#endif

