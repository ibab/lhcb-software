// ------------------------------------------------------------

#ifndef WorldCuts_h
#define WorldCuts_h 1

#include "G4ios.hh"
#include "globals.hh"
#include "SpecialCuts.h"


class WorldCuts : public SpecialCuts
{
  public:     

  WorldCuts(const G4String& processName ="WorldCuts",
            double xmin=-10000.0, double ymin=-10000.0, double zmin=-5000.0, 
            double xmax=10000.0, double ymax=10000.0, double zmax=25000.0);
  
  virtual ~WorldCuts();
  
  // PostStep GPIL
  virtual G4double PostStepGetPhysicalInteractionLength
  (const G4Track& track,
   G4double   previousStepSize,
   G4ForceCondition* condition);
  
  
private:
  
  double m_xmin, m_ymin, m_zmin, m_xmax, m_ymax, m_zmax;
  
  // hide assignment operator as private 
  WorldCuts(WorldCuts&);
  WorldCuts& operator=(const WorldCuts& right);
  
};

#endif

