// ============================================================================
// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
// GiGa
#include "GiGa/GiGaMACROs.h"
// local
#include "HadronPhysicsLHEP_GN.h"
// G4
#include "globals.hh"
#include "G4ios.hh"
#include "g4std/iomanip" 

// ============================================================================
/// Factory
// ============================================================================
IMPLEMENT_GiGaFactory( HadronPhysicsLHEP_GN) ;
// ============================================================================

HadronPhysicsLHEP_GN::HadronPhysicsLHEP_GN
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaPhysConstructorBase (type, name, parent)
{
  theNeutrons.RegisterMe(&theLHEPNeutron);
  theProton.RegisterMe(&theLHEPProton);
  thePiK.RegisterMe(&theLHEPPiK);
};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
HadronPhysicsLHEP_GN::~HadronPhysicsLHEP_GN(){};
// ============================================================================

// ============================================================================
// ============================================================================
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4ShortLivedConstructor.hh"

void HadronPhysicsLHEP_GN::ConstructParticle()
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

void HadronPhysicsLHEP_GN::ConstructProcess()
{
  theNeutrons.Build();
  thePiK.Build();
  theProton.Build();
  theMiscLHEP.Build();
  theStoppingHadron.Build();
  theHadronQED.Build();
};
  

// ============================================================================
// The END 
// ============================================================================
