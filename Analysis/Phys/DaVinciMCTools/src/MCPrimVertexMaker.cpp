// $Id: MCPrimVertexMaker.cpp,v 1.7 2008-09-02 09:10:23 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/IRndmGenSvc.h"

// from LHCb
#include "Event/MCHeader.h" 
#include "Event/RecVertex.h"

#include "MCInterfaces/IVisPrimVertTool.h"
// local
#include "MCPrimVertexMaker.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCPrimVertexMaker
//
// 2006-05-03 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MCPrimVertexMaker )
//
using namespace Gaudi::Units;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCPrimVertexMaker::MCPrimVertexMaker( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_prods(0)
  , m_vtx(0)
  , m_evts(0)
  , m_vis(0)
  , m_visPV()
{
  declareProperty( "xSmear", m_xSmear = 0.1*mm ) ; // tuned to get 10 mum with 100 tracks
  declareProperty( "ySmear", m_ySmear = 0.1*mm ) ; // tuned to get 10 mum with 100 tracks
  declareProperty( "zSmear", m_zSmear = 0.4*mm ) ; // tuned to get 40 mum with 100 tracks
  declareProperty( "OnlyVisible", m_onlyVisible = true ) ;
  
}
//=============================================================================
// Destructor
//=============================================================================
MCPrimVertexMaker::~MCPrimVertexMaker() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode MCPrimVertexMaker::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  if ( m_onlyVisible ) {
    m_visPV = tool<IVisPrimVertTool>("VisPrimVertTool",this);
    info() << "Considering only visible PVs and tracks" << endmsg ;
  }
  IRndmGenSvc* r = svc<IRndmGenSvc>("RndmGenSvc",true);
  if( m_ranGauss.initialize(r,Rndm::Gauss(0,1)).isFailure()){
    fatal() << "Unable to locate RndmGenSvc Gauss distribution" << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MCPrimVertexMaker::execute() {

  debug() << "==> Execute" << endmsg;
  m_evts++;

  LHCb::RecVertices* primaries = new LHCb::RecVertices();

  LHCb::MCHeader* mch = get<LHCb::MCHeader>(LHCb::MCHeaderLocation::Default);
  for ( SmartRefVector< LHCb::MCVertex >::const_iterator ipv = mch->primaryVertices().begin() ;
        ipv != mch->primaryVertices().end() ; ++ipv ){
    m_vtx++;
    if ( !m_onlyVisible || m_visPV->isVisible(*ipv)){
      m_vis++;
      LHCb::RecVertex* newPV = new LHCb::RecVertex;
      StatusCode sc = makePV(*ipv,newPV);
      if (!sc) {
        delete primaries;
        return sc;  
      }
      
      primaries->insert(newPV);
    } 
  }
  put(primaries, LHCb::RecVertexLocation::Primary );
  debug() << "Storing " << primaries->size() << " PVs" << endmsg ;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  make PV
//=============================================================================
StatusCode MCPrimVertexMaker::makePV(const LHCb::MCVertex* mc, LHCb::RecVertex* pv){
  int ntk = 0 ;
  if ( m_onlyVisible ){
    ntk = m_visPV->countVisTracks(mc);
  } else {
    ntk = mc->products().size() ;
  }
  m_prods += ntk ;
  debug() << "PV is at " << mc->position() << " and has " << ntk << " tracks" << endmsg ;
  Gaudi::Vector3 smearV = Gaudi::Vector3( m_xSmear, m_ySmear, m_zSmear)/sqrt(float(ntk));
  Gaudi::Vector3 gauss(m_ranGauss.shoot(),m_ranGauss.shoot(),m_ranGauss.shoot()) ;
  Gaudi::XYZVector smearPV(smearV(0)*gauss(0),smearV(1)*gauss(1),smearV(2)*gauss(2));
  Gaudi::XYZPoint posPV = mc->position() + smearPV ;
  Gaudi::SymMatrix3x3 errPV;
  Gaudi::Vector3 smearV2 = Gaudi::Vector3( m_xSmear*m_xSmear, m_ySmear*m_ySmear, m_zSmear*m_zSmear)/float(ntk);
  errPV.SetDiagonal(smearV2);
  debug() << "Generated PV is smeared by " << smearPV << " to be at "<< posPV << endmsg ;
  pv->setPosition(posPV);
  pv->setCovMatrix(errPV);
  pv->setChi2(gauss(0)*gauss(0)+gauss(1)*gauss(1)+gauss(2)*gauss(2));
  pv->setNDoF(1);
  pv->setTechnique(LHCb::RecVertex::Primary); // some arbitrary number
  return StatusCode::SUCCESS;
  
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode MCPrimVertexMaker::finalize() {

  debug() << "==> Finalize" << endmsg;

  if ( m_evts>0) info() << "Average of " << (double)m_vtx/m_evts << " vertices, " 
                        << (double)m_vis/m_evts << " visible (" 
                        << 100.*m_vis/m_vtx << "%) with " 
                        << (double)m_prods/m_vis << " tracks" << endmsg ;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
