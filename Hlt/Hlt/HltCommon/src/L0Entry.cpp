// $Id: L0Entry.cpp,v 1.10 2007-12-11 09:50:00 hernando Exp $
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
  m_l0Channels  = m_l0ChannelsName.value();
  
  typedef std::pair< std::string, int>  Config;  

  
  for (std::vector< std::string>::const_iterator it = m_l0Channels.begin(); 
       it != m_l0Channels.end(); ++it) {
    info() << " accepting L0 trigger channel  " << *it << endreq;
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
    
    for (std::vector<std::string>::iterator it = m_l0Channels.begin();
         it != m_l0Channels.end(); ++it) {
      ok = ( ok || m_l0->channelDecisionByName( *it ) );
      debug() << " accepted L0 channel trigger " << *it 
              << " ? " << m_l0->channelDecisionByName(*it) << endreq;
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
