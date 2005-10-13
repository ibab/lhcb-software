
//-----------------------------------------------------------------------------
/** @file MCRichHitToMCRichOpPhotAlg.cpp
 *
 * Implementation file for class : MCRichHitToMCRichOpPhotAlg
 *
 * CVS Log :-
 * $Id: MCRichHitToMCRichOpPhotAlg.cpp,v 1.1.1.1 2005-10-13 15:13:40 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2004-02-11
 */
//-----------------------------------------------------------------------------

// local
#include "MCRichHitToMCRichOpPhotAlg.h"

// Declaration of the Algorithm Factory
static const  AlgFactory<MCRichHitToMCRichOpPhotAlg>          s_factory;
const        IAlgFactory& MCRichHitToMCRichOpPhotAlgFactory = s_factory;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCRichHitToMCRichOpPhotAlg::MCRichHitToMCRichOpPhotAlg( const std::string& name,
                                                        ISvcLocator* pSvcLocator)
  : RichAlgBase ( name , pSvcLocator )
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
  return RichAlgBase::initialize();
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode MCRichHitToMCRichOpPhotAlg::execute()
{
  debug() << "Execute" << endreq;

  // New linker object
  MCRichHitsToPhotons links( eventSvc(), msgSvc(),
                             MCRichOpticalPhotonLocation::LinksFromMCRichHits );

  // set the ordering
  links.setDecreasingWeight();

  // Loop over all MCRichOpticalPhotons in each spillover event
  for ( EventList::const_iterator iEvt = m_evtLocs.begin();
        iEvt != m_evtLocs.end(); ++iEvt ) 
  {
    if ( !addEvent(links,*iEvt) ) return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode MCRichHitToMCRichOpPhotAlg::addEvent( MCRichHitsToPhotons & links,
                                                 const std::string & evtLoc )
{

  // load MCRichTracks in this event
  SmartDataPtr<MCRichOpticalPhotons> mcPhotons( eventSvc(), evtLoc );
  if ( !mcPhotons ) 
  {
    debug() << "Cannot locate MCRichOpticalPhotons at " << evtLoc << endreq;
    return StatusCode::SUCCESS;
  }
  debug() << "Successfully located " << mcPhotons->size()
          << " MCRichOpticalPhotons at " << evtLoc << endreq;

  // add links to linker
  for ( MCRichOpticalPhotons::const_iterator iPhot = mcPhotons->begin();
        iPhot != mcPhotons->end(); ++iPhot ) 
  {
    links.link( (*iPhot)->mcRichHit(), *iPhot );
  }
  
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
