// $Id: WorldCuts.cpp,v 1.3 2009-04-05 17:33:20 gcorti Exp $
// Include files 

// from G4
#include "G4Step.hh"
#include "G4UserLimits.hh"
#include "G4VParticleChange.hh"

// local
#include "WorldCuts.h"


//-----------------------------------------------------------------------------
// Implementation file for class : WorldCuts
//
// 2003-07-28 : Witek POKORSKI
// 2009-03-26 : Gloria CORTI, adapt to LHCb template
//-----------------------------------------------------------------------------

//=============================================================================
// Constructor, initializes variables
//=============================================================================
GiGa::WorldCuts::WorldCuts(const G4String& aName, double xmin, double ymin, 
                     double zmin, double xmax, double ymax, double zmax)
  : SpecialCuts(aName)
{  
  m_xmin = xmin;
  m_ymin = ymin;
  m_zmin = zmin;
  m_xmax = xmax;
  m_ymax = ymax;
  m_zmax = zmax;
  
  if (verboseLevel>1) {
    std::cout << GetProcessName() << " is created "<< std::endl;
  }
  SetProcessType(fUserDefined);
}

//=============================================================================
// Destructor
//=============================================================================
GiGa::WorldCuts::~WorldCuts() {}

//=============================================================================
// Private assignment operator
//=============================================================================
GiGa::WorldCuts::WorldCuts(GiGa::WorldCuts& right) : SpecialCuts(right) {}

 
//=============================================================================
// Implementation of process methods: PostStepGetPhysicalInteractionLength
//=============================================================================
G4double GiGa::WorldCuts::
PostStepGetPhysicalInteractionLength(const G4Track& track,
                                     G4double ,
                                     G4ForceCondition* condition)
{

  // condition is set to "Not Forced"
  *condition = NotForced;

  // default proposed step
  G4double proposedStep = DBL_MAX;
  
  double xpos = track.GetPosition().x();
  double ypos = track.GetPosition().y();
  double zpos = track.GetPosition().z();  
  
  if( xpos>m_xmax || xpos<m_xmin ||
      ypos>m_ymax || ypos<m_ymin ||
      zpos>m_zmax || zpos<m_zmin) {          
    proposedStep = 0.;
  } 
  
  return proposedStep;    
}

//=============================================================================
