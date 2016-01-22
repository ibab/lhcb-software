// $Id: CompareTrack.cpp,v 1.12 2009-11-12 07:27:01 ocallot Exp $
// Include files 

// local
#include "CompareTrack.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CompareTrack
//
// 2008-11-14 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CompareTrack )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CompareTrack::CompareTrack( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::TrackLocation::Default );
  declareProperty( "TestName"  , m_testName   = LHCb::TrackLocation::Default+"Test" );
}

//=============================================================================
// Destructor
//=============================================================================
CompareTrack::~CompareTrack() {} 

//=============================================================================
// Main execution
//=============================================================================
StatusCode CompareTrack::execute()
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  LHCb::Tracks* old  = get<LHCb::Tracks>( m_inputName  );
  LHCb::Tracks* test = get<LHCb::Tracks>( m_testName  );

  // Track Packer
  const LHCb::TrackPacker packer(this);

  // compare
  return packer.check( *old, *test );
}
