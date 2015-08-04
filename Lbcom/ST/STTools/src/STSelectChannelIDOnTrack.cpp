// Kernel
#include "GaudiKernel/IIncidentSvc.h" 

// Event
#include "Kernel/STChannelID.h"
#include "Event/Track.h"

// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/foreach.hpp>

using namespace boost::lambda;
using namespace LHCb;
using namespace Gaudi;


#include "STSelectChannelIDOnTrack.h"

DECLARE_TOOL_FACTORY( STSelectChannelIDOnTrack)
 
STSelectChannelIDOnTrack::STSelectChannelIDOnTrack( const std::string& type, 
                                    const std::string& name,
                                    const IInterface* parent ) :
  ST::ToolBase(type, name, parent),
  m_configured(false)
{

  declareProperty("trackLocation" , m_trackLocation  , LHCb::TrackLocation::Default);
  m_ids.clear();

  declareInterface<ISTChannelIDSelector>(this);
}

StatusCode STSelectChannelIDOnTrack::initialize() {
  
  StatusCode sc = ST::ToolBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);


  incSvc()->addListener(this, IncidentType::BeginEvent);

  return StatusCode::SUCCESS;
}

STSelectChannelIDOnTrack::~STSelectChannelIDOnTrack()
{
  //destructer
}


void STSelectChannelIDOnTrack::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) {
    m_configured = false; 
  }
}

bool STSelectChannelIDOnTrack::select( const LHCb::STChannelID& id ) const{
  return (*this) (id);  
}
  
bool STSelectChannelIDOnTrack::operator()( const LHCb::STChannelID& id ) const{

  if (!m_configured){
    // get the spillover clusters
    initEvent();
    m_configured = true;
  }

  return std::binary_search(m_ids.begin(), m_ids.end(), id);  
}

void STSelectChannelIDOnTrack::initEvent() const {

  m_ids.clear();

  const LHCb::Tracks* tracks = get<LHCb::Tracks>(m_trackLocation);
  for (LHCb::Tracks::const_iterator iterT = tracks->begin(); iterT != tracks->end(); ++iterT){
    const std::vector<LHCb::LHCbID>& ids = (*iterT)->lhcbIDs();
    BOOST_FOREACH( LHCb::LHCbID chan, ids) {
      if (chan.isST() == true){
	if (detType() == "IT" ){
          if (chan.isIT() == true) m_ids.push_back(chan.stID()); 
	}
        else if (detType() == "TT"){
          if (chan.isTT() == true) m_ids.push_back(chan.stID());  
	}
      }
    } // ids
  }  // tracks

  std::sort(m_ids.begin(), m_ids.end());
}
