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
#include "G4Decay.hh"
#include "G4ios.hh"
#include "g4std/iomanip"                
// local
#include "GiGaPhysConstructorOp.h"
#include "RichPhotoElectron.h"
#include "G4Electron.hh"
#include "G4Transportation.hh"
#include "G4MultipleScattering.hh"
#include "G4ProcessVector.hh"
#include "G4LossTableManager.hh"


// ============================================================================
/// Factory
// ============================================================================
IMPLEMENT_GiGaFactory( GiGaPhysConstructorOp ) ;
// ============================================================================

// ============================================================================
GiGaPhysConstructorOp::GiGaPhysConstructorOp
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaPhysConstructorBase( type , name , parent )
{};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GiGaPhysConstructorOp::~GiGaPhysConstructorOp(){};
// ============================================================================

// ============================================================================
// ============================================================================
void GiGaPhysConstructorOp::ConstructParticle()
{
  //   RichPhotoElectron::PhotoElectronDefinition(); 
};

// ============================================================================
// ============================================================================
void GiGaPhysConstructorOp::ConstructProcess()
{
  // ConstructPeProcess();
  ConstructOp();

};

void  GiGaPhysConstructorOp::ConstructPeProcess() 
{
  //        G4double aPeCut=10.0*km;
       G4double aPeCut=0.1*mm;
       G4ParticleDefinition* photoelectronDef = 
                RichPhotoElectron::PhotoElectron();
       photoelectronDef->SetCuts(aPeCut);
       photoelectronDef->SetApplyCutsFlag(true);
       photoelectronDef-> DumpTable() ;

       G4Transportation* theTransportationProcess= new G4Transportation();
       G4MultipleScattering* theMultipleScattering = new G4MultipleScattering();
       theParticleIterator->reset();
      while( (*theParticleIterator)() ){
       G4ParticleDefinition* particle = theParticleIterator->value();
      if(  particle->GetParticleName() == "pe-" ){
       G4ProcessManager* pmanager =  particle->GetProcessManager();
       pmanager ->AddProcess(theTransportationProcess,-1,2,2);
       pmanager->AddProcess(theMultipleScattering,-1,1,1);
       pmanager ->SetProcessOrderingToFirst
                       (theTransportationProcess, idxAlongStep);
       pmanager ->SetProcessOrderingToFirst
                        (theTransportationProcess, idxPostStep);
        pmanager ->SetProcessOrderingToFirst
                         (theMultipleScattering, idxAlongStep);
       pmanager ->SetProcessOrderingToFirst(theMultipleScattering, idxPostStep);
      particle->SetCuts(aPeCut);
       particle->SetApplyCutsFlag(true);
       //       BuildPhysicsTable(particle);
       //  G4LossTableManager* aG4LossTableManager = G4LossTableManager::Instance();
       // aG4LossTableManager->BuildPhysicsTable(particle);

      G4int j;
     // Rebuild the physics tables for every process for this particle type
     G4ProcessVector* pVector = 
                         (particle->GetProcessManager())->GetProcessList();
     G4cout<<"size ProcList pe- "<< pVector->size()<< G4endl;
 
     for ( j=0; j < pVector->size(); ++j) {
      (*pVector)[j]->BuildPhysicsTable(*particle);
     }

       particle->DumpTable();
        pmanager->DumpInfo();
       G4int  an1 =  pmanager ->GetProcessListLength() ;
       G4cout<<"NUm proc for pe so far = "<< an1<<G4endl;

      }
      }

      //       G4ProcessManager* pmanager =  photoelectronDef->GetProcessManager();
       //   G4ProcessManager* pmanager =  new  G4ProcessManager( photoelectronDef);
       // photoelectronDef->SetProcessManager( pmanager);

       //   pmanager->SetParticleType(photoelectronDef);
      //test by SE
      //   G4ParticleDefinition* aph =  pmanager->GetParticleType();
      // G4String aPhname = aph-> GetParticleName();
      // G4cout<<"Name of photoelectron "<<aPhname <<G4endl;
       //       G4ParticleDefinition* bel = G4Electron::Electron();
       //  G4ProcessManager* bpman =  bel->GetProcessManager(); 
       // bpman ->DumpInfo();
      // end of test by SE   

       //       AddTransportation();

      //       G4Transportation* theTransportationProcess= new G4Transportation();
      // pmanager ->AddProcess(theTransportationProcess);
      //  pmanager ->SetProcessOrderingToFirst(theTransportationProcess, idxAlongStep);
      //  pmanager ->SetProcessOrderingToFirst(theTransportationProcess, idxPostStep);
      
      // pmanager->DumpInfo();
      //       G4int  an1 =  pmanager ->GetProcessListLength() ;
      //  G4cout<<"NUm proc for pe so far = "<< an1<<G4endl;
}

// ============================================================================
#include "RichG4Cerenkov.hh"
#include "G4OpAbsorption.hh"
#include "RichG4OpRayleigh.hh"
#include "RichG4OpBoundaryProcess.hh"
#include "RichHpdPhotoElectricEffect.h"
// ============================================================================
void GiGaPhysConstructorOp::ConstructOp() {

  G4cout<<"Now creating Optical processes"<<G4endl;
  G4cout<<"This is the phys list Op from GiGaRich1 "<<G4endl;
  RichG4Cerenkov*   theCerenkovProcess = new RichG4Cerenkov("RichG4Cerenkov");
  G4OpAbsorption* theAbsorptionProcess = new G4OpAbsorption();
  RichG4OpRayleigh*   theRayleighScatteringProcess = new RichG4OpRayleigh();
  RichG4OpBoundaryProcess* theBoundaryProcess = new RichG4OpBoundaryProcess();

  G4cout<<"Now creating Photoelectric  processes"<<G4endl;
  RichHpdPhotoElectricEffect* theRichHpdPhotoElectricProcess= 
    new RichHpdPhotoElectricEffect(this,"RichHpdPhotoelectricProcess");

  theCerenkovProcess->SetVerboseLevel(0);
  theAbsorptionProcess->SetVerboseLevel(0);
  theRayleighScatteringProcess->SetVerboseLevel(0);
  theBoundaryProcess->SetVerboseLevel(0);
  //  G4int MaxNumPhotons = 300;
  G4int MaxNumPhotons = 900;
  theCerenkovProcess->SetTrackSecondariesFirst(true);
  theCerenkovProcess->SetMaxNumPhotonsPerStep(MaxNumPhotons);
  G4OpticalSurfaceModel themodel = glisur;
  theBoundaryProcess->SetModel(themodel);
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    // Avoid the cherenkov preocess for the photoelectron.
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    if(particleName != "pe-" ){
    if (theCerenkovProcess->IsApplicable(*particle)) {
      pmanager->AddContinuousProcess(theCerenkovProcess);
    }
    }
    //    G4cout<<"Particle name  "<<particleName<<endl;
     if (particleName == "opticalphoton") {
      G4cout << " AddDiscreteProcess to OpticalPhoton " << G4endl;
      pmanager->AddDiscreteProcess(theAbsorptionProcess);
      pmanager->AddDiscreteProcess(theRayleighScatteringProcess);
      pmanager->AddDiscreteProcess(theBoundaryProcess);
      pmanager->AddDiscreteProcess(theRichHpdPhotoElectricProcess);

     }
  }
}

// void GiGaPhysConstructorOp::SetCuts() {
  //       G4double apeCut = 10.0*km;
  //     SetCutValue(apeCut,"pe-");
  
// }
// ============================================================================
// The END 
// ============================================================================
