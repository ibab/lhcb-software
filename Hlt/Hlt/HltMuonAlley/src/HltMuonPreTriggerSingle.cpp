// $Id: HltMuonPreTriggerSingle.cpp,v 1.5 2007-06-25 21:02:12 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "HltMuonPreTriggerSingle.h"
#include "Event/HltEnums.h"
#include "HltBase/ESequences.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltMuonPreTriggerSingle
//
// 2006-08-28 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltMuonPreTriggerSingle );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltMuonPreTriggerSingle::HltMuonPreTriggerSingle( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{

  declareProperty("PtMin", m_ptMin = 0.);
  declareCondition("PtMin", m_ptMin);

}
//=============================================================================
// Destructor
//=============================================================================
HltMuonPreTriggerSingle::~HltMuonPreTriggerSingle() {
  delete _ptFun;
  delete _ptCut;
} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltMuonPreTriggerSingle::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_ptKey = HltNames::particleInfoID("PT");

  _ptFun = new Hlt::PT();
  Hlt::Info<Track> ptInfo(m_ptKey);
  _ptCut = (( ptInfo > m_ptMin).clone());
  //  _ptCut = (( *_ptFun > m_ptMin).clone());
  initializeHisto(m_histoPt,"PT",0.,6000.,100);
  initializeHisto(m_histoPt1,"PT1",0.,6000.,100);

  debug() << "==> Initialize" << endmsg;

  checkInput(m_inputTracks," input tracks");
  checkInput(m_outputTracks," output tracks");

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltMuonPreTriggerSingle::execute() {
//info()<<" sono qui "<<endreq;

  StatusCode sc = StatusCode::SUCCESS;
//info()<<" sono qui "<<endreq;

  ELoop::map(*m_inputTracks,*_ptFun,m_ptKey);
//  info()<<" size "<<*m_inputTracks->size()
  double pt1 = (*m_inputTracks->begin())->pt();
  fillHisto( m_histoPt1, pt1, 1.);

  // HltAlgorithm::monitor(*m_inputTracks,m_ptKey,m_histoPt);

  m_outputTracks->clear();
  ELoop::select(*m_inputTracks,*_ptCut,*m_outputTracks);
  int n1 = m_outputTracks->size();
  if (m_outputTracks->size() == 0) return stop(" No tracks in PT window");

  setDecisionType(HltEnums::Muon);

  if (m_debug) {
    debug() << " Pt1 " << pt1 << endreq;
    debug() << " number of tracks in PT window " << n1 << endreq;
  }
  return sc;
}



//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltMuonPreTriggerSingle::finalize() {

  debug() << "==> Finalize" << endmsg;

  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
