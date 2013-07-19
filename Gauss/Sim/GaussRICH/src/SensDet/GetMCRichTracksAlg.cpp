// $Id: GetMCRichTracksAlg.cpp,v 1.8 2009-03-26 21:49:47 robbep Exp $
// Include files

// from Gaudi
//#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "GaussRICH/GetMCRichTracksAlg.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : GetMCRichTracksAlg
//
// 2005-12-06 : Sajan EASO
// 2007-01-11 : Gloria Corti, adapt to Gaudi v19 (also compatible with v18)
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory moved to Factories.cpp
//DECLARE_ALGORITHM_FACTORY( GetMCRichTracksAlg );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GetMCRichTracksAlg::GetMCRichTracksAlg( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GetMCRichInfoBase ( name , pSvcLocator )
  , m_nEvts           ( 0 )
  , m_hitTally        ( 0 )
{
  declareProperty( "MCRichTracksLocation",
                   m_dataToFill = MCRichTrackLocation::Default );
  declareProperty( "MCParticles" , m_mcParticles = MCParticleLocation::Default ) ;
  declareProperty( "MCRichSegments" , m_mcRichSegments = MCRichSegmentLocation::Default ) ;
}

//=============================================================================
// Destructor
//=============================================================================
GetMCRichTracksAlg::~GetMCRichTracksAlg() { }

//=============================================================================
// Initialization
//=============================================================================
StatusCode GetMCRichTracksAlg::initialize()
{
  const StatusCode sc = GetMCRichInfoBase::initialize();
  if ( sc.isFailure() ) return Error( "Failed to initialise", sc );

  // add custom initialisations here

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode GetMCRichTracksAlg::execute()
{
  debug() << "Execute" << endmsg;

  // Create the MCRichTracks and put them in the TES
  MCRichTracks * richTracks = new MCRichTracks();
  put( richTracks, dataLocationInTES() );
  richTracks->reserve( 100 );

  // Get the G4 necessary hit collections from GiGa
  G4HCofThisEvent* hitscollections = 0;

  // get hitscollections from GiGa
  *gigaSvc() >> hitscollections;

  if ( 0 != hitscollections )
  {

    // retrieve the trajectory container from GiGa Service
    G4TrajectoryContainer* trajectories = 0 ;
    // get trajectories from GiGa
    *gigaSvc() >> trajectories;
    if ( 0 == trajectories ) { return Error("No G4TrajectoryContainer* object is found !"); }

    // get the references between MCParticles and Geant4 TrackIDs
    const GiGaKineRefTable & table = kineSvc()->table();

    // Locate the MCRichSegments
    MCRichSegments * segments   = get<MCRichSegments>( m_mcRichSegments );
    if ( segments->empty() ) { return StatusCode::SUCCESS; }
    // locate MCParticles
    const MCParticles * mcParts = get<MCParticles>( m_mcParticles );
    if ( mcParts->empty()  ) { return Warning( "Empty MCParticles", StatusCode::SUCCESS ); }

    // loop over trajectories and form links from MCP to trajectories
    typedef std::map<const MCParticle*, const GiGaTrajectory*> MCPartToGiGaTraj;
    MCPartToGiGaTraj mcpToTraj;
    TrajectoryVector * tv = trajectories->GetVector();
    for ( TrajectoryVector::const_iterator iTr = tv->begin();
          tv->end() != iTr; ++iTr )
    {
      // get the GiGa trajectory
      const GiGaTrajectory * traj = gigaTrajectory(*iTr);
      if ( !traj ) { Warning( "Null GiGaTrajectory" ); continue; }
      mcpToTraj[table[traj->trackID()].particle()] = traj;
    }

    // map of vectors to store segments associated to each MCParticle
    typedef const MCParticle* HitListKey;
    typedef std::vector<MCRichSegment*> SegmentList;
    typedef std::map< HitListKey, SegmentList > SortedSegments;
    SortedSegments sortedSegs;

    // Iterate over segments and sort according to MCParticle
    for ( MCRichSegments::iterator iSeg = segments->begin();
          iSeg != segments->end(); ++iSeg )
    {
      if ( !(*iSeg) )
      {
        Warning( "Null RichRecSegment pointer" );
        continue;
      }
      if ( !(*iSeg)->mcParticle() )
      {
        Warning( "RichRecSegment has null MCParticle pointer" );
        continue;
      }
      sortedSegs[(*iSeg)->mcParticle()].push_back( *iSeg );
    }

    // Loop over sorted segments
    for ( SortedSegments::iterator iList = sortedSegs.begin();
          iList != sortedSegs.end(); ++iList )
    {
      const MCParticle * mcPart = (*iList).first;
      if ( !mcPart ) { Warning( "Null MCParticle pointer"); continue; }

      // new MCRichTrack
      MCRichTrack * mcTrack = new MCRichTrack();
      richTracks->insert( mcTrack, mcPart->key() );

      // data
      mcTrack->setMcParticle( mcPart );

      if ( msgLevel(MSG::DEBUG) )
      {
        debug()
          << "Creating MCRichTrack for MCParticle " << mcPart->key()
          << endreq;
      }

      // Loop over segments for this track
      for ( SegmentList::iterator iSeg = (*iList).second.begin();
            iSeg != (*iList).second.end(); ++iSeg )
      {
        if ( !(*iSeg) ) continue;
        mcTrack->addToMcSegments( *iSeg );
        (*iSeg)->setMCRichTrack( mcTrack );
        if ( msgLevel(MSG::DEBUG) )
        {
          debug() << " Adding " << (*iSeg)->radiator()
                  << " MCRichSegment " << (*iSeg)->key() << endreq;
        }
      }

      // Locate GiGaTrajectory for this MCParticle
      const GiGaTrajectory * traj = mcpToTraj[mcPart];
      if ( !traj )
      {
        Warning( "Failed to find trajectory for MCParticle" );
        continue;
      }

      // debug printout
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << " Total number of trajectory points = " << traj->GetPointEntries()
                << endreq;
        if ( msgLevel(MSG::VERBOSE) )
        {
          for ( int iPoint = 0; iPoint < traj->GetPointEntries(); ++iPoint )
          {
            verbose() << "  TrajPoint " << iPoint << " "
                      << traj->point(iPoint)->GetPosition() << endreq;
          }
        }
      }

    }

    // count tracks
    m_hitTally += richTracks->size();

  }
  else
  {
    info() << "No RichG4Hits to be converted since no Collections available"
           << endmsg;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode GetMCRichTracksAlg::finalize()
{
  const Rich::StatDivFunctor occ;

  info() << "Av. # MCRichTracks         : Overall = "
         << occ(m_hitTally,m_nEvts) << endreq;

  return GetMCRichInfoBase::finalize();  // must be called after all other actions
}

//=============================================================================
