// GaudiKernel
#include "GaudiKernel/IIncidentSvc.h"
 
// Event
#include "Kernel/STChannelID.h"
#include "Kernel/ISTReadoutTool.h"
#include "Kernel/StripRepresentation.h"
#include "Kernel/BeetleRepresentation.h"

#include "STRawADCInfo.h"

//DET
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"


//boost
#include <boost/foreach.hpp>


DECLARE_TOOL_FACTORY( STRawADCInfo)
 
STRawADCInfo::STRawADCInfo( const std::string& type, 
                                    const std::string& name,
                                    const IInterface* parent ) :
  ST::ToolBase(type, name, parent),
  m_configured(false)
{
  declareSTConfigProperty("dataLocation", m_dataLocation , LHCb::STTELL1DataLocation::TTFull);
  setForcedInit();
  declareInterface<ISTRawADCInfo>(this);
}


STRawADCInfo::~STRawADCInfo()
{
  //destructer
}


StatusCode STRawADCInfo::initialize(){
  StatusCode sc = ST::ToolBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  // Add incident at begin of each event
  incSvc()->addListener( this, IncidentType::BeginEvent );

  return StatusCode::SUCCESS; 
}

void STRawADCInfo::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) m_configured = false;;
}

void STRawADCInfo::initEvent() const{
 
  m_data = get<LHCb::STTELL1Datas>(m_dataLocation);
  m_configured = true;
}

StatusCode STRawADCInfo::link(const LHCb::STChannelID& chan,
                              unsigned int& value ) const{

  // initialize this event
  if (!m_configured) initEvent();

  StatusCode sc = StatusCode::SUCCESS;

  // get the Tell1 this channel corresponds to 
  STDAQ::chanPair tPair = readoutTool()->offlineChanToDAQ(chan,0); 
  LHCb::STTELL1Data* tell1Data = m_data->object(tPair.first.id());
  if (tell1Data != 0){
    // we have NZS data for this board, convert strip to beetle rep
    STDAQ::BeetleRepresentation beetleRep =
       STDAQ::BeetleRepresentation(STDAQ::StripRepresentation(tPair.second)) ;
    unsigned int beetle, strip;
    beetleRep.decompose(beetle,strip); 

    // get the data
    value = tell1Data->data()[beetle][strip];

  }
  else {
    // too bad
    sc = StatusCode::FAILURE;
  }
  return sc;
}


StatusCode STRawADCInfo::link(const std::vector<LHCb::STChannelID>& chans, 
                              std::vector<unsigned int>& values) const{

  unsigned int val;
  StatusCode sc(StatusCode::SUCCESS);
  BOOST_FOREACH(LHCb::STChannelID aChan,chans ){
    if (link(aChan,val).isSuccess()) {
     values.push_back(val);
    }
    else {
      values.push_back(999);
      sc = StatusCode::FAILURE;
    }
  }   // boost for each 
  return sc;
}

StatusCode STRawADCInfo::link(const LHCb::STChannelID& chan, 
                              const unsigned int window,
                              std::vector<unsigned int>& values) const{

  StatusCode sc = StatusCode::SUCCESS;
  const DeSTSector* sector = findSector(chan);
  const unsigned int firststrip = chan.strip() - window;
  const unsigned int laststrip = chan.strip() + window;

  for (unsigned int i = firststrip; i <= laststrip; ++i){ 
    if (sector->isStrip(i)){
      unsigned int val;
      LHCb::STChannelID chanStrip = sector->stripToChan(i);
      if (link(chanStrip,val).isSuccess()) {
       values.push_back(val);
      }
      else {
        values.push_back(999);
        sc = StatusCode::FAILURE;
      }
    }  // isStrip
  }  // i
  return sc;
}



