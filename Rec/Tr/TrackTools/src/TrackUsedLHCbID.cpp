#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentSvc.h"


#include "TrackInterfaces/ITrackSelector.h"

// Tsa
#include "TrackUsedLHCbID.h"
#include "Event/Track.h"

//stl
#include <algorithm>


DECLARE_TOOL_FACTORY( TrackUsedLHCbID )
  
TrackUsedLHCbID::TrackUsedLHCbID(const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent):
GaudiTool(type, name, parent),
m_configured(false)
{

  /** Define containers and corresponding selectors in same order.
   *  E.g. inputContainers = "Rec/Track/Forward" and selectorNames = "ForwardSelector".
   */
 
  declareProperty("inputContainers", m_inputs = {""});
  declareProperty("selectorNames", m_names = {""});


  declareInterface<IUsedLHCbID>(this);
  m_usedCont.reserve(5000);
}

TrackUsedLHCbID::~TrackUsedLHCbID(){
  // destructer
}


StatusCode TrackUsedLHCbID::initialize()
{

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  if ( m_names.size() != m_inputs.size() ){
    debug()<<"Number of selectors and number of inputs disagree. Initialize as many selectors as inputs."<<endmsg;
    m_names.clear();
    for (auto inputs : m_inputs )
      m_names.push_back("");
  }
  

  // make the selector tools
  for (auto name : m_names){
    ITrackSelector* aTool = !name.empty() ? tool<ITrackSelector>(name, this) : nullptr;
    m_selectors.push_back(aTool); 
  } // iter

  incSvc()->addListener( this, IncidentType::BeginEvent );
 
  return StatusCode::SUCCESS; 
}

bool TrackUsedLHCbID::used(const LHCb::LHCbID id) const{

  // get the input - seeds
  if (!m_configured) initEvent();       

  return std::binary_search(m_usedCont.begin(),m_usedCont.end(),id,lessByID());
}

void TrackUsedLHCbID::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) { m_configured = false; }
}
 
void TrackUsedLHCbID::initEvent() const{

 m_configured = true;
 m_usedCont.clear();

 auto iterSelector = m_selectors.begin();

 for (auto iterS = m_inputs.begin(); iterS != m_inputs.end(); ++iterS, ++iterSelector){
 
   // get selection tool
   ITrackSelector* tSelector = *iterSelector;
    
   // get the containers and extract the ids from the track
   auto tCont = getIfExists<LHCb::Track::Range>(*iterS);
   if ( tCont.empty() ){
     if( msgLevel(MSG::DEBUG) ) debug() << "Track container '" << *iterS << "' does not exist" <<endmsg;
     continue;
   }
   for ( auto iterTrack : tCont){
     auto ids = iterTrack->lhcbIDs();
     if ( tSelector && !(tSelector->accept(*iterTrack)) ) continue;
     m_usedCont.insert(m_usedCont.begin(),ids.begin(),ids.end()); 
   } //iterTrack
 }  // iterS
 
 // sort
 std::sort(m_usedCont.begin(),m_usedCont.end(), lessByID());
 
 // strip out duplicates
 m_usedCont.erase(std::unique(m_usedCont.begin(), m_usedCont.end()),
                  m_usedCont.end());
 
}
