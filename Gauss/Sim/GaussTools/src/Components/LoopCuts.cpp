// --------------------------------------------------------------

#include "LoopCuts.h"

#include "G4Step.hh"
#include "G4UserLimits.hh"
#include "G4VParticleChange.hh"

LoopCuts::LoopCuts(const G4String& aName, int maxs, double minstep, 
                   double xmin, double ymin, double zmin, 
                   double xmax, double ymax, double zmax)
  : SpecialCuts(aName)
{ 

  m_maxstepnumber = maxs;
  m_minstep = minstep;

  m_xmin=xmin;
  m_ymin=ymin;
  m_zmin=zmin;
  m_xmax=xmax;
  m_ymax=ymax;
  m_zmax=zmax;
  
  
  if (verboseLevel>1) {
    G4cout << GetProcessName() << " is created "<< G4endl;
  }
  SetProcessType(fUserDefined);
}

LoopCuts::~LoopCuts()
{}

LoopCuts::LoopCuts(LoopCuts& right)
  :SpecialCuts(right)
{}

 
G4double LoopCuts::
PostStepGetPhysicalInteractionLength(const G4Track& track,
                                     G4double ,
                                     G4ForceCondition* condition)
{
  // condition is set to "Not Forced"
  *condition = NotForced;
  // default proposed step
  G4double     proposedStep = DBL_MAX;

  double steplenght = track.GetStepLength();
  int stepn = track.GetCurrentStepNumber();
  double xpos=track.GetPosition().x();
  double ypos=track.GetPosition().y();
  double zpos=track.GetPosition().z();
  
  
  if (stepn==1)
    {
      m_counter=0;
    }
  else if (steplenght < m_minstep) m_counter++;

  if (m_counter>m_maxstepnumber || 
      xpos>m_xmax || xpos<m_xmin ||
      ypos>m_ymax || ypos<m_ymin ||
      zpos>m_zmax || zpos<m_zmin) 
    {          
      proposedStep = 0.;
    } 
  
  return proposedStep;    
}
  
