// $Id: HltL0Monitor.cpp,v 1.2 2007-06-20 16:06:21 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/L0CaloCandidate.h"
#include "Event/L0MuonCandidate.h"

// local
#include "HltL0Monitor.h"
#include "Event/HltNames.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltL0Monitor
//
// 2006-01-16 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltL0Monitor );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltL0Monitor::HltL0Monitor( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
    , m_CaloDataLocation ( LHCb::L0ProcessorDataLocation::Calo )
    , m_MuonDataLocation ( LHCb::L0ProcessorDataLocation::Muon )
    , m_PileUpDataLocation ( LHCb::L0ProcessorDataLocation::PileUp )
    , m_L0DUReportLocation ( LHCb::L0DUReportLocation::Default  )
{
  
  declareProperty("L0DUReportLocation", m_L0DUReportLocation );

  declareProperty( "ADCconvert", m_ADCconvert = true );  
}
//=============================================================================
// Destructor
//=============================================================================
HltL0Monitor::~HltL0Monitor() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltL0Monitor::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_CaloCandidate   = tool<IL0Candidate>("L0CandidateTool",
                                         "L0CaloCandidate",this);
  m_MuonCandidate   = tool<IL0Candidate>("L0CandidateTool",
                                         "L0MuonCandidate",this);
  m_PileUpCandidate = tool<IL0Candidate>("L0CandidateTool",
                                         "L0PileUpCandidate",this);

  m_CaloCandidate   -> setUp(m_CaloDataLocation     , m_ADCconvert);
  m_MuonCandidate   -> setUp(m_MuonDataLocation     , m_ADCconvert);
  m_PileUpCandidate -> setUp(m_PileUpDataLocation   , m_ADCconvert);

  initializeHisto(m_histoL0,"L0",0.,14.,28);

  initializeHisto(m_histoEt,"Et",0., 40000.,100);
  initializeHisto(m_histoSPDMult,"SPD",0.,500.,100);

  initializeHisto(m_histoPUHits,"PUHits",0.,200.,100);
  initializeHisto(m_histoPUPeak2,"PUPeak2",0.,15.,100);
  
  initializeHisto(m_histoEtEle,  "EtEle",0.,6000.,100);
  initializeHisto(m_histoEtGamma,"EtGamma",0.,6000.,100);
  initializeHisto(m_histoEtHad,  "EtHad",0.,6000.,100);
  initializeHisto(m_histoEtPi0L, "EtPi0L",0.,6000.,100);
  initializeHisto(m_histoEtPi0G, "EtPi0G",0.,6000.,100);

  initializeHisto(m_histoPtMu,"PtMu",0.,4000.,100);
  initializeHisto(m_histoPtDiMu,"PtDiMu",0.,4000.,100);

  initializeHisto(m_histoEtHad1,  "EtHad1",0.,6000.,100);
  initializeHisto(m_histoPtMu1,   "PtMu1",0.,4000.,100);
  initializeHisto(m_histoPtDiMu1,"PtDiMu1",0.,4000.,100);

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltL0Monitor::execute() {  

  if (!m_monitor) stop(" No monitor ?");

  m_l0 = get<L0DUReport>(m_L0DUReportLocation);
  if (!m_l0) return stop(" No L0 report");
  
  monitorL0();

  monitorGlobalEvent();
  
  monitorCalo();
  
  monitorMuon();

  return StatusCode::SUCCESS;
}


void HltL0Monitor::monitorL0() {

  bool ok = m_l0->decision();
  if (!ok) return;
  
  for (int i = 0; i<14; i+=1) 
    if (m_l0->channelDecision(i)) {
      fillHisto( m_histoL0, 1.*i , 1.);
      debug() << " l0 decision " << i << endreq;
    }
  
  debug() << " accepted  l0 entry " << endreq;
};


void HltL0Monitor::monitorGlobalEvent() {

  m_veto = false;
  for (int i = 0; i <4; ++i) {
    m_veto = m_veto || !(m_l0->conditionValue(i));
    if (!m_l0->conditionValue(i))
      debug() << " condition " << i << endreq;
  }

  double Et      =  m_CaloCandidate->value( L0DUBase::Sum::Et);
  double SPDMult =  m_CaloCandidate->value( L0DUBase::Spd::Mult);

  fillHisto(m_histoEt,      Et,1.);
  fillHisto(m_histoSPDMult, SPDMult,1.);  

  double PUHits  =  m_PileUpCandidate->value(L0DUBase::PileUp::Hits);
  double PUPeak2 =  m_PileUpCandidate->value(L0DUBase::PileUp::Peak2);

  fillHisto(m_histoPUHits,  PUHits,1.);
  fillHisto(m_histoPUPeak2, PUPeak2,1.);
  
  debug() << " Veto " << m_veto << endreq;
  debug() << " L0 Et " << Et << endreq;
  debug() << " L0 SPDMult " << SPDMult << endreq;  
  debug() << " L0 PUHits " << PUHits << endreq;
  debug() << " L0 PUPeak2 " << PUPeak2 << endreq;  

}


void HltL0Monitor::monitorCalo() 
{

  if (m_veto) return;

  double EtEle   =  m_CaloCandidate->value( L0DUBase::Electron::Et);
  double EtGamma =  m_CaloCandidate->value( L0DUBase::Photon::Et);
  double EtHad   =  m_CaloCandidate->value( L0DUBase::Hadron::Et);
  double EtPi0G  =  m_CaloCandidate->value( L0DUBase::Pi0Global::Et);
  double EtPi0L  =  m_CaloCandidate->value( L0DUBase::Pi0Local::Et);

  fillHisto(m_histoEtEle,  EtEle,1.);
  fillHisto(m_histoEtGamma,EtGamma,1.);
  fillHisto(m_histoEtHad,  EtHad,1.);
  fillHisto(m_histoEtPi0L, EtPi0L,1.);
  fillHisto(m_histoEtPi0G, EtPi0G,1.);

  if (m_l0->channelDecision(9)) fillHisto(m_histoEtHad1, EtHad, 1.);

  debug() << " L0 Et Ele "   << EtEle << endreq;
  debug() << " L0 Et Gamma " << EtGamma << endreq;
  debug() << " L0 Et Had "   << EtHad << endreq;
  debug() << " L0 Et Pi0L "  << EtPi0L << endreq;
  debug() << " L0 Et Pi0G "  << EtPi0G << endreq;
}

void HltL0Monitor::monitorMuon() {
  
  std::vector<double> ptmus;
  
  ptmus.push_back(m_MuonCandidate->value( L0DUBase::Muon1::Pt));
  ptmus.push_back(m_MuonCandidate->value( L0DUBase::Muon2::Pt));
  ptmus.push_back(m_MuonCandidate->value( L0DUBase::Muon3::Pt));
  ptmus.push_back(m_MuonCandidate->value( L0DUBase::Muon4::Pt));
  ptmus.push_back(m_MuonCandidate->value( L0DUBase::Muon5::Pt));
  ptmus.push_back(m_MuonCandidate->value( L0DUBase::Muon6::Pt));
  ptmus.push_back(m_MuonCandidate->value( L0DUBase::Muon7::Pt));
  ptmus.push_back(m_MuonCandidate->value( L0DUBase::Muon8::Pt));
  
  std::sort(ptmus.begin(),ptmus.end());
  double ptmu = ptmus[7];
  double ptdimu = ptmus[6]+ptmus[7];

  fillHisto(m_histoPtMu,ptmu,1.);
  fillHisto(m_histoPtDiMu,ptdimu,1.);
 
  if (m_l0->channelDecision(11)) fillHisto(m_histoPtMu1,ptmu,1.);
  if (m_l0->channelDecision(13)) fillHisto(m_histoPtDiMu1,ptdimu,1.);
 
  debug() << " L0 Pt mu " << ptmu << endreq;
  debug() << " L0 Pt dimu " << ptdimu << endreq;
  for (int i = 0; i < 8; i++) debug() << " ptmu i " << ptmus[i] << endreq;

}


//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltL0Monitor::finalize() {

  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
