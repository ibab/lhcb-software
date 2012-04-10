// $Id: $

#include "TimingTuple.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TimingTuple
//
// 2010-08-18 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TimingTuple )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  TimingTuple::TimingTuple( const std::string& name,
                            ISvcLocator* pSvcLocator )
    : GaudiTupleAlg     ( name , pSvcLocator ),
      m_timerTool       ( NULL ),
      m_timer           ( 0    ),
      m_evtCounter      ( 0    )
{
  declareProperty( "RecSummaryLocation", 
                   m_recSummaryLoc = LHCb::RecSummaryLocation::Default );
}

//=============================================================================
// Destructor
//=============================================================================
TimingTuple::~TimingTuple() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TimingTuple::initialize()
{
  const StatusCode sc = GaudiTupleAlg::initialize(); 
  if ( sc.isFailure() ) return sc;  

  m_timerTool = tool<ISequencerTimerTool>( "SequencerTimerTool" ); // global tool
  m_timer = m_timerTool->addTimer( name() );
  m_timerTool->start(m_timer) ; // start it now

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TimingTuple::execute()
{

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  ++m_evtCounter;

  const double t =  m_timerTool->stop(m_timer) ; // stop
  if (msgLevel(MSG::DEBUG)) debug() << "Time is " << t  << endmsg ;
  m_timerTool->start(m_timer) ; // start again

  // get ODIN
  LHCb::ODIN* odin = get<LHCb::ODIN> ( LHCb::ODINLocation::Default );

  // Get Rec Summary object
  const LHCb::RecSummary * summary = get<LHCb::RecSummary>(m_recSummaryLoc);

  // Fill tuple
  Tuple tuple = nTuple( "TimingTuple", "TimingTuple" );

  fillTuple(tuple, "EventInSequence",m_evtCounter);
  fillTuple(tuple, "RunNumber", odin->runNumber());
  fillTuple(tuple, "EvtNumber", (int)odin->eventNumber());
  fillTuple(tuple, "Memory", (double)System::virtualMemory());
  fillTuple(tuple, "Time", t );

  fillTuple(tuple, "nPVs", summary->info(LHCb::RecSummary::nPVs,-999) );
  fillTuple(tuple, "VeloClusters", summary->info(LHCb::RecSummary::nVeloClusters,-999) );
  fillTuple(tuple, "TTClusters", summary->info(LHCb::RecSummary::nTTClusters,-999) );
  fillTuple(tuple, "ITClusters", summary->info(LHCb::RecSummary::nITClusters,-999) );
  fillTuple(tuple, "OTClusters", summary->info(LHCb::RecSummary::nOTClusters,-999) );
  fillTuple(tuple, "spdMult", summary->info(LHCb::RecSummary::nSPDhits,-999) );

  fillTuple(tuple, "MuonCoordsS0", summary->info(LHCb::RecSummary::nMuonCoordsS0,-999) );
  fillTuple(tuple, "MuonCoordsS1", summary->info(LHCb::RecSummary::nMuonCoordsS1,-999) );
  fillTuple(tuple, "MuonCoordsS2", summary->info(LHCb::RecSummary::nMuonCoordsS2,-999) );
  fillTuple(tuple, "MuonCoordsS3", summary->info(LHCb::RecSummary::nMuonCoordsS3,-999) );
  fillTuple(tuple, "MuonCoordsS4", summary->info(LHCb::RecSummary::nMuonCoordsS4,-999) );
  fillTuple(tuple, "MuonTracks", summary->info(LHCb::RecSummary::nMuonTracks,-999) );

  fillTuple(tuple, "BestTracks", summary->info(LHCb::RecSummary::nTracks,-999) );
  fillTuple(tuple, "BackwardTracks", summary->info(LHCb::RecSummary::nBackTracks,-999) );
  fillTuple(tuple, "VeloTracks", summary->info(LHCb::RecSummary::nVeloTracks,-999) );
  fillTuple(tuple, "LongTracks", summary->info(LHCb::RecSummary::nLongTracks,-999) );
  fillTuple(tuple, "DownstreamTracks", summary->info(LHCb::RecSummary::nDownstreamTracks,-999) );
  fillTuple(tuple, "UpstreamTracks", summary->info(LHCb::RecSummary::nUpstreamTracks,-999) );

  fillTuple(tuple, "Rich1Hits", summary->info(LHCb::RecSummary::nRich1Hits,-999) );
  fillTuple(tuple, "Rich2Hits", summary->info(LHCb::RecSummary::nRich2Hits,-999) );

  // write and return
  return tuple->write();
}
