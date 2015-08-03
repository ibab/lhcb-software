// Include files 
// local
#include "DecodeVeloFullRawBuffer.h"

// Kernel
#include "Kernel/VeloEventFunctor.h"

// STL
#include <vector>

//-----------------------------------------------------------------------------
// Implementation file for class : DecodeVeloFullRawBuffer
//
// 2006-04-21 : Tomasz Szumlak
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DecodeVeloFullRawBuffer )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DecodeVeloFullRawBuffer::DecodeVeloFullRawBuffer( const std::string& name,
    ISvcLocator* pSvcLocator)
: GaudiAlgorithm ( name , pSvcLocator ),
  m_veloADCLocation ( VeloFullBankLocation::Default ),
  m_veloPartialADCLocation ( "Raw/Velo/PreparedPartialADC" ),
  m_veloPedLocation ( VeloFullBankLocation::Pedestals ),
  m_decodedADCLocation ( LHCb::VeloTELL1DataLocation::ADCs ),
  m_decodedPartialADCLocation ( "Raw/Velo/PartialADCs" ),
  m_decodedPedLocation ( LHCb::VeloTELL1DataLocation::Pedestals ),
  m_decodedHeaderLocation ( LHCb::VeloTELL1DataLocation::Headers ),
  m_evtInfoLocation ( EvtInfoLocation::Default ),
  m_veloADCs ( 0 ),
  m_veloPartialADCs ( 0 ),
  m_veloPeds ( 0 ),
  m_decodedADC ( 0 ),
  m_decodedPartialADC ( 0 ),
  m_decodedPed ( 0 ),
  m_decodedHeader ( 0 ),
  m_evtInfo ( 0 ),
  m_adcDataPresent ( false ),
  m_pedDataPresent ( false ),
  m_isDebug ( false ),
  m_signADC ( VeloTELL1::SENSOR_CHANNELS ),
  m_signPartialADC ( VeloTELL1::SENSOR_CHANNELS ),
  m_signADCReordered ( VeloTELL1::SENSOR_CHANNELS ),
  m_signPartialADCReordered ( VeloTELL1::SENSOR_CHANNELS ),
  m_signHeader ( 256 ),
  m_signHeaderReordered ( 256 ),
  m_signPed ( VeloTELL1::SENSOR_CHANNELS ),
  m_signPedReordered ( VeloTELL1::SENSOR_CHANNELS ),
  m_ADCDecoder ( VeloFull ),
  m_ADCPartialDecoder ( VeloFull ),
  m_HeaderDecoder ( VeloHeader ),
  m_PedDecoder ( VeloPedestal )
{ 
  declareProperty("ADCLocation",
                  m_veloADCLocation=VeloFullBankLocation::Default );
  declareProperty("PedestalLocation",
                  m_veloPedLocation=VeloFullBankLocation::Pedestals );
  declareProperty("DecodedADCLocation",
                  m_decodedADCLocation=LHCb::VeloTELL1DataLocation::ADCs );
  declareProperty("DecodedPartialADCLocation",
                  m_decodedPartialADCLocation="Raw/Velo/PartialADCs" );
  declareProperty("DecodedPedestalLocation",
                  m_decodedPedLocation=LHCb::VeloTELL1DataLocation::Pedestals );
  declareProperty("DecodedHeaderLocation",
                  m_decodedHeaderLocation=LHCb::VeloTELL1DataLocation::Headers );
  declareProperty("EventInfoLocation", 
                  m_evtInfoLocation=EvtInfoLocation::Default );
  declareProperty("SectorCorrection", m_sectorCorrection=true);
  declareProperty("CableOrder", m_cableOrder);
}
//=============================================================================
// Destructor
//=============================================================================
DecodeVeloFullRawBuffer::~DecodeVeloFullRawBuffer() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode DecodeVeloFullRawBuffer::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  // Check cable order configuration.  If the configuration is empty,
  // create a configuration with default order.  Otherwise perform
  // sanity checks and bail out when encountering nonsense.
  if (m_cableOrder.empty()) {
    for (unsigned int i=0; i<4; ++i) {
      m_cableOrder.push_back(i);
    }
    std::reverse(m_cableOrder.begin(), m_cableOrder.end());
   } else if (4 != m_cableOrder.size()) {
     error() << "The cable order configuration must have exactly 4 entries." << endmsg;
     return StatusCode::FAILURE;
   } else {
     std::vector<unsigned int> used;
     for (unsigned int i=0; i<4; ++i) {
       if (m_cableOrder[i] > 3) {
         error() << "Only cable positions 0-3 are allowed." << endmsg;
         return StatusCode::FAILURE;
       } else if (used.end() != std::find(used.begin(),used.end(),m_cableOrder[i])) {
         error() << "Each cable position must appear exactly once." << endmsg;
         return StatusCode::FAILURE;
       }
       used.push_back(m_cableOrder[i]);
     }
   }
  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode DecodeVeloFullRawBuffer::execute() {

  m_isDebug   = msgLevel( MSG::DEBUG );

  if (m_isDebug) debug() << "==> Execute" << endmsg;
  //
  getData();
  // decode ADC, Pedestals and EvtInfos data
  decodeData();
  sortAndWriteDecodedData();
  resetMemory();
  //
  return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode DecodeVeloFullRawBuffer::getData()
{
  if (m_isDebug) debug()<< " ==> getData() " <<endmsg;
  //
  m_veloADCs = getIfExists<VeloFullBanks>(adcContName());
  if( NULL == m_veloADCs ){
    if (m_isDebug) debug() << " ==> There is no data banks at: "
      << adcContName() <<endmsg;
  }else{  
    // get the data banks from default TES location
    if (m_isDebug) debug() << " ==> The data banks have been read-in from location: "
        << adcContName()
        << ", size of data container (number of read-out TELL1s): "
        << m_veloADCs->size() <<endmsg;  
    // create container for decoded data
    m_decodedADC=new LHCb::VeloTELL1Datas();
    m_decodedHeader=new LHCb::VeloTELL1Datas();
    m_evtInfo=new EvtInfos();
    //
    setADCDataFlag();
  }

  m_veloPartialADCs = getIfExists<VeloFullBanks>(m_veloPartialADCLocation);
  if( NULL ==  m_veloPartialADCs ){

    if (m_isDebug) debug() << " ==> There is no data banks at: "
      << m_veloPartialADCLocation <<endmsg;

  }else{  

    // get the partial data banks from default TES location
    if (m_isDebug) debug() << " ==> The data banks have been read-in from location: "
        << m_veloPartialADCLocation
        << ", size of data container (number of read-out TELL1s): "
        << m_veloPartialADCs->size() <<endmsg;  

    // --> create container for decoded data
    m_decodedPartialADC=new LHCb::VeloTELL1Datas();

  }

  //
  m_veloPeds = getIfExists<VeloFullBanks>(pedContName());
  if( NULL == m_veloPeds ){
    if (m_isDebug) debug()<< " ==> There is no Pedestals at: "
      << pedContName() <<endmsg;
  }else{  
    // get the pedestals banks from Pedestals TES location
    if (m_isDebug) debug()<< " ==> The ped. banks have been read-in from location: "
      << pedContName()
      << ", size of pedestals container (number of read-out TELL1s): "
      << m_veloPeds->size() <<endmsg;  
    // create container for decoded Ped
    m_decodedPed=new LHCb::VeloTELL1Datas();
    setPedDataFlag();
  }
  //
  return ( StatusCode::SUCCESS );
}
//==============================================================================
std::string DecodeVeloFullRawBuffer::adcContName()
{
  return ( m_veloADCLocation );
}
//==============================================================================
std::string DecodeVeloFullRawBuffer::pedContName()
{
  return ( m_veloPedLocation );
}
//=============================================================================
std::string DecodeVeloFullRawBuffer::decADCName()
{
  return ( m_decodedADCLocation );
}
//=============================================================================
std::string DecodeVeloFullRawBuffer::decPedName()
{
  return ( m_decodedPedLocation );
}
//=============================================================================
std::string DecodeVeloFullRawBuffer::decHeaderName()
{
  return ( m_decodedHeaderLocation );
}
//=============================================================================
std::string DecodeVeloFullRawBuffer::evtInfoName()
{
  return ( m_evtInfoLocation );
}
//=============================================================================
StatusCode DecodeVeloFullRawBuffer::decodeData()
{
  if (m_isDebug) debug()<< " ==> decodeData() " <<endmsg;
  //
  VeloFullBanks::const_iterator sensIt;
  //
  if(adcDataFlag()){
    // decode both ADC and ADCHeaders
    //
    for(sensIt=m_veloADCs->begin(); sensIt!=m_veloADCs->end(); sensIt++){
      m_ADCDecoder.decode(*sensIt,m_signADC);
      m_HeaderDecoder.decode(*sensIt,m_signHeader);
      //
      LHCb::VeloTELL1Data* adcData=new LHCb::VeloTELL1Data((*sensIt)->key(), VeloFull);
      LHCb::VeloTELL1Data* headerData=new LHCb::VeloTELL1Data((*sensIt)->key(), VeloHeader);
      EvtInfo* anInfo=new EvtInfo((*sensIt)->key());
      // data coming from TELL1 board is unsigned int
      // for the subsequent algorithms we required signed
      // values; change from unsigned int -> signed int
      // In the following we apply the cable reordering (sector correction)
      // is necessary for the NZS data to agree with the ZS data.
      // It corrects for the fact that the data cables between 
      // repeater board and Tell1 have to be connected in reverse
      // to their numbering (data 0 -> tell1 input 3,data 1-> tell1 input 2 etc)

      if(m_sectorCorrection){ // need to correct for wrong cabling 
        int counter=0;
        for(scdatIt iT=m_signADC.begin(); iT!=m_signADC.end(); ++iT){ 
          int channelposition=counter;
          channelposition=m_cableOrder[channelposition/512]*512 + channelposition % 512;

          if (m_isDebug) debug() << "ADCbanks: "<< channelposition << " "  <<(*iT) << endmsg;

          m_signADCReordered[channelposition]=static_cast<signed int>(*iT);

          counter++;
        }
        counter=0;
        for(scdatIt iT=m_signHeader.begin(); iT!=m_signHeader.end(); iT++){
          int channelposition=counter;
          channelposition = m_cableOrder[channelposition/64]*64 + channelposition % 64;


          if (m_isDebug) debug() << "Header: "<< channelposition << " " << (*iT) << endmsg;
          m_signHeaderReordered[channelposition]=(static_cast<signed int>(*iT));
          counter++;
        }
        adcData->setDecodedData(m_signADCReordered);
        headerData->setDecodedData(m_signHeaderReordered);
      } else { // no cable reordering requested
        adcData->setDecodedData(m_signADC);
        headerData->setDecodedData(m_signHeader);
      }

      anInfo->setEvtInfo((*sensIt)->getEvtInfo());
      //
      m_decodedADC->insert(adcData);
      m_decodedHeader->insert(headerData);
      m_evtInfo->insert(anInfo);
    }
  }

  if(NULL!=m_veloPartialADCs){
    // --> this part is responsible for the decoding of the partial data
    //     associated with an error bank

    VeloFullBanks::const_iterator partIt=m_veloPartialADCs->begin();
    
    for( ; partIt!=m_veloPartialADCs->end(); ++partIt){

      m_ADCPartialDecoder.decode(*partIt, m_signPartialADC);
      LHCb::VeloTELL1Data* partData=new LHCb::VeloTELL1Data((*partIt)->key(), VeloFull);

      if(m_sectorCorrection){ // need to correct for wrong cabling 

        int counter=0;
        scdatIt pIT=m_signPartialADC.begin();
        
        for( ; pIT!=m_signPartialADC.end(); ++pIT){ 

          int channelposition=counter;
          channelposition=m_cableOrder[channelposition/512]*512 + channelposition % 512;
          m_signPartialADCReordered[channelposition]=static_cast<signed int>(*pIT);
          counter++;

        }

        partData->setDecodedData(m_signPartialADCReordered);

      }else{ // --> no cable reordering requested

        partData->setDecodedData(m_signPartialADC);

      }

      m_decodedPartialADC->insert(partData);

    }

  }

  //
  if(pedDataFlag()){
    for(sensIt=m_veloPeds->begin(); sensIt!=m_veloPeds->end(); sensIt++){
      m_PedDecoder.decode(*sensIt, m_signPed);
      LHCb::VeloTELL1Data* pedData=new LHCb::VeloTELL1Data((*sensIt)->key(), VeloPedestal);

      if(true==m_sectorCorrection){ // need to correct for wrong cabling
        int counter=0;
        for(scdatIt iT=m_signPed.begin(); iT!=m_signPed.end(); iT++){
          int channelposition=counter;
          channelposition = m_cableOrder[channelposition/512]*512 + channelposition % 512;
          if (m_isDebug) debug() << "Ped bank: " << channelposition << " "  <<(*iT) << endmsg;  
          m_signPedReordered[channelposition]=static_cast<signed int>(*iT);
          counter++;
        }
        pedData->setDecodedData(m_signPedReordered);
      } else { // no cable reordering requested
        pedData->setDecodedData(m_signPed);
      }
      m_decodedPed->insert(pedData);
    }
  }
  //
  if((!adcDataFlag())&&(!pedDataFlag())){
    if ( m_isDebug ) debug()<< " ==> No data decoded! " <<endmsg;
    return ( StatusCode::SUCCESS );
  }else{
    return ( StatusCode::SUCCESS );
  }
}
//=============================================================================
void DecodeVeloFullRawBuffer::sortAndWriteDecodedData()
{
  if (m_isDebug) debug()<< " ==> writeDecodedData() " <<endmsg;
  //
  if(adcDataFlag()){
    // sort the output containers by key (TELL1 number)
    std::stable_sort(m_decodedADC->begin(), m_decodedADC->end(),
        VeloEventFunctor::Less_by_key<LHCb::VeloTELL1Data*>());
    std::stable_sort(m_decodedHeader->begin(), m_decodedHeader->end(),
        VeloEventFunctor::Less_by_key<LHCb::VeloTELL1Data*>());
    std::stable_sort(m_evtInfo->begin(), m_evtInfo->end(),
        VeloEventFunctor::Less_by_key<EvtInfo*>());
    // put the data to TES
    put(m_decodedADC, decADCName());
    put(m_decodedHeader, decHeaderName());
    put(m_evtInfo, evtInfoName());
  }

  if(NULL!=m_decodedPartialADC)
  {
    if(!m_decodedPartialADC->empty()) if (m_isDebug) debug()<< " --> Write some data " <<endmsg;
    
    put(m_decodedPartialADC, m_decodedPartialADCLocation);

  }

  if(pedDataFlag()){
    // sort decoded pedestals
    std::stable_sort(m_decodedPed->begin(), m_decodedPed->end(),
        VeloEventFunctor::Less_by_key<LHCb::VeloTELL1Data*>());
    // put decoded peds in TES
    put(m_decodedPed, decPedName());
  }
}
//=============================================================================
void DecodeVeloFullRawBuffer::setADCDataFlag()
{
  m_adcDataPresent=true;
}
//=============================================================================
void DecodeVeloFullRawBuffer::setPedDataFlag()
{
  m_pedDataPresent=true;
}
//=============================================================================
bool DecodeVeloFullRawBuffer::adcDataFlag()
{
  return ( m_adcDataPresent );
}
//=============================================================================
bool DecodeVeloFullRawBuffer::pedDataFlag()
{
  return ( m_pedDataPresent );
}
//=============================================================================
void DecodeVeloFullRawBuffer::unsetADCDataFlag()
{
  m_adcDataPresent=false;
}
//=============================================================================
void DecodeVeloFullRawBuffer::unsetPedDataFlag()
{
  m_pedDataPresent=false;
}
//
void DecodeVeloFullRawBuffer::resetMemory()
{
  if (m_isDebug) debug()<< " ==> resetMemory() " <<endmsg;
  //
  unsetADCDataFlag();
  unsetPedDataFlag();
}
//
