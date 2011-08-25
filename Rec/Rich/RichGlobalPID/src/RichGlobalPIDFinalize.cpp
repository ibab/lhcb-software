
//--------------------------------------------------------------------------
/** @file RichGlobalPIDFinalize.cpp
 *
 *  Implementation file for RICH Global PID algorithm class : Rich::Rec::GlobalPID::Finalize
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */
//--------------------------------------------------------------------------

// local
#include "RichGlobalPIDFinalize.h"

// namespaces
using namespace Rich::Rec::GlobalPID;

//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( Finalize )

// Standard constructor, initializes variables
Finalize::Finalize( const std::string& name,
                    ISvcLocator* pSvcLocator )
  : AlgBase      ( name, pSvcLocator ),
    m_gtkCreator ( NULL  ) { }

// Destructor
Finalize::~Finalize() { }

StatusCode Finalize::initialize()
{
  const StatusCode sc = AlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  // get tools
  acquireTool( "GPIDTrackCreator", m_gtkCreator );

  return sc;
}

StatusCode Finalize::execute()
{
  // Event Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Iterate over working tracks and keep/delete PID results
  if ( msgLevel(MSG::DEBUG) ) debug() << "Final PID results :-" << endmsg;
  for ( LHCb::RichGlobalPIDTracks::iterator track = gpidTracks()->begin();
        track != gpidTracks()->end(); ++track )
  {

    // Only store results for physics quality tracks
    if ( (*track)->trQuality() != Rich::Rec::GlobalPID::Physics )
    {
      gpidPIDs()->erase( (*track)->globalPID() );
      continue;
    }

    // finalize this PID
    m_gtkCreator->finaliseTrack( *track );

    // Printout
    if ( msgLevel(MSG::DEBUG) )
      debug() << " " << *(*track)->globalPID() << endmsg;

  }

  return StatusCode::SUCCESS;
}

