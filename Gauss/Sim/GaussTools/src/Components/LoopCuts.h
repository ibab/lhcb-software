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
              int maxs=100, double minstep=0.001, 
              double xmin=-10000.0, double ymin=-10000.0, double zmin=-5000.0, 
              double xmax=10000.0, double ymax=10000.0, double zmax=25000.0);

     virtual ~LoopCuts();

     // PostStep GPIL
     virtual G4double PostStepGetPhysicalInteractionLength(
                             const G4Track& track,
			     G4double   previousStepSize,
			     G4ForceCondition* condition
			    );
            
			    
  private:

  int m_counter;
  int m_maxstepnumber;
  double m_minstep;
  double m_xmin, m_ymin, m_zmin, m_xmax, m_ymax, m_zmax;
  
  // hide assignment operator as private 
      LoopCuts(LoopCuts&);
      LoopCuts& operator=(const LoopCuts& right);

};

#endif

