#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "Data.hh"

#include "G4UImessenger.hh"

class RunActionMessenger;
class Data;

class G4Run;

/// Run action class
class RunAction : public G4UserRunAction
{
  	public:
    		RunAction(Data* data);
    		virtual ~RunAction();

    		virtual void BeginOfRunAction(const G4Run* run);
    		virtual void EndOfRunAction(const G4Run* run);

		inline void SetSeedOption(G4int value){seedCmd = value;}
		inline void SetTotalNoEvents(G4int value){TotalNoEvents = value;}

		G4int GetSeedOption(){return seedCmd;}
		G4int GetTotalNoEvents(){return TotalNoEvents;}

	private:
		//Pointer to PrimaryGeneratorActionMessenger
		RunActionMessenger* runMessenger;

		//Pointer to Data class
		Data* data;
		
		//Declaring data variables
		G4int seedCmd;
		G4int TotalNoEvents;
};


#endif
