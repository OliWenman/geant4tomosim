#ifndef Data_h
#define Data_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4UserLimits.hh"
#include <iostream>
#include <vector>
#include <math.h>       // Used to access the function floor 
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include <climits>

class DataMessenger;

class Data
{
	public:
		Data();
		virtual ~Data();
		
		//Get Methods
		std::vector<G4int> GetHitData(){ return HitDataMatrix;}
		std::vector<std::vector<G4int> > GetEnergyData(){return EnergyMatrix;}

		inline G4int GetNumberRows(){return rows;}
		inline G4int GetNumberColumns(){return columns;}
		G4String GetDetectorMaterial(){return DetectorMaterial;}
		G4ThreeVector GetDetectorDimensions(){return DetectorDimensions;}

		G4double GetMaxEnergy(){return MaxE;}
		G4double GetBeamWidth(){return BeamWidth;}
		G4double GetBeamHeight(){return BeamHeight;}
		G4double GetBeamLength(){return BeamLength;}

		inline G4int GetCurrentImage(){return CurrentImage;}

		G4String GetPhysicsUsed(){return PhysicsUsed;}

		G4double GetSimulationTime(){return SimulationTime;}

		G4bool GetDetectorEfficiency(){return DetectorEfficiency_Cmd;}

		//Set Methods
		void SetHitData(std::vector<G4int> UpdatedData){HitDataMatrix = UpdatedData;}
		void SetEnergyData(std::vector<std::vector<G4int> >  UpdatedData){EnergyMatrix = UpdatedData;}
		
		void SetNumberRows(G4int value){rows = value;}
		void SetNumberColumns(G4int value){columns = value;}
		void SetDetectorMaterial(G4String value){DetectorMaterial = value;}
		void SetDetectorDimensions(G4ThreeVector value){DetectorDimensions = value;}

		void SetMaxEnergy(G4double value){MaxE = value;}
		void SetBeamWidth(G4double value){BeamWidth = value;}
		void SetBeamHeight(G4double value){BeamHeight = value;}
		void SetBeamLength(G4double value){BeamLength = value;}

		void SetCurrentImage(G4int value){CurrentImage = value;}

		void SetPhysicsUsed(G4String value){PhysicsUsed = value;}

		void SetSimulationTime(G4double value){SimulationTime = value;}

		void SetDetectorEfficiency(G4bool value){DetectorEfficiency_Cmd = value;}

		

		//Commands
		G4int GetNoBins(){return NoBins_Cmd;}
		G4int GetNoImages(){return NoImages_Cmd;}
		G4String GetNoPhotons(){return NoPhotons_Cmd;}
		G4bool GetVisualization(){return Visualization_Cmd;}
		G4bool GetTextFileCmd(){return TextFileCmd;}
		G4bool GetHDF5FileCmd(){return HDF5FileCmd;}
		G4int GetSeedOption(){return seedCmd;}
		G4bool GetEnergyDataOption(){return EnergyDataCmd;}
		void SetNoBins(G4int value){NoBins_Cmd = value;}
		void SetNoImages(G4int value){NoImages_Cmd = value;}
		void SetNoPhotons(G4String value){NoPhotons_Cmd = value;}
		void SetVisualization(G4bool value){Visualization_Cmd = value;}
		void SetTextFileCmd(G4bool value){TextFileCmd = value;}
		void SetHDF5FileCmd(G4bool value){HDF5FileCmd = value;}
		void SetSeedOption(G4int value){seedCmd = value;}
		void SetEnergyDataOption(G4bool value){EnergyDataCmd = value;}

		//Method functions
		inline G4int Quotient(G4int dividend, G4int divisor ) {return dividend / divisor;}
		inline G4int Remainder(G4int dividend, G4int divisor) {return dividend % divisor;}

		virtual void SetUpHitData(G4int Nrow, G4int Ncolumn);
		virtual void SetUpEnergyData();
		virtual void SaveHitData(G4int DetectorNumber);
		virtual void PrintHitData();
		virtual G4int BinNumber(G4int DetectorNumber, G4double edep);
		virtual void SaveEnergyData(G4int DetectorNumber, G4double edep);
		virtual void PrintEnergyData();
		virtual void WriteToTextFile(G4String);
		virtual void WriteToHDF5();

	private:	
		DataMessenger* dataMessenger;

		//Data members
		std::vector<G4int> HitDataMatrix;
		std::vector<std::vector<G4int> > EnergyMatrix;

		G4int rows;
		G4int columns;	
		G4String DetectorMaterial;
		G4ThreeVector DetectorDimensions;

		G4double MaxE;
		G4double BeamHeight;
		G4double BeamWidth;
		G4double BeamLength;

		G4int Seed;
		G4int CurrentImage;

		G4String PhysicsUsed;
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

		
