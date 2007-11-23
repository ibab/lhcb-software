// $Id: L0Entry.cpp,v 1.9 2007-11-23 12:22:46 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/L0CaloCandidate.h"
#include "Event/L0MuonCandidate.h"

// local
#include "L0Entry.h"
#include "Event/HltNames.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : L0Entry
//
// 2006-01-16 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0Entry );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0Entry::L0Entry( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
  declareProperty("L0DULocation", m_l0Location = L0DUReportLocation::Default );
  declareProperty("L0ChannelsName", m_l0ChannelsName);
}
//=============================================================================
// Destructor
//=============================================================================
L0Entry::~L0Entry() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0Entry::initialize() {
  debug() << "==> Initialize" << endmsg;
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_l0Channels.clear();
  const std::vector< std::string > values = m_l0ChannelsName.value();

  typedef std::pair< std::string, int>  Config;  

  // default configuration
  // TODO; this needs to be solved with the TCK!!
  std::vector<std::string> conf;
  if (conf.size() == 0) {
    const std::string& TCK = m_conf->retrieve<std::string>("TCKName");
    if (TCK == "UpdatedL0") {
      conf.push_back("GEC");
      conf.push_back("electron");
      conf.push_back("photon");
      conf.push_back("hadron");
      conf.push_back("localPi0");
      conf.push_back("globalPi0");
      conf.push_back("muon");
      conf.push_back("dimuon");
      conf.push_back("muonnoglob");
    } else {
      conf.push_back("GEC");
      conf.push_back("electron");
      conf.push_back("electron_low");
      conf.push_back("photon");
      conf.push_back("photon_low");
      conf.push_back("localPi0");
      conf.push_back("localPi0_low");
      conf.push_back("globalPi0");
      conf.push_back("globalPi0_low");
      conf.push_back("hadron");
      conf.push_back("hadron_low");
      conf.push_back("muon");
      conf.push_back("muon_low");
      conf.push_back("dimuon");
      conf.push_back("dimuon_low");
      conf.push_back("muonnoglob");
      conf.push_back("muonnoglob_low");
    } 
  }
  
  std::vector< Config > configs;
  for (unsigned i = 0; i < conf.size(); i++) configs.push_back( Config(conf[i],i) );

  for (std::vector< std::string>::const_iterator it = values.begin(); it != values.end(); ++it) {
    for (std::vector<Config>::const_iterator it2 = configs.begin(); it2 != configs.end(); ++it2) 
      if (*it == it2->first) {
        m_l0Channels.push_back( it2->second );
        info() << " accepting L0 trigger channel  " << it2->first
               << " (int) " << it2->second << endreq;  
      }
  }

  initializeHisto(m_histoL0,"L0",0.,14.,28);
  
  if (m_l0Channels.empty())
    info() << " accepting all L0 triggers " << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0Entry::execute() {
  
  if (!retrieve(m_l0,m_l0Location)) return stop(" No L0 report");
  
  bool ok = m_l0->decision();
  if (!ok) return stop(" No L0 decision");


  if (!m_l0Channels.empty()) {
    ok = false;
    if (m_monitor) 
      for (int i = 0; i<14; ++i)
        if (m_l0->channelDecision(i)) fillHisto( m_histoL0, 1.*i , 1.);

    for (std::vector<int>::iterator it = m_l0Channels.begin();
         it != m_l0Channels.end(); ++it) {
      ok = ( ok || m_l0->channelDecision( *it ) );
      debug() << " accepted L0 channel trigger " << m_l0->channelName(*it ) 
              << " ? " << m_l0->channelDecision(*it) << endreq;
    }
    if (!ok) return stop(" No L0 channels decision");
  }

  candidateFound(1);
  
  debug() << " accepted  l0 entry " << endreq;
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0Entry::finalize() {
  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
