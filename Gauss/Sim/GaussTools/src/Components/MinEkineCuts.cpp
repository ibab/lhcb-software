// --------------------------------------------------------------

#include "MinEkineCuts.h"

#include "G4Step.hh"
#include "G4UserLimits.hh"
#include "G4VParticleChange.hh"
#include "G4EnergyLossTables.hh"

MinEkineCuts::MinEkineCuts(const G4String& aName, double cut)
  : SpecialCuts(aName)
{ 
  m_cut=cut;
  
  if (verboseLevel>1) {
    G4cout << GetProcessName() << " is created "<< G4endl;
  }
  SetProcessType(fUserDefined);
}

MinEkineCuts::~MinEkineCuts()
{}

MinEkineCuts::MinEkineCuts(MinEkineCuts& right)
  :SpecialCuts(right)
{}

 
G4double MinEkineCuts::
PostStepGetPhysicalInteractionLength(const G4Track& aTrack,
                                     G4double ,
                                     G4ForceCondition* condition)
{
  // condition is set to "Not Forced"
  *condition = NotForced;
  // default proposed step
  G4double     proposedStep = DBL_MAX;
  // default cut
  G4double eMin=m_cut;
  
  const G4DynamicParticle* aParticle = aTrack.GetDynamicParticle();
  G4ParticleDefinition* aParticleDef = aTrack.GetDefinition();
  
  // kinetic energy
  G4double eKine = aParticle->GetKineticEnergy();
  
  // get the pointer to UserLimits
  G4UserLimits* pUserLimits = aTrack.GetVolume()
    ->GetLogicalVolume()->GetUserLimits();

  // if UserLimits attached to the volume, use them
  // if not use the default cuts
  if (pUserLimits)
    {      
      G4double eMin = pUserLimits->GetUserMinEkine(aTrack);
    }

  if (eKine < eMin ) 
    {          
      proposedStep = 0.;
    } 
  else if (aParticleDef->GetPDGCharge() != 0.0 
           && G4EnergyLossTables::GetRangeTable(aParticleDef)) 
    {
      // charged particles only       
      G4double temp = DBL_MAX;
      G4double rangeNow = DBL_MAX;
      
      G4Material* aMaterial = aTrack.GetMaterial();          
      rangeNow = G4EnergyLossTables::GetRange(aParticleDef,eKine,aMaterial);
      G4double rangeMin = 
        G4EnergyLossTables::GetRange(aParticleDef,eMin,aMaterial);
      temp = rangeNow - rangeMin;
      if (proposedStep > temp) proposedStep = temp;        
    }
  return proposedStep;    
}
  
