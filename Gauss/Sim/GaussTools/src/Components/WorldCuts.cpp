// --------------------------------------------------------------

#include "WorldCuts.h"

#include "G4Step.hh"
#include "G4UserLimits.hh"
#include "G4VParticleChange.hh"

WorldCuts::WorldCuts(const G4String& aName, double xmin, double ymin, double zmin, 
                     double xmax, double ymax, double zmax)
  : SpecialCuts(aName)
{  
  m_xmin=xmin;
  m_ymin=ymin;
  m_zmin=zmin;
  m_xmax=xmax;
  m_ymax=ymax;
  m_zmax=zmax;
  
  if (verboseLevel>1) {
    std::cout << GetProcessName() << " is created "<< std::endl;
  }
  SetProcessType(fUserDefined);
}

WorldCuts::~WorldCuts()
{}

WorldCuts::WorldCuts(WorldCuts& right)
  :SpecialCuts(right)
{}

 
G4double WorldCuts::
PostStepGetPhysicalInteractionLength(const G4Track& track,
                                     G4double ,
                                     G4ForceCondition* condition)
{
  // condition is set to "Not Forced"
  *condition = NotForced;
  // default proposed step
  G4double     proposedStep = DBL_MAX;
  
  double xpos=track.GetPosition().x();
  double ypos=track.GetPosition().y();
  double zpos=track.GetPosition().z();  

  if (xpos>m_xmax || xpos<m_xmin ||
      ypos>m_ymax || ypos<m_ymin ||
      zpos>m_zmax || zpos<m_zmin) 
    {          
      proposedStep = 0.;
    } 
  
  return proposedStep;    
}

