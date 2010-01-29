// $Id: HltGlobalMonitor.cpp,v 1.49 2010-01-29 15:45:57 kvervink Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD&STL
// ============================================================================
#include <string>
#include <vector>
// ============================================================================
// BOOST
// ============================================================================
#include "boost/assign/list_of.hpp"
// ============================================================================
// AIDA
// ============================================================================
#include "AIDA/IHistogram1D.h"
#include "AIDA/IProfile1D.h"
#include "AIDA/IAxis.h"

// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IIncidentSvc.h"
// ============================================================================
// GaudiUtils
// ============================================================================
#include "GaudiUtils/HistoStats.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/HltDecReports.h"
// ============================================================================
// Memory Usage
// ============================================================================
#include "GaudiKernel/Timing.h"
#include "GaudiKernel/Memory.h"
// ============================================================================
// HLT
// ============================================================================
#include "HltBase/HltHistogramUtilities.h"
using namespace Hlt::HistogramUtilities;
// ============================================================================
// local
// ============================================================================
#include "HltGlobalMonitor.h"
// ============================================================================

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltGlobalMonitor
//
// 2007-05-16 : Bruno Souza de Paula
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltGlobalMonitor );

// utilities


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltGlobalMonitor::HltGlobalMonitor( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : HltBaseAlg ( name , pSvcLocator )
  , m_startClock(0)
  , m_startEvent(0)
  , m_currentTime(0), m_gpstimesec(0), m_virtmem(0), m_events(0), m_lastL0TCK(0)
{
  declareProperty("ODIN",              m_ODINLocation = LHCb::ODINLocation::Default);
  declareProperty("L0DUReport",        m_L0DUReportLocation = LHCb::L0DUReportLocation::Default);
  declareProperty("HltDecReports",     m_HltDecReportsLocation = LHCb::HltDecReportsLocation::Default);
  declareProperty("Hlt1Decisions",     m_Hlt1Lines );
  declareProperty("Hlt2Decisions",     m_Hlt2Lines );
  declareProperty("ScanEvents",        m_scanevents = 1 );
  declareProperty("TotalMemory",       m_totalmem   = 3000 );
  declareProperty("TimeSize",          m_timeSize = 120 );   // number of minutes of history (half an hour)
  declareProperty("TimeInterval",      m_timeInterval = 1 ); // binwidth in minutes 
  declareProperty("DecToGroup",        m_DecToGroup);
  declareProperty("DecToGroup2",       m_DecToGroup2);
  declareProperty("GroupLabels",       m_GroupLabels);
  declareProperty("GroupLabels2",      m_GroupLabels2);
}
//=============================================================================
// Destructor
//=============================================================================
HltGlobalMonitor::~HltGlobalMonitor() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltGlobalMonitor::initialize() {
  StatusCode sc = HltBaseAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_gpstimesec=0;
  m_startClock = System::currentTime( System::microSec );
  

  m_L0Input         = book1D("L0 channel",-0.5,18.5,19);
  // this code may break when the enums are no longer directly exposing the hardware
  m_odin            = book1D("ODIN trigger type",  "ODIN trigger Type ",-0.5, 7.5, 8);
  std::vector<std::pair<unsigned,std::string> > odinLabels = boost::assign::list_of< std::pair<unsigned,std::string> >
                (ODIN::PhysicsTrigger,    "Physics")
                (ODIN::BeamGasTrigger,    "BeamGas")
                (ODIN::LumiTrigger,       "Lumi")
                (ODIN::TechnicalTrigger,  "Technical")
                (ODIN::AuxiliaryTrigger,  "Auxiliary")
                (ODIN::NonZSupTrigger,    "NonZSup")
                (ODIN::TimingTrigger,     "Timing")
                (ODIN::CalibrationTrigger,"Calibration");
   if (!setBinLabels( m_odin, odinLabels )) {
    error() << "failed to set binlables on ODIN hist" << endmsg;
  }

  // create a histogram with one bin per Alley
  // the order and the names for the bins are
  // configured in HLTConf/Configuration.py  
  m_hlt1alley       = book1D("Hlt1 Alleys", "Hlt1 Alleys", -0.5, m_GroupLabels.size()-0.5 , m_GroupLabels.size() );
  if (!setBinLabels( m_hlt1alley, m_GroupLabels )) {
    error() << "failed to set binlables on Alley hist" << endmsg;
  }

  m_hlt2alley       = book1D("Hlt2 Alleys", "Hlt2 Alleys", -0.5, m_GroupLabels2.size()-0.5 , m_GroupLabels2.size() );
  if (!setBinLabels( m_hlt2alley, m_GroupLabels2 )) {
    error() << "failed to set binlables on Hlt2 Alley hist" << endmsg;
  } 

  std::vector<std::string> labels;
  for (std::vector<std::string>::const_iterator i = m_Hlt1Lines.begin(); i!=m_Hlt1Lines.end();++i) {
      std::string s = *i;
      std::vector<std::string> strip = boost::assign::list_of<std::string>("Hlt1")("Decision"); //TODO: replace with regex with capture
      for (std::vector<std::string>::const_iterator j = strip.begin();j!=strip.end();++j) {
     	std::string::size_type k =  s.find(*j);
     	if (k != std::string::npos) s.erase(k,k+j->size());
      }
      labels.push_back(s);  
}

  std::vector<std::string> labels2;
  for (std::vector<std::string>::const_iterator i = m_Hlt2Lines.begin(); i!=m_Hlt2Lines.end();++i) {
      std::string s = *i;
      std::vector<std::string> strip = boost::assign::list_of<std::string>("Hlt2")("Decision"); //TODO: replace with regex with capture
      for (std::vector<std::string>::const_iterator j = strip.begin();j!=strip.end();++j) {
     	std::string::size_type k =  s.find(*j);
     	if (k != std::string::npos) s.erase(k,k+j->size());
      }
      labels2.push_back(s);  
}

  m_hlt1NAcc         = book1D("# positive Hlt1Lines ", -0.5,m_Hlt1Lines.size()+0.5, m_Hlt1Lines.size()+1);
  m_hlt2NAcc         = book1D("# positive Hlt2Lines ", -0.5,m_Hlt2Lines.size()+0.5, m_Hlt2Lines.size()+1);
  m_hlt1Inclusive    = book1D("Hlt1Lines accepted events",   -0.5,m_Hlt1Lines.size()-0.5, m_Hlt1Lines.size());
  m_hlt2Inclusive    = book1D("Hlt2Lines accepted events",   -0.5,m_Hlt2Lines.size()-0.5, m_Hlt2Lines.size());

  if (!setBinLabels( m_hlt1Inclusive,  labels )) {
    error() << "failed to set binlables on inclusive hist" << endmsg;
  }

  if (!setBinLabels( m_hlt2Inclusive,  labels2 )) {
    error() << "failed to set binlables on hlt2 inclusive hist" << endmsg;
  }
  
  m_hlt1Correlations = book2D("Hlt1Lines Correlations",-0.5,m_Hlt1Lines.size()-0.5,
                             m_Hlt1Lines.size(),-0.5,m_Hlt1Lines.size()-0.5,
                             m_Hlt1Lines.size());

  m_hlt2Correlations = book2D("Hlt2Lines Correlations",-0.5,m_Hlt2Lines.size()-0.5,
                             m_Hlt2Lines.size(),-0.5,m_Hlt2Lines.size()-0.5,
                             m_Hlt2Lines.size());

   if (!setBinLabels( m_hlt1Correlations, labels, labels )) {
    error() << "failed to set binlables on hlt1 correlation hist" << endmsg;
  }

   if (!setBinLabels( m_hlt2Correlations, labels2, labels2 )) {
    error() << "failed to set binlables on hlt2 correlation hist" << endmsg;
  } 

  m_hlt1AlleysCorrelations      = book2D("Hlt1Alleys Correlations", -0.5, m_GroupLabels.size()-0.5,
                                          m_GroupLabels.size(), -0.5, m_GroupLabels.size()-0.5,
                                          m_GroupLabels.size() );

  m_hlt2AlleysCorrelations      = book2D("Hlt2Alleys Correlations", -0.5, m_GroupLabels2.size()-0.5,
                                          m_GroupLabels2.size(), -0.5, m_GroupLabels2.size()-0.5,
                                          m_GroupLabels2.size() );

  if (!setBinLabels( m_hlt1AlleysCorrelations, m_GroupLabels, m_GroupLabels )) {
    error() << "failed to set binlables on Hlt1Alleys Correlation hist" << endmsg;
  }

  if (!setBinLabels( m_hlt2AlleysCorrelations, m_GroupLabels2, m_GroupLabels2 )) {
    error() << "failed to set binlables on Hlt2Alleys Correlation hist" << endmsg;
  }

                    /*One Histogram for each alley*/

    //for hlt1
    
  std::vector<std::string> labHlt1L0;
  std::vector<std::string> labHlt1Lumi;
  std::vector<std::string> labHlt1Velo;
  std::vector<std::string> labHlt1XPress;
  std::vector<std::string> labHlt1Hadron;
  std::vector<std::string> labHlt1SingleMuon;
  std::vector<std::string> labHlt1DiMuon;
  std::vector<std::string> labHlt1Electron;
  std::vector<std::string> labHlt1Photon;
  std::vector<std::string> labHlt1PA;
  std::vector<std::string> labHlt1BeamGas;
  std::vector<std::string> labHlt1MinBias;
  std::vector<std::string> labHlt1Other;

  for(std::map<std::string,int>::const_iterator i=m_DecToGroup.begin();i!=m_DecToGroup.end();i++){
    if((*i).second==0){
      std::string s = (*i).first;
      std::vector<std::string> strip = boost::assign::list_of<std::string>("Hlt1L0")("Decision"); //TODO: replace with regex with capture
      for (std::vector<std::string>::const_iterator j = strip.begin();j!=strip.end();++j) {
     	std::string::size_type k =  s.find(*j);
     	if (k != std::string::npos) s.erase(k,k+j->size());
      }
      labHlt1L0.push_back(s);
      m_Hlt1L0.push_back((*i).first);
    }

    if((*i).second==1){
      std::string s = (*i).first;
      std::vector<std::string> strip = boost::assign::list_of<std::string>("Hlt1Lumi")("Decision"); //TODO: replace with regex with capture
      for (std::vector<std::string>::const_iterator j = strip.begin();j!=strip.end();++j) {
     	std::string::size_type k =  s.find(*j);
     	if (k != std::string::npos) s.erase(k,k+j->size());
      }
      labHlt1Lumi.push_back(s);
      m_Hlt1Lumi.push_back((*i).first);
    }

    if((*i).second==2){
      std::string s = (*i).first;
      std::vector<std::string> strip = boost::assign::list_of<std::string>("Hlt1Velo")("Decision"); //TODO: replace with regex with capture
      for (std::vector<std::string>::const_iterator j = strip.begin();j!=strip.end();++j) {
     	std::string::size_type k =  s.find(*j);
     	if (k != std::string::npos) s.erase(k,k+j->size());
      }
      labHlt1Velo.push_back(s);
      m_Hlt1Velo.push_back((*i).first);
    }

    if((*i).second==3){
      std::string s = (*i).first;
      std::vector<std::string> strip = boost::assign::list_of<std::string>("Hlt1")("Decision"); //TODO: replace with regex with capture
      for (std::vector<std::string>::const_iterator j = strip.begin();j!=strip.end();++j) {
     	std::string::size_type k =  s.find(*j);
     	if (k != std::string::npos) s.erase(k,k+j->size());
      }
      labHlt1XPress.push_back(s);
      m_Hlt1XPress.push_back((*i).first);
    }

    if((*i).second==4){
      std::string s = (*i).first;
      std::vector<std::string> strip = boost::assign::list_of<std::string>("Hlt1")("Decision"); //TODO: replace with regex with capture
      for (std::vector<std::string>::const_iterator j = strip.begin();j!=strip.end();++j) {
     	std::string::size_type k =  s.find(*j);
     	if (k != std::string::npos) s.erase(k,k+j->size());
      }
      labHlt1Hadron.push_back(s);
      m_Hlt1Hadron.push_back((*i).first);
    }

    if((*i).second==5){
      std::string s = (*i).first;
      std::vector<std::string> strip = boost::assign::list_of<std::string>("Hlt1")("Decision"); //TODO: replace with regex with capture
      for (std::vector<std::string>::const_iterator j = strip.begin();j!=strip.end();++j) {
     	std::string::size_type k =  s.find(*j);
     	if (k != std::string::npos) s.erase(k,k+j->size());
      }
      labHlt1SingleMuon.push_back(s);
      m_Hlt1SingleMuon.push_back((*i).first);
    }

    if((*i).second==6){
      std::string s = (*i).first;
      std::vector<std::string> strip = boost::assign::list_of<std::string>("Hlt1DiMuon")("Decision"); //TODO: replace with regex with capture
      for (std::vector<std::string>::const_iterator j = strip.begin();j!=strip.end();++j) {
     	std::string::size_type k =  s.find(*j);
     	if (k != std::string::npos) s.erase(k,k+j->size());
      }
      labHlt1DiMuon.push_back(s);
      m_Hlt1DiMuon.push_back((*i).first);
    }

    if((*i).second==7){
      std::string s = (*i).first;
      std::vector<std::string> strip = boost::assign::list_of<std::string>("Hlt1")("Decision"); //TODO: replace with regex with capture
      for (std::vector<std::string>::const_iterator j = strip.begin();j!=strip.end();++j) {
     	std::string::size_type k =  s.find(*j);
     	if (k != std::string::npos) s.erase(k,k+j->size());
      }
      labHlt1Electron.push_back(s);
      m_Hlt1Electron.push_back((*i).first);
    }

    if((*i).second==8){
      std::string s = (*i).first;
      std::vector<std::string> strip = boost::assign::list_of<std::string>("Hlt1Photon")("Decision"); //TODO: replace with regex with capture
      for (std::vector<std::string>::const_iterator j = strip.begin();j!=strip.end();++j) {
     	std::string::size_type k =  s.find(*j);
     	if (k != std::string::npos) s.erase(k,k+j->size());
      }
      labHlt1Photon.push_back(s);
      m_Hlt1Photon.push_back((*i).first);
    }

    if((*i).second==9){
      std::string s = (*i).first;
      std::vector<std::string> strip = boost::assign::list_of<std::string>("Hlt1")("Decision"); //TODO: replace with regex with capture
      for (std::vector<std::string>::const_iterator j = strip.begin();j!=strip.end();++j) {
     	std::string::size_type k =  s.find(*j);
     	if (k != std::string::npos) s.erase(k,k+j->size());
      }
      labHlt1PA.push_back(s);
      m_Hlt1PA.push_back((*i).first);
    }

    if((*i).second==10){
      std::string s = (*i).first;
      std::vector<std::string> strip = boost::assign::list_of<std::string>("Hlt1")("Decision"); //TODO: replace with regex with capture
      for (std::vector<std::string>::const_iterator j = strip.begin();j!=strip.end();++j) {
     	std::string::size_type k =  s.find(*j);
     	if (k != std::string::npos) s.erase(k,k+j->size());
      }
      labHlt1BeamGas.push_back(s);
      m_Hlt1BeamGas.push_back((*i).first);
    }

    if((*i).second==11){
      std::string s = (*i).first;
      std::vector<std::string> strip = boost::assign::list_of<std::string>("Hlt1")("Decision"); //TODO: replace with regex with capture
      for (std::vector<std::string>::const_iterator j = strip.begin();j!=strip.end();++j) {
     	std::string::size_type k =  s.find(*j);
     	if (k != std::string::npos) s.erase(k,k+j->size());
      }
      labHlt1MinBias.push_back(s);
      m_Hlt1MinBias.push_back((*i).first);
    }

    if((*i).second==13){
      std::string s = (*i).first;
      std::vector<std::string> strip = boost::assign::list_of<std::string>("Hlt1")("Decision"); //TODO: replace with regex with capture
      for (std::vector<std::string>::const_iterator j = strip.begin();j!=strip.end();++j) {
     	std::string::size_type k =  s.find(*j);
     	if (k != std::string::npos) s.erase(k,k+j->size());
      }
      labHlt1Other.push_back(s);
      m_Hlt1Other.push_back((*i).first);
    }
  }

  m_hlt1L0Alley    = book1D("Hlt1 L0 Alley",   -0.5,m_Hlt1L0.size()-0.5, m_Hlt1L0.size());

  if (!setBinLabels( m_hlt1L0Alley,  labHlt1L0 )) {
    error() << "failed to set binlables on hlt1 L0 alley hist" << endmsg;
  }

  m_hlt1LumiAlley    = book1D("Hlt1 Lumi Alley",   -0.5,m_Hlt1Lumi.size()-0.5, m_Hlt1Lumi.size());

  if (!setBinLabels( m_hlt1LumiAlley,  labHlt1Lumi )) {
    error() << "failed to set binlables on hlt1 Lumi alley hist" << endmsg;
  }

  m_hlt1VeloAlley    = book1D("Hlt1 Velo Alley",   -0.5,m_Hlt1Velo.size()-0.5, m_Hlt1Velo.size());

  if (!setBinLabels( m_hlt1VeloAlley,  labHlt1Velo )) {
    error() << "failed to set binlables on hlt1 Velo alley hist" << endmsg;
  }

  m_hlt1XPressAlley    = book1D("Hlt1 XPress Alley",   -0.5,m_Hlt1XPress.size()-0.5, m_Hlt1XPress.size());

  if (!setBinLabels( m_hlt1XPressAlley,  labHlt1XPress )) {
    error() << "failed to set binlables on hlt1 XPress alley hist" << endmsg;
  }

  m_hlt1HadronAlley    = book1D("Hlt1 Hadron Alley",   -0.5,m_Hlt1Hadron.size()-0.5, m_Hlt1Hadron.size());

  if (!setBinLabels( m_hlt1HadronAlley,  labHlt1Hadron )) {
    error() << "failed to set binlables on hlt1 Hadron alley hist" << endmsg;
  }

  m_hlt1SingleMuonAlley    = book1D("Hlt1 SingleMuon Alley",   -0.5,m_Hlt1SingleMuon.size()-0.5, m_Hlt1SingleMuon.size());

  if (!setBinLabels( m_hlt1SingleMuonAlley,  labHlt1SingleMuon )) {
    error() << "failed to set binlables on hlt1 SingleMuon alley hist" << endmsg;
  }

  m_hlt1DiMuonAlley    = book1D("Hlt1 DiMuon Alley",   -0.5,m_Hlt1DiMuon.size()-0.5, m_Hlt1DiMuon.size());

  if (!setBinLabels( m_hlt1DiMuonAlley,  labHlt1DiMuon )) {
    error() << "failed to set binlables on hlt1 DiMuon alley hist" << endmsg;
  }

  m_hlt1ElectronAlley    = book1D("Hlt1 Electron Alley",   -0.5,m_Hlt1Electron.size()-0.5, m_Hlt1Electron.size());

  if (!setBinLabels( m_hlt1ElectronAlley,  labHlt1Electron )) {
    error() << "failed to set binlables on hlt1 Electron alley hist" << endmsg;
  }

  m_hlt1PhotonAlley    = book1D("Hlt1 Photon Alley",   -0.5,m_Hlt1Photon.size()-0.5, m_Hlt1Photon.size());

  if (!setBinLabels( m_hlt1PhotonAlley,  labHlt1Photon )) {
    error() << "failed to set binlables on hlt1 Photon alley hist" << endmsg;
  }

  m_hlt1PAAlley    = book1D("Hlt1 PA Alley",   -0.5,m_Hlt1PA.size()-0.5, m_Hlt1PA.size());

  if (!setBinLabels( m_hlt1PAAlley,  labHlt1PA )) {
    error() << "failed to set binlables on hlt1 PA alley hist" << endmsg;
  }

  m_hlt1BeamGasAlley    = book1D("Hlt1 BeamGas Alley",   -0.5,m_Hlt1BeamGas.size()-0.5, m_Hlt1BeamGas.size());

  if (!setBinLabels( m_hlt1BeamGasAlley,  labHlt1BeamGas )) {
    error() << "failed to set binlables on hlt1 BeamGas alley hist" << endmsg;
  }

  m_hlt1MinBiasAlley    = book1D("Hlt1 MinBias Alley",   -0.5,m_Hlt1MinBias.size()-0.5, m_Hlt1MinBias.size());

  if (!setBinLabels( m_hlt1MinBiasAlley,  labHlt1MinBias )) {
    error() << "failed to set binlables on hlt1 MinBias alley hist" << endmsg;
  }

  m_hlt1OtherAlley    = book1D("Hlt1 Other Alley",   -0.5,m_Hlt1Other.size()-0.5, m_Hlt1Other.size());

  if (!setBinLabels( m_hlt1OtherAlley,  labHlt1Other )) {
    error() << "failed to set binlables on hlt1 other alley hist" << endmsg;
  }

     //for hlt2

  std::vector<std::string> labHlt2Topological;
  std::vector<std::string> labHlt2InclusivePhi;
  std::vector<std::string> labHlt2SingleMuon;
  std::vector<std::string> labHlt2DiMuon;
  std::vector<std::string> labHlt2B2DX;
  std::vector<std::string> labHlt2B2XGamma;
  std::vector<std::string> labHlt2B2JpsiX;
  std::vector<std::string> labHlt2B2XPhi;
  std::vector<std::string> labHlt2B2HH;
  std::vector<std::string> labHlt2Express;
  std::vector<std::string> labHlt2Commissioning;
  std::vector<std::string> labHlt2DisplVertices;
  std::vector<std::string> labHlt2Other;

  for(std::map<std::string,int>::const_iterator i=m_DecToGroup2.begin();i!=m_DecToGroup2.end();i++){
    if((*i).second==0){
      std::string s = (*i).first;
      std::vector<std::string> strip = boost::assign::list_of<std::string>("Hlt2Topo")("Decision"); //TODO: replace with regex with capture
      for (std::vector<std::string>::const_iterator j = strip.begin();j!=strip.end();++j) {
     	std::string::size_type k =  s.find(*j);
     	if (k != std::string::npos) s.erase(k,k+j->size());
      }
       labHlt2Topological.push_back(s);
       m_Hlt2Topological.push_back((*i).first);
    }

    if((*i).second==1){
      std::string s = (*i).first;
      std::vector<std::string> strip = boost::assign::list_of<std::string>("Hlt2")("Decision"); //TODO: replace with regex with capture
      for (std::vector<std::string>::const_iterator j = strip.begin();j!=strip.end();++j) {
     	std::string::size_type k =  s.find(*j);
     	if (k != std::string::npos) s.erase(k,k+j->size());
      }
      labHlt2InclusivePhi.push_back(s);
      m_Hlt2InclusivePhi.push_back((*i).first);
    }

    if((*i).second==2){
      std::string s = (*i).first;
      std::vector<std::string> strip = boost::assign::list_of<std::string>("Hlt2")("Decision"); //TODO: replace with regex with capture
      for (std::vector<std::string>::const_iterator j = strip.begin();j!=strip.end();++j) {
     	std::string::size_type k =  s.find(*j);
     	if (k != std::string::npos) s.erase(k,k+j->size());
      }
      labHlt2SingleMuon.push_back(s);
      m_Hlt2SingleMuon.push_back((*i).first);
    }

    if((*i).second==3){
      std::string s = (*i).first;
      std::vector<std::string> strip = boost::assign::list_of<std::string>("Hlt2")("Decision"); //TODO: replace with regex with capture
      for (std::vector<std::string>::const_iterator j = strip.begin();j!=strip.end();++j) {
     	std::string::size_type k =  s.find(*j);
     	if (k != std::string::npos) s.erase(k,k+j->size());
      }
      labHlt2DiMuon.push_back(s);
      m_Hlt2DiMuon.push_back((*i).first);
    }

    if((*i).second==4){
      std::string s = (*i).first;
      std::vector<std::string> strip = boost::assign::list_of<std::string>("Hlt2")("Decision"); //TODO: replace with regex with capture
      for (std::vector<std::string>::const_iterator j = strip.begin();j!=strip.end();++j) {
     	std::string::size_type k =  s.find(*j);
     	if (k != std::string::npos) s.erase(k,k+j->size());
      }
      labHlt2B2DX.push_back(s);
      m_Hlt2B2DX.push_back((*i).first);
    }

    if((*i).second==5){
      std::string s = (*i).first;
      std::vector<std::string> strip = boost::assign::list_of<std::string>("Hlt2")("Decision"); //TODO: replace with regex with capture
      for (std::vector<std::string>::const_iterator j = strip.begin();j!=strip.end();++j) {
     	std::string::size_type k =  s.find(*j);
     	if (k != std::string::npos) s.erase(k,k+j->size());
      }
      labHlt2B2XGamma.push_back(s);
      m_Hlt2B2XGamma.push_back((*i).first);
    }

    if((*i).second==6){
      std::string s = (*i).first;
      std::vector<std::string> strip = boost::assign::list_of<std::string>("Hlt2")("Decision"); //TODO: replace with regex with capture
      for (std::vector<std::string>::const_iterator j = strip.begin();j!=strip.end();++j) {
     	std::string::size_type k =  s.find(*j);
     	if (k != std::string::npos) s.erase(k,k+j->size());
      }
      labHlt2B2JpsiX.push_back(s);
      m_Hlt2B2JpsiX.push_back((*i).first);
    }

    if((*i).second==7){
      std::string s = (*i).first;
      std::vector<std::string> strip = boost::assign::list_of<std::string>("Hlt2")("Decision"); //TODO: replace with regex with capture
      for (std::vector<std::string>::const_iterator j = strip.begin();j!=strip.end();++j) {
     	std::string::size_type k =  s.find(*j);
     	if (k != std::string::npos) s.erase(k,k+j->size());
      }
      labHlt2B2XPhi.push_back(s);
      m_Hlt2B2XPhi.push_back((*i).first);
    }

    if((*i).second==8){
      std::string s = (*i).first;
      std::vector<std::string> strip = boost::assign::list_of<std::string>("Hlt2")("Decision"); //TODO: replace with regex with capture
      for (std::vector<std::string>::const_iterator j = strip.begin();j!=strip.end();++j) {
     	std::string::size_type k =  s.find(*j);
     	if (k != std::string::npos) s.erase(k,k+j->size());
      }
      labHlt2B2HH.push_back(s);
      m_Hlt2B2HH.push_back((*i).first);
    }

    if((*i).second==9){
      std::string s = (*i).first;
      std::vector<std::string> strip = boost::assign::list_of<std::string>("Hlt2")("Decision"); //TODO: replace with regex with capture
      for (std::vector<std::string>::const_iterator j = strip.begin();j!=strip.end();++j) {
     	std::string::size_type k =  s.find(*j);
     	if (k != std::string::npos) s.erase(k,k+j->size());
      }
      labHlt2Express.push_back(s);
      m_Hlt2Express.push_back((*i).first);
    }

    if((*i).second==10){
      std::string s = (*i).first;
      std::vector<std::string> strip = boost::assign::list_of<std::string>("Hlt2")("Decision"); //TODO: replace with regex with capture
      for (std::vector<std::string>::const_iterator j = strip.begin();j!=strip.end();++j) {
     	std::string::size_type k =  s.find(*j);
     	if (k != std::string::npos) s.erase(k,k+j->size());
      }
      labHlt2Commissioning.push_back(s);
      m_Hlt2Commissioning.push_back((*i).first);
    }

    if((*i).second==11){
      std::string s = (*i).first;
      std::vector<std::string> strip = boost::assign::list_of<std::string>("Hlt2")("Decision"); //TODO: replace with regex with capture
      for (std::vector<std::string>::const_iterator j = strip.begin();j!=strip.end();++j) {
     	std::string::size_type k =  s.find(*j);
     	if (k != std::string::npos) s.erase(k,k+j->size());
      }
      labHlt2DisplVertices.push_back(s);
      m_Hlt2DisplVertices.push_back((*i).first);
    }

    if((*i).second==13){
      std::string s = (*i).first;
      std::vector<std::string> strip = boost::assign::list_of<std::string>("Hlt2")("Decision"); //TODO: replace with regex with capture
      for (std::vector<std::string>::const_iterator j = strip.begin();j!=strip.end();++j) {
     	std::string::size_type k =  s.find(*j);
     	if (k != std::string::npos) s.erase(k,k+j->size());
      }
      labHlt2Other.push_back(s);
      m_Hlt2Other.push_back((*i).first);
    }
  }

  m_hlt2TopologicalAlley    = book1D("Hlt2 Topological Alley",   -0.5,m_Hlt2Topological.size()-0.5, m_Hlt2Topological.size());

  if (!setBinLabels( m_hlt2TopologicalAlley, labHlt2Topological )) {
    error() << "failed to set binlables on hlt2 Topological alley hist" << endmsg;
  }

  m_hlt2InclusivePhiAlley    = book1D("Hlt2 InclusivePhi Alley",   -0.5,m_Hlt2InclusivePhi.size()-0.5, m_Hlt2InclusivePhi.size());

  if (!setBinLabels( m_hlt2InclusivePhiAlley,  labHlt2InclusivePhi )) {
    error() << "failed to set binlables on hlt2 InclusivePhi alley hist" << endmsg;
  }

  m_hlt2SingleMuonAlley    = book1D("Hlt2 SingleMuon Alley",   -0.5,m_Hlt2SingleMuon.size()-0.5, m_Hlt2SingleMuon.size());

  if (!setBinLabels( m_hlt2SingleMuonAlley,  labHlt2SingleMuon )) {
    error() << "failed to set binlables on hlt2 SingleMuon alley hist" << endmsg;
  }

  m_hlt2DiMuonAlley    = book1D("Hlt2 DiMuon Alley",   -0.5,m_Hlt2DiMuon.size()-0.5, m_Hlt2DiMuon.size());

  if (!setBinLabels( m_hlt2DiMuonAlley,  labHlt2DiMuon )) {
    error() << "failed to set binlables on hlt2 DiMuon alley hist" << endmsg;
  }

  m_hlt2B2DXAlley    = book1D("Hlt2 B2DX Alley",   -0.5,m_Hlt2B2DX.size()-0.5, m_Hlt2B2DX.size());

  if (!setBinLabels( m_hlt2B2DXAlley,  labHlt2B2DX )) {
    error() << "failed to set binlables on hlt2 B2DX alley hist" << endmsg;
  }

  m_hlt2B2XGammaAlley    = book1D("Hlt2 B2XGamma Alley",   -0.5,m_Hlt2B2XGamma.size()-0.5, m_Hlt2B2XGamma.size());

  if (!setBinLabels( m_hlt2B2XGammaAlley,  labHlt2B2XGamma )) {
    error() << "failed to set binlables on hlt2 B2XGamma alley hist" << endmsg;
  }

  m_hlt2B2JpsiXAlley    = book1D("Hlt2 B2JpsiX Alley",   -0.5,m_Hlt2B2JpsiX.size()-0.5, m_Hlt2B2JpsiX.size());

  if (!setBinLabels( m_hlt2B2JpsiXAlley,  labHlt2B2JpsiX )) {
    error() << "failed to set binlables on hlt2 B2JpsiX alley hist" << endmsg;
  }

  m_hlt2B2XPhiAlley    = book1D("Hlt2 B2XPhi Alley",   -0.5,m_Hlt2B2XPhi.size()-0.5, m_Hlt2B2XPhi.size());

  if (!setBinLabels( m_hlt2B2XPhiAlley,  labHlt2B2XPhi )) {
    error() << "failed to set binlables on hlt2 B2XPhi alley hist" << endmsg;
  }

  m_hlt2B2HHAlley    = book1D("Hlt2 B2HH Alley",   -0.5,m_Hlt2B2HH.size()-0.5, m_Hlt2B2HH.size());

  if (!setBinLabels( m_hlt2B2HHAlley,  labHlt2B2HH )) {
    error() << "failed to set binlables on hlt2 B2HH alley hist" << endmsg;
  }

  m_hlt2ExpressAlley    = book1D("Hlt2 Express Alley",   -0.5,m_Hlt2Express.size()-0.5, m_Hlt2Express.size());

  if (!setBinLabels( m_hlt2ExpressAlley,  labHlt2Express )) {
    error() << "failed to set binlables on hlt2 Express alley hist" << endmsg;
  }

  m_hlt2CommissioningAlley    = book1D("Hlt2 Commissioning Alley",   -0.5,m_Hlt2Commissioning.size()-0.5, m_Hlt2Commissioning.size());

  if (!setBinLabels( m_hlt2CommissioningAlley,  labHlt2Commissioning )) {
    error() << "failed to set binlables on hlt2 Commissioning alley hist" << endmsg;
  }

  m_hlt2DisplVerticesAlley    = book1D("Hlt2 DisplVertices Alley",   -0.5,m_Hlt2DisplVertices.size()-0.5, m_Hlt2DisplVertices.size());

  if (!setBinLabels( m_hlt2DisplVerticesAlley,  labHlt2DisplVertices )) {
    error() << "failed to set binlables on hlt2 DisplVertices alley hist" << endmsg;
  }

  m_hlt2OtherAlley    = book1D("Hlt2 Other Alley",   -0.5,m_Hlt2Other.size()-0.5, m_Hlt2Other.size());

  if (!setBinLabels( m_hlt2OtherAlley,  labHlt2Other )) {
    error() << "failed to set binlables on hlt2 other alley hist" << endmsg;
  }


  m_hltVirtTime  = bookProfile1D("Virtual memory",   0,m_timeSize,int(m_timeSize/m_timeInterval+0.5));
  setAxisLabels( m_hltVirtTime, "time since start of run [min]", "memory[MB]");
  
  m_hltEventsTime  = bookProfile1D("average time per event", 0,m_timeSize,int(m_timeSize/m_timeInterval+0.5));
  setAxisLabels( m_hltEventsTime, "time since start of run [min]", "average time/event [ms]");

  m_hltTime  = book1D("time per event ", -1, 4 );
  setAxisLabels( m_hltTime, "log10(time/event/ms)", "events");

  declareInfo("COUNTER_TO_RATE[virtmem]", m_virtmem, "Virtual memory");
  declareInfo("COUNTER_TO_RATE[elapsed time]", m_currentTime, "Elapsed time");
  

  declareInfo("COUNTER_TO_RATE[L0Accept]",counter("L0Accept"),"L0Accept");
  declareInfo("COUNTER_TO_RATE[GpsTimeoflast]",m_gpstimesec,"Gps time of last event");

  for (unsigned i=0; i!=m_GroupLabels.size();++i) {
    m_alley.push_back(&counter(m_GroupLabels.at(i)));
    declareInfo("COUNTER_TO_RATE["+m_GroupLabels.at(i)+"]", *m_alley.back(),m_GroupLabels.at(i)+" Alley");
  }

  // register for incidents...
  IIncidentSvc* incidentSvc = svc<IIncidentSvc>( "IncidentSvc" );
  // insert at high priority, so we also time what happens during BeginEvent incidents...
  long priority = std::numeric_limits<long>::max();
  incidentSvc->addListener(this,IncidentType::BeginEvent, priority ) ;
  incidentSvc->addListener(this,IncidentType::BeginRun, 0 ) ;

  // start by kicking ourselves in action -- just in case we don't get one otherwise...
  this->handle(Incident(name(), IncidentType::BeginRun ));

  return StatusCode::SUCCESS;
};

void HltGlobalMonitor::handle ( const Incident& incident ) {
  m_startEvent = System::currentTime( System::microSec );
  if (m_startClock == 0 || incident.type() == IncidentType::BeginRun) m_startClock = m_startEvent;
  m_currentTime = double(m_startEvent-m_startClock)/1000000 ;  // seconds
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltGlobalMonitor::execute() {  

  LHCb::ODIN*         odin = fetch<LHCb::ODIN>( LHCb::ODINLocation::Default);
  LHCb::L0DUReport*   l0du = fetch<LHCb::L0DUReport>( m_L0DUReportLocation );
  LHCb::HltDecReports* hlt = fetch<LHCb::HltDecReports>( m_HltDecReportsLocation );

  monitorODIN(odin,l0du,hlt);
  monitorL0DU(odin,l0du,hlt);
  monitorHLT1(odin,l0du,hlt);
  monitorHLT2(odin,l0du,hlt);

 if(  (m_events)%m_scanevents ==0) monitorMemory();

  counter("#events")   += 1.0;
  m_events             += 1;

  return StatusCode::SUCCESS;
  
}

//==============================================================================
void HltGlobalMonitor::monitorODIN(const LHCb::ODIN* odin,
                                   const LHCb::L0DUReport*,
                                   const LHCb::HltDecReports* hlt) {
  if (odin == 0 ) return;
  unsigned long long gpstime=odin->gpsTime();
  if (msgLevel(MSG::DEBUG)) debug() << "gps time" << gpstime << endreq;
  m_gpstimesec=int(gpstime/1000000-904262401); //@TODO: is this still OK with ODIN v6?
  counter("ODIN::Lumi")    += (odin->triggerType()==ODIN::LumiTrigger);
  counter("ODIN::NotLumi") += (odin->triggerType()!=ODIN::LumiTrigger);
  fill(m_odin, odin->triggerType(), 1.);
  if ( hlt == 0 ) return;


}

//==============================================================================
void HltGlobalMonitor::monitorL0DU(const LHCb::ODIN*,
                                   const LHCb::L0DUReport* l0du,
                                   const LHCb::HltDecReports* ) {
  if (l0du == 0) return;

  counter("L0Accept") += l0du->decision();
  counter("L0Forced") += l0du->forceBit();

  if (!l0du->decision()) return;

  LHCb::L0DUChannel::Map channels = l0du->configuration()->channels();
  for(LHCb::L0DUChannel::Map::iterator i = channels.begin();i!=channels.end();++i){
      fill( m_L0Input, i->second->id(), l0du->channelDecision( i->second->id() ) );
  }

  unsigned int L0TCK = l0du->tck();
  if (L0TCK != m_lastL0TCK && m_L0Input!=0) {
      std::vector< std::pair<unsigned, std::string> > labels;
      for(LHCb::L0DUChannel::Map::iterator i = channels.begin();i!=channels.end();++i){
        labels.push_back(std::make_pair( i->second->id(), i->first ));
      }
      setBinLabels( m_L0Input, labels );
      m_lastL0TCK = L0TCK;
  }
};

//==============================================================================
void HltGlobalMonitor::monitorHLT1(const LHCb::ODIN*,
                                   const LHCb::L0DUReport*,
                                   const LHCb::HltDecReports* hlt) {


  if (hlt==0) return;

  ///////////////////////////////////////////////////////////////////////////////
  std::vector<std::pair<std::string,const LHCb::HltDecReport*> > reps;
  unsigned nAcc = 0;
  std::vector<unsigned> nAccAlley(m_GroupLabels.size(),unsigned(0));

  for (std::vector<std::string>::const_iterator i = m_Hlt1Lines.begin(); i!=m_Hlt1Lines.end();i++) {
    const LHCb::HltDecReport*  report = hlt->decReport( *i );
    if (report == 0 ) {  
       warning() << "report " << *i << " not found" << endreq;
    }
    reps.push_back( std::make_pair( *i, report ) );
    if (report && report->decision()){
      ++nAcc;
      ++nAccAlley[m_DecToGroup.find(*i)->second];
    }
  }

  for (unsigned i=0; i<m_GroupLabels.size();i++) {
    *m_alley[i] += ( nAccAlley[i] > 0 );
    fill(m_hlt1alley,i,(nAccAlley[i]>0));
  }

  fill( m_hlt1NAcc, nAcc, 1.0);  //by how many lines did the current event get accepted?
 
  for (size_t i = 0; i<reps.size();++i) {
    bool accept = reps[i].second->decision();
    fill( m_hlt1Inclusive, i, accept);
    if (!accept) continue;
    if (nAcc==1) fill( m_hlt1Exclusive, i, accept );
    for (size_t j = 0; j<reps.size(); ++j) fill(m_hlt1Correlations,i,j,reps[j].second->decision());

           //filling the histograms for each alley

    if((m_DecToGroup.find(reps[i].first)->second)==0){
      for(size_t k = 0; k < m_Hlt1L0.size(); k++){
        if(m_Hlt1L0[k]==m_DecToGroup.find(reps[i].first)->first) fill( m_hlt1L0Alley, k, accept);
      }
    }

    if((m_DecToGroup.find(reps[i].first)->second)==1){
      for(size_t k = 0; k < m_Hlt1Lumi.size(); k++){
        if(m_Hlt1Lumi[k]==m_DecToGroup.find(reps[i].first)->first) fill( m_hlt1LumiAlley, k, accept);
      }
    }

    if((m_DecToGroup.find(reps[i].first)->second)==2){
      for(size_t k = 0; k < m_Hlt1Velo.size(); k++){
        if(m_Hlt1Velo[k]==m_DecToGroup.find(reps[i].first)->first) fill( m_hlt1VeloAlley, k, accept);
      }
    }

    if((m_DecToGroup.find(reps[i].first)->second)==3){
      for(size_t k = 0; k < m_Hlt1XPress.size(); k++){
        if(m_Hlt1XPress[k]==m_DecToGroup.find(reps[i].first)->first) fill( m_hlt1XPressAlley, k, accept);
      }
    }

    if((m_DecToGroup.find(reps[i].first)->second)==4){
      for(size_t k = 0; k < m_Hlt1Hadron.size(); k++){
        if(m_Hlt1Hadron[k]==m_DecToGroup.find(reps[i].first)->first) fill( m_hlt1HadronAlley, k, accept);
      }
    }

    if((m_DecToGroup.find(reps[i].first)->second)==5){
      for(size_t k = 0; k < m_Hlt1SingleMuon.size(); k++){
        if(m_Hlt1SingleMuon[k]==m_DecToGroup.find(reps[i].first)->first) fill( m_hlt1SingleMuonAlley, k, accept);
      }
    }

    if((m_DecToGroup.find(reps[i].first)->second)==6){
      for(size_t k = 0; k < m_Hlt1DiMuon.size(); k++){
        if(m_Hlt1DiMuon[k]==m_DecToGroup.find(reps[i].first)->first) fill( m_hlt1DiMuonAlley, k, accept);
      }
    }

    if((m_DecToGroup.find(reps[i].first)->second)==7){
      for(size_t k = 0; k < m_Hlt1Electron.size(); k++){
        if(m_Hlt1Electron[k]==m_DecToGroup.find(reps[i].first)->first) fill( m_hlt1ElectronAlley, k, accept);
      }
    }

    if((m_DecToGroup.find(reps[i].first)->second)==8){
      for(size_t k = 0; k < m_Hlt1Photon.size(); k++){
        if(m_Hlt1Photon[k]==m_DecToGroup.find(reps[i].first)->first) fill( m_hlt1PhotonAlley, k, accept);
      }
    }

    if((m_DecToGroup.find(reps[i].first)->second)==9){
      for(size_t k = 0; k < m_Hlt1PA.size(); k++){
        if(m_Hlt1PA[k]==m_DecToGroup.find(reps[i].first)->first) fill( m_hlt1PAAlley, k, accept);
      }
    }

    if((m_DecToGroup.find(reps[i].first)->second)==10){
      for(size_t k = 0; k < m_Hlt1BeamGas.size(); k++){
        if(m_Hlt1BeamGas[k]==m_DecToGroup.find(reps[i].first)->first) fill( m_hlt1BeamGasAlley, k, accept);
      }
    }

    if((m_DecToGroup.find(reps[i].first)->second)==11){
      for(size_t k = 0; k < m_Hlt1MinBias.size(); k++){
        if(m_Hlt1MinBias[k]==m_DecToGroup.find(reps[i].first)->first) fill( m_hlt1MinBiasAlley, k, accept);
      }
    }

    if((m_DecToGroup.find(reps[i].first)->second)==13){
      for(size_t k = 0; k < m_Hlt1Other.size(); k++){
        if(m_Hlt1Other[k]==m_DecToGroup.find(reps[i].first)->first) fill( m_hlt1OtherAlley, k, accept);
        }
    }
  }

       //filling the histograms for the alleys instead of the lines

  for (unsigned i=0; i<m_GroupLabels.size();i++) {
    fill(m_hlt1alley,i,(nAccAlley[i]>0));
    if(!nAccAlley[i]>0) continue;
    for(unsigned j=0; j<m_GroupLabels.size();j++){
      fill(m_hlt1AlleysCorrelations,i,j,(nAccAlley[j]>0));
    }
  }

}

//==============================================================================

void HltGlobalMonitor::monitorHLT2(const LHCb::ODIN*,
                                   const LHCb::L0DUReport*,
                                   const LHCb::HltDecReports* hlt) {


  if (hlt==0) return;

  ///////////////////////////////////////////////////////////////////////////////
  std::vector<std::pair<std::string,const LHCb::HltDecReport*> > reps;
  unsigned nAcc = 0;
  std::vector<unsigned> nAccAlley(m_GroupLabels2.size(),unsigned(0));

  for (std::vector<std::string>::const_iterator i = m_Hlt2Lines.begin(); i!=m_Hlt2Lines.end();i++) {
    const LHCb::HltDecReport*  report = hlt->decReport( *i );
    if (report == 0 ) {  
       warning() << "report " << *i << " not found" << endreq;
    }
    reps.push_back( std::make_pair( *i, report ) );
    if (report && report->decision()){
      ++nAcc;
      ++nAccAlley[m_DecToGroup2.find(*i)->second];
    }
  }

  for (unsigned i=0; i<m_GroupLabels2.size();i++) {
    *m_alley[i] += ( nAccAlley[i] > 0 );
  } 


  fill( m_hlt2NAcc, nAcc, 1.0);  //by how many lines did 1 event get accepted?

  for (size_t i = 0; i<reps.size();++i) {
    bool accept = reps[i].second->decision();
    fill( m_hlt2Inclusive, i, accept);
    if (!accept) continue;
    if (nAcc==1) fill( m_hlt2Exclusive, i, accept );
    for (size_t j = 0; j<reps.size(); ++j) fill(m_hlt2Correlations,i,j,reps[j].second->decision());
    
           //filling the histograms for each alley

    if((m_DecToGroup2.find(reps[i].first)->second)==0){
      for(size_t k = 0; k < m_Hlt2Topological.size(); k++){
        if(m_Hlt2Topological[k]==m_DecToGroup2.find(reps[i].first)->first) fill( m_hlt2TopologicalAlley, k, accept);
      }
    }

    if((m_DecToGroup2.find(reps[i].first)->second)==1){
      for(size_t k = 0; k < m_Hlt2InclusivePhi.size(); k++){
        if(m_Hlt2InclusivePhi[k]==m_DecToGroup2.find(reps[i].first)->first) fill( m_hlt2InclusivePhiAlley, k, accept);
      }
    }

    if((m_DecToGroup2.find(reps[i].first)->second)==2){
      for(size_t k = 0; k < m_Hlt2SingleMuon.size(); k++){
        if(m_Hlt2SingleMuon[k]==m_DecToGroup2.find(reps[i].first)->first) fill( m_hlt2SingleMuonAlley, k, accept);
      }
    }

    if((m_DecToGroup2.find(reps[i].first)->second)==3){
      for(size_t k = 0; k < m_Hlt2DiMuon.size(); k++){
        if(m_Hlt2DiMuon[k]==m_DecToGroup2.find(reps[i].first)->first) fill( m_hlt2DiMuonAlley, k, accept);
      }
    }

    if((m_DecToGroup2.find(reps[i].first)->second)==4){
      for(size_t k = 0; k < m_Hlt2B2DX.size(); k++){
        if(m_Hlt2B2DX[k]==m_DecToGroup2.find(reps[i].first)->first) fill( m_hlt2B2DXAlley, k, accept);
      }
    }

    if((m_DecToGroup2.find(reps[i].first)->second)==5){
      for(size_t k = 0; k < m_Hlt2B2XGamma.size(); k++){
        if(m_Hlt2B2XGamma[k]==m_DecToGroup2.find(reps[i].first)->first) fill( m_hlt2B2XGammaAlley, k, accept);
      }
    }

    if((m_DecToGroup2.find(reps[i].first)->second)==6){
      for(size_t k = 0; k < m_Hlt2B2JpsiX.size(); k++){
        if(m_Hlt2B2JpsiX[k]==m_DecToGroup2.find(reps[i].first)->first) fill( m_hlt2B2JpsiXAlley, k, accept);
      }
    }

    if((m_DecToGroup2.find(reps[i].first)->second)==7){
      for(size_t k = 0; k < m_Hlt2B2XPhi.size(); k++){
        if(m_Hlt2B2XPhi[k]==m_DecToGroup2.find(reps[i].first)->first) fill( m_hlt2B2XPhiAlley, k, accept);
      }
    }

    if((m_DecToGroup2.find(reps[i].first)->second)==8){
      for(size_t k = 0; k < m_Hlt2B2HH.size(); k++){
        if(m_Hlt2B2HH[k]==m_DecToGroup2.find(reps[i].first)->first) fill( m_hlt2B2HHAlley, k, accept);
      }
    }

    if((m_DecToGroup2.find(reps[i].first)->second)==9){
      for(size_t k = 0; k < m_Hlt2Express.size(); k++){
        if(m_Hlt2Express[k]==m_DecToGroup2.find(reps[i].first)->first) fill( m_hlt2ExpressAlley, k, accept);
      }
    }

    if((m_DecToGroup2.find(reps[i].first)->second)==10){
      for(size_t k = 0; k < m_Hlt2Commissioning.size(); k++){
        if(m_Hlt2Commissioning[k]==m_DecToGroup2.find(reps[i].first)->first) fill( m_hlt2CommissioningAlley, k, accept);
      }
    }

    if((m_DecToGroup2.find(reps[i].first)->second)==11){
      for(size_t k = 0; k < m_Hlt2DisplVertices.size(); k++){
        if(m_Hlt2DisplVertices[k]==m_DecToGroup2.find(reps[i].first)->first) fill( m_hlt2DisplVerticesAlley, k, accept);
      }
    }

    if((m_DecToGroup2.find(reps[i].first)->second)==13){
      for(size_t k = 0; k < m_Hlt2Other.size(); k++){
        if(m_Hlt2Other[k]==m_DecToGroup2.find(reps[i].first)->first) fill( m_hlt2OtherAlley, k, accept);
      }
    }
  }
       //filling the histograms for the alleys instead of the lines

  for (unsigned i=0; i<m_GroupLabels2.size();i++) {
    fill(m_hlt2alley,i,(nAccAlley[i]>0));
    if(!nAccAlley[i]>0) continue;
    for(unsigned j=0; j<m_GroupLabels2.size();j++){
      fill(m_hlt2AlleysCorrelations,i,j,(nAccAlley[j]>0));
    }
  }

}

//==============================================================================

void HltGlobalMonitor::monitorMemory() {

  double elapsedTime = double(System::currentTime( System::microSec ) - m_startEvent);
  double t = log10(elapsedTime)-3; // convert to log(time/ms)
  fill( m_hltTime, t ,1.0); 
  storeTrend(m_hltEventsTime, elapsedTime/1000 );

  //TODO: only need to do this once 'per bin'  interval...
  m_virtmem  = virtualMemory(System::MByte, System::Memory);
  storeTrend(m_hltVirtTime, double(m_virtmem));

 
}

//=============================================================================
void HltGlobalMonitor::storeTrend(AIDA::IProfile1D* h, double Value) 
{
#ifdef TODO_IMPLEMENT_SHIFT_USING_AIDA2ROOT_ON_A_PROFILE_HISTOGRAM
  double offset = m_currentTime - m_trendLHSEdge;
  if ( offset > ... ) {
      const AIDA::IAxis & axis = h->axis();
      long bins = axis.bins();
      for ( long i = 0; i < bins; ++i ) {
        double binValue = h->binHeight(i);
        double nextValue = ( i < bins - 1 ) ? h->binHeight(i+1)
                                            : Value;
        double x = 0.5*(axis.binUpperEdge(i)+axis.binLowerEdge(i));
        h->fill(x, nextValue - binValue);
      }
       update m_trendLHSEdge
       shift bins to left
  }
#endif
  h->fill(m_currentTime/60, Value); // go from seconds -> minutes
}
