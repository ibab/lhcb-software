// $Id: SpecialCuts.cpp,v 1.8 2009-04-05 17:34:01 gcorti Exp $
// Include files 
// Geant4
#include "G4VParticleChange.hh"
#include "G4Track.hh"
#include "G4Step.hh"

// local
#include "SpecialCuts.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SpecialCuts
//
// 2003-04-11 : Witold POKORSKI
// 2009-03-26 : Gloria CORTI
//-----------------------------------------------------------------------------

//=============================================================================
// Constructor, initializes variables
//=============================================================================
GiGa::SpecialCuts::SpecialCuts(const G4String& aName)
  : G4VProcess(aName) {

  if (verboseLevel>1) {
    std::cout << GetProcessName() << " is created "<< std::endl;
  }
}

//=============================================================================
// Destructor
//=============================================================================
GiGa::SpecialCuts::~SpecialCuts() {}


//=============================================================================
// Implementation of cut methods: PostStepDoIt
//  set particle energy to zero
//  deposit all its kinetic energy at the point
//  stop and kill the particle to prevent it from decaying,
//=============================================================================
G4VParticleChange* GiGa::SpecialCuts::PostStepDoIt( const G4Track& aTrack,
                                                    const G4Step& ) {

   aParticleChange.Initialize(aTrack);
   aParticleChange.ProposeEnergy(0.) ;
   aParticleChange.ProposeLocalEnergyDeposit(aTrack.GetKineticEnergy()) ;
   aParticleChange.ProposeTrackStatus(fStopAndKill);
   return &aParticleChange;

}

//=============================================================================
// Implementation of cut methods: PostStepGetPhysicalInteractionLength
//   return max possible step, likely to be re-implemented by derived classes
//=============================================================================
G4double GiGa::SpecialCuts::PostStepGetPhysicalInteractionLength( 
  const G4Track& /* track            */ ,
  G4double          /* previousStepSize */ ,
  G4ForceCondition* /* condition        */ )
{
  return DBL_MAX;
}

//=============================================================================
