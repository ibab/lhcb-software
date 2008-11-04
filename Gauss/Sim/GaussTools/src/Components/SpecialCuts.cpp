// ------------------------------------------------------------

#include "SpecialCuts.h"
#include "G4VParticleChange.hh"
#include "G4Track.hh"
#include "G4Step.hh"

SpecialCuts::SpecialCuts(const G4String& aName)
  : G4VProcess(aName)
{
  if (verboseLevel>1) {
    std::cout << GetProcessName() << " is created "<< std::endl;
  }
}

SpecialCuts::~SpecialCuts()  { }

G4VParticleChange* SpecialCuts::PostStepDoIt( const G4Track& aTrack,
					      const G4Step& ) {
   aParticleChange.Initialize(aTrack);
   aParticleChange.ProposeEnergy(0.) ;
   aParticleChange.ProposeLocalEnergyDeposit(aTrack.GetKineticEnergy()) ;
   // By default kill the particle to prevent it from decaying
   aParticleChange.ProposeTrackStatus(fStopAndKill);
   return &aParticleChange;
}

G4double SpecialCuts::PostStepGetPhysicalInteractionLength
( const G4Track&    /* track            */ ,
  G4double          /* previousStepSize */ ,
  G4ForceCondition* /* condition        */ )
{
  return DBL_MAX;
}

