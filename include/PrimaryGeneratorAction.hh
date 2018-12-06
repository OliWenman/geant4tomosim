#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

class G4Event;
class G4ParticleDefination;
class PrimaryGeneratorActionMessenger;


class G4GeneralParticleSource;
//It defines a single particle which hits the detectors perpendicular to the input face

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  	public:
    		PrimaryGeneratorAction();    
    		~PrimaryGeneratorAction();
    		void GeneratePrimaries(G4Event* );
		void ReadOutInfo(G4String SaveFilePath);
    		
		//Set methods		
		void SetDefaultKinematic();

		//Set methods
		inline void SetParticleEnergy(G4double value ){energyCmd = value;}
		inline void SetBeamWidthY(G4double value){BeamWidthY_Cmd = value;} 
		inline void SetBeamHeightZ(G4double value){BeamHeightZ_Cmd = value;} 
		void SetWorldLength(G4double value){WorldLength = value;}
			
		//Get methods
		G4ParticleGun* GetParticleGun() {return ParticleGun;}
		G4double GetBeamWidthY() {return BeamWidthY_Cmd;}
		G4double GetBeamHeightZ() {return BeamHeightZ_Cmd;}
		G4double GetMaxEnergy(){return energyCmd;}

  	private:
		//Pointer to G4ParticleGun
    		G4ParticleGun* ParticleGun;
		//G4GeneralParticleSource* ParticleGun;

		G4ParticleDefinition *gamma;
		//G4ParticleDefinition *electron;
		//G4ParticleDefinition *positron;

		//Pointer to PrimaryGeneratorActionMessenger
		PrimaryGeneratorActionMessenger* gunMessenger;
		
		//Declaring data variables
		G4double energyCmd;
		G4double BeamWidthY_Cmd;
		G4double BeamHeightZ_Cmd;
		G4double WorldLength;
};

#endif
