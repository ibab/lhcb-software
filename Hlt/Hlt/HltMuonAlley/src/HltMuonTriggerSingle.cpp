// $Id: HltMuonTriggerSingle.cpp,v 1.1.1.1 2006-09-25 16:29:11 asatta Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "HltMuonTriggerSingle.h"
#include "Event/HltEnums.h"
#include "HltCommon/ESequences.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltMuonTriggerSingle
//
// 2006-08-28 : Alessia Satta 
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltMuonTriggerSingle );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltMuonTriggerSingle::HltMuonTriggerSingle( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{

  declareProperty("PtMin", m_ptMin = 0.);
  declareCondition("PtMin", m_ptMin);

}
//=============================================================================
// Destructor
//=============================================================================
HltMuonTriggerSingle::~HltMuonTriggerSingle() {
  delete _ptFun;
  delete _ptCut;
} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltMuonTriggerSingle::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_ptKey = HltNames::particleInfoID("PT");
  //  m_ipKey = HltNames::particleInfoID("IP");

  _ptFun = new Hlt::PT();
  //  _ipFun = new Hlt::IP();

  Hlt::Info<Track> ptInfo(m_ptKey);
  _ptCut = (( ptInfo > m_ptMin).clone());
  //  _ptCut = (( *_ptFun > m_ptMin).clone());
  initializeHisto(m_histoPt,"PT",0.,6000.,100);
  //  initializeHisto(m_histoIP,"IP",0.,10.,100);

  debug() << "==> Initialize" << endmsg;

  checkInput(m_inputTracks," input tracks");
  checkInput(m_outputTracks," output tracks");

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltMuonTriggerSingle::execute() {
//info()<<" sono qui "<<endreq;

  StatusCode sc = StatusCode::SUCCESS;
//info()<<" sono qui "<<endreq;
  bool ok  = HltAlgorithm::beginExecute();
  if (!ok) return stop(" No input tracks");

  ELoop::map(*m_inputTracks,*_ptFun,m_ptKey);
  //  ELoop::map(*m_inputTracks,*_ipFun,m_ipKey);
//  info()<<" size "<<*m_inputTracks->size()
//  double pt1 = (*m_inputTracks->begin())->pt();
//  fillHisto( m_histoPt1, pt1, 1.);

  HltAlgorithm::monitor(*m_inputTracks,m_ptKey,m_histoPt);
  //  HltAlgorithm::monitor(*m_inputTracks,m_ipKey,m_histoIP);

  m_outputTracks->clear();
  ELoop::select(*m_inputTracks,*_ptCut,*m_outputTracks);
  int n1 = m_outputTracks->size();
  if (m_outputTracks->size() == 0) return stop(" No tracks in PT window");

  HltAlgorithm::endExecute();

  setDecisionType(HltEnums::Muon);
  saveInSummary(*m_outputTracks);

  if (m_debug) {
    //    debug() << " Pt1 " << pt1 << endreq;
    debug() << " number of tracks in PT window " << n1 << endreq;
  }
  return sc;
}



//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltMuonTriggerSingle::finalize() {

  debug() << "==> Finalize" << endmsg;

  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
