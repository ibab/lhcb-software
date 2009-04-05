// $Id: MinEkineCuts.cpp,v 1.7 2009-04-05 17:35:03 gcorti Exp $
// Include files 

// from G4
#include "G4Step.hh"
#include "G4UserLimits.hh"
#include "G4VParticleChange.hh"
#include "G4EnergyLossTables.hh"

// local
#include "MinEkineCuts.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MinEkineCuts
//
// 2003-04-11 : Witek POKORSKI
// 2009-04-01 : Gloria CORTI, adapt to LHCb template
//-----------------------------------------------------------------------------

//=============================================================================
// Constructor, initializes variables
//=============================================================================
GiGa::MinEkineCuts::MinEkineCuts(const G4String& aName, double cut)
  : SpecialCuts(aName)
{ 
  m_cut=cut;
  
  if (verboseLevel>1) {
    std::cout << GetProcessName() << " is created "<< std::endl;
  }
  SetProcessType(fUserDefined);
}

//=============================================================================
// Destructor
//=============================================================================
GiGa::MinEkineCuts::~MinEkineCuts() {}

//=============================================================================
// Private assignment operator
//=============================================================================
GiGa::MinEkineCuts::MinEkineCuts(GiGa::MinEkineCuts& right) 
  : SpecialCuts(right) {}

 
//=============================================================================
// Implementation of process methods: PostStepGetPhysicalInteractionLength
//=============================================================================
G4double GiGa::MinEkineCuts::
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
  if (pUserLimits) {
      eMin = pUserLimits->GetUserMinEkine(aTrack);
  }
  
  if (eKine < eMin ) {
      proposedStep = 0.;
  } 
  else if (aParticleDef->GetPDGCharge() != 0.0 
           && G4EnergyLossTables::GetRangeTable(aParticleDef)) {

    // charged particles only       
    G4double temp = DBL_MAX;
    G4double rangeNow = DBL_MAX;
      
    const G4MaterialCutsCouple* aMaterial= aTrack.GetMaterialCutsCouple();          
    rangeNow = G4EnergyLossTables::GetRange(aParticleDef,eKine,aMaterial);
    G4double rangeMin = 
      G4EnergyLossTables::GetRange(aParticleDef,eMin,aMaterial);
    temp = rangeNow - rangeMin;
    if (proposedStep > temp) proposedStep = temp;        
  }

  return proposedStep;    
}
  
//=============================================================================
