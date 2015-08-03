// Include files 
#include <stdexcept>
#include <exception>

// from Gaudi
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
DECLARE_ALGORITHM_FACTORY( PrepareVeloFullRawBuffer )

typedef std::map<unsigned int, std::pair<unsigned int, unsigned int*> > DATA_REPO;
typedef DATA_REPO::const_iterator DR_IT;
typedef std::pair<unsigned int, unsigned int* > DATA_PAIR;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrepareVeloFullRawBuffer::PrepareVeloFullRawBuffer( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator ),
    m_rawEvent ( 0 ),
    m_veloADCData ( 0 ),
    m_veloADCPartialData ( 0 ),
    m_veloPedestals ( 0 ),
    m_veloPedestalsLoc ( VeloFullBankLocation::Pedestals ),
    m_adcBankPresent ( false ),
    m_pedBankPresent ( false ),
    m_isDebug ( msgLevel( MSG::DEBUG ) )
{

  declareProperty("RunWithODIN", m_runWithODIN=true); 
  declareProperty("RawEventLocation", 
                  m_rawEventLoc=LHCb::RawEventLocation::Default);
  declareProperty("ADCLocation", 
                  m_veloADCDataLoc=VeloFullBankLocation::Default);
  declareProperty("ADCPartialLoc",
                  m_veloADCPartialDataLoc="Raw/Velo/PreparedPartialADC");
  declareProperty("RoundRobin", m_roundRobin=false);
  declareProperty("IgnoreErrorBanks", m_ignoreErrorBanks=true);

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

  if(m_isDebug) debug() << "==> Initialize" << endmsg;
  //
  //  setHistoTopDir( "Vetra/" );
  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode PrepareVeloFullRawBuffer::execute() {

  if(m_isDebug) debug() << "==> Execute" << endmsg;
  //  
  StatusCode rawEvtStatus=getRawEvent();

  if(rawEvtStatus.isSuccess()){

    getRawBanks();
    // if there is adc bank or pedestal bank begin ordering

    if(adcBankFlag()||pedBankFlag()){

      if(adcBankFlag()||pedBankFlag()) createOrderedSections();
      writeVeloFull();
      // flush the memory after each event
      resetMemory();

    }else if(m_roundRobin){
      
      writeVeloFull();
      
    }

  }else{

    return ( StatusCode::SUCCESS);

  }
  //
  if(m_isDebug) debug()<< " end of execute" <<endmsg;

  return ( StatusCode::SUCCESS );
}

//=============================================================================
StatusCode PrepareVeloFullRawBuffer::getRawEvent()
{
  if(m_isDebug) {
    debug()<< " ==> getRawEvent() " <<endmsg;
    debug()<< "--------------------" <<endmsg;
  }
  //
  // get the RawEvent from default TES location
  m_rawEvent = getIfExists<LHCb::RawEvent>(m_rawEventLoc);
  if( NULL == m_rawEvent ){
    return Error( " ==> There is no RawEvent at: " + m_rawEventLoc );
  }else{  
    if(m_isDebug) debug()<< " ==> The RawEvent has been read-in from location: "
           << m_rawEventLoc  <<endmsg;  
  }
  //
  return ( StatusCode::SUCCESS );
}
//=============================================================================
StatusCode PrepareVeloFullRawBuffer::getRawBanks()
{
  if(m_isDebug) debug()<< " ==> getRawBanks() " <<endmsg;
  // check if there is non-zero suppressed bank present
  const std::vector<LHCb::RawBank*>& fullBanks=
        m_rawEvent->banks(LHCb::RawBank::VeloFull);
  const std::vector<LHCb::RawBank*>& errorBanks=
        m_rawEvent->banks(LHCb::RawBank::VeloError);
  std::vector<LHCb::RawBank*>::const_iterator bIt;
  std::vector<LHCb::RawBank*>::const_iterator isError;

  // if so get all info needed for decoding
  if(!fullBanks.empty()){

    setADCBankFlag();
    m_veloADCData=new VeloFullBanks();
    m_veloADCPartialData=new VeloFullBanks();
    //

    for(bIt=fullBanks.begin(); bIt!=fullBanks.end(); bIt++){

      LHCb::RawBank* aBank=(*bIt);
      // protection against wrong magic pattern

      if(LHCb::RawBank::MagicPattern!=aBank->magic()){

        info()<< " --> Bank with source ID: " << (aBank->sourceID())
              << " is corrupted! Skipping to the next event" <<endmsg;
        return ( StatusCode::SUCCESS );

      }

      // get the sensor number == sourceID
      int sensor=aBank->sourceID();

      if(!m_ignoreErrorBanks)
      {
        
        // handle the data sent out together with an error bank properly
        isError=find_if(errorBanks.begin(), errorBanks.end(), errorBankFinder(sensor));
        if(isError!=errorBanks.end())
        {
        
          DATA_PAIR data_info=std::make_pair(aBank->size(), aBank->data());
          m_partialData2Decode[(aBank->sourceID())]=data_info;

          DATA_PAIR eb_info=std::make_pair((*isError)->size(), (*isError)->data());
          m_errorBanks2Check[(aBank->sourceID())]=eb_info;

        }else{
        
          DATA_PAIR full_info=std::make_pair(aBank->size(), aBank->data());
          m_fullData2Decode[(aBank->sourceID())]=full_info;
        
        }

      }else{
        
        // put all the nzs data in one container
        DATA_PAIR full_info=std::make_pair(aBank->size(), aBank->data());
        m_fullData2Decode[(aBank->sourceID())]=full_info;

      }
        
    }

  }

  // check if there is pedestal bank present
  const std::vector<LHCb::RawBank*>& pedBanks=
    m_rawEvent->banks(LHCb::RawBank::VeloPedestal);
  // if so get informations about it

  if(pedBanks.size()!=0){

    setPedBankFlag();
    m_veloPedestals=new VeloFullBanks();
    //

    if(m_isDebug) debug()<< "VeloPed bank detected of size: "
                         << pedBanks.size() <<endmsg;

    for(bIt=pedBanks.begin(); bIt!=pedBanks.end(); bIt++){

      LHCb::RawBank* aBank=(*bIt);
      DATA_PAIR ped_info;
      ped_info=std::make_pair(aBank->size(), aBank->data());
      m_pedestalData2Decode[aBank->sourceID()]=ped_info;

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
  if(fullBanks.empty()){

    m_veloADCData=new VeloFullBanks();
    m_veloADCPartialData=new VeloFullBanks();
    unsigned int msgCount=0;
    if(m_isDebug) msgCount=10;
    Warning(" --> There are no NZS banks!", StatusCode::SUCCESS, msgCount).ignore();

  }
  //
  return ( StatusCode::SUCCESS );
}
//=============================================================================
// /// ///////////////////////////////////////////////////////////////// /// //
// if there are both banks type order the data and create new data object    //
// and pass it to the next stage of the processing                           //
// /// ///////////////////////////////////////////////////////////////// /// //
void PrepareVeloFullRawBuffer::createOrderedSections()
{
  if(m_isDebug) debug()<< " ==> createOrderedSection() " <<endmsg;
  //
  if(adcBankFlag()){

    int type_1=LHCb::RawBank::VeloFull;
    DR_IT full_IT=m_fullData2Decode.begin();

    for( ; full_IT!=m_fullData2Decode.end(); ++full_IT)
    {

      VeloFullBank* aBank=new VeloFullBank((*full_IT).first, 
                                           (*full_IT).second.second, type_1);
      m_veloADCData->insert(aBank);

    }

    DR_IT part_IT=m_partialData2Decode.begin();
    
    for( ; part_IT!=m_partialData2Decode.end(); ++part_IT)
    {

      if(WORD2BYTE*FPGAx4==(*part_IT).second.first)
      {
        //if (m_isDebug) debug()<< " --> Will write partial data! " <<endmsg;
        //if (m_isDebug) debug()<< " --> source id: " << ((*part_IT).first) <<endmsg;
        info()<< " --> Will write partial data! " <<endmsg;
        info()<< " --> source id: " << ((*part_IT).first) <<endmsg;
        
        VeloFullBank* aBank=new VeloFullBank((*part_IT).first,
                                             (*part_IT).second.second, type_1);
        m_veloADCPartialData->insert(aBank);

      }else{
        
        // --> partial data decoding using information form the associated EB

      }

    }
      
  }
  //
  if(pedBankFlag()){

    int type_2=LHCb::RawBank::VeloPedestal;
    DR_IT ped_IT=m_pedestalData2Decode.begin();
    
    for( ; ped_IT!=m_pedestalData2Decode.end(); ++ped_IT)
    {
      
      VeloFullBank* aBank=new VeloFullBank((*ped_IT).first,
                                           (*ped_IT).second.second, type_2);
      m_veloPedestals->insert(aBank);

    }

  }

}
//=============================================================================
StatusCode PrepareVeloFullRawBuffer::writeVeloFull()
{
  if(m_isDebug) debug()<< " ==> writeVeloFull() " <<endmsg;
  //

  if(adcBankFlag()||m_roundRobin){

    if(m_isDebug) debug()<< "Registered container with bank data of size: "
          << m_veloADCData->size() << ", at" 
          << m_veloADCDataLoc <<endmsg;

    put(m_veloADCData, m_veloADCDataLoc);
    put(m_veloADCPartialData, m_veloADCPartialDataLoc);

  }
  //  

  if(pedBankFlag()){

    if(m_isDebug) debug()<< "Registered container with bank data of size: "
          << m_veloPedestals->size() << ", at" 
          << m_veloPedestalsLoc <<endmsg;

    put(m_veloPedestals, m_veloPedestalsLoc);

  }
  //
  return ( StatusCode::SUCCESS );
}
//=============================================================================
void PrepareVeloFullRawBuffer::resetMemory()
{
  if(m_isDebug) debug()<< " ==> resetMemory() " <<endmsg;
  //
  m_fullData2Decode.clear();
  m_partialData2Decode.clear();
  m_pedestalData2Decode.clear();
  m_errorBanks2Check.clear();
  
  if(adcBankFlag()) unsetADCBankFlag();
  if(pedBankFlag()) unsetPedBankFlag();
}
//=============================================================================
void PrepareVeloFullRawBuffer::dumpADCs(const dataVec& inADCs)
{
  if(m_isDebug) debug()<< " Dumping ADC values for all channels (1 TELL1)" <<endmsg;
  //
  cdatIt adcIt;
  int ii=0;
  for(adcIt=inADCs.begin(); adcIt!=inADCs.end(); adcIt++, ii++){
    if(!(ii%32)){
      if(m_isDebug) debug()<< " Beggining dumping ALink number: " << ii/32 <<endmsg;
    }
    //
    std::cout<< "[Beetle, Channel] = " << "[" << ii/128 << ", "
             << ii%32 << "]" << (*adcIt) <<std::endl;
  }
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
//--
