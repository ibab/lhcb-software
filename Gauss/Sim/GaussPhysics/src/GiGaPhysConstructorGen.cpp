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
#include "G4ShortLivedConstructor.hh" 
// local
#include "GiGaPhysConstructorGen.h"

// ============================================================================
/// Factory
// ============================================================================
IMPLEMENT_GiGaFactory( GiGaPhysConstructorGen ) ;
// ============================================================================

// ============================================================================
GiGaPhysConstructorGen::GiGaPhysConstructorGen
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaPhysConstructorBase( type , name , parent )
{};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GiGaPhysConstructorGen::~GiGaPhysConstructorGen(){};
// ============================================================================

// ============================================================================
// ============================================================================
void GiGaPhysConstructorGen::ConstructParticle()
{
  ConstructAllShortLiveds();
};

// ============================================================================
// ============================================================================
void GiGaPhysConstructorGen::ConstructAllShortLiveds()
{
  //  Construct  resonaces and quarks
  G4ShortLivedConstructor pConstructor;
  pConstructor.ConstructParticle();  
};

// ============================================================================
// ============================================================================
void GiGaPhysConstructorGen::ConstructProcess()
{  
  ConstructGeneral();
};

#include "G4Decay.hh"
//#include "RichHpdSiEnergyLoss.h"

void GiGaPhysConstructorGen::ConstructGeneral()
{
  // Add Decay Process
  G4Decay* theDecayProcess = new G4Decay();
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();

    //    RichHpdSiEnergyLoss* theRichHpdSiEnergyLossProcess =
    //      new RichHpdSiEnergyLoss("RichHpdSiEnergyLossProcess");    
    //    pmanager->AddProcess( theRichHpdSiEnergyLossProcess ,-1,2,2);

    if (theDecayProcess->IsApplicable(*particle)) { 
      pmanager ->AddProcess(theDecayProcess);
      // set ordering for PostStepDoIt and AtRestDoIt
      pmanager ->SetProcessOrdering(theDecayProcess, idxPostStep);
      pmanager ->SetProcessOrdering(theDecayProcess, idxAtRest);
    }
  }
}

// ============================================================================
// The END 
// ============================================================================
