// $Id: TrackVertex2PrimVertex.cpp,v 1.1 2006-07-06 14:39:06 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "TrackVertex2PrimVertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackVertex2PrimVertex
//
// 2006-07-06 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackVertex2PrimVertex );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackVertex2PrimVertex::TrackVertex2PrimVertex( const std::string& name,
                                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty("InputLocation", 
                  m_inputLocation = LHCb::TrackVertexLocation::Primary );
  declareProperty("OutputLocation", 
                  m_outputLocation = LHCb::VertexLocation::Primary );
  
}
//=============================================================================
// Destructor
//=============================================================================
TrackVertex2PrimVertex::~TrackVertex2PrimVertex() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackVertex2PrimVertex::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackVertex2PrimVertex::execute() {

  debug() << "==> Execute" << endmsg;

  LHCb::PrimVertices* primaries = new LHCb::PrimVertices();
  LHCb::TrackVertices* tVtx = get<LHCb::TrackVertices>(m_inputLocation);

  verbose() << "Converting " << tVtx->size() << " TrackVertices from " 
            << m_inputLocation << endmsg ;
  
  for (LHCb::TrackVertices::const_iterator ipv = tVtx->begin();
        ipv != tVtx->end() ; ++ipv ){
    LHCb::PrimVertex* newPV = makePrimVertex(*ipv);
    primaries->insert(newPV);
     
  }
  put(primaries, m_outputLocation );
  verbose() << "Storing " << primaries->size() << " PrimVertices in " 
            << m_outputLocation << endmsg ;

  return StatusCode::SUCCESS;
}
//=============================================================================
LHCb::PrimVertex*
TrackVertex2PrimVertex::makePrimVertex( const LHCb::TrackVertex* trVtx )
{
  LHCb::PrimVertex* pv = new LHCb::PrimVertex;
  pv->setPosition(  trVtx->position()     );
  pv->setCovMatrix( trVtx->positionErr()  );
  pv->setChi2(      trVtx->chi2()         );
  pv->setNDoF(      trVtx->nDoF()         );
  pv->setTechnique( LHCb::Vertex::Primary );
  pv->setTracks(    trVtx->tracks()       );
  
  return pv;  
  
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode TrackVertex2PrimVertex::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
