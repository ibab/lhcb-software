// Include files 

// from Boost
#include "boost/numeric/conversion/bounds.hpp"

#include "Event/MCVertex.h"
// local
#include "CompareMCVertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CompareMCVertex
//
// 2005-03-18 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( CompareMCVertex )

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
CompareMCVertex::~CompareMCVertex() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CompareMCVertex::execute() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Execute" << endmsg;

  LHCb::MCVertices* old  = get<LHCb::MCVertices>( m_inputName  );
  LHCb::MCVertices* test = get<LHCb::MCVertices>( m_testName  );

  if ( old->size() != test->size() ) {
    err() << "Old MCVertex size " << old->size()
          << " differs form Test " << test->size()
          << endmsg;
    return StatusCode::FAILURE;
  }
  LHCb::MCVertices::const_iterator itOld  = old->begin();
  LHCb::MCVertices::const_iterator itTest = test->begin();

  static const double smallest = boost::numeric::bounds<float>::smallest();
  static const double tiny     = boost::numeric::bounds<double>::smallest();

  while ( old->end() != itOld ) {
    LHCb::MCVertex* oVert = (*itOld++);
    LHCb::MCVertex* tVert = (*itTest++);

    if ( oVert->key() != tVert->key() ) {
      warning() << "Wrong key : old " <<  oVert->key() << " test " << tVert->key() << endmsg;
    }
    bool isOK = true;
    if ( 5.e-5 < fabs( oVert->position().x() - tVert->position().x() ) ) isOK = false;
    if ( 5.e-5 < fabs( oVert->position().y() - tVert->position().y() ) ) isOK = false;
    if ( 5.e-5 < fabs( oVert->position().z() - tVert->position().z() ) ) isOK = false;
    // Protect crazy vertex times in DC06 data. Test times should be OK after unpacking
    double oldTime;
    if( oVert->time() > 0. && oVert->time() < smallest ) {
      if( msgLevel(MSG::DEBUG) ) {
        if( oVert->time() < tiny )
          debug() << "time smaller than " << tiny;
        else
          debug() << "time " << oVert->time();

        debug() << " set to zero for vertex "
                << oVert->key() << " of type " << oVert->type() << endmsg;
      }
      oldTime = 0;
    }
    else {
      oldTime = oVert->time();
    }
    if ( 1.e-3 < fabs( oldTime - tVert->time() ) ) isOK = false;

    if ( oVert->type() != tVert->type() ) isOK = false;

    if ( oVert->mother()    != tVert->mother()    ) isOK = false;
    if (isOK && msgLevel(MSG::VERBOSE)) verbose() << "diff mother OK" << endmsg;
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
    if (isOK && msgLevel(MSG::VERBOSE)) verbose() << "diff products OK" << endmsg;
    if ( !isOK || msgLevel(MSG::VERBOSE) ) {
      if( !isOK ) {
        Warning( "Packed MCVertex info truncated. Set DEBUG OutputLevel for details", StatusCode::SUCCESS, 0 ).ignore();
        debug() << "Packed MCVertex info truncated" << endmsg;
      }
      debug() << "=== MCVertex key " << oVert->key() << endmsg;
      debug() << format( "  old point %12.5f %12.5f %12.5f %12.4f %2d",
                         oVert->position().x(), oVert->position().y(),
                         oVert->position().z(), oVert->time(), oVert->type() )
              << " mother " << oVert->mother()
              << endmsg;
      debug() << format( " test point %12.5f %12.5f %12.5f %12.4f %2d",
                         tVert->position().x(), tVert->position().y(),
                         tVert->position().z(), tVert->time(), tVert->type() )
              << " mother " << tVert->mother()
              << endmsg << "  old products ";
      for ( kk = 0; oVert->products().size() > kk; kk++ ) {
        debug() << " " << oVert->products()[kk];
      }
      debug() << endmsg << " test products ";
      for ( kk = 0; tVert->products().size() > kk; kk++ ) {
        debug() << " " << tVert->products()[kk];
      }
      debug() << endmsg;
    }
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
