// $Id: NewMCUtilityTool.cpp,v 1.1.1.1 2001-10-19 17:06:05 gcorti Exp $
// Framework include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolFactory.h"

// Using Particle properties
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// Accessing data:
#include "CLHEP/Units/PhysicalConstants.h"
#include "LHCbEvent/MCParticle.h"

// New Tool example
#include "NewMCUtilityTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : NewMCUtilityTool
//
// 18/10/2001 : Gloria Corti
//-----------------------------------------------------------------------------

// Declaration of the AlgTool Factory
static ToolFactory<NewMCUtilityTool>          Factory;
const IToolFactory& NewMCUtilityToolFactory = Factory;

//=============================================================================
// Standard creator, initializes variables
//=============================================================================
NewMCUtilityTool::NewMCUtilityTool( const std::string& type,
                                    const std::string& name, 
                                    const IInterface* parent )
  : MCUtilityTool( type, name, parent ) {

}

//=============================================================================
// Standard destructor
//=============================================================================

NewMCUtilityTool::~NewMCUtilityTool( ) {   }

//=============================================================================
// Print decay tree for a given particle
//=============================================================================
void NewMCUtilityTool::printDecayTree(long depth, const std::string& prefix,
     const MCParticle* mother) {

  MsgStream log(msgSvc(), name());

  ParticleProperty* p = ppSvc()->find( mother->particleID().id() );

  if( depth == 0 ) {
    log << MSG::INFO << "Name -> decay     M(GeV)    E(GeV)" 
        << "     Px(GeV)    Py(GeV)       Pz(GeV)     Vz(cm)"
        << endreq;
  }

  log << MSG::INFO << prefix.substr(0, prefix.length()-1)
      << std::setw(20) << p->particle()
      << std::setw(12) << std::setprecision(5) 
      << mother->fourMomentum().m()/GeV
      << std::setw(12) << std::setprecision(5)  
      << mother->fourMomentum().e()/GeV
      << std::setw(12) << std::setprecision(5) 
      << mother->fourMomentum().px()/GeV
      << std::setw(12) << std::setprecision(5) 
      << mother->fourMomentum().py()/GeV
      << std::setw(12) << std::setprecision(5) 
      << mother->fourMomentum().pz()/GeV
      << std::setw(12) << std::setprecision(5) 
      << (mother->originMCVertex())->position().z()/cm
      << endreq;

  const SmartRefVector<MCVertex>& decays = mother->decayMCVertices();
  if ( depth < printingDepth() )  {
    SmartRefVector<MCVertex>::const_iterator ivtx;
    for ( ivtx = decays.begin(); ivtx != decays.end(); ivtx++ )  {
      const SmartRefVector<MCParticle>& daughters = (*ivtx)->daughterMCParticles
();
      SmartRefVector<MCParticle>::const_iterator idau;
      int ndau = 0;
      std::string pref = " ";
      int nstep = prefix.length();
      nstep = (nstep-5)/4;
      std::string preft = "    ";
      for ( int i=1; i<=nstep; i++ ) {
        preft = preft+" |  ";
      }
      int mdau = daughters.size();
      for ( idau   = daughters.begin(); idau != daughters.end(); idau++ )  {
        ndau++;
        if ( ndau < mdau ) {
          pref = preft+" |-> ";
        }
        else {
          pref = preft+" +-> ";
        }
        printDecayTree( depth+1, pref, *idau );
      }
    }
  }
}
