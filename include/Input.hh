#ifndef Input_h
#define Input_h 1

#include "Simulation.hh"
#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4UserLimits.hh"
#include <iostream>
#include <vector>
#include <math.h>       // Used to access the function floor 
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"

class InputMessenger;
class Data;

class Input 
{
	public:
		Input(Data* data);
		~Input();
		
		//Own class function
		void WriteToTextFile();

		//Get Methods
		//inline int GetNumberRows(){return rows;}
		//inline int GetNumberColumns(){return columns;}
		//G4String GetDetectorMaterial(){return DetectorMaterial;}
		//G4ThreeVector GetDetectorDimensions(){return DetectorDimensions;}
		//G4bool GetDetectorEfficiency(){return DetectorEfficiency_Cmd;}

		//G4int GetNoBins(){return NoBins_Cmd;}
		G4bool GetEnergyDataOption(){return EnergyDataCmd;}
		//G4double GetMaxEnergy(){return MaxE;}
		//G4double GetBeamWidth(){return BeamWidth;}
		//G4double GetBeamHeight(){return BeamHeight;}
		//G4double GetBeamLength(){return BeamLength;}

		//G4bool GetVisualization(){return Visualization_Cmd;}
		//G4String GetPhysicsUsed(){return PhysicsUsed;}
		G4int GetSeedOption(){return seedCmd;}
		//G4double GetSimulationTime(){return SimulationTime;}

		inline G4int GetCurrentImage(){return CurrentImage;}
		G4int GetNoImages(){return NoImages_Cmd;}
		G4String GetNoPhotons(){return NoPhotons_Cmd;}

		//G4bool GetTextFileCmd(){return TextFileCmd;}
		//G4bool GetHDF5FileCmd(){return HDF5FileCmd;}

		//Set Methods
		void SetNumberRows(int value){rows = value;}
		void SetNumberColumns(int value){columns = value;}
		void SetDetectorMaterial(G4String value){DetectorMaterial = value;}
		void SetDetectorDimensions(G4ThreeVector value){DetectorDimensions = value;}
		void SetDetectorEfficiency(G4bool value){DetectorEfficiency_Cmd = value;}

		void SetNoBins(G4int value){NoBins_Cmd = value;}
		void SetEnergyDataOption(G4bool value){EnergyDataCmd = value;}
		void SetMaxEnergy(G4double value){MaxE = value;}
		void SetBeamWidth(G4double value){BeamWidth = value;}
		void SetBeamHeight(G4double value){BeamHeight = value;}
		void SetBeamLength(G4double value){BeamLength = value;}

		void SetVisualization(G4bool value){Visualization_Cmd = value;}
		void SetPhysicsUsed(G4String value){PhysicsUsed = value;}
		void SetPhysicProcesses(std::vector<G4String> value){PhysicProcesses = value;}
		void SetSeedOption(G4int value){seedCmd = value;}
		void SetSimulationTime(G4double value){SimulationTime = value;}

		void SetCurrentImage(G4int value){CurrentImage = value;}
		void SetNoImages(G4int value){NoImages_Cmd = value;}
		void SetNoPhotons(G4String value){NoPhotons_Cmd = value;}

		void SetTextFileCmd(G4bool value){TextFileCmd = value;}
		void SetHDF5FileCmd(G4bool value){HDF5FileCmd = value;}

	private:	
		InputMessenger* inputMessenger;
		Data* data;

		//Data members
		int rows;
		int columns;	
		G4String DetectorMaterial;
		G4ThreeVector DetectorDimensions;

		G4double MaxE;
		G4double BeamHeight;
		G4double BeamWidth;
		G4double BeamLength;

		G4int Seed;
		G4int CurrentImage;

		G4String PhysicsUsed;
		std::vector<G4String> PhysicProcesses;
		G4bool TextFileCmd;
		G4bool HDF5FileCmd;
		G4double SimulationTime;
		G4bool DetectorEfficiency_Cmd;

		G4int NoBins_Cmd;
		G4int NoImages_Cmd;
		G4String NoPhotons_Cmd;
		G4bool Visualization_Cmd;
		G4int seedCmd;
		G4bool EnergyDataCmd;
};

#endif

		
