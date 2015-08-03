// $Id: DumpTracks.cpp,v 1.1 2009-07-10 06:15:55 cattanem Exp $
// Include files

// Event Model
#include "Event/Track.h"

// local
#include "DumpTracks.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DumpTracks
//
// 2004-07-14 : Marco Cattaneo
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(DumpTracks)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DumpTracks::DumpTracks( const std::string& name,
                        ISvcLocator* pSvcLocator)
: GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "NumberOfObjectsToPrint", m_numObjects = 5 );
  declareProperty( "TracksLocation", m_tracksLocation = LHCb::TrackLocation::Default );
}

//=============================================================================
// Destructor
//=============================================================================
DumpTracks::~DumpTracks() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode DumpTracks::execute() 
{
  LHCb::Tracks* tracksCont = get<LHCb::Tracks>( m_tracksLocation );

  info() << "There are " << tracksCont->size() << " tracks in "
         << m_tracksLocation << endmsg;

  counter("#Tracks") += tracksCont->size();

  if ( msgLevel( MSG::DEBUG ) ) 
  {
    unsigned int numPrinted = 0;

    for ( const LHCb::Track * track : *tracksCont )
    {
      if ( !msgLevel(MSG::VERBOSE) && ++numPrinted > m_numObjects ) break;
      debug() << *track << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
