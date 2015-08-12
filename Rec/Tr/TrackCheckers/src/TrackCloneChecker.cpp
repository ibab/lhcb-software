// $Id: TrackCloneChecker.cpp,v 1.4 2009-07-10 11:33:57 cattanem Exp $
// Include files

// local
#include "TrackCloneChecker.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackCloneChecker
//
// 2007-09-13 : Chris Jones
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackCloneChecker )

//=============================================================================
// Standard constructor
//=============================================================================
TrackCloneChecker::TrackCloneChecker( const std::string& name,
                                      ISvcLocator* pSvcLocator )
  : TrackCheckerBase ( name , pSvcLocator ),
    m_nEvts          ( 0 )
{
  declareProperty( "CloneLinkerLocation",
                   m_cloneInfoTES = LHCb::TrackLocation::Default+"Clones");
  declareProperty( "CloneCut", m_klCut = 5000 );
}

//=============================================================================
// Destructor
//=============================================================================
TrackCloneChecker::~TrackCloneChecker() {}

//=============================================================================
// initialize
//=============================================================================
StatusCode TrackCloneChecker::initialize()
{
  const StatusCode sc = TrackCheckerBase::initialize();
  if ( sc.isFailure() ) return sc;

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackCloneChecker::execute()
{

  // Get the track clone linker info
  LinkedFrom<LHCb::Track,LHCb::Track> linker( evtSvc(), msgSvc(), m_cloneInfoTES );
  if ( linker.notFound() )
  {
    return Warning("Failed to load Track clone information from "+m_cloneInfoTES,
                   StatusCode::SUCCESS);
  }

  // get the input data
  LHCb::Tracks* tracks = get<LHCb::Tracks>(inputContainer());

  // set up stuff for this event
  initializeEvent();
  ++m_nEvts;

  // loop over tracks
  for ( LHCb::Tracks::const_iterator iTk = tracks->begin();
        iTk != tracks->end(); ++iTk )
  {
    if ( msgLevel(MSG::DEBUG) && (*iTk)->hasInfo(LHCb::Track::CloneDist) )
    {
      debug() << "Track " << (*iTk)->key() << " given CloneDist "
              << (*iTk)->info(LHCb::Track::CloneDist,1e99)
              << endmsg;
    }

    // MCP for main track
    const LHCb::MCParticle * mcP = mcTruth(*iTk);
    //if ( !mcP ) continue;
    if ( !selected(mcP) ) continue;

    debug() << "Track " << (*iTk)->key() << " " << (*iTk)->history() << endmsg;
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << **iTk << endmsg;

    // tally object
    TrackTally & tally = m_trackMap[(*iTk)->history()];

    // pick up the clone info for this track
    LHCb::Track * cloneTrack = linker.first( *iTk );
    while ( cloneTrack != NULL )
    {
      debug() << " -> Clone Info Found :-" << endmsg;

      if ( mcP )
      {
        // MCP for clone track
        const LHCb::MCParticle * mcP_clone = mcTruth(cloneTrack);
        const bool mcSel = ( mcP_clone ? selected(mcP_clone) : false );

        // log10(KLdistance)
        const double logFLdist = log10(linker.weight());
        // const bool cloneID = ( linker.weight() < m_klCut );

        if ( mcP_clone && mcSel )
        {
          if ( mcP == mcP_clone )
          {
            debug() << "  -> True Clone : klDist = " << linker.weight() << endmsg;
            plot1D( logFLdist, "KLDtrueClones", "Log10(KLDistance) | True Clones", -5, 13, 100 );
          }
          else
          {
            debug() << "  -> Not a Clone : klDist = " << linker.weight() << endmsg;
            plot1D( logFLdist, "KLDnotClones", "Log10(KLDistance) | Not Clones", -5, 13, 100 );
          }
        }
        else if ( mcP_clone && !mcSel )
        {
          debug() << "  -> Rejected MCP : klDist = " << linker.weight() << endmsg;
          plot1D( logFLdist, "KLDrejMCPs", "Log10(KLDistance) | Rejected MCParticles", -5, 13, 100 );
        }
        else
        {
          debug() << "  -> Ghost track : klDist = " << linker.weight() << endmsg;
          plot1D( logFLdist, "KLDghosts", "Log10(KLDistance) | Ghosts", -5, 13, 100 );
        }
      }

      cloneTrack = linker.next();
    } // clone track

    // clone ID
    const bool cloneID = ( (*iTk)->info(LHCb::Track::CloneDist,9e99) < m_klCut );

    // real clone ?
    const bool hasMCclones = hasMCClone( mcP );

    // tallies
    if ( mcP )
    {
      if ( msgLevel(MSG::DEBUG) )
      {
        if ( (*iTk)->hasInfo(LHCb::Track::CloneDist) )
          debug() << " -> Clone Flag "
                  << LHCb::Track::CloneDist << " = "
                  << (*iTk)->info(LHCb::Track::CloneDist,9e99) << endmsg;
        if ( hasMCclones  && cloneID ) debug() << "  -> REJECTED CLONE" << endmsg;
        if ( !hasMCclones && cloneID ) debug() << "  -> REJECTED NONCLONE" << endmsg;
      }
      if ( hasMCclones  ) ++tally.totalClones;
      if ( !hasMCclones ) ++tally.totalNonClones;
      if ( hasMCclones  && cloneID ) ++tally.rejectedClones;
      if ( !hasMCclones && cloneID ) ++tally.rejectedNonClones;
    }
    else
    {
      ++tally.totalGhosts;
      if ( cloneID ) ++tally.rejectedGhosts;
    }

  } // track loop

  return StatusCode::SUCCESS;
}

StatusCode TrackCloneChecker::finalize()
{
  const std::string & lines
    = "============================================================================================";
  always() << lines << endmsg;
  always() << "      Clone summary for '" << m_cloneInfoTES
           << "' IDed clones with KLdist<" <<  m_klCut<< endmsg;
  always() << lines << endmsg;

  std::pair<double,double> r1,r2,r3,r4,r5;

  always() << "   Track type     NonClones/Evt  Clones/Evt     ClonesID/%     NonClonesID/%  GhostsID/%" << endmsg;
  for ( TrackTally::Map::const_iterator iM = m_trackMap.begin(); iM != m_trackMap.end(); ++iM )
  {
    const TrackTally & tally = iM->second;
    r1 = getEff1( tally.rejectedClones, tally.totalClones/2.0 );
    r2 = getEff1( tally.rejectedNonClones, tally.totalNonClones );
    r3 = getEff1( tally.rejectedGhosts, tally.totalGhosts );
    r4 = getEff2( tally.totalNonClones, m_nEvts );
    r5 = getEff2( tally.totalClones/2.0, m_nEvts );
    always() << boost::format( "%15s %6.2f +-%5.2f %6.2f +-%5.2f %6.2f +-%5.2f %6.2f +-%5.2f %6.2f +-%5.2f" )
      % iM->first
      % r4.first % r4.second % r5.first % r5.second
      % r1.first % r1.second % r2.first % r2.second % r3.first % r3.second
             << endmsg;
  }

  always() << lines << endmsg;

  return TrackCheckerBase::finalize();
}

//=============================================================================
