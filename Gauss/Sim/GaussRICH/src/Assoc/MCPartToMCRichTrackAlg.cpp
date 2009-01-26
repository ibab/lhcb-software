
//-----------------------------------------------------------------------------
/** @file MCPartToMCRichTrackAlg.cpp
 *
 * Implementation file for class : MCPartToMCRichTrackAlg
 *
 * CVS Log :-
 * $Id: MCPartToMCRichTrackAlg.cpp,v 1.1 2009-01-26 13:45:31 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 14/01/2002
 */
//-----------------------------------------------------------------------------

// local
#include "MCPartToMCRichTrackAlg.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// namespace
using namespace Rich::MC;

DECLARE_ALGORITHM_FACTORY( MCPartToMCRichTrackAlg );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCPartToMCRichTrackAlg::MCPartToMCRichTrackAlg( const std::string& name,
                                                ISvcLocator* pSvcLocator)
  : RichAlgBase ( name , pSvcLocator ),
    m_linker    ( NULL )
{

  m_evtLocs.clear();
  m_evtLocs.push_back(                    LHCb::MCRichTrackLocation::Default );
  m_evtLocs.push_back( "Prev/"          + LHCb::MCRichTrackLocation::Default );
  m_evtLocs.push_back( "PrevPrev/"      + LHCb::MCRichTrackLocation::Default );
  m_evtLocs.push_back( "Next/"          + LHCb::MCRichTrackLocation::Default );
  m_evtLocs.push_back( "NextNext/"      + LHCb::MCRichTrackLocation::Default );
  m_evtLocs.push_back( "LHCBackground/" + LHCb::MCRichTrackLocation::Default );
  declareProperty( "EventLocations", m_evtLocs );

}

//=============================================================================
// Destructor
//=============================================================================
MCPartToMCRichTrackAlg::~MCPartToMCRichTrackAlg() {};

//=============================================================================
// Main execution
//=============================================================================
StatusCode MCPartToMCRichTrackAlg::execute()
{
  debug() << "Execute" << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  // Loop over all event locations
  for ( EventList::const_iterator iEvt = m_evtLocs.begin();
        iEvt != m_evtLocs.end(); ++iEvt )
  {
    sc = addEvent(*iEvt);
    if ( sc.isFailure() ) break;
  }

  // force a new linker for next event
  resetLinker();

  // return final status code
  return sc;
}

//=============================================================================
// Create linker object
//=============================================================================
MCPartToMCRichTrackAlg::MCPartToRichTracks *
MCPartToMCRichTrackAlg::linker()
{
  if ( !m_linker )
  {
    // New linker object
    m_linker =
      new MCPartToRichTracks( evtSvc(), msgSvc(),
                              LHCb::MCParticleLocation::Default+"2MCRichTracks" );
    // set the ordering
    m_linker->setDecreasingWeight();
  }
  return m_linker;
}

//=============================================================================
// Process the given event location
//=============================================================================
StatusCode MCPartToMCRichTrackAlg::addEvent( const std::string & evtLoc )
{

  // load MCRichTracks in this event
  SmartDataPtr<LHCb::MCRichTracks> mcTracks( evtSvc(), evtLoc );
  if ( !mcTracks )
  {
    if ( msgLevel(MSG::DEBUG) )
    { debug() << "Cannot locate MCRichTracks at " << evtLoc << endreq; }
    return StatusCode::SUCCESS;
  }
  if ( msgLevel(MSG::DEBUG) )
  { debug() << "Successfully located " << mcTracks->size()
            << " MCRichTracks at " << evtLoc << endreq; }

  // add links to linker
  for ( LHCb::MCRichTracks::const_iterator iTk = mcTracks->begin();
        iTk != mcTracks->end(); ++iTk )
  {
    const LHCb::MCRichTrack * mcT = *iTk;
    if ( mcT )
    {
      const LHCb::MCParticle * mcP = mcT->mcParticle();
      if ( mcP )
      {
        if ( msgLevel(MSG::VERBOSE) )
        { verbose() << "Linking MCParticle " << mcP->key()
                    << " to MCRichTrack " << mcT->key() << endreq; }
        linker()->link( mcP, mcT );
      }
      else
      {
        Warning( "MCRichTrack has null MCParticle reference" );
      }
    }
    else
    {
      Warning( "Null MCRichTrack in container" );
    }
  }

  if ( msgLevel(MSG::DEBUG) )
  { debug() << "Finished processing MCRichTracks at " << evtLoc << endreq; }

  return StatusCode::SUCCESS;
}

//=============================================================================
