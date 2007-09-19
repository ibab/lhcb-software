// $Id: DecodeVeloFullRawBuffer.cpp,v 1.2 2007-09-19 15:06:15 szumlat Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "DecodeVeloFullRawBuffer.h"
#include "VeloFullDecoder.h"

// Kernel
#include "Kernel/VeloEventFunctor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DecodeVeloFullRawBuffer
//
// 2006-04-21 : Tomasz Szumlak
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DecodeVeloFullRawBuffer );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DecodeVeloFullRawBuffer::DecodeVeloFullRawBuffer( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_veloADCLocation ( VeloFullBankLocation::Default ),
    m_veloPedLocation ( VeloFullBankLocation::Pedestals ),
    m_decodedADCLocation ( VeloTELL1DataLocation::ADCs ),
    m_decodedPedLocation ( VeloTELL1DataLocation::Pedestals ),
    m_decodedHeaderLocation ( VeloTELL1DataLocation::Headers ),
    m_evtInfoLocation ( EvtInfoLocation::Default ),
    m_veloADCs ( 0 ),
    m_veloPeds ( 0 ),
    m_decodedADC ( 0 ),
    m_decodedPed ( 0 ),
    m_decodedHeader ( 0 ),
    m_evtInfo ( 0 ),
    m_adcDataPresent ( false ),
    m_pedDataPresent ( false )
{ 
  declareProperty("SectorCorrection", m_sectorCorrection=true);
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

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode DecodeVeloFullRawBuffer::execute() {

  debug() << "==> Execute" << endmsg;
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
//  Finalize
//=============================================================================
StatusCode DecodeVeloFullRawBuffer::finalize() {

  debug() << "==> Finalize" << endmsg;
  //
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
//=============================================================================
StatusCode DecodeVeloFullRawBuffer::getData()
{
  debug()<< " ==> getData() " <<endmsg;
  //
  if(!exist<VeloFullBanks>(adcContName())){
    info()<< " ==> There is no data banks at: "
           << adcContName() <<endmsg;
  }else{  
    // get data banks from default TES location
    m_veloADCs=get<VeloFullBanks>(adcContName());
    debug()<< " ==> The data banks have been read-in from location: "
           << adcContName()
           << ", size of data container (number of read-out TELL1s): "
           << m_veloADCs->size() <<endmsg;  
    // create container for decoded data
    m_decodedADC=new VeloTELL1Datas();
    m_decodedHeader=new VeloTELL1Datas();
    m_evtInfo=new EvtInfos();
    //
    setADCDataFlag();
  }
  //
  if(!exist<VeloFullBanks>(pedContName())){
    debug()<< " ==> There is no Pedestals at: "
           << pedContName() <<endmsg;
  }else{  
    // get the pedestals banks from Pedestals TES location
    m_veloPeds=get<VeloFullBanks>(pedContName());
    debug()<< " ==> The ped. banks have been read-in from location: "
           << pedContName()
           << ", size of pedestals container (number of read-out TELL1s): "
           << m_veloPeds->size() <<endmsg;  
    // create container for decoded Ped
    m_decodedPed=new VeloTELL1Datas();
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
  debug()<< " ==> decodeData() " <<endmsg;
  //
  VeloFullBanks::const_iterator sensIt;
  //
  if(adcDataFlag()){
    // decode both ADC and ADCHeaders
    VeloFullDecoder* ADCDecoder=new VeloFullDecoder(VeloFull);
    VeloFullDecoder* HeaderDecoder=new VeloFullDecoder(VeloHeader);
    //
    for(sensIt=m_veloADCs->begin(); sensIt!=m_veloADCs->end(); sensIt++){
      dataVec ADCData=ADCDecoder->decode(*sensIt);
      dataVec HeaderData=HeaderDecoder->decode(*sensIt);
      //
      VeloTELL1Data* adcData=new VeloTELL1Data((*sensIt)->key(), VeloFull);
      VeloTELL1Data* headerData=new VeloTELL1Data((*sensIt)->key(), VeloHeader);
      EvtInfo* anInfo=new EvtInfo((*sensIt)->key());
      // data coming from TELL1 board is unsigned int
      // for the subsequent algorithms we required signed
      // values; change from unsigned int -> signed int
      // In the following we apply the cable reordering (sector correction)
      // is necessary for the NZS data to agree with the ZS data.
      // It corrects for the fact that the data cables between 
      // repeater board and Tell1 have to be connected in reverse
      // to their numbering (data 0 -> tell1 input 3,data 1-> tell1 input 2 etc)
      

      sdataVec signADC(2048);
      int counter=0;
      for(cdatIt iT=ADCData.begin(); iT!=ADCData.end(); iT++){	
     	int channelposition=counter;
	if(true==m_sectorCorrection){  
	  
     
	  switch(counter/512){
	    
	  case 0:
	    channelposition+=1536; break;
	  case 1:
	    channelposition+=512; break;
	  case 2:
	    channelposition-=512; break;
	  case 3:
	    channelposition-=1536; break;
	  default:
	    break;
	    
	     
	  }
	}
	
	debug() << "ADCbanks: "<< channelposition << " "  <<(*iT) << endmsg;

        signADC[channelposition]=static_cast<signed int>(*iT);
	
	counter++;
      }
      // the same operation for headers

      sdataVec signHeader(256);
      counter=0;
      for(cdatIt iT=HeaderData.begin(); iT!=HeaderData.end(); iT++){
	int channelposition=counter;
	if(m_sectorCorrection==true){
	  switch(channelposition/64){
	  case 0:
	    channelposition+=192; break;
	  case 1:
	    channelposition+=64; break;
	  case 2:
	    channelposition-=64; break;
	  case 3:
	    channelposition-=192; break;
	  default:
	    break;
	  }
	}
	
	
	debug() << "Header: "<< channelposition << " " << (*iT) << endmsg;
        signHeader[channelposition]=(static_cast<signed int>(*iT));
	counter++;
      }
    
      adcData->setDecodedData(signADC);
      headerData->setDecodedData(signHeader);
      anInfo->setEvtInfo((*sensIt)->getEvtInfo());
      //
      m_decodedADC->insert(adcData);
      m_decodedHeader->insert(headerData);
      m_evtInfo->insert(anInfo);
    }
    //
    delete ADCDecoder;
    delete HeaderDecoder;
  }
  //
  if(pedDataFlag()){
    VeloFullDecoder* PedDecoder=new VeloFullDecoder(VeloPedestal);
    for(sensIt=m_veloPeds->begin(); sensIt!=m_veloPeds->end(); sensIt++){
      dataVec PedData=PedDecoder->decode(*sensIt);
      VeloTELL1Data* pedData=new VeloTELL1Data((*sensIt)->key(), VeloPedestal);
      // change unsigned to signed
      sdataVec signPed(2048);
 
      //Do the sector reordering for the pedestal banks.
      //See decoding of ADC banks for more info.
     int counter=0;
      for(cdatIt iT=PedData.begin(); iT!=PedData.end(); iT++){
	int channelposition=counter;
	if(true==m_sectorCorrection){  
	       
	  switch(counter/512){
	    
	  case 0:
	    channelposition+=1536; break;
	  case 1:
	    channelposition+=512; break;
	  case 2:
	    channelposition-=512; break;
	  case 3:
	    channelposition-=1536; break;
	  default:
	    break;
	    
	     
	  }
	}
	debug() << "Ped bank: " << channelposition << " "  <<(*iT) << endmsg;	
        signPed[channelposition]=static_cast<signed int>(*iT);
	counter++;
      }
      pedData->setDecodedData(signPed);
      m_decodedPed->insert(pedData);
    }
    delete PedDecoder;
  }
  //
  if((!adcDataFlag())&&(!pedDataFlag())){
    info()<< " ==> No data decoded! " <<endmsg;
    return ( StatusCode::SUCCESS );
  }else{
    return ( StatusCode::SUCCESS );
  }
}
//=============================================================================
void DecodeVeloFullRawBuffer::sortAndWriteDecodedData()
{
  debug()<< " ==> writeDecodedData() " <<endmsg;
  //
  if(adcDataFlag()){
    // sort the output containers by key (TELL1 number)
    std::stable_sort(m_decodedADC->begin(), m_decodedADC->end(),
                     VeloEventFunctor::Less_by_key<VeloTELL1Data*>());
    std::stable_sort(m_decodedHeader->begin(), m_decodedHeader->end(),
                     VeloEventFunctor::Less_by_key<VeloTELL1Data*>());
    std::stable_sort(m_evtInfo->begin(), m_evtInfo->end(),
                     VeloEventFunctor::Less_by_key<EvtInfo*>());
    // put the data to TES
    put(m_decodedADC, decADCName());
    put(m_decodedHeader, decHeaderName());
    put(m_evtInfo, evtInfoName());
  }
  if(pedDataFlag()){
    // sort decoded pedestals
    std::stable_sort(m_decodedPed->begin(), m_decodedPed->end(),
                     VeloEventFunctor::Less_by_key<VeloTELL1Data*>());
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
  debug()<< " ==> resetMemory() " <<endmsg;
  //
  unsetADCDataFlag();
  unsetPedDataFlag();
}
//
