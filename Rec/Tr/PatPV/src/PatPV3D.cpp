// Include files
// -------------

// From Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// Local
//#include "PatVertexState.h"
#include "PatPV3D.h"
#include "Event/RecVertex.h"
#include "Event/PrimaryVertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatPV3D
//
// 2004-02-17 : Eduardo Rodrigues
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PatPV3D )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatPV3D::PatPV3D( const std::string& name,
                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
  m_pvsfit(0)
{
  declareProperty( "OutputVerticesName" , m_recVertexLocation  =  LHCb::RecVertexLocation::Velo3D );
  declareProperty( "PrimaryVertexLocation" , m_primaryVertexLocation =  LHCb::PrimaryVertexLocation::Default );
  declareProperty( "RefitPV", m_refitpv = false ) ;
}

//=============================================================================
// Destructor
//=============================================================================
PatPV3D::~PatPV3D() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode PatPV3D::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (!sc) return sc;

  debug() << "==> Initialise" << endmsg;

  // Access PVOfflineTool
  m_pvsfit = tool<IPVOfflineTool>("PVOfflineTool",this);
  if(!m_pvsfit) {
    err() << "Unable to retrieve the PVOfflineTool" << endmsg;
    return  StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
  
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatPV3D::execute() {

  MsgStream msg(msgSvc(), name());
  bool isDebug   = msg.level() <= MSG::DEBUG;

  if ( isDebug ) {
    debug() << "==> Execute" << endmsg;
  }

  LHCb::RecVertices* outputRecVertices(0) ;
  if( !m_recVertexLocation.empty() ) {
    outputRecVertices = new LHCb::RecVertices();
    put(outputRecVertices, m_recVertexLocation);
  }
  
  LHCb::PrimaryVertices* outputPrimaryVertices(0) ;
  if( !m_primaryVertexLocation.empty() ) {
    outputPrimaryVertices = new LHCb::PrimaryVertices() ;
    put( outputPrimaryVertices, m_primaryVertexLocation ) ;
  }

  std::vector<LHCb::RecVertex> rvts;
  StatusCode scfit = m_pvsfit->reconstructMultiPV(rvts);
  if (scfit == StatusCode::SUCCESS) {
    setFilterPassed( !rvts.empty() ) ;
    if( outputRecVertices ) {
      for( std::vector<LHCb::RecVertex>::iterator iv = rvts.begin(); iv != rvts.end(); iv++) {
        LHCb::RecVertex* vertex = new LHCb::RecVertex(*iv);
        vertex->setTechnique(LHCb::RecVertex::Primary);
        outputRecVertices->insert(vertex);
      }
    }
    if( outputPrimaryVertices ) {
      for( const auto& iv : rvts ) {
        LHCb::PrimaryVertex* vertex = new LHCb::PrimaryVertex(iv,m_refitpv);
        outputPrimaryVertices->insert(vertex);
      }
    }
  } else {
    setFilterPassed( false ) ;
    Warning("reconstructMultiPV failed!",scfit).ignore();
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PatPV3D::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();

}
