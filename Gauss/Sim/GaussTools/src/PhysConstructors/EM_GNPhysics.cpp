// ============================================================================
// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
// GiGa
#include "GiGa/GiGaMACROs.h"
// G4 
#include "g4std/iomanip"   
#include "G4ProcessManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4NeutrinoE.hh"
#include "G4AntiNeutrinoE.hh"

// local
#include "EM_GNPhysics.h"

/** @file 
 * 
 *  implementation of class EM_GNPhysics
 *
 *  @author Witek Pokorski Witold.Pokorski@cern.ch
 */

// ============================================================================
/// Factory
// ============================================================================
IMPLEMENT_GiGaFactory( EM_GNPhysics) ;
// ============================================================================

EM_GNPhysics::EM_GNPhysics
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
EM_GNPhysics::~EM_GNPhysics(){};
// ============================================================================

// ============================================================================
// ============================================================================
void EM_GNPhysics::ConstructParticle()
{
  G4Gamma::GammaDefinition();
  G4Electron::ElectronDefinition();
  G4Positron::PositronDefinition();
  G4NeutrinoE::NeutrinoEDefinition();
  G4AntiNeutrinoE::AntiNeutrinoEDefinition();
};

// ============================================================================
// ============================================================================
void EM_GNPhysics::ConstructProcess()
{
  theEMPhysics.Build();
  theGNPhysics.Build();
};

// ============================================================================
// The END 
// ============================================================================
