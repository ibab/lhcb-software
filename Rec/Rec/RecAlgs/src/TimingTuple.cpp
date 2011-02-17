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
      m_rawBankDecoder  ( NULL ),
      m_fromRawTool     ( ""   ),
      m_l0BankDecoder   ( NULL ),
      m_evtCounter      ( 0    ),
      m_richTool        ( NULL ),
      m_countVeloTracks ( NULL )
{ }

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

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  m_rawBankDecoder = tool<IOTRawBankDecoder>("OTRawBankDecoder");
  m_l0BankDecoder = tool<IL0DUFromRawTool>("L0DUFromRawTool");
  m_richTool =
    tool<Rich::DAQ::IRawBufferToSmartIDsTool>
    ("Rich::DAQ::RawBufferToSmartIDsTool","RichSmartIDDecoder");
  m_countVeloTracks = tool<ICountContainedObjects>("CountVeloTracks");
  m_timerTool = tool<ISequencerTimerTool>( "SequencerTimerTool" ); //global tool
  m_timer = m_timerTool->addTimer( name() );
  m_timerTool->start(m_timer) ; /// start it now
  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TimingTuple::execute()
{

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  ++m_evtCounter;

  double t =  m_timerTool->stop(m_timer) ; /// stop
  if (msgLevel(MSG::DEBUG)) debug() << "Time is " << t  << endmsg ;
  m_timerTool->start(m_timer) ; /// start again

  Tuple tuple = nTuple( "TimingTuple", "TimingTuple" );
  LHCb::ODIN* odin = get<LHCb::ODIN> ( LHCb::ODINLocation::Default );
  fillTuple(tuple,  "EventInSequence",m_evtCounter);
  fillTuple(tuple,  "RunNumber", odin->runNumber());
  fillTuple(tuple,  "EvtNumber", (int)odin->eventNumber());
  fillTuple(tuple,  "Memory", (double)System::virtualMemory());
  fillTuple(tuple,  "Time", t);

  fillTuple(tuple, "VeloClusters",number<LHCb::VeloClusters>(LHCb::VeloClusterLocation::Default));
  fillTuple(tuple, "ITClusters",number<LHCb::STClusters>(LHCb::STClusterLocation::ITClusters));
  const unsigned int nHitsInOT = m_rawBankDecoder->totalNumberOfHits();
  fillTuple(tuple, "OTClusters", nHitsInOT);
  m_l0BankDecoder->fillDataMap();
  bool l0BankDecoderOK = m_l0BankDecoder->decodeBank();
  if(!l0BankDecoderOK ){
    Error("Readout error : unable to monitor the L0DU rawBank", StatusCode::SUCCESS,StatusCode::SUCCESS).ignore();
    return StatusCode::SUCCESS;
  }
  int nSpd = m_l0BankDecoder->data("Spd(Mult)");
  fillTuple(tuple, "spdMult", nSpd);

  const LHCb::Track::Container* tracks =  get<LHCb::Track::Container> ( LHCb::TrackLocation::Default ) ;

  unsigned int nBack = 0;
  int veloTracks = 0;

  if (msgLevel(MSG::VERBOSE)) verbose() << "Number of Tracks " << tracks->size() << endmsg ;

  // Protection from empty track container
  if( tracks!=0 && !tracks->empty() ) {
    LHCb::Tracks::const_iterator iterT = tracks->begin();
    for(; iterT != tracks->end() ;++iterT) {
      if ((*iterT)->checkFlag( LHCb::Track::Backward) == true) ++nBack;
    }
    veloTracks = m_countVeloTracks->nObj(tracks);
  }

  fillTuple(tuple, "backwardTracks", nBack);
  fillTuple(tuple, "veloTracks", veloTracks);
  fillTuple(tuple, "BestTracks",number<LHCb::Tracks>(LHCb::TrackLocation::Default));
  fillTuple(tuple, "HPDHits", m_richTool->nTotalHits());

  return tuple->write();

}
