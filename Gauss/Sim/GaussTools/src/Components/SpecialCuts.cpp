// ------------------------------------------------------------

#include "SpecialCuts.h"
#include "G4VParticleChange.hh"
#include "G4Track.hh"
#include "G4Step.hh"

SpecialCuts::SpecialCuts(const G4String& aName)
  : G4VProcess(aName)
{
   if (verboseLevel>1) {
     G4cout << GetProcessName() << " is created "<< G4endl;
   }
}

SpecialCuts::~SpecialCuts() 
{                                     
}                                     

#include "G4ParticleDefinition.hh"
G4VParticleChange* SpecialCuts::PostStepDoIt(
			     const G4Track& aTrack,
			     const G4Step& 
			    )
//
// Stop the current particle, if requested by G4UserLimits 
// 			    			    			    
{
  //  cout << " killing particle " << aTrack.GetDefinition()->GetParticleName() << endl;
   aParticleChange.Initialize(aTrack);
   aParticleChange.SetEnergyChange(0.) ;
   aParticleChange.SetLocalEnergyDeposit (aTrack.GetKineticEnergy()) ;
   aParticleChange.SetStatusChange(fStopButAlive);
   return &aParticleChange;
}

G4double SpecialCuts::PostStepGetPhysicalInteractionLength(
                             const G4Track& track,
                             G4double   previousStepSize,
                             G4ForceCondition* condition
                            )
{
  return DBL_MAX;
}

