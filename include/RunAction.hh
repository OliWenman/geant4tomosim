#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "Data.hh"

#include "G4UImessenger.hh"

class RunActionMessenger;
class Input;
class G4Run;

//Run action class
class RunAction : public G4UserRunAction
{
  	public:
    		RunAction(Input* input);
    		virtual ~RunAction();

    		virtual void BeginOfRunAction(const G4Run* run);
    		virtual void EndOfRunAction(const G4Run* run);
		virtual void GenerateSeed();

		inline void SetSeedOption(G4int value){seedCmd = value;}
		inline void SetTotalNoEvents(G4int value){TotalNoEvents = value;}
		inline void SetPrintOption(G4bool value){PrintCmd = value;} 

		G4int GetSeedOption(){return seedCmd;}
		G4int GetTotalNoEvents(){return TotalNoEvents;}
		G4bool GetPrintOption(){return PrintCmd;}	

	private:
		//Pointers to classes
		RunActionMessenger* runMessenger;
		Input* input;
		
		//Declaring data variables
		G4int seedCmd;
		G4int TotalNoEvents;
		G4bool PrintCmd;
};


#endif
