#ifndef TrackingAction_h
#define TrackingAction_h 1

#include "globals.hh"
#include "G4UserTrackingAction.hh"

class G4Navigator;
class G4Track; 

class TrackingAction  :  public G4UserTrackingAction 
{
public:
	TrackingAction(void);
	virtual ~TrackingAction(void);
     virtual void PreUserTrackingAction(const G4Track*);

private:
};

#endif
