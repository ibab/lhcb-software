// $Id: L0Entry.cpp,v 1.2 2006-09-26 13:54:58 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

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
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_l0Channels.clear();
  const std::vector< std::string > values = m_l0ChannelsName.value();

  typedef std::pair< std::string, int>  Config;  
  std::vector< Config > configs;

  // TODO do not hardwire this!!!
  configs.push_back( Config("hadron",9) );
  configs.push_back( Config("muon",11) );
  configs.push_back( Config("dimuon",13) );

  for (std::vector< std::string>::const_iterator it = values.begin();
       it != values.end(); ++it) {
    const std::string& name = *it;
    for (std::vector<Config>::const_iterator it2 = configs.begin();
         it2 != configs.end(); ++it2) 
      if (name == it2->first) {
        m_l0Channels.push_back( it2->second );
        info() << " accepting L0 trigger channel  " << it2->first
               << " (int) " << it2->second << endreq;  
      }
  }

  initializeHisto(m_histoL0,"L0",0.,14.,28);
  
  if (m_l0Channels.size() == 0)
    info() << " accepting all L0 triggers " << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0Entry::execute() {  

  bool ok = HltAlgorithm::beginExecute();
  if (!ok) return stop(" No inputs");
  
  m_l0 = get<L0DUReport>(m_l0Location);
  if (!m_l0) return stop(" No L0 report");
  
  ok = m_l0->decision();
  if (!ok) return stop(" No L0 decsion");

  if (m_l0Channels.size()>0 ) {
    ok = false;
    if (m_monitor) 
      for (int i = 0; i<14; i+=1)
        if (m_l0->channelDecision(i)) fillHisto( m_histoL0, 1.*i , 1.);
    for (std::vector<int>::iterator it = m_l0Channels.begin();
         it != m_l0Channels.end(); ++it)
      ok = ( ok || m_l0->channelDecision( *it ) );
    debug() << " accepted L0 channel trigger ?" << ok << endreq;
    if (!ok) return stop(" No L0 channels decision");
  }
  
  HltAlgorithm::endExecute();

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
