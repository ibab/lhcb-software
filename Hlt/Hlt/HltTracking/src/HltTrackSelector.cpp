// $Id: HltTrackSelector.cpp,v 1.1 2006-01-27 21:50:24 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "HltTrackSelector.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackSelector
//
// 2006-01-26 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( HltTrackSelector );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltTrackSelector::HltTrackSelector( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ITrackSelector>(this);

  declareProperty("Mode", m_mode = "all");

}
//=============================================================================
// Destructor
//=============================================================================
HltTrackSelector::~HltTrackSelector() {} 

//=============================================================================

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltTrackSelector::initialize() {

  StatusCode sc = GaudiTool::initialize();
  
  m_accept.clear();
  m_noAccept.clear();
  m_noAccept.push_back(Track::Backward);
  m_noAccept.push_back(Track::Invalid);

  m_ok = true;  
  if (m_mode == "selected") {
    m_ok = false;
    m_accept.push_back(Track::IPSelected);
    m_accept.push_back(Track::PIDSelected);
  } else if (m_mode == "rest") {
    m_noAccept.push_back(Track::IPSelected);
    m_noAccept.push_back(Track::PIDSelected);
  }  

  for (std::vector<Track::Flags>::const_iterator it = m_noAccept.begin();
       it != m_noAccept.end(); it++) 
    debug() << " No accept Flag " << *it << endreq;
  for (std::vector<Track::Flags>::const_iterator it2 = m_accept.begin();
       it2 != m_accept.end(); it2++) 
    debug() << "    Accept Flag " << *it2 << endreq;

  return sc;
};


bool HltTrackSelector::accept(const Track& track) const 
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

