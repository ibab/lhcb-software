// ============================================================================
// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
// GiGa
#include "GiGa/GiGaMACROs.h"
// G4 
#include "G4ParticleTypes.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleWithCuts.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4ParticleTable.hh"
#include "G4Material.hh"
#include "G4ios.hh"
#include "g4std/iomanip"                
// local
#include "GiGaPhysConstructorHpd.h"

// ============================================================================
/// Factory
// ============================================================================
IMPLEMENT_GiGaFactory( GiGaPhysConstructorHpd ) ;
// ============================================================================

// ============================================================================
GiGaPhysConstructorHpd::GiGaPhysConstructorHpd
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaPhysConstructorBase( type , name , parent )
{};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GiGaPhysConstructorHpd::~GiGaPhysConstructorHpd(){};
// ============================================================================

// ============================================================================
// ============================================================================
void GiGaPhysConstructorHpd::ConstructParticle()
{
 
};

// ============================================================================
// ============================================================================
void GiGaPhysConstructorHpd::ConstructProcess()
{  
  ConstructHpdSiEnLoss();
};

//#include "G4Decay.hh"
#include "RichHpdSiEnergyLoss.h"

void GiGaPhysConstructorHpd::ConstructHpdSiEnLoss()
{
  // Add Decay Process
  //  G4Decay* theDecayProcess = new G4Decay();

  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();

    RichHpdSiEnergyLoss* theRichHpdSiEnergyLossProcess =
      new RichHpdSiEnergyLoss("RichHpdSiEnergyLossProcess");
    
    pmanager->AddProcess( theRichHpdSiEnergyLossProcess ,-1,2,2);

    //    if (theDecayProcess->IsApplicable(*particle)) { 
    //      pmanager ->AddProcess(theDecayProcess);
      // set ordering for PostStepDoIt and AtRestDoIt
    //      pmanager ->SetProcessOrdering(theDecayProcess, idxPostStep);
    //     pmanager ->SetProcessOrdering(theDecayProcess, idxAtRest);
    //}
  }
}

// ============================================================================
// The END 
// ============================================================================
