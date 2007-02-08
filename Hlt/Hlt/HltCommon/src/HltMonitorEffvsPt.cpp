// $Id: HltMonitorEffvsPt.cpp,v 1.1 2007-02-08 17:38:06 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "HltBase/ParserDescriptor.h"
// local
#include "HltMonitorEffvsPt.h"
#include "HltBase/HltFunctions.h"

#include "L0Names.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltMonitorEffvsPt
//
// 2006-09-01 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltMonitorEffvsPt );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltMonitorEffvsPt::HltMonitorEffvsPt( const std::string& name,
				      ISvcLocator* pSvcLocator)
  : HltMonitorAlgorithm ( name , pSvcLocator )
{

  declareProperty("L0DULocation", 
		  m_l0Location = LHCb::L0DUReportLocation::Default );
  declareProperty("TracksLocation", 
		  m_tracksLocation  = LHCb::TrackLocation::Default);

  declareProperty("MaxPt",m_maxPt = 10000.);
  declareProperty("NBins",m_nBins = 20);
  declareProperty("L0ChannelsName", m_l0ChannelsName);
}
//=============================================================================
// Destructor
//=============================================================================
HltMonitorEffvsPt::~HltMonitorEffvsPt() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltMonitorEffvsPt::initialize() {
  StatusCode sc = HltMonitorAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  initializeHisto(m_histoPt,"Pt",0.,m_maxPt,m_nBins);
  initializeHisto(m_histoPtL0,"Pt L0",0.,m_maxPt,m_nBins);

  // set the IDs for the L0 channels
  m_l0IDs.clear();
  m_counterL0Channel.clear();
  m_histoL0Channel.clear();
  const std::vector< std::string > values = m_l0ChannelsName.value();
  for (std::vector< std::string>::const_iterator it = values.begin();
       it != values.end(); ++it) {
    const std::string& name = *it;  
    int id = L0Names::l0ChannelID(name);
    if (id<0) warning() << " Not valid L0 Channel name " << name << endreq;
    else {
      info() << " accepting L0 channel " << name << " ID " << id << endreq;
      m_l0IDs.push_back(id);
      HltCounter hc;
      initializeCounter(hc,"L0 channel "+name);
      m_counterL0Channel.push_back(hc);
      HltHisto hh;
      initializeHisto(hh,"Pt L0 channel "+name,0.,m_maxPt,m_nBins);
      m_histoL0Channel.push_back(hh);
    }
  }


  // set the IDs for the L0 channels
  m_hltSelIDs.clear();
  m_counterHltSel.clear();  
  m_histoHltSel.clear();
  m_histoHltSel1.clear();
  const std::vector< std::string > values2 = m_selectionsName.value();
  for (std::vector< std::string>::const_iterator it = values2.begin();
       it != values2.end(); ++it) {
    const std::string& name = *it;  
    int id = HltNames::selectionSummaryID(name);
    if (id<0) info() << " Not valid hlt selection name " << name << endreq;
    else {
      info() << " accepting hlt selection " << name << " ID " << id << endreq;
      m_hltSelIDs.push_back(id);  
      HltCounter hc;
      initializeCounter(hc,"Hlt Selection "+name);
      m_counterHltSel.push_back(hc);
      HltHisto hh;
      initializeHisto(hh,"Pt Hlt Selection "+name,0.,m_maxPt,m_nBins);
      m_histoHltSel.push_back(hh);
      HltHisto hh1;
      initializeHisto(hh1,"Pt Hlt Selection "+name+" yes",0.,m_maxPt,m_nBins);
      m_histoHltSel1.push_back(hh1);
    };
  }

  // initializeHistosFromDescriptor();

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================

StatusCode HltMonitorEffvsPt::execute() {

  StatusCode sc = StatusCode::SUCCESS;

  bool ok = HltMonitorAlgorithm::beginExecute();
  if (!ok) return sc;

  double pt = referencePt();
  debug() << " pt of selected track " << pt << endreq;
  if (pt>=m_maxPt) pt = m_maxPt-0.1;
  fillHisto(m_histoPt,pt,1.);

  if (!retrieve(m_l0,m_l0Location)) return stop(" No L0 report");
  if (!m_l0->decision()) return sc;

  fillHisto(m_histoPtL0,pt,1.);

  if (!m_l0IDs.empty()) {
    ok = false;
    int i = 0;
    for (std::vector<int>::const_iterator it = m_l0IDs.begin();
         it != m_l0IDs.end(); it++, i++) {
      int id = (*it);
      bool ok1 = m_l0->channelDecision(id);
      debug() << " l0 channel " << id << "? " <<  ok1 << endreq;
      if (ok1) {
	increaseCounter(m_counterL0Channel[i]);
	fillHisto(m_histoL0Channel[i],pt,1.);
      }
      ok = ok || ok1;
    }
    if (!ok) return sc;
  }

  if (!m_hltSelIDs.empty()) {
    int i = 0;
    ok = false;
    for (std::vector<int>::const_iterator it = m_hltSelIDs.begin();
         it != m_hltSelIDs.end(); it++, i++) {
      int id = (*it);
      debug() << " Hlt Selection ID " << id 
	      << summary().hasSelectionSummary(id);
      if ( summary().hasSelectionSummary(id)) {
	increaseCounter(m_counterHltSel[i]);
	fillHisto(m_histoHltSel[i],pt,1.);
	bool ok2 = selectionSummary(id).decision();
	debug() << " Hlt Selection decision  " << id << " ? " 
		<< ok2 << endreq;
	if (ok2) 
	  fillHisto(m_histoHltSel1[i],pt,1.);
	ok = ok || ok2;
      } 
    }
    if (!ok) return sc;
  }

  ok = HltMonitorAlgorithm::endExecute();
  if (!ok) return sc;

  return sc;
}

double HltMonitorEffvsPt::referencePt() {
  
  bool ok = retrieve(m_tracks,m_tracksLocation);
  if (!ok) return 0.;

  Hlt::TrackContainer tracks;
  for (Tracks::iterator it = m_tracks->begin(); 
       it != m_tracks->end(); it++) {
    Track& track = (**it);
    if (track.checkType(Track::Long)) tracks.push_back(&track);
  }
  std::sort(tracks.begin(),tracks.end(),Hlt::SortTrackByPt());
  
  const Track& track = *(*tracks.begin());
  debug() << " selected track pt " << track.pt() 
          << " from " << tracks.size() 
          << " in TES " << m_tracks->size() << endreq;
  
  return track.pt();
}


//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltMonitorEffvsPt::finalize() {

  debug() << "==> Finalize" << endmsg;
  
  return HltMonitorAlgorithm::finalize();
}

//=============================================================================
