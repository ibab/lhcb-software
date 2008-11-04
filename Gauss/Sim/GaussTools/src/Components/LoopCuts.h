// ------------------------------------------------------------

#ifndef LoopCuts_h
#define LoopCuts_h 1

#include "G4ios.hh"
#include "globals.hh"
#include "SpecialCuts.h"


class LoopCuts : public SpecialCuts
{
  public:     

     LoopCuts(const G4String& processName ="LoopCuts",
              int maxs=100, double minstep=0.001);

     virtual ~LoopCuts();

     // PostStep GPIL
     virtual G4double PostStepGetPhysicalInteractionLength(
                             const G4Track& track,
			     G4double   previousStepSize,
			     G4ForceCondition* condition
			    );
            
     virtual G4VParticleChange* PostStepDoIt(
			     const G4Track& ,
			     const G4Step& 
			    );            
			    
  private:

  int m_counter;
  int m_maxstepnumber;
  double m_minstep;
  
  // hide assignment operator as private 
      LoopCuts(LoopCuts&);
      LoopCuts& operator=(const LoopCuts& right);

};

#endif

