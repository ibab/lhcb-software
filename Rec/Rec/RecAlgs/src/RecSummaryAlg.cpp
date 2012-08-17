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
DECLARE_ALGORITHM_FACTORY( RecSummaryAlg )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RecSummaryAlg::RecSummaryAlg( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm    ( name , pSvcLocator ),
    m_richTool        ( NULL ),
    m_otTool          ( NULL ),
    m_countVeloTracks ( NULL )
{
  declareProperty( "SummaryLocation",
                   m_summaryLoc = LHCb::RecSummaryLocation::Default );
  declareProperty( "TracksLocation",
                   m_trackLoc   = LHCb::TrackLocation::Default );
  declareProperty( "PVsLocation",
                   m_pvLoc      = LHCb::RecVertexLocation::Primary );
  declareProperty( "VeloClustersLocation", 
                   m_veloLoc    = LHCb::VeloClusterLocation::Default );
  declareProperty( "ITClustersLocation", 
                   m_itLoc      = LHCb::STClusterLocation::ITClusters );
  declareProperty( "TTClustersLocation", 
                   m_ttLoc      = LHCb::STClusterLocation::TTClusters );
  declareProperty( "SpdDigitsLocation",
                   m_spdLoc     = LHCb::CaloDigitLocation::Spd );
  declareProperty( "MuonCoordsLocation", 
                   m_muonCoordsLoc = LHCb::MuonCoordLocation::MuonCoords );
  declareProperty( "MuonTracksLocation",
                   m_muonTracksLoc = LHCb::TrackLocation::Muon );
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

  m_countVeloTracks = tool<ICountContainedObjects>("CountVeloTracks");

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
  const LHCb::Tracks * tracks = getIfExists<LHCb::Tracks>(m_trackLoc);
  if ( tracks )
  {

    // Save total number of tracks
    summary->addInfo( LHCb::RecSummary::nTracks, tracks->size() );

    // Count each track type
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
    const int nVelo = m_countVeloTracks->nObj(tracks);

    // Save track info by type to summary
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

  // TT
  addSizeSummary<LHCb::STClusters>( summary, LHCb::RecSummary::nTTClusters, m_ttLoc ); 

  // IT
  addSizeSummary<LHCb::STClusters>( summary, LHCb::RecSummary::nITClusters, m_itLoc ); 

  // OT
  summary->addInfo( LHCb::RecSummary::nOTClusters, 
                    m_otTool->totalNumberOfHits() );

  // SPD
  addSizeSummary<LHCb::CaloDigits>( summary, LHCb::RecSummary::nSPDhits, m_spdLoc );

  // Muon Tracks
  addSizeSummary<LHCb::Tracks>( summary, LHCb::RecSummary::nMuonTracks, m_muonTracksLoc );

  // Muon Coords
  const LHCb::MuonCoords * coords = getIfExists<LHCb::MuonCoords>(m_muonCoordsLoc);
  if ( coords )
  {

    // Count by stations
    std::map<int,unsigned int> mCount;
    for ( LHCb::MuonCoords::const_iterator iC = coords->begin();
          iC != coords->end(); ++iC ) { ++mCount[(*iC)->key().station()]; }

    // Save to summary
    if ( mCount[0]>0 ) { summary->addInfo( LHCb::RecSummary::nMuonCoordsS0, mCount[0] ); }
    if ( mCount[1]>0 ) { summary->addInfo( LHCb::RecSummary::nMuonCoordsS1, mCount[1] ); }
    if ( mCount[2]>0 ) { summary->addInfo( LHCb::RecSummary::nMuonCoordsS2, mCount[2] ); }
    if ( mCount[3]>0 ) { summary->addInfo( LHCb::RecSummary::nMuonCoordsS3, mCount[3] ); }
    if ( mCount[4]>0 ) { summary->addInfo( LHCb::RecSummary::nMuonCoordsS4, mCount[4] ); }

  }
  else
  {
    Warning( "No MuonCoords available at '"+m_muonCoordsLoc+"'" ).ignore();
  }

  if ( msgLevel(MSG::DEBUG) ) { debug() << *summary << endmsg; }

  return StatusCode::SUCCESS;
}

//=============================================================================
