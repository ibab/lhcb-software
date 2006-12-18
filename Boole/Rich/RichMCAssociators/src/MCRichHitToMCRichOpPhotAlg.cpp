
//-----------------------------------------------------------------------------
/** @file MCRichHitToMCRichOpPhotAlg.cpp
 *
 * Implementation file for class : MCRichHitToMCRichOpPhotAlg
 *
 * CVS Log :-
 * $Id: MCRichHitToMCRichOpPhotAlg.cpp,v 1.6 2006-12-18 15:44:49 cattanem Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2004-02-11
 */
//-----------------------------------------------------------------------------

// local
#include "MCRichHitToMCRichOpPhotAlg.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// namespace
using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( MCRichHitToMCRichOpPhotAlg );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCRichHitToMCRichOpPhotAlg::MCRichHitToMCRichOpPhotAlg( const std::string& name,
                                                        ISvcLocator* pSvcLocator)
  : RichAlgBase ( name , pSvcLocator ),
    m_linker    ( NULL )
{

  // Event locations to process
  m_evtLocs.clear();
  m_evtLocs.push_back(                    MCRichOpticalPhotonLocation::Default );
  m_evtLocs.push_back( "Prev/"          + MCRichOpticalPhotonLocation::Default );
  m_evtLocs.push_back( "PrevPrev/"      + MCRichOpticalPhotonLocation::Default );
  m_evtLocs.push_back( "Next/"          + MCRichOpticalPhotonLocation::Default );
  m_evtLocs.push_back( "NextNext/"      + MCRichOpticalPhotonLocation::Default );
  m_evtLocs.push_back( "LHCBackground/" + MCRichOpticalPhotonLocation::Default );
  declareProperty( "EventLocations", m_evtLocs );

}

//=============================================================================
// Destructor
//=============================================================================
MCRichHitToMCRichOpPhotAlg::~MCRichHitToMCRichOpPhotAlg() {};

//=============================================================================
// Initialisation
//=============================================================================
StatusCode MCRichHitToMCRichOpPhotAlg::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  // add custom initialisations here

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MCRichHitToMCRichOpPhotAlg::execute()
{
  debug() << "Execute" << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  // Loop over all MCRichOpticalPhotons in each spillover event
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
MCRichHitToMCRichOpPhotAlg::MCRichHitsToPhotons *
MCRichHitToMCRichOpPhotAlg::linker()
{
  if ( !m_linker )
  {
    // New linker object
    m_linker =
      new MCRichHitsToPhotons( evtSvc(), msgSvc(),
                               MCRichHitLocation::Default+"2MCRichOpticalPhotons" );
    // set the ordering
    m_linker->setDecreasingWeight();
  }
  return m_linker;
}

//=============================================================================
// Process a single event location
//=============================================================================
StatusCode MCRichHitToMCRichOpPhotAlg::addEvent( const std::string & evtLoc )
{

  // load MCRichTracks in this event
  SmartDataPtr<MCRichOpticalPhotons> mcPhotons( eventSvc(), evtLoc );
  if ( !mcPhotons )
  {
    if ( msgLevel(MSG::DEBUG) )
    { debug() << "Cannot locate MCRichOpticalPhotons at " << evtLoc << endreq; }
    return StatusCode::SUCCESS;
  }
  if ( msgLevel(MSG::DEBUG) )
  { debug() << "Successfully located " << mcPhotons->size()
            << " MCRichOpticalPhotons at " << evtLoc << endreq; }

  // add links to linker
  for ( MCRichOpticalPhotons::const_iterator iPhot = mcPhotons->begin();
        iPhot != mcPhotons->end(); ++iPhot )
  {
    const MCRichOpticalPhoton * mcPhot = *iPhot;
    if ( mcPhot )
    {
      const MCRichHit * mchit = mcPhot->mcRichHit();
      if ( mchit )
      {
        if ( msgLevel(MSG::VERBOSE) )
        { verbose() << "Linking MCRichHit " << mchit->sensDetID()
                    << " to MCRichOpticalPhoton " << mcPhot->key() << endreq; }
        linker()->link( mchit, *iPhot );
      }
      else
      {
        Warning( "MCRichOpticalPhoton has NULL MCRichHit reference" );
      }
    }
    else
    {
      Warning( "NULL MCRichOpticalPhoton in container" );
    }
  }

  if ( msgLevel(MSG::DEBUG) )
  { debug() << "Finished processing MCRichOpticalPhotons at " << evtLoc << endreq; }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MCRichHitToMCRichOpPhotAlg::finalize()
{
  return RichAlgBase::finalize();
}

//=============================================================================
