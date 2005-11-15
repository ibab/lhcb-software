
//-----------------------------------------------------------------------------
/** @file MCPartToMCRichTrackAlg.cpp
 *
 * Implementation file for class : MCPartToMCRichTrackAlg
 *
 * CVS Log :-
 * $Id: MCPartToMCRichTrackAlg.cpp,v 1.2 2005-11-15 13:24:29 jonrob Exp $
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2005/10/13 15:13:38  jonrob
 * First import of RichMCAssociators under Rich
 *
 * Revision 1.3  2004/07/26 17:56:09  jonrob
 * Various improvements to the doxygen comments
 *
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 14/01/2002
 */
//-----------------------------------------------------------------------------

// local
#include "MCPartToMCRichTrackAlg.h"

// Declaration of the Algorithm Factory
static const  AlgFactory<MCPartToMCRichTrackAlg>          s_factory;
const        IAlgFactory& MCPartToMCRichTrackAlgFactory = s_factory;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCPartToMCRichTrackAlg::MCPartToMCRichTrackAlg( const std::string& name,
                                                ISvcLocator* pSvcLocator)
  : RichAlgBase ( name , pSvcLocator ) 
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
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode MCPartToMCRichTrackAlg::execute() 
{
  debug() << "Execute" << endreq;

  // New linker object
  MCPartToRichTracks links( eventSvc(), msgSvc(), 
                            MCRichTrackLocation::LinksFromMCParticles );
  
  // set the ordering
  links.setDecreasingWeight();

  // Loop over all MCRichTracks in each spillover event
  for ( EventList::const_iterator iEvt = m_evtLocs.begin();
        iEvt != m_evtLocs.end(); ++iEvt ) {
    if ( !addEvent(links,*iEvt) ) return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
};

StatusCode MCPartToMCRichTrackAlg::addEvent( MCPartToRichTracks & links, 
                                             const std::string & evtLoc )
{

  // load MCRichTracks in this event
  SmartDataPtr<MCRichTracks> mcTracks( eventSvc(), evtLoc );
  if ( !mcTracks ) 
  {
    debug() << "Cannot locate MCRichTracks at " << evtLoc << endreq;
    return StatusCode::SUCCESS;
  }
  debug() << "Successfully located " << mcTracks->size()
          << " MCRichTracks at " << evtLoc << endreq;

  // add links to linker
  for ( MCRichTracks::const_iterator iTk = mcTracks->begin();
        iTk != mcTracks->end(); ++iTk ) {
    links.link( (*iTk)->mcParticle(), *iTk );
  }

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
