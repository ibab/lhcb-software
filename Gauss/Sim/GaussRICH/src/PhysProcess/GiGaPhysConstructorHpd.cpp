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
//#include "g4std/iomanip"                
// local
#include "GiGaPhysConstructorHpd.h"
#include "RichPhotoElectron.h"

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

  RichHpdSiEnergyLoss* theRichHpdSiEnergyLossProcess =
    new RichHpdSiEnergyLoss("RichHpdSiEnergyLossProcess");

  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    //    G4cout<<"ConstructHpdSiEnLoss: Now at particle:  "<< particle->GetParticleName()<<G4endl;
    
    pmanager->SetVerboseLevel(0);
    
    if( theRichHpdSiEnergyLossProcess->IsApplicable(*particle) ) 
      {
        //    RichHpdSiEnergyLoss* theRichHpdSiEnergyLossProcess =
        //  new RichHpdSiEnergyLoss("RichHpdSiEnergyLossProcess");
        //  G4cout<<"Now HpdSiLoss adding for "<< particle->GetParticleName()<<G4endl;
        
        if( ( particle->GetParticleName() == "pe-") || ( particle->GetParticleName() =="e-" ))
          {
            //            G4cout<<"Now at particle "<< particle->GetParticleName()<<G4endl;
            pmanager->AddProcess( theRichHpdSiEnergyLossProcess ,-1,2,2);
          }
        
        if(particle->GetParticleName() == "pe-")
          {
            pmanager->DumpInfo();  
            
            (RichPhotoElectron::PhotoElectron())->SetProcessManager(pmanager);
            (RichPhotoElectron::PhotoElectron())->GetProcessManager()->DumpInfo();
          }
      }
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
