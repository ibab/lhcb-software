// ------------------------------------------------------------

#ifndef MinEkineCuts_h
#define MinEkineCuts_h 1

#include "G4ios.hh"
#include "globals.hh"
#include "SpecialCuts.h"


class MinEkineCuts : public SpecialCuts
{
  public:     

     MinEkineCuts(const G4String& processName ="MinEkineCuts", 
                  double m_cut=DBL_MAX);

     virtual ~MinEkineCuts();

     // PostStep GPIL
     virtual G4double PostStepGetPhysicalInteractionLength(
                             const G4Track& track,
			     G4double   previousStepSize,
			     G4ForceCondition* condition
			    );
            
			    
  private:

  double m_cut;
  
  // hide assignment operator as private 
      MinEkineCuts(MinEkineCuts&);
      MinEkineCuts& operator=(const MinEkineCuts& right);

};

#endif

