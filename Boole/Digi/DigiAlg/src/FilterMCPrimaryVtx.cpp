// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// Event Model
#include "Event/MCHeader.h"
#include "Event/MCVertex.h"

// local
#include "FilterMCPrimaryVtx.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FilterMCPrimaryVtx
//
// 2009-04-06 : Marco Cattaneo
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FilterMCPrimaryVtx )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FilterMCPrimaryVtx::FilterMCPrimaryVtx( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
FilterMCPrimaryVtx::~FilterMCPrimaryVtx() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode FilterMCPrimaryVtx::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FilterMCPrimaryVtx::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  LHCb::MCHeader* mcHeader = get<LHCb::MCHeader>( LHCb::MCHeaderLocation::Default );

  // Make local copy of primary vertices
  std::vector<LHCb::MCVertex*> primVtx;
  SmartRefVector<LHCb::MCVertex>::const_iterator oldVtx;
  for( oldVtx  = mcHeader->primaryVertices().begin();
       oldVtx != mcHeader->primaryVertices().end();   oldVtx++ ) {
    LHCb::MCVertex* theVtx = (**oldVtx).cloneWithKey();
    theVtx->clearProducts();
    primVtx.push_back( theVtx );
  }

  // Clear the existing MCVertex container
  LHCb::MCVertices* mcVerts = get<LHCb::MCVertices>( LHCb::MCVertexLocation::Default );
  mcVerts->erase( mcVerts->begin(), mcVerts->end() );
  
  // Clear the obsolete primary vertex pointers 
  mcHeader->clearPrimaryVertices();
  
  // Add the new vertices
  std::vector<LHCb::MCVertex*>::iterator newVtx;
  for( newVtx = primVtx.begin(); newVtx != primVtx.end(); newVtx++ ){
    mcHeader->addToPrimaryVertices( *newVtx );
    mcVerts->insert( *newVtx );
  }

  // Check everything OK
  if ( msgLevel(MSG::DEBUG) ) {
    debug() << "There are " << mcVerts->size() << " vertices" << endmsg;
    LHCb::MCVertices::const_iterator mcV;
    for( mcV  = mcVerts->begin();
         mcV != mcVerts->end(); mcV++ ) {
      debug() << **mcV << endmsg;
    }
    debug() << "There are " << mcHeader->primaryVertices().size() << " primary vertices" << endmsg;
    SmartRefVector<LHCb::MCVertex>::const_iterator pV;
    for( pV  = mcHeader->primaryVertices().begin();
         pV != mcHeader->primaryVertices().end();   pV++ ) {
      debug() << **pV << endmsg;
    }
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FilterMCPrimaryVtx::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
