// ============================================================================
// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
// GiGa
#include "GiGa/GiGaMACROs.h"
// local
#include "HadronPhysicsLHEP.h"
// G4
#include "globals.hh"
#include "G4ios.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4ShortLivedConstructor.hh"

// ============================================================================
/// Factory
// ============================================================================
IMPLEMENT_GiGaFactory( HadronPhysicsLHEP) ;
// ============================================================================

HadronPhysicsLHEP::HadronPhysicsLHEP
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaPhysConstructorBase (type, name, parent)
{
  theNeutrons.RegisterMe(&theLHEPNeutron);
  thePro.RegisterMe(&theLHEPPro);
  thePiK.RegisterMe(&theLHEPPiK);
};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
HadronPhysicsLHEP::~HadronPhysicsLHEP(){};
// ============================================================================

void HadronPhysicsLHEP::ConstructParticle()
{
  G4MesonConstructor pMesonConstructor;
  pMesonConstructor.ConstructParticle();
  
  G4BaryonConstructor pBaryonConstructor;
  pBaryonConstructor.ConstructParticle();
  
  G4ShortLivedConstructor pShortLivedConstructor;
  pShortLivedConstructor.ConstructParticle();  
};

// ============================================================================
// ============================================================================
#include "G4ProcessManager.hh"

void HadronPhysicsLHEP::ConstructProcess()
{
  theNeutrons.Build();
  thePro.Build();
  thePiK.Build();
  theMiscLHEP.Build();
  theStoppingHadron.Build();
  theHadronQED.Build();
};
  

// ============================================================================
// The END 
// ============================================================================
