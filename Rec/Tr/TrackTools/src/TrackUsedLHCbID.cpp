#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentSvc.h"


#include "TrackInterfaces/ITrackSelector.h"

// Tsa
#include "TrackUsedLHCbID.h"
#include "Event/Track.h"

// BOOST
#include <boost/assign/list_of.hpp> // for 'map_list_of()

//stl
#include <algorithm>

using namespace boost::assign; // bring 'map_list_of()' into scope

DECLARE_TOOL_FACTORY( TrackUsedLHCbID )
  
TrackUsedLHCbID::TrackUsedLHCbID(const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent):
GaudiTool(type, name, parent),
m_configured(false)
{

  declareProperty("inputContainers", m_inputs);
  declareProperty("selectorNames", m_names);

  m_inputs = std::vector<std::string>(1, "Rec/Track/Forward");
  m_names = std::vector<std::string>(1, "ForwardSelector");

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


  // make the selector tools
  for (std::vector<std::string>::const_iterator iter = m_names.begin(); iter != m_names.end(); ++iter){
    ITrackSelector* aTool = tool<ITrackSelector>("TrackSelector",*iter,this);
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

 Selectors::const_iterator iterSelector = m_selectors.begin();

 for (TrackContainers::const_iterator iterS = m_inputs.begin();
      iterS != m_inputs.end(); ++iterS, ++iterSelector){
 
   // get selection tool
   ITrackSelector* tSelector = *iterSelector;

   // get the containers and extract the ids from the track
   const LHCb::Tracks* tCont = get<LHCb::Tracks>(*iterS);
   LHCb::Tracks::const_iterator iterTrack = tCont->begin();
   for (; iterTrack != tCont->end() ;++iterTrack){
     const std::vector<LHCb::LHCbID>& ids = (*iterTrack)->lhcbIDs();
     if (tSelector->accept(**iterTrack) == true) m_usedCont.insert(m_usedCont.begin(),ids.begin(),ids.end()); 
   } //iterTrack
 }  // iterS
 
 // sort
 std::sort(m_usedCont.begin(),m_usedCont.end(), lessByID());
 
 // strip out duplicates
 m_usedCont.erase(std::unique(m_usedCont.begin(), m_usedCont.end()),
                  m_usedCont.end());

}
