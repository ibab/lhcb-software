// ============================================================================
// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
// GiGa
#include "GiGa/GiGaMACROs.h"
// G4 
#include "G4ProcessManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4NeutrinoE.hh"
#include "G4AntiNeutrinoE.hh"
#include "G4OpticalPhoton.hh"

// local
#include "EMPhysics.h"

/** @file 
 * 
 *  implementation of class EMPhysics
 *
 *  @author Witek Pokorski Witold.Pokorski@cern.ch
 *  reusing code by J.P. Wellisch
 */

// ============================================================================
/// Factory
// ============================================================================
IMPLEMENT_GiGaFactory( EMPhysics) ;
// ============================================================================

EMPhysics::EMPhysics
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaPhysConstructorBase (type, name, parent)
{
};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
EMPhysics::~EMPhysics(){};
// ============================================================================

// ============================================================================
// ============================================================================
void EMPhysics::ConstructParticle()
{
  G4Gamma::GammaDefinition();
  G4Electron::ElectronDefinition();
  G4Positron::PositronDefinition();
  G4NeutrinoE::NeutrinoEDefinition();
  G4AntiNeutrinoE::AntiNeutrinoEDefinition();
  G4OpticalPhoton::OpticalPhotonDefinition();
};

// ============================================================================
// ============================================================================
void EMPhysics::ConstructProcess()
{
  theEMPhysics.Build();
};

// ============================================================================
// The END 
// ============================================================================
