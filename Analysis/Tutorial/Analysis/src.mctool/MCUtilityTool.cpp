// $Id: MCUtilityTool.cpp,v 1.1.1.1 2001-10-19 17:06:05 gcorti Exp $
// Framework include files
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"

// Using Particle properties
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// Accessing data:
#include "CLHEP/Units/PhysicalConstants.h"
#include "LHCbEvent/MCParticle.h"

// Tool example
#include "MCUtilityTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCUtilityTool
//
// 14/10/2001 : Gloria Corti
//-----------------------------------------------------------------------------

// Declaration of the AlgTool Factory
static ToolFactory<MCUtilityTool>          Factory;
const IToolFactory& MCUtilityToolFactory = Factory;

//=============================================================================
// Standard creator, initializes variables
//=============================================================================
MCUtilityTool::MCUtilityTool( const std::string& type,
                              const std::string& name, 
                              const IInterface* parent )
  : AlgTool( type, name, parent ),
  m_ppSvc(0)  {

  if( serviceLocator() ) {
    StatusCode sc = StatusCode::FAILURE;
    sc = serviceLocator()->service("ParticlePropertySvc",m_ppSvc);
  }
  if( !m_ppSvc ) {
    throw GaudiException( "ParticlePropertySvc not found", 
                          "MCUtilityToolException",
                          StatusCode::FAILURE );
  }

  declareProperty( "PrintDepth", m_depth = 999 );
                         
}
//=============================================================================
// Standard destructor
//=============================================================================

MCUtilityTool::~MCUtilityTool( ) {   }


//=============================================================================
// Query the interface
//=============================================================================
StatusCode MCUtilityTool::queryInterface(const IID& riid, void** ppvInterface){
  if ( IID_IMCUtilityTool == riid )    {
    *ppvInterface = (IMCUtilityTool*)this;
  }
  else  {
    // Interface is not directly available: try the AlgTool base class
    return AlgTool::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

//=============================================================================
// Print decay tree for a given particle
//=============================================================================
void MCUtilityTool::printDecayTree(long depth, const std::string& prefix,
     const MCParticle* mother) {

  MsgStream log(msgSvc(), name());

  ParticleProperty* p = m_ppSvc->find( mother->particleID().id() );

  log << MSG::INFO << depth << prefix.substr(0, prefix.length()-1)
      << "+--->" << std::setw(12) << std::setiosflags(std::ios::left) 
      << p->particle()
      << " E:"   << std::setw(12) << mother->fourMomentum().e()/GeV
      << endreq;

  const SmartRefVector<MCVertex>& decays = mother->decayMCVertices();
  if ( depth < m_depth )  {
    SmartRefVector<MCVertex>::const_iterator ivtx;
    for ( ivtx = decays.begin(); ivtx != decays.end(); ivtx++ )  {
      const SmartRefVector<MCParticle>& daughters = (*ivtx)->daughterMCParticles();
      SmartRefVector<MCParticle>::const_iterator idau;
      for ( idau   = daughters.begin(); idau != daughters.end(); idau++ )  {
        printDecayTree( depth+1, prefix+" |", *idau );
      }
    }
  }
}

//=============================================================================
// Match decay tree for a given particle
//=============================================================================
bool MCUtilityTool::matchDecayTree( const MCParticle* mother, 
                                    const std::vector<long> daughtersID )  {

  bool result = false;
  const SmartRefVector<MCVertex>& decays = mother->decayMCVertices();
  SmartRefVector<MCVertex>::const_iterator ivtx;
  for ( ivtx = decays.begin(); ivtx != decays.end(); ivtx++ ) {
    const SmartRefVector<MCParticle>& daughters = (*ivtx)->daughterMCParticles();
    if ( daughtersID.size() == daughters.size() ) {
      long nDecSize = daughtersID.size();
      SmartRefVector<MCParticle>::const_iterator idau;

      // Flag holding if a decay product has been found
      std::vector<bool> daugFound;      
      for ( long i = 0 ; i < nDecSize ; i++ ) {
        daugFound.push_back(false);
      }
      for ( idau = daughters.begin(); idau != daughters.end(); idau++ ) {
        long ipID = (*idau)->particleID().id();
        for ( long j = 0; j < nDecSize; j++ ) {
          if ( (!daugFound[j]) && (ipID == daughtersID[j]) ) {
            daugFound[j] = true;
            break;
          }
        }
        // break to here 
      }
      // Now check if all products have been found 
      long nFound = 0;
      for ( long k = 0; k < nDecSize; k++ ) {
        if ( daugFound[k] ) {
          nFound++;
        }
      }
      if ( nFound == nDecSize ) {
        result = true;
      }
    }
  }

  return result;
  
}
