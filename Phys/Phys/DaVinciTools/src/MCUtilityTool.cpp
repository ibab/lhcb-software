// $Id: MCUtilityTool.cpp,v 1.1 2002-02-08 18:25:42 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "LHCbEvent/MCParticle.h"
#include "LHCbEvent/MCVertex.h"

// from CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// local
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

  declareInterface<IMCUtilityTool>(this);

  declareProperty( "PrintDepth", m_depth = 999 );

}

//=============================================================================
// Standard destructor
//=============================================================================

MCUtilityTool::~MCUtilityTool( ) { }

//=============================================================================
// Print decay tree for a given particle
//=============================================================================
void MCUtilityTool::printDecayTree(long depth, const std::string& prefix,
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
      const SmartRefVector<MCParticle>& daughters = 
                                        (*ivtx)->daughterMCParticles();
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

//=============================================================================
// Match decay tree for a given particle
//=============================================================================
bool MCUtilityTool::matchDecayTree( const MCParticle* mother,
                                    const std::vector<long> daughtersID )  {

  bool result = false;
  const SmartRefVector<MCVertex>& decays = mother->decayMCVertices();
  SmartRefVector<MCVertex>::const_iterator ivtx;
  for ( ivtx = decays.begin(); ivtx != decays.end(); ivtx++ ) {
    const SmartRefVector<MCParticle>& daughters =
      (*ivtx)->daughterMCParticles();
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

//=============================================================================
// Return the ParticlePropSvc stored
//=============================================================================
IParticlePropertySvc* MCUtilityTool::ppSvc( ) {
  return m_ppSvc;
}


//=============================================================================
// Return the printing depth
//=============================================================================
long MCUtilityTool::printingDepth( ) {
  return m_depth;
}

//=============================================================================
// Return list of parentsIDs
//=============================================================================
StatusCode MCUtilityTool::history( const MCParticle* product,
                                   std::vector<long>& parentsID ) {

  const SmartRef<MCVertex>& vorigin = product->originMCVertex();
  // A particle should always have an origin vertex
  // BUT careful muon background particles do not
  if ( !vorigin ) {
    int npar = parentsID.size();
    if ( 0 == npar ) {
      return StatusCode::FAILURE;
    } else {
      return StatusCode::SUCCESS;
    }
  }

  // Now get the particle that produced this vertex
  // Again all vertices whould have a parent particle BUT the primary does not
  const SmartRef<MCParticle>& mother = vorigin->motherMCParticle();
  if ( !mother ) {
    int npar = parentsID.size();
    if ( 0 == npar ) {
      return StatusCode::FAILURE;
    } else {
      return StatusCode::SUCCESS;
    }
  }

  // Store the mother particleID in the vector of ancestors
  parentsID.push_back( mother->particleID().id() );
  StatusCode sc = history( mother, parentsID );

  return sc;

}

//=============================================================================
// Print history of a particle
//=============================================================================
void MCUtilityTool::printHistory( const MCParticle* product ) {
  
  MsgStream log(msgSvc(), name());

  // Retrieve ancestors
  std::vector<long> ancestors;      
  StatusCode sc = this->history(product,ancestors);
  if ( sc.isSuccess() ) {
    ParticleProperty* p = ppSvc()->find((product)->particleID().id());
    log << MSG::INFO << p->particle();
    for ( unsigned long i=0; i<ancestors.size(); i++ ) {
      p = m_ppSvc->find(ancestors[i]);
      log << MSG::INFO << " <--- " << p->particle();
    }
    log << MSG::INFO << endreq;
  }
  
}

  
