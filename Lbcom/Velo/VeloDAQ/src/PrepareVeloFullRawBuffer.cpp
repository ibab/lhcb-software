// $Id: PrepareVeloFullRawBuffer.cpp,v 1.1 2007-09-18 00:04:14 szumlat Exp $
// Include files 
#include <stdexcept>
#include <exception>

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiAlg/Tuples.h"

// local
#include "PrepareVeloFullRawBuffer.h"

// data model
#include "Event/RawEvent.h"
#include "Event/RawBank.h"

//-----------------------------------------------------------------------------

// Implementation file for class : PrepareVeloFullRawBuffer
//
// 2006-03-22 : Tomasz Szumlak
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PrepareVeloFullRawBuffer );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrepareVeloFullRawBuffer::PrepareVeloFullRawBuffer( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator ),
    m_rawEvent ( 0 ),
    m_rawEventLoc ( LHCb::RawEventLocation::Default ),
    m_data ( ),
    m_ped ( ),
    m_error ( ),
    m_numberOfSensors ( 0 ),
    m_sensors ( ),
    m_veloADCData ( 0 ),
    m_veloPedestals ( 0 ),
    m_errorBank ( 0 ),
    m_veloADCDataContainer ( VeloFullBankLocation::Default ),
    m_veloPedestalsContainer ( VeloFullBankLocation::Pedestals ),
    m_errorBankContainer ( VeloErrorBankLocation::Default ),
    m_adcBankPresent ( false ),
    m_pedBankPresent ( false ),
    m_errorBankPresent ( false )
{

}
//=============================================================================
// Destructor
//=============================================================================
PrepareVeloFullRawBuffer::~PrepareVeloFullRawBuffer() {}
//=============================================================================
// Initialization
//=============================================================================
StatusCode PrepareVeloFullRawBuffer::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  //
  //  setHistoTopDir( "Vetra/" );
  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode PrepareVeloFullRawBuffer::execute() {

  debug() << "==> Execute" << endmsg;
  //  
  StatusCode sc=getRawEvent();
  if(sc){
    getRawBanks();
    // if there is adc bank or pedestal bank begin ordering
    // for all read-out sensors
    if(adcBankFlag()||pedBankFlag()||errorBankFlag()){      
      int readedSensors=numberOfSensors();
      debug()<< " readed sens: " << readedSensors <<endmsg;
      for(int sensor=0; sensor<readedSensors; sensor++){
        if(adcBankFlag()||pedBankFlag()) createOrderedSections(sensor);
        if(errorBankFlag()) writeErrorBanks(sensor);
      }
      writeVeloFull();
      // flush the vectors with pointers 
      // to the data bank after each event
      resetMemory();
    }
  }else{
    return ( StatusCode::FAILURE );
  }
  //
  debug()<< " end of execute" <<endmsg;
  return StatusCode::SUCCESS;
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode PrepareVeloFullRawBuffer::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
//=============================================================================
StatusCode PrepareVeloFullRawBuffer::getRawEvent()
{
  debug()<< " ==> getRawBuffer() " <<endmsg;
  debug()<< "--------------------" <<endmsg;
  //
  if(!exist<LHCb::RawEvent>(m_rawEventLoc)){
    error()<< " ==> There is no RawEvent at: "
           << LHCb::RawEventLocation::Default <<endmsg;
    return ( StatusCode::FAILURE );
  }else{  
    // get the RawEvent from default TES location
    m_rawEvent=get<LHCb::RawEvent>(m_rawEventLoc);
    debug()<< " ==> The RawEvent has been read-in from location: "
           << LHCb::RawEventLocation::Default <<endmsg;  
  }
  //
  return ( StatusCode::SUCCESS );
}
//=============================================================================
// /// ///////////////////////////////////////////////////////////////// /// //
// The getRawBanks() method is meant to check if non-zero suppresed data     //
// banks and pedestal banks are present in the read-in RawEvent, if this is  //
// the case the additional information needed to decoding are retrieved.     //
// For the VeloFull bank (non-zero data) pointers to the beggining of the    //
// data body of the bank are put into m_data container. Also, numbers of     //
// the read-out sensors (TELL1s) are stored inside m_sensors vector this     //
// allows to keep both number of read-out sensors and their IDs.             //
// The same procedure is repeated for VeloPedestal banks if they are         //
// detected (beside of caching the information about the sensors).           //
// /// ///////////////////////////////////////////////////////////////// /// //
StatusCode PrepareVeloFullRawBuffer::getRawBanks()
{
  debug()<< " ==> getRawBanks() " <<endmsg;
  // check if there is non-zero suppressed bank present
  const std::vector<LHCb::RawBank*>& fullBanks=
        m_rawEvent->banks(LHCb::RawBank::VeloFull);
  std::vector<LHCb::RawBank*>::const_iterator bIt;
  // if so get all info needed to decoding
  if(fullBanks.size()!=0){
    setNumberOfSensors(fullBanks.size());
    setADCBankFlag();
    m_veloADCData=new VeloFullBanks();
    //
    debug()<< "VeloFull bank detected of size: " << fullBanks.size() <<endmsg;
    for(bIt=fullBanks.begin(); bIt!=fullBanks.end(); bIt++){
      LHCb::RawBank* aBank=(*bIt);
      // get the sensor number == sourceID
      int sensor=aBank->sourceID();
      debug()<< " sensor number: " << sensor <<endmsg;
      debug()<< " sensor vect size: " << m_sensors.size() <<endmsg;
      m_sensors.push_back(sensor);
      // get pointer to the beginning of data body of the bank
      unsigned int* data=aBank->data();
      debug()<< " ptr data[0] " << data <<endmsg;
      m_data.push_back(data);
    }
  }
  // check if there is pedestal bank present
  const std::vector<LHCb::RawBank*>& pedBanks=
    m_rawEvent->banks(LHCb::RawBank::VeloPedestal);
  // if so get informations about it
  if(pedBanks.size()!=0){
    setPedBankFlag();
    // check if number of sensor is set up
    if(adcBankFlag()==0) setNumberOfSensors(pedBanks.size());
    m_veloPedestals=new VeloFullBanks();
    //
    debug()<< "VeloPed bank detected of size: " << pedBanks.size() <<endmsg;
    for(bIt=pedBanks.begin(); bIt!=pedBanks.end(); bIt++){
      LHCb::RawBank* aBank=(*bIt);
      // get the sensor number == sourceID if there is no full data
      if(adcBankFlag()==0){
        int sensor=aBank->sourceID();
        debug()<< " sensor number: " << sensor <<endmsg;
        m_sensors.push_back(sensor);
      }
      // get pointer to the bank data
      unsigned int* data=aBank->data();
      debug()<< "Pointer to ped bank: " << data << " of number: "
            << aBank->sourceID() <<endmsg;
      m_ped.push_back(data);
    }
  }
  // check if there is error bank present
  const std::vector<LHCb::RawBank*>& errorBank=
    m_rawEvent->banks(LHCb::RawBank::VeloError);
  // if so write out the banks
  if(errorBank.size()!=0){
    setErrorBankFlag();
    // check if number of sensor is set up
    if(adcBankFlag()==0&&pedBankFlag()==0) 
      setNumberOfSensors(errorBank.size());
    m_errorBank=new VeloErrorBanks();
    //
    debug()<< "Error bank detected of size: " << errorBank.size() <<endmsg;
    for(bIt=errorBank.begin(); bIt!=errorBank.end(); ++bIt){
      LHCb::RawBank* aBank=(*bIt);
      // get the sensor number == sourceID if there is no full data
      if(adcBankFlag()==0&&pedBankFlag()==0){
        int sensor=aBank->sourceID();
        debug()<< " sensor number: " << sensor <<endmsg;
        m_sensors.push_back(sensor);
      }
      // get pointer to the bank data
      unsigned int* data=aBank->data();
      m_error.push_back(data);
    }
  }
  // check if there is ODIN bank present
  const std::vector<LHCb::RawBank*>& odinBank=
    m_rawEvent->banks(LHCb::RawBank::ODIN);
  // cache the bank if present
  if(odinBank.empty()){
    Error(" ==> Data stream corrupted!");
    return ( StatusCode::FAILURE );
  }
  //
  if(m_data.empty()) return ( StatusCode::FAILURE );
  //
  return ( StatusCode::SUCCESS );
}
//=============================================================================
void PrepareVeloFullRawBuffer::setNumberOfSensors(int sensors)
{
  debug()<< " ==> setNumberOfTELL1s() " <<endmsg;
  //
  m_numberOfSensors=sensors;
  //
  return;
}
//=============================================================================
int PrepareVeloFullRawBuffer::numberOfSensors()
{
  debug()<< " ==> numberOfTELL1s() " <<endmsg;
  //
  return ( m_numberOfSensors );
}
//=============================================================================
// /// ///////////////////////////////////////////////////////////////// /// //
// if there are both banks type order the data and create new data object    //
// and pass it to the next stage of the processing                           //
// /// ///////////////////////////////////////////////////////////////// /// //
void PrepareVeloFullRawBuffer::createOrderedSections(int nbOfBank)
{
  debug()<< " ==> createOrderedSection() " <<endmsg;
  //
  const int source=sourceID(nbOfBank);
  //
  debug()<< "adcBank: " << adcBankFlag() <<endmsg;
  if(adcBankFlag()){
    int type_1=LHCb::RawBank::VeloFull;
    //
    debug()<< " sensor: " << source <<endmsg;
    VeloFullBank* aFullBank=new VeloFullBank(source, data(nbOfBank), type_1);
    m_veloADCData->insert(aFullBank);
  }
  //
  if(pedBankFlag()){
    int type_2=LHCb::RawBank::VeloPedestal;
    //
    debug()<< " sensor: " << source <<endmsg;
    VeloFullBank* aPedBank=new VeloFullBank(source, pedestals(nbOfBank), type_2);
    m_veloPedestals->insert(aPedBank);
  }
}
//=============================================================================
StatusCode PrepareVeloFullRawBuffer::writeVeloFull()
{
  debug()<< " ==> writeVeloFull() " <<endmsg;
  //
  if(adcBankFlag()){
    put(m_veloADCData, m_veloADCDataContainer);
    debug()<< "Registered container with bank data of size: "
          << m_veloADCData->size() << ", at" 
          << m_veloADCDataContainer <<endmsg;
  }
  //  
  if(pedBankFlag()){
    put(m_veloPedestals, m_veloPedestalsContainer);
    debug()<< "Registered container with bank data of size: "
          << m_veloPedestals->size() << ", at" 
          << m_veloPedestalsContainer <<endmsg;
  }
  //
  if(errorBankFlag()){
    put(m_errorBank, m_errorBankContainer);
    debug()<< "Registered container with error bank of size: "
          << m_errorBank->size() << ", at: "
          << m_errorBankContainer <<endmsg;
  }
  //
  return ( StatusCode::SUCCESS );
}
//=============================================================================
int PrepareVeloFullRawBuffer::sourceID(int nbOfBank)
{
  debug()<< " ==> sourceID() " <<endmsg;
  //
  return ( m_sensors[nbOfBank] );
}
//=============================================================================
void PrepareVeloFullRawBuffer::resetMemory()
{
  debug()<< " ==> resetMemory() " <<endmsg;
  //
  m_data.clear();
  m_sensors.clear();
  m_ped.clear();
  m_error.clear();
  if(adcBankFlag()) unsetADCBankFlag();
  if(pedBankFlag()) unsetPedBankFlag();
  if(errorBankFlag()) unsetErrorBankFlag();
}
//=============================================================================
void PrepareVeloFullRawBuffer::dumpADCs(const dataVec& inADCs)
{
  debug()<< " Dumping ADC values for all channels (1 TELL1)" <<endmsg;
  //
  cdatIt adcIt;
  int ii=0;
  for(adcIt=inADCs.begin(); adcIt!=inADCs.end(); adcIt++, ii++){
    if(!(ii%32)){
      debug()<< " Beggining dumping ALink number: " << ii/32 <<endmsg;
    }
    //
    std::cout<< "[Beetle, Channel] = " << "[" << ii/128 << ", "
             << ii%32 << "]" << (*adcIt) <<std::endl;
  }
}
//=============================================================================
unsigned int* PrepareVeloFullRawBuffer::data(int noOfSensor)
{
  return ( m_data[noOfSensor] );
}
//=============================================================================
unsigned int* PrepareVeloFullRawBuffer::pedestals(int noOfSensor)
{
  return ( m_ped[noOfSensor] );
}
//=============================================================================
unsigned int* PrepareVeloFullRawBuffer::errors(int noOfSensor)
{
  return ( m_error[noOfSensor] );
}
//=============================================================================
void PrepareVeloFullRawBuffer::setADCBankFlag()
{
  m_adcBankPresent=true;
}
//=============================================================================
void PrepareVeloFullRawBuffer::unsetADCBankFlag()
{
  m_adcBankPresent=false;
}
//=============================================================================
void PrepareVeloFullRawBuffer::setPedBankFlag()
{
  m_pedBankPresent=true;
}
//=============================================================================
void PrepareVeloFullRawBuffer::unsetPedBankFlag()
{
  m_pedBankPresent=false;
}
//=============================================================================
void PrepareVeloFullRawBuffer::setErrorBankFlag()
{
  m_errorBankPresent=true;
}
//=============================================================================
void PrepareVeloFullRawBuffer::unsetErrorBankFlag()
{
  m_errorBankPresent=false;
}
//=============================================================================
bool PrepareVeloFullRawBuffer::adcBankFlag()
{
  return ( m_adcBankPresent );
}
//=============================================================================
bool PrepareVeloFullRawBuffer::pedBankFlag()
{
  return ( m_pedBankPresent );
}
//=============================================================================
bool PrepareVeloFullRawBuffer::errorBankFlag()
{
  return ( m_errorBankPresent );
}
//=============================================================================
void PrepareVeloFullRawBuffer::writeErrorBanks(int noOfBank)
{
  debug()<< " ==> writeErrorBank() " <<endmsg;
  // get the pointer to the current error bank
  unsigned int* errorData=errors(noOfBank);
  const int tell1=sourceID(noOfBank);
  debug()<< "pointers to error bank: " << errorData <<endmsg;  
  dataVec sources;
  // check how many errors were sent, sources == number of PPFPGA
  sources=countErrorBanks(noOfBank);
  // if there are errors create EvtInfo object to cache
  // EvtInfo sections from ErrorBank
  EvtInfo anEvtInfo(tell1);
  allEvt evtInfoData;
  // create allError vector always for default ErrorBank content
  allError errorInfoData;
  // create new ErrorBank object for the current sensor
  VeloErrorBank* anBank=new VeloErrorBank(tell1);  
  // initialize error bank using sources
  if(sources.size()!=0){
    for(int PPFPGA=0; PPFPGA<NumberOfPPFPGA; PPFPGA++){
      bool isError=findObject(sources, PPFPGA);
      if(isError){
        evtInfo anInfo;
        errorInfo anErrorData;
        // fill the errorInfo section
        anInfo.push_back(*errorData);      
        anInfo.push_back(*(errorData+ERROR_HEADER_1));
        int tempWord=((*(errorData+ERROR_HEADER_2))>>bitShift16)&bitMask16;
        anInfo.push_back(tempWord);
        for(int word=0; word<ERROR_FIRST_SECTION; word++){
          anInfo.push_back(*(errorData+ERROR_FIRST_SECTION+word));
        }
        //
        evtInfoData.push_back(anInfo);
        //
        for(int word=0; word<ERROR_FIRST_SECTION; word++){
          anErrorData.push_back(*(errorData+word));
        }
        for(int word=0; word<ERROR_SECOND_SECTION; word++){
          anErrorData.push_back(*(errorData+ERROR_THIRD_SECTION+word));
        }
        //
        errorInfoData.push_back(anErrorData);
        //
        errorData+=PPFPGASectionJump;
        debug()<< " jump: " << PPFPGASectionJump <<endmsg;
        debug()<< " errorData after jump: " << errorData <<endmsg;
        
      }
    } 
    anEvtInfo.setEvtInfo(evtInfoData);
    //
    anBank->setEvtInfoSection(anEvtInfo);
    anBank->setErrorInfoSection(errorInfoData);
    anBank->setErrorSources(sources);
  }else{
    for(int PPFPGA=0; PPFPGA<NumberOfPPFPGA; PPFPGA++){
      errorInfo anErrorData;
      for(int word=0; word<ERROR_EVT_INFO; word++){
        anErrorData.push_back(*(errorData+word));
      }
      errorInfoData.push_back(anErrorData);
      errorData+=BankJump;
    }
    anBank->setErrorInfoSection(errorInfoData);
  }
  //
  m_errorBank->insert(anBank);
}
//=============================================================================
dataVec PrepareVeloFullRawBuffer::countErrorBanks(int noOfBank)
{
  debug()<< " ==> countErrorBanks() " <<endmsg;
  // get pointer to the current error bank
  unsigned int* errorData=errors(noOfBank);
  debug()<< "ptr err bank: " << errorData <<endmsg;
  // number of PPFPGA
  unsigned int PPFPGA=0;
  // vector for PPFPGA for which error ocurred
  dataVec sources;
  try{
    while(PPFPGA<NumberOfPPFPGA){
      // go to the first marker
      /// @DEBUG: look at the content of the mem. cell using pointers
      debug()<< "ptr err bank: " << errorData <<endmsg;
      debug()<< " no of PP: " << PPFPGA <<endmsg;
      debug()<< "bnt cnt: " << ((*errorData)&bitMask12) <<endmsg;
      debug()<< "l0: " << (*(errorData+ERROR_HEADER_1)) <<endmsg;
      errorData+=FirstMarkerLocation;
      debug()<< " first marker: "
             << std::hex <<(((*errorData)>>bitShift8)&bitMask8)
             << " second marker: " << ((*errorData)&bitMask8) 
             << std::dec <<endmsg;      
      /// @DEBUG
      if(((((*errorData)>>bitShift8)&bitMask8)==EBMarker)&&
         ((*errorData)&bitMask8)==EBMarker0){
        errorData+=NextMarker;
         if(((((*errorData)>>bitShift8)&bitMask8)==EBMarker)&&
           ((*errorData)&bitMask8)==EBMarker1 ){
          // error bank is empty, do not write the PPFPGA number!
          PPFPGA++;
          errorData+=NextBankIfEmptyJump;
        }else{
          // bank is not empty, write the number of PPFPGA
          sources.push_back(PPFPGA);
          PPFPGA++;
          errorData+=BankJump;
        }
      }else{
        debug()<< "value: 0" << ((*errorData)&bitMask8) <<endmsg;
        debug()<< "value: 1" << (((*errorData)>>bitShift8)&bitMask8) <<endmsg;
        throw std::string(" ==> Data corruption!");
      }
    }
  }catch (std::string& str){
    info()<< ( str ) <<endmsg;
  }
  //
  return ( sources );
}
//=============================================================================
bool PrepareVeloFullRawBuffer::findObject(const dataVec& inCont,
                                          const unsigned int obj) const
{
  // predicate like method to search the vector for a given object
  cdatIt It;
  It=inCont.begin();
  while(It!=inCont.end()){
    if((*It)==obj) return ( true );
    It++;
  }
  It=inCont.end();
  if((*It)==obj) return ( true );
  //
  return ( false );
}
//

