// ============================================================================
// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
// GiGa
#include "GiGa/GiGaMACROs.h"
// local
#include "GeneralPhysics.h"

// ============================================================================
/// Factory
// ============================================================================
IMPLEMENT_GiGaFactory( GeneralPhysics ) ;
// ============================================================================

// ============================================================================
GeneralPhysics::GeneralPhysics
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaPhysConstructorBase( type , name , parent )
{};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GeneralPhysics::~GeneralPhysics(){};
// ============================================================================

// ============================================================================
// ============================================================================

#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
// Bosons
#include "G4ChargedGeantino.hh"
#include "G4Geantino.hh"

void GeneralPhysics::ConstructParticle()
{
  // pseudo-particles
  G4Geantino::GeantinoDefinition();
  G4ChargedGeantino::ChargedGeantinoDefinition();  
};

// ============================================================================
// ============================================================================
void GeneralPhysics::ConstructProcess()
{  
  // Add Decay Process
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    if (fDecayProcess.IsApplicable(*particle)) { 
      pmanager ->AddProcess(&fDecayProcess);
      // set ordering for PostStepDoIt and AtRestDoIt
      pmanager ->SetProcessOrdering(&fDecayProcess, idxPostStep);
      pmanager ->SetProcessOrdering(&fDecayProcess, idxAtRest);
    }
  }
};


// ============================================================================
// The END 
// ============================================================================
