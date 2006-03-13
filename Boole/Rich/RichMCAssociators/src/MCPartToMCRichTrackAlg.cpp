
//-----------------------------------------------------------------------------
/** @file MCPartToMCRichTrackAlg.cpp
 *
 * Implementation file for class : MCPartToMCRichTrackAlg
 *
 * CVS Log :-
 * $Id: MCPartToMCRichTrackAlg.cpp,v 1.7 2006-03-13 13:18:53 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 14/01/2002
 */
//-----------------------------------------------------------------------------

// local
#include "MCPartToMCRichTrackAlg.h"

// namespace
using namespace LHCb;

// Declaration of the Algorithm Factory
static const  AlgFactory<MCPartToMCRichTrackAlg>          s_factory;
const        IAlgFactory& MCPartToMCRichTrackAlgFactory = s_factory;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCPartToMCRichTrackAlg::MCPartToMCRichTrackAlg( const std::string& name,
                                                ISvcLocator* pSvcLocator)
  : RichAlgBase ( name , pSvcLocator ),
    m_linker    ( NULL )
{

  m_evtLocs.clear();
  m_evtLocs.push_back(                    MCRichTrackLocation::Default );
  m_evtLocs.push_back( "Prev/"          + MCRichTrackLocation::Default );
  m_evtLocs.push_back( "PrevPrev/"      + MCRichTrackLocation::Default );
  m_evtLocs.push_back( "Next/"          + MCRichTrackLocation::Default );
  m_evtLocs.push_back( "NextNext/"      + MCRichTrackLocation::Default );
  m_evtLocs.push_back( "LHCBackground/" + MCRichTrackLocation::Default );
  declareProperty( "EventLocations", m_evtLocs );

}

//=============================================================================
// Destructor
//=============================================================================
MCPartToMCRichTrackAlg::~MCPartToMCRichTrackAlg() {};

//=============================================================================
// Initialisation
//=============================================================================
StatusCode MCPartToMCRichTrackAlg::initialize()
{
  // Sets up various tools and services
  return RichAlgBase::initialize();
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MCPartToMCRichTrackAlg::execute()
{
  debug() << "Execute" << endreq;

  // Loop over all event locations
  for ( EventList::const_iterator iEvt = m_evtLocs.begin();
        iEvt != m_evtLocs.end(); ++iEvt )
  {
    if ( !addEvent(*iEvt) ) return StatusCode::FAILURE;
  }

  // force a new linker for next event
  resetLinker();

  // return
  return StatusCode::SUCCESS;
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
                              MCParticleLocation::Default+"2MCRichTracks" );
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
  SmartDataPtr<MCRichTracks> mcTracks( evtSvc(), evtLoc );
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
  for ( MCRichTracks::const_iterator iTk = mcTracks->begin();
        iTk != mcTracks->end(); ++iTk )
  {
    const MCRichTrack * mcT = *iTk;
    if ( mcT )
    {
      const MCParticle * mcP = mcT->mcParticle();
      if ( mcP )
      {
        if ( msgLevel(MSG::VERBOSE) )
        { verbose() << "Linking MCParticle " << mcP->key()
                    << " to MCRichTrack " << mcT->key() << endreq; }
        linker()->link( mcP, mcT );
      }
      else
      {
        //Warning( "MCRichTrack has null MCParticle reference" );
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
//  Finalize
//=============================================================================
StatusCode MCPartToMCRichTrackAlg::finalize()
{
  return RichAlgBase::finalize();
}

//=============================================================================
