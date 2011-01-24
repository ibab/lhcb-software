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
    m_richTool     ( NULL ),
    m_otTool       ( NULL )
{
  declareProperty( "SummaryLocation",
                   m_summaryLoc = LHCb::RecSummaryLocation::Default );
  declareProperty( "TracksLocation",
                   m_trackLoc   = LHCb::TrackLocation::Default );
  declareProperty( "PVsLocation",
                   m_pvLoc      = LHCb::RecVertexLocation::Primary );
  declareProperty( "VeloClusterLocation", 
                   m_veloLoc    = LHCb::VeloClusterLocation::Default );
  declareProperty( "ITClusterLocation", 
                   m_itLoc      = LHCb::STClusterLocation::ITClusters );
  declareProperty( "SpdDigitLocation",
                   m_spdLoc     = LHCb::CaloDigitLocation::Spd );
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

  m_richTool = 
    tool<Rich::DAQ::IRawBufferToSmartIDsTool>
    ("Rich::DAQ::RawBufferToSmartIDsTool","RichSmartIDDecoder");
  m_otTool = tool<IOTRawBankDecoder>("OTRawBankDecoder");

  m_countVeloTracks = tool<ICountVeloTracks>("CountVeloTracks");

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
    int nLong(0), nDownstream(0), nUpstream(0), nT(0), nBack(0);
    for ( LHCb::Tracks::const_iterator iTk = tracks->begin();
          iTk != tracks->end(); ++iTk )
    {
      if      ( (*iTk)->type() == LHCb::Track::Long       ) { ++nLong; }
      else if ( (*iTk)->type() == LHCb::Track::Downstream ) { ++nDownstream; }
      else if ( (*iTk)->type() == LHCb::Track::Upstream   ) { ++nUpstream; }
      else if ( (*iTk)->type() == LHCb::Track::Ttrack     ) { ++nT; }
      else if ( (*iTk)->type() == LHCb::Track::Velo &&
		(*iTk)->checkFlag(LHCb::Track::Backward)  ) { ++nBack; }
    }
    int nVelo = m_countVeloTracks->nVeloTracks(*tracks);

    // Save track info to summary
    summary->addInfo( LHCb::RecSummary::nLongTracks,       nLong );
    summary->addInfo( LHCb::RecSummary::nDownstreamTracks, nDownstream );
    summary->addInfo( LHCb::RecSummary::nUpstreamTracks,   nUpstream );
    summary->addInfo( LHCb::RecSummary::nVeloTracks,       nVelo );
    summary->addInfo( LHCb::RecSummary::nBackTracks,       nBack );
    summary->addInfo( LHCb::RecSummary::nTTracks,          nT );

  }
  else
  {
    Warning( "No tracks available at '"+m_trackLoc+"'" ).ignore();
  }

  // PVs
  addSizeSummary<LHCb::RecVertices>( summary, LHCb::RecSummary::nPVs, m_pvLoc );

  // RICH information
  summary->addInfo( LHCb::RecSummary::nRich1Hits, 
                    m_richTool->nTotalHits(Rich::Rich1) );
  summary->addInfo( LHCb::RecSummary::nRich2Hits, 
                    m_richTool->nTotalHits(Rich::Rich2) );

  // Velo
  addSizeSummary<LHCb::VeloClusters>( summary, LHCb::RecSummary::nVeloClusters, m_veloLoc ); 

  // IT
  addSizeSummary<LHCb::STClusters>( summary, LHCb::RecSummary::nITClusters, m_itLoc ); 

  // OT
  summary->addInfo( LHCb::RecSummary::nOTClusters, 
                    m_otTool->totalNumberOfHits() );

  // SPD
  addSizeSummary<LHCb::CaloDigits>( summary, LHCb::RecSummary::nSPDhits, m_spdLoc );

  if ( msgLevel(MSG::DEBUG) ) { debug() << *summary << endmsg; }

  return StatusCode::SUCCESS;
}

//=============================================================================
