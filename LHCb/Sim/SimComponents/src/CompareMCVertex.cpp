// $Id: CompareMCVertex.cpp,v 1.1 2006-02-20 08:21:40 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/MCVertex.h"
// local
#include "CompareMCVertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CompareMCVertex
//
// 2005-03-18 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( CompareMCVertex );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CompareMCVertex::CompareMCVertex( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::MCVertexLocation::Default );
  declareProperty( "TestName"  , m_testName   = LHCb::MCVertexLocation::Default+"Test" );
}
//=============================================================================
// Destructor
//=============================================================================
CompareMCVertex::~CompareMCVertex() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CompareMCVertex::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CompareMCVertex::execute() {

  debug() << "==> Execute" << endmsg;

  LHCb::MCVertices* old  = get<LHCb::MCVertices>( m_inputName  );
  LHCb::MCVertices* test = get<LHCb::MCVertices>( m_testName  );

  if ( old->size() != test->size() ) {
    info() << "Old MCVertex size " << old->size()
           << " differs form Test " << test->size()
           << endreq;
    return StatusCode::FAILURE;
  }  
  LHCb::MCVertices::const_iterator itOld  = old->begin();
  LHCb::MCVertices::const_iterator itTest = test->begin();

  while ( old->end() != itOld ) {
    LHCb::MCVertex* oVert = (*itOld++);
    LHCb::MCVertex* tVert = (*itTest++);
    if ( oVert->key() != tVert->key() ) {
      info() << "Wrong key : old " <<  oVert->key() << " test " << tVert->key() << endreq;
    }
    bool isOK = true;
    if ( MSG::VERBOSE >= msgLevel() ) isOK = false; //== force printing
    if ( 5.e-5 < fabs( oVert->position().x() - tVert->position().x() ) ) isOK = false;
    if ( 5.e-5 < fabs( oVert->position().y() - tVert->position().y() ) ) isOK = false;
    if ( 5.e-5 < fabs( oVert->position().z() - tVert->position().z() ) ) isOK = false;
    if ( 1.e-3 < fabs( oVert->time() - tVert->time() ) ) isOK = false;
    if ( oVert->type() != tVert->type() ) isOK = false;

    if ( oVert->mother()    != tVert->mother()    ) isOK = false;
    unsigned int kk;
    if ( oVert->products().size() != tVert->products().size() ) {
      isOK = false;
    } else { 
      for ( kk = 0; oVert->products().size() > kk; kk++ ) {
        const LHCb::MCParticle* dum  = oVert->products()[kk];  // convert smartref to pointers
        const LHCb::MCParticle* dum1 = tVert->products()[kk];
        if ( dum != dum1 ) isOK = false;
      }
    }
    if ( !isOK ) {
      info() << "=== MCVertex key " << oVert->key() << endreq;
      info() << format( "  old point %12.5f %12.5f %12.5f %12.4f %2d",
                        oVert->position().x(), oVert->position().y(),
                        oVert->position().z(), oVert->time(), oVert->type() )
             << " mother " << oVert->mother()
             << endreq;
      info() << format( " test point %12.5f %12.5f %12.5f %12.4f %2d",
                        tVert->position().x(), tVert->position().y(),
                        tVert->position().z(), tVert->time(), tVert->type() )
             << " mother " << tVert->mother()
             << endreq << "  old products ";
      for ( kk = 0; oVert->products().size() > kk; kk++ ) {
        info() << " " << oVert->products()[kk];
      }
      info() << endreq << " test products ";
      for ( kk = 0; tVert->products().size() > kk; kk++ ) {
        info() << " " << tVert->products()[kk];
      }
      info() << endreq; 
    }
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CompareMCVertex::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
