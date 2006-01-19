// $Id: HltForward.cpp,v 1.1.1.1 2006-01-19 10:11:25 hernando Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

#include "PatTools/PatDataStore.h"

// local
#include "HltForward.h"

// Declaration of the Algorithm Factory
static const  AlgFactory<HltForward>          s_factory ;
const        IAlgFactory& HltForwardFactory = s_factory ;


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltForward::HltForward( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : PatForward ( name , pSvcLocator )

{
  // possible modes: "selected", "rest", "all"
  declareProperty("Mode",m_mode = "all");
  
  m_accept.clear();
  m_noAccept.clear();
  m_noAccept.push_back(Track::Backward);
  // declareProperty( "Accept", m_accept);
  // declareProperty( "NoAccept", m_noAccept);
  
}
//=============================================================================
// Destructor
//=============================================================================
HltForward::~HltForward() {};

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltForward::initialize() {
  StatusCode sc =  PatForward::initialize();  

  m_ok = true;  
  if (m_mode == "selected") {
    m_ok = false;
    m_accept.push_back(Track::IPSelected);
    m_accept.push_back(Track::PIDSelected);
  } else if (m_mode == "rest") {
    m_noAccept.push_back(Track::IPSelected);
    m_noAccept.push_back(Track::PIDSelected);
  }  

  return sc;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltForward::execute() {
  return PatForward::execute();
}


bool HltForward::acceptTrack(const Track& track) const 
{
  for (std::vector<Track::Flags>::const_iterator it = m_noAccept.begin();
       it != m_noAccept.end(); it++) {
    Track::Flags flag = *it;
    if (track.checkFlag(flag)) {
      debug() << " No accepting track: reason, flag = " << flag << endreq;
      return false;
    }
    
  }

  for (std::vector<Track::Flags>::const_iterator it = m_accept.begin();
       it != m_accept.end(); it++) {
    Track::Flags flag = *it;
    if (track.checkFlag(flag)) {
      debug() << " Accepting track: reason, flag = " << flag << endreq;
      return true;
    } 
  }
  
  debug() << " accepting track at the end  = " << m_ok << endreq;
  return m_ok;  
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltForward::finalize() {
  return PatForward::finalize();
}

//=============================================================================
