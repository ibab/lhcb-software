
//--------------------------------------------------------------------------
/** @file RichGlobalPIDFinalize.cpp
 *
 *  Implementation file for RICH Global PID algorithm class : Rich::Rec::GlobalPID::Finalize
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDFinalize.cpp,v 1.23 2009-07-30 11:06:48 jonrob Exp $
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
DECLARE_ALGORITHM_FACTORY( Finalize );

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
  for ( LHCb::RichGlobalPIDTracks::iterator track = gpidTracks()->begin();
        track != gpidTracks()->end(); ++track )
  {
    LHCb::RichRecTrack * rRTrack = (*track)->richRecTrack();

    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "PID'ed Track "
                << (*track)->key() << " (" << (*track)->trQuality()
                << "), as " << rRTrack->currentHypothesis() << endmsg;
    }

    // Only store results for physics quality tracks
    if ( (*track)->trQuality() != Rich::Rec::GlobalPID::Physics )
    {
      gpidPIDs()->erase( (*track)->globalPID() );
      continue;
    }

    // finalize this PID
    m_gtkCreator->finaliseTrack( *track );

  }

  // All OK - Update ProcStatus with number of PIDs
  procStatus()->addAlgorithmStatus( gpidName(), gpidPIDs()->size() );

  return StatusCode::SUCCESS;
}

