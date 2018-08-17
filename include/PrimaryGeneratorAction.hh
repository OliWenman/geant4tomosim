#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"
#include "G4UImessenger.hh"

class G4Event;
class PrimaryGeneratorActionMessenger;

/// The primary generator action class with particle gun.

/// It defines a single particle which hits the Tracker 
/// perpendicular to the input face. 

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  	public:
    		PrimaryGeneratorAction();    
    		virtual ~PrimaryGeneratorAction();

	public:
    		virtual void GeneratePrimaries(G4Event* );
    		
		//Set methods		
		void SetDefaultKinematic();

		//Set methods
		inline void SetParticleX(G4double value){ParticleX_Cmd = value;} 

		inline void SetParticleEnergy(G4double value ){energyCmd = value;}
		inline void SetBeamWidthY(G4double value) {BeamWidthY_Cmd = value;} 
		inline void SetBeamHeightZ(G4double value) {BeamHeightZ_Cmd = value;} 
			
		//Get methods
		G4ParticleGun* GetParticleGun() {return ParticleGun;}
		G4double GetBeamWidthY() {return BeamWidthY_Cmd;}
		G4double GetParticleX(){return ParticleX_Cmd;}

  	private:
		//Pointer to G4ParticleGun
    		G4ParticleGun* ParticleGun;

		//Pointer to PrimaryGeneratorActionMessenger
		PrimaryGeneratorActionMessenger* gunMessenger;
		
		//Declaring data variables
		G4double energyCmd;
		G4double BeamWidthY_Cmd;
		G4double BeamHeightZ_Cmd;
		
		G4double ParticleX_Cmd;

};


#endif
