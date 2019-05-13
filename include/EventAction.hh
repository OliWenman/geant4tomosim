/*
Class that can control what happens for each event for a particle. Useful
for debugging or storing information

Author: Oliver Jack Wenman
*/

#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"

#include "globals.hh"

class EventAction : public G4UserEventAction
{
  	public:
    		EventAction();
    		~EventAction();

    		void  BeginOfEventAction(const G4Event* );
    		void  EndOfEventAction(const G4Event* );
};

#endif
