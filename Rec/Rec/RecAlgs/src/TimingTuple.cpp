// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiAlg/ISequencerTimerTool.h"

// local
#include "TimingTuple.h"
// from DAQEvent
#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "Event/ODIN.h"
#include "GaudiKernel/Memory.h"
#include "Event/STCluster.h"
#include "Event/VeloCluster.h"
#include "Event/Track.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TimingTuple
//
// 2010-08-18 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TimingTuple );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TimingTuple::TimingTuple( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator ), 
    m_timerTool(0),
    m_timer(0),
    m_fromRawTool(),
    m_l0BankDecoder(),
    m_evtCounter(0),
    m_richTool()
{

}
//=============================================================================
// Destructor
//=============================================================================
TimingTuple::~TimingTuple() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TimingTuple::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTupleAlg

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  m_rawBankDecoder = tool<IOTRawBankDecoder>("OTRawBankDecoder");
  m_l0BankDecoder = tool<IL0DUFromRawTool>("L0DUFromRawTool");
  m_richTool = 
    tool<Rich::DAQ::IRawBufferToSmartIDsTool>("Rich::DAQ::RawBufferToSmartIDsTool","RichSmartIDDecoder");
  m_timerTool = tool<ISequencerTimerTool>( "SequencerTimerTool" ); //global tool
  m_timer = m_timerTool->addTimer( name() );
  m_timerTool->start(m_timer) ; /// start it now
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TimingTuple::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  m_evtCounter++;

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
    veloTracks = nVelo(tracks);
  }
    
  fillTuple(tuple, "backwardTracks", nBack);
  fillTuple(tuple, "veloTracks", veloTracks);
  fillTuple(tuple, "BestTracks",number<LHCb::Tracks>(LHCb::TrackLocation::Default));
  fillTuple(tuple, "HPDHits", m_richTool->nTotalHits());
  
  return tuple->write();

}

//=============================================================================
// nVelo by Matt, with a protection by PK
//============================================================================
unsigned TimingTuple::nVelo(const LHCb::Tracks* tracks) {

  if (msgLevel(MSG::VERBOSE)) verbose() << "nVelo" << endmsg ;
    std::vector<LHCb::Track*> tmpCont;
    LHCb::Tracks::const_iterator iterT = tracks->begin();
    for(; iterT != tracks->end() ;++iterT) {
      if ((*iterT)->hasVelo() == true) tmpCont.push_back(*iterT); 
    }
    if (msgLevel(MSG::VERBOSE)) verbose() << "tmpCont " << tmpCont.size() << endmsg ;
    if (tmpCont.empty()) return 0;
    std::vector<LHCb::Track*> keepCont;
    keepCont.push_back(tmpCont.front());
    
    std::vector<LHCb::Track*>::const_iterator iterT2 = tmpCont.begin();
    for (;iterT2 != tmpCont.end(); ++iterT2 ){
      const std::vector<LHCb::LHCbID>& vids = (*iterT2)->lhcbIDs();
      std::vector<LHCb::LHCbID> veloHits; veloHits.reserve(vids.size());
      LoKi::select(vids.begin(), vids.end(), std::back_inserter(veloHits), boost::bind(&LHCb::LHCbID::isVelo,_1));
      
      if (inCloneContainer(keepCont,veloHits) == false){
        // nothing
      } 
      else {
        keepCont.push_back(*iterT2);   
      }
    } // iterT2
    if (msgLevel(MSG::VERBOSE)) verbose() << "keepCont " << keepCont.size() << endmsg ;
    return keepCont.size();
}
