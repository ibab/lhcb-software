// $Id: L0Entry.cpp,v 1.16 2008-06-02 11:39:20 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 


// local
#include "L0Entry.h"

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

  // default configuration
  // TODO; this needs to be solved with the TCK!!
  defineL0();

  registerSelection();
  
  m_histoL0 = initializeHisto("L0",0.,14.,28);
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0Entry::execute() {

  LHCb::L0DUReport* l0 = get<L0DUReport>(m_l0Location);
  
  bool pass = l0->decision();
  debug() << " L0 decision " << l0->decision() << endreq;

  if (!m_l0Channels.empty()) {

    if (produceHistos())
      for (int i = 0; i<14; ++i)
        if (l0->channelDecision(i)) fillHisto(*m_histoL0, 1.*i , 1.);
    
    pass = false;
    for (std::vector<int>::iterator it = m_l0Channels.begin();
         it != m_l0Channels.end() && !pass ; ++it) {
      pass = l0->channelDecision( *it );
    }
  }
  
  setDecision(pass);
  debug() << "L0Entry: " << (pass?"accept":"reject") << endreq;
  
  return StatusCode::SUCCESS;
};

//=============================================================================

void L0Entry::defineL0() {

  m_l0Channels.clear();
  const std::vector< std::string > values = m_l0ChannelsName.value();

  std::vector<std::string> conf;
  conf.push_back("GEC");
  conf.push_back("Electron");
  conf.push_back("Photon");
  conf.push_back("Hadron");
  conf.push_back("LocalPi0");
  conf.push_back("GlobalPi0");
  conf.push_back("Muon");
  conf.push_back("DiMuon");
  conf.push_back("MuonNoGlob");

  typedef std::pair< std::string, int>  Config;  
  
  std::vector< Config > configs;
  for (unsigned i = 0; i < conf.size(); i++) 
    configs.push_back( Config(conf[i],i) );

  for (std::vector< std::string>::const_iterator it = values.begin();
       it != values.end(); ++it) {
    for (std::vector<Config>::const_iterator it2 = configs.begin();
         it2 != configs.end(); ++it2) 
      if (*it == it2->first) {
        m_l0Channels.push_back( it2->second );
        info() << " accepting L0 trigger channel  " << it2->first
               << " (int) " << it2->second << endreq;  
      }
  }

  if (m_l0Channels.empty())
    info() << " accepting all L0 triggers " << endreq;

}
