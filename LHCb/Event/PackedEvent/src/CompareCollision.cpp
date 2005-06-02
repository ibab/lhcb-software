// $Id: CompareCollision.cpp,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/Collision.h"
// local
#include "CompareCollision.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CompareCollision
//
// 2005-03-18 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<CompareCollision>          s_factory ;
const        IAlgFactory& CompareCollisionFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CompareCollision::CompareCollision( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = CollisionLocation::Default );
  declareProperty( "TestName"  , m_testName   = CollisionLocation::Default+"Test" );
}
//=============================================================================
// Destructor
//=============================================================================
CompareCollision::~CompareCollision() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CompareCollision::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CompareCollision::execute() {

  debug() << "==> Execute" << endmsg;
  Collisions* old  = get<Collisions>( m_inputName  );
  Collisions* test = get<Collisions>( m_testName  );

  if ( old->size() != test->size() ) {
    info() << "Old Collision size " << old->size()
           << " differs form Test " << test->size()
           << endreq;
    return StatusCode::FAILURE;
  }  
  Collisions::const_iterator itOld  = old->begin();
  Collisions::const_iterator itTest = test->begin();

  while ( old->end() != itOld ) {
    Collision* oColl = (*itOld++);
    Collision* tColl = (*itTest++);
    if ( oColl->key() != tColl->key() ) {
      info() << "Wrong key : old " <<  oColl->key() << " test " << tColl->key() << endreq;
    }
    bool isOK = true;
    if ( MSG::VERBOSE >= msgLevel() ) isOK = false; //== force printing
    if ( oColl->isSignal() != tColl->isSignal() ) isOK = false;
    if ( oColl->processType() != tColl->processType() ) isOK = false;
    if ( 5.e-5 < fabs( oColl->primVtxPosition().x() - tColl->primVtxPosition().x() ) ) isOK = false;
    if ( 5.e-5 < fabs( oColl->primVtxPosition().y() - tColl->primVtxPosition().y() ) ) isOK = false;
    if ( 5.e-5 < fabs( oColl->primVtxPosition().z() - tColl->primVtxPosition().z() ) ) isOK = false;
    if ( !isOK ) {
      info() << "=== Collision key " << oColl->key() << endreq;
      info() << format( "  old signal %1d process%6d pos %12.5f %12.5f %12.5f",
                        oColl->isSignal(), oColl->processType(),
                        oColl->primVtxPosition().x(), 
                        oColl->primVtxPosition().y(), 
                        oColl->primVtxPosition().z() ) << endreq;
      info() << format( " test signal %1d process%6d pos %12.5f %12.5f %12.5f",
                        tColl->isSignal(), tColl->processType(),
                        tColl->primVtxPosition().x(), 
                        tColl->primVtxPosition().y(), 
                        tColl->primVtxPosition().z() ) << endreq;
    }
  }
  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CompareCollision::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
