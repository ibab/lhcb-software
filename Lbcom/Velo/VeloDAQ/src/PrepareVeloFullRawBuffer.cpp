// $Id: PrepareVeloFullRawBuffer.cpp,v 1.9 2010-03-03 21:03:20 szumlat Exp $
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

// stl
#include <algorithm>

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
    m_data ( ),
    m_ped ( ),
    m_numberOfSensors ( 0 ),
    m_sensors ( ),
    m_veloADCData ( 0 ),
    m_veloPedestals ( 0 ),
    m_veloPedestalsContainer ( VeloFullBankLocation::Pedestals ),
    m_adcBankPresent ( false ),
    m_pedBankPresent ( false )
{
  declareProperty("RunWithODIN", m_runWithODIN=true); 
  declareProperty("RawEventLocation", m_rawEventLoc=LHCb::RawEventLocation::Default);
  declareProperty("ADCLocation", m_veloADCDataContainer=VeloFullBankLocation::Default);
  declareProperty("RoundRobin", m_roundRobin=false);
  declareProperty("DecodeErrorStream", m_decodeErrorStream=false);
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
    if(adcBankFlag()||pedBankFlag()){
      int readedSensors=numberOfSensors();
      debug()<< " read sens: " << readedSensors <<endmsg;
      for(int sensor=0; sensor<readedSensors; sensor++){
        if(adcBankFlag()||pedBankFlag()) createOrderedSections(sensor);
      }
      writeVeloFull();
      // flush the vectors with pointers 
      // to the data bank after each event
      resetMemory();
    }else if(m_roundRobin){
      writeVeloFull();
    }
  }else{
    return ( StatusCode::SUCCESS);
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
  debug()<< " ==> getRawEvent() " <<endmsg;
  debug()<< "--------------------" <<endmsg;
  //
  if(!exist<LHCb::RawEvent>(m_rawEventLoc)){
    error()<< " ==> There is no RawEvent at: "
           <<  m_rawEventLoc  <<endmsg;
    return ( StatusCode::FAILURE );
  }else{  
    // get the RawEvent from default TES location
    m_rawEvent=get<LHCb::RawEvent>(m_rawEventLoc);
    debug()<< " ==> The RawEvent has been read-in from location: "
           << m_rawEventLoc  <<endmsg;  
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
  const std::vector<LHCb::RawBank*>& errorBanks=
        m_rawEvent->banks(LHCb::RawBank::VeloError);
  std::vector<LHCb::RawBank*>::const_iterator bIt;
  // if so get all info needed to decoding
  if(fullBanks.size()!=0){
    m_decodeErrorStream ? setNumberOfSensors(errorBanks.size()) :
                          setNumberOfSensors(fullBanks.size()-errorBanks.size());
    setADCBankFlag();
    m_veloADCData=new VeloFullBanks();
    //
    debug()<< "VeloFull bank detected of size: " << fullBanks.size() <<endmsg;
    for(bIt=fullBanks.begin(); bIt!=fullBanks.end(); bIt++){
      LHCb::RawBank* aBank=(*bIt);
      // protection against wrong magic pattern
      if(LHCb::RawBank::MagicPattern!=aBank->magic()){
        debug()<< " --> Bank with source ID: " << (aBank->sourceID())
               << " is corrupted! " <<endmsg;
        return ( StatusCode::FAILURE );
      }
      // get the sensor number == sourceID
      int sensor=aBank->sourceID();
      // skip data send out together with an error bank
      if(0!=errorBanks.size()){
        std::vector<LHCb::RawBank*>::const_iterator isError;
        isError=find_if(errorBanks.begin(), errorBanks.end(), errorBankFinder(sensor));
        if(m_decodeErrorStream){
          if(isError==errorBanks.end()) continue;
        }else{
          if(isError!=errorBanks.end()) continue;
        }
      }
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
  // check if there is ODIN bank present
  const std::vector<LHCb::RawBank*>& odinBank=
    m_rawEvent->banks(LHCb::RawBank::ODIN);
  // cache the bank if present
  if(odinBank.empty()){
    if(m_runWithODIN){
      Error(" ==> ODIN bank missing!");
      return ( StatusCode::FAILURE );
    }
  }
  //
  if(m_data.empty()){
    m_veloADCData=new VeloFullBanks();
    unsigned int msgCount=0;
    if(msgLevel(MSG::DEBUG)) msgCount=10;
    Warning(" --> There are no NZS banks!", StatusCode::SUCCESS, msgCount).ignore();
  }
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
  if(adcBankFlag()||(m_roundRobin)){
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
  if(adcBankFlag()) unsetADCBankFlag();
  if(pedBankFlag()) unsetPedBankFlag();
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

