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
#include "GiGaPhysConstructorOp.h"

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
 
};

// ============================================================================
// ============================================================================
void GiGaPhysConstructorOp::ConstructProcess()
{
  ConstructOp();
};

// ============================================================================
#include "G4Cerenkov.hh"
#include "G4OpAbsorption.hh"
#include "G4OpRayleigh.hh"
#include "G4OpBoundaryProcess.hh"
#include "RichHpdPhotoElectricEffect.h"
// ============================================================================
void GiGaPhysConstructorOp::ConstructOp() {

  G4cout<<"Now creating Optical processes"<<G4endl;
  G4cout<<"This is the phys list Op from GiGaRich1 "<<G4endl;
  G4Cerenkov*   theCerenkovProcess = new G4Cerenkov("Cerenkov");
  G4OpAbsorption* theAbsorptionProcess = new G4OpAbsorption();
  G4OpRayleigh*   theRayleighScatteringProcess = new G4OpRayleigh();
  G4OpBoundaryProcess* theBoundaryProcess = new G4OpBoundaryProcess();

  G4cout<<"Now creating Photoelectric  processes"<<endl;
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
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    if (theCerenkovProcess->IsApplicable(*particle)) {
      pmanager->AddContinuousProcess(theCerenkovProcess);
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

// ============================================================================
// The END 
// ============================================================================
