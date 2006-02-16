
//-----------------------------------------------------------------------------
/** @file MCPartToMCRichTrackAlg.cpp
 *
 * Implementation file for class : MCPartToMCRichTrackAlg
 *
 * CVS Log :-
 * $Id: MCPartToMCRichTrackAlg.cpp,v 1.5 2006-02-16 15:57:39 jonrob Exp $
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
  : RichAlgBase  ( name , pSvcLocator ),
    m_testLinker ( false )
{

  m_evtLocs.clear();
  m_evtLocs.push_back(                    MCRichTrackLocation::Default );
  m_evtLocs.push_back( "Prev/"          + MCRichTrackLocation::Default );
  m_evtLocs.push_back( "PrevPrev/"      + MCRichTrackLocation::Default );
  m_evtLocs.push_back( "Next/"          + MCRichTrackLocation::Default );
  m_evtLocs.push_back( "NextNext/"      + MCRichTrackLocation::Default );
  m_evtLocs.push_back( "LHCBackground/" + MCRichTrackLocation::Default );
  declareProperty( "EventLocations", m_evtLocs );

  declareProperty( "TestLinker", m_testLinker );

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

  // build links
  StatusCode sc = buildLinks();
  if ( sc.isFailure() ) return sc;

  // test links
  if ( m_testLinker ) { sc = testLinks(); }

  // return
  return sc;
}

StatusCode MCPartToMCRichTrackAlg::buildLinks()
{
  // New linker object
  MCPartToRichTracks links( evtSvc(), msgSvc(),
                            MCParticleLocation::Default+"2MCRichTracks" );

  // set the ordering
  links.setDecreasingWeight();

  // Loop over all MCRichTracks in each event
  for ( EventList::const_iterator iEvt = m_evtLocs.begin();
        iEvt != m_evtLocs.end(); ++iEvt ) 
  {
    if ( !addEvent(links,*iEvt) ) return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode MCPartToMCRichTrackAlg::addEvent( MCPartToRichTracks & links,
                                             const std::string & evtLoc )
{

  // load MCRichTracks in this event
  SmartDataPtr<MCRichTracks> mcTracks( evtSvc(), evtLoc );
  if ( !mcTracks )
  {
    debug() << "Cannot locate MCRichTracks at " << evtLoc << endreq;
    return StatusCode::SUCCESS;
  }
  debug() << "Successfully located " << mcTracks->size()
          << " MCRichTracks at " << evtLoc << endreq;

  // add links to linker
  for ( MCRichTracks::const_iterator iTk = mcTracks->begin();
        iTk != mcTracks->end(); ++iTk )
  {
    links.link( (*iTk)->mcParticle(), *iTk );
  }

  return StatusCode::SUCCESS;
}

StatusCode MCPartToMCRichTrackAlg::testLinks()
{

  // relation interface

  /*
  // typedefs for testing the linker
  typedef LinkerTool<LHCb::MCParticle,LHCb::MCRichTrack> MCPartToRichTracksTest;
  typedef MCPartToRichTracksTest::DirectType             Table;

  // load linker
  MCPartToRichTracksTest linker1( evtSvc(), MCParticleLocation::Default+"2MCRichTracks" );

  // table
  Table * table = linker1.direct();
  if ( !table ) return Error( "Failed to load LinkerTool", StatusCode::SUCCESS );

  Table::Range all = table->relations();
  info() << "Found " << all.size() << " association(s) for "
         << MCRichTrackLocation::LinksFromMCParticles << endreq;
  */

  // original interface

  LinkedTo<LHCb::MCRichTrack,LHCb::MCParticle>
    linker2( evtSvc(), msgSvc(),
             MCParticleLocation::Default+"2MCRichTracks" );

  // loop over MCParticles and test links
  MCParticles * mcps = get<MCParticles>( MCParticleLocation::Default );
  for ( MCParticles::const_iterator imp = mcps->begin(); 
        imp != mcps->end(); ++imp )
  {

    /*
    // test relation type interface
    Table::Range range = table->relations( *imp );
    // ( only one, so just take first )
    const MCRichTrack * mcrtrack = ( range.empty() ? 0 : (*range.begin()).to() );
    debug() << "Found " << range.size() << " association(s) for MCParticle " 
            << (*imp)->key() << " : MCRichTrack " << mcrtrack << endreq;
    */

    // test original interface
    const MCRichTrack * mcrtrack = linker2.first( *imp );
    debug() << "MCParticle " << (*imp)->key() << " : MCRichTrack " << mcrtrack << endreq;

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
