// $Id: LoopCuts.cpp,v 1.6 2009-04-05 17:31:55 gcorti Exp $
// Include files 

// from G4
#include "G4Step.hh"
#include "G4UserLimits.hh"
#include "G4VParticleChange.hh"

// local
#include "LoopCuts.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LoopCuts
//
// 2003-06-17 : Witek POKORSKI
// 2009-03-26 : Gloria CORTI, clean up
//-----------------------------------------------------------------------------

//=============================================================================
// Constructor, initializes variables
//=============================================================================
GiGa::LoopCuts::LoopCuts(const G4String& aName, int maxs, double minstep)
  : GiGa::SpecialCuts(aName)
{ 

  m_maxstepnumber = maxs;
  m_minstep = minstep;
  
  if (verboseLevel>1) {
    std::cout << GetProcessName() << " is created "<< std::endl;
  }
  SetProcessType(fUserDefined);
}

//=============================================================================
// Destructor
//=============================================================================
GiGa::LoopCuts::~LoopCuts() {}

//=============================================================================
// Private assignment operator
//=============================================================================
GiGa::LoopCuts::LoopCuts(GiGa::LoopCuts& right) : SpecialCuts(right) {}

 
//=============================================================================
// Implementation of process methods: PostStepGetPhysicalInteractionLength
//=============================================================================
G4double GiGa::LoopCuts::
PostStepGetPhysicalInteractionLength(const G4Track& track,
                                     G4double ,
                                     G4ForceCondition* condition)
{
  // condition is set to "Not Forced"
  *condition = NotForced;

  // default proposed step
  G4double proposedStep = DBL_MAX;

  double steplenght = track.GetStepLength();
  int stepn = track.GetCurrentStepNumber();  
  
  // Check number of steps less of lenght less than min
  if (stepn==1) {
      m_counter=0;
  } else if (steplenght < m_minstep) {
    m_counter++;
  }
  
  if (m_counter > m_maxstepnumber) {          
    proposedStep = 0.;
  } 
  
  return proposedStep;    
}

//=============================================================================
// Overrides default behaviour of SpecialCuts to be able to leave
// the particle alive for subsequent decays
//=============================================================================
G4VParticleChange* GiGa::LoopCuts::PostStepDoIt( const G4Track& aTrack,
                                           const G4Step& ) {

   aParticleChange.Initialize(aTrack);
   aParticleChange.ProposeEnergy(0.) ;
   aParticleChange.ProposeLocalEnergyDeposit(aTrack.GetKineticEnergy()) ;
   aParticleChange.ProposeTrackStatus(fStopButAlive);
   return &aParticleChange;
}

//=============================================================================

