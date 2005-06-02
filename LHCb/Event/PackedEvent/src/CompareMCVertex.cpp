// $Id: CompareMCVertex.cpp,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
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

// Declaration of the Algorithm Factory
static const  AlgFactory<CompareMCVertex>          s_factory ;
const        IAlgFactory& CompareMCVertexFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CompareMCVertex::CompareMCVertex( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = MCVertexLocation::Default );
  declareProperty( "TestName"  , m_testName   = MCVertexLocation::Default+"Test" );
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

  MCVertices* old  = get<MCVertices>( m_inputName  );
  MCVertices* test = get<MCVertices>( m_testName  );

  if ( old->size() != test->size() ) {
    info() << "Old MCVertex size " << old->size()
           << " differs form Test " << test->size()
           << endreq;
    return StatusCode::FAILURE;
  }  
  MCVertices::const_iterator itOld  = old->begin();
  MCVertices::const_iterator itTest = test->begin();

  while ( old->end() != itOld ) {
    MCVertex* oVert = (*itOld++);
    MCVertex* tVert = (*itTest++);
    if ( oVert->key() != tVert->key() ) {
      info() << "Wrong key : old " <<  oVert->key() << " test " << tVert->key() << endreq;
    }
    bool isOK = true;
    if ( MSG::VERBOSE >= msgLevel() ) isOK = false; //== force printing
    if ( 5.e-5 < fabs( oVert->position().x() - tVert->position().x() ) ) isOK = false;
    if ( 5.e-5 < fabs( oVert->position().y() - tVert->position().y() ) ) isOK = false;
    if ( 5.e-5 < fabs( oVert->position().z() - tVert->position().z() ) ) isOK = false;
    if ( 1.e-3 < fabs( oVert->timeOfFlight() - tVert->timeOfFlight() ) ) isOK = false;
    if ( oVert->type() != tVert->type() ) isOK = false;

    if ( oVert->mother()    != tVert->mother()    ) isOK = false;
    if ( oVert->collision() != tVert->collision() ) isOK = false;
    unsigned int kk;
    if ( oVert->products().size() != tVert->products().size() ) {
      isOK = false;
    } else { 
      for ( kk = 0; oVert->products().size() > kk; kk++ ) {
        MCParticle* dum  = oVert->products()[kk];  // convert smartref to pointers
        MCParticle* dum1 = tVert->products()[kk];
        if ( dum != dum1 ) isOK = false;
      }
    }
    if ( !isOK ) {
      info() << "=== MCVertex key " << oVert->key() << endreq;
      info() << format( "  old point %12.5f %12.5f %12.5f %12.4f %2d",
                        oVert->position().x(), oVert->position().y(),
                        oVert->position().z(), oVert->timeOfFlight(), oVert->type() )
             << " mother " << oVert->mother()
             << " coll " << oVert->collision() 
             << endreq;
      info() << format( " test point %12.5f %12.5f %12.5f %12.4f %2d",
                        tVert->position().x(), tVert->position().y(),
                        tVert->position().z(), tVert->timeOfFlight(), tVert->type() )
             << " mother " << tVert->mother()
             << " coll " << tVert->collision() 
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
