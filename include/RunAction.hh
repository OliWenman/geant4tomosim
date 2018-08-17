#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

#include "G4UImessenger.hh"

class RunActionMessenger;

class G4Run;

/// Run action class
class RunAction : public G4UserRunAction
{
  	public:
    		RunAction();
    		virtual ~RunAction();

    		virtual void BeginOfRunAction(const G4Run* run);
    		virtual void EndOfRunAction(const G4Run* run);

		inline void SetSeedOption(G4int value){seedCmd = value;}

		G4int GetSeedOption(){return seedCmd;}

	private:
		//Pointer to PrimaryGeneratorActionMessenger
		RunActionMessenger* runMessenger;
		
		//Declaring data variables
		G4int seedCmd;
};


#endif
