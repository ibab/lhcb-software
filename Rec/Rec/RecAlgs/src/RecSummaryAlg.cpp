// $Id: $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RecSummaryAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RecSummaryAlg
//
// 2011-01-19 : Chris Jones
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RecSummaryAlg );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RecSummaryAlg::RecSummaryAlg( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_richTool     ( NULL )
{
  declareProperty( "SummaryLocation",
                   m_summaryLoc = LHCb::RecSummaryLocation::Default );
  declareProperty( "TracksLocation",
                   m_trackLoc   = LHCb::TrackLocation::Default );
  declareProperty( "PVsLocation",
                   m_pvLoc      = LHCb::RecVertexLocation::Primary );
}

//=============================================================================
// Destructor
//=============================================================================
RecSummaryAlg::~RecSummaryAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode RecSummaryAlg::initialize()
{
  const StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  m_richTool = tool<Rich::DAQ::IRawBufferToSmartIDsTool>("Rich::DAQ::RawBufferToSmartIDsTool");

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RecSummaryAlg::execute()
{

  // Create a new summary object and save to the TES
  LHCb::RecSummary * summary = new LHCb::RecSummary();
  put( summary, m_summaryLoc );

  // can we add tracking info ?
  if ( exist<LHCb::Tracks>(m_trackLoc) )
  {

    // Load the reconstructed tracks
    const LHCb::Tracks * tracks = get<LHCb::Tracks>(m_trackLoc);

    // Count each track type (Is there a more elegant way to do this ?)
    unsigned int nLong(0), nDownstream(0), nUpstream(0), nVelo(0), nT(0);
    for ( LHCb::Tracks::const_iterator iTk = tracks->begin();
          iTk != tracks->end(); ++iTk )
    {
      if      ( (*iTk)->type() == LHCb::Track::Long       ) { ++nLong; }
      else if ( (*iTk)->type() == LHCb::Track::Downstream ) { ++nDownstream; }
      else if ( (*iTk)->type() == LHCb::Track::Upstream   ) { ++nUpstream; }
      else if ( (*iTk)->type() == LHCb::Track::Velo       ) { ++nVelo; }
      else if ( (*iTk)->type() == LHCb::Track::Ttrack     ) { ++nT; }
    }

    // Save track info tom summary
    summary->addInfo( LHCb::RecSummary::nLongTracks,       (double)nLong );
    summary->addInfo( LHCb::RecSummary::nDownstreamTracks, (double)nDownstream );
    summary->addInfo( LHCb::RecSummary::nUpstreamTracks,   (double)nUpstream );
    summary->addInfo( LHCb::RecSummary::nVeloTracks,       (double)nVelo );
    summary->addInfo( LHCb::RecSummary::nTTracks,          (double)nT );

  }
  else
  {
    Warning( "No tracks available at '"+m_trackLoc+"'" ).ignore();
  }

  // Do we have reconstructed PVs ?
  if ( exist<LHCb::RecVertices>(m_pvLoc) )
  {

    // Load the reconstructed PVs
    const LHCb::RecVertices * pvs = get<LHCb::RecVertices>(m_pvLoc);
    
    // Save PV information
    summary->addInfo( LHCb::RecSummary::nPVs, (double)pvs->size() );

  }
  else
  {
    Warning( "No PVs available at '"+m_pvLoc+"'" ).ignore();
  }

  // RICH information
  summary->addInfo( LHCb::RecSummary::nRich1Hits, 
                    (double)m_richTool->nTotalHits(Rich::Rich1) );
  summary->addInfo( LHCb::RecSummary::nRich2Hits, 
                    (double)m_richTool->nTotalHits(Rich::Rich2) );

  if ( msgLevel(MSG::DEBUG) ) { debug() << *summary << endmsg; }

  return StatusCode::SUCCESS;
}

//=============================================================================
