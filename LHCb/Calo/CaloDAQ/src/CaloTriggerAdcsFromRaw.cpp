// $Id: CaloTriggerAdcsFromRaw.cpp,v 1.16 2007-12-11 21:10:56 odescham Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "CaloTriggerAdcsFromRaw.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloTriggerAdcsFromRaw
//
// 2005-01-05 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( CaloTriggerAdcsFromRaw );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloTriggerAdcsFromRaw::CaloTriggerAdcsFromRaw( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : CaloReadoutTool ( type, name , parent )
{
  declareInterface<ICaloTriggerAdcsFromRaw>(this);
  // define default detector name
  int index = name.find_first_of(".",0) +1 ; // return -1+1=0 if '.' not found --> OK !!
  m_detectorName = name.substr( index, 4 );
  // clear data
  clear();  
}
//=============================================================================
// Destructor
//=============================================================================
CaloTriggerAdcsFromRaw::~CaloTriggerAdcsFromRaw() {};

//=========================================================================
//  Initialise
//=========================================================================
StatusCode CaloTriggerAdcsFromRaw::initialize ( ) {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTool

  debug() << "==> Initialize " << name() << endmsg;
  
  if ( "Ecal" == m_detectorName ) {
    m_calo     = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
    m_packedType = LHCb::RawBank::EcalPacked;
    m_shortType  = LHCb::RawBank::EcalTrig;
  } else if ( "Hcal" == m_detectorName ) {
    m_calo     = getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal );
    m_packedType = LHCb::RawBank::HcalPacked;
    m_shortType  = LHCb::RawBank::HcalTrig;
  } else {
    error() << "Unknown detector name '" << m_detectorName 
            << "'. Set it by option 'DetectorName', should be Ecal or Hcal" << endreq;
    return StatusCode::FAILURE;
  }

  m_pinArea = m_calo->pinArea();      
  long nCells = m_calo->numberOfCells();
  long nPins = m_calo->numberOfPins();
  m_data.reserve( nCells );
  m_pinData.reserve( nPins );
  clear();
  return StatusCode::SUCCESS;
}

//-------------------------------------
void CaloTriggerAdcsFromRaw::clear( ) {
  m_data.clear();
  m_pinData.clear();
}


void CaloTriggerAdcsFromRaw::cleanData(int feb ) {
  if(feb<0)return;
  std::vector<LHCb::L0CaloAdc> tempP;
  std::vector<LHCb::L0CaloAdc> temp;
  if(m_calo->isPinCard(feb)){
    for(std::vector<LHCb::L0CaloAdc>::iterator iadc = m_pinData.begin();iadc!=m_pinData.end();++iadc){
      if( m_calo->cellParam( (*iadc).cellID() ).cardNumber() == feb)continue;  
      tempP.push_back( *iadc );
    }  
    m_pinData.clear();
    for(std::vector<LHCb::L0CaloAdc>::iterator iadc = tempP.begin();iadc!=tempP.end();++iadc){
      m_pinData.push_back(*iadc);
    }

  }else{
    for(std::vector<LHCb::L0CaloAdc>::iterator iadc = m_data.begin();iadc!=m_data.end();++iadc){
      if( m_calo->cellParam( (*iadc).cellID() ).cardNumber() == feb)continue;  
      temp.push_back(*iadc);
    }  
    m_data.clear();
    for(std::vector<LHCb::L0CaloAdc>::iterator iadc = temp.begin();iadc!=temp.end();++iadc){
      m_data.push_back(*iadc);
    }
  } 
}



//=========================================================================
//  Return the specific ADCs for PIN diode
//  Warning : it requires a decoding adcs(...) method to be executed before
//=========================================================================
std::vector<LHCb::L0CaloAdc>& CaloTriggerAdcsFromRaw::pinAdcs () {
  return m_pinData;
}
//=========================================================================
//  Decode the adcs for ALL banks of one event
//=========================================================================
std::vector<LHCb::L0CaloAdc>& CaloTriggerAdcsFromRaw::adcs () {
  return adcs(-1);
}
//=========================================================================
//  Decode the adcs for a single bank (given by sourceID)
//  Decode ALL banks if source < 0
//=========================================================================
std::vector<LHCb::L0CaloAdc>& CaloTriggerAdcsFromRaw::adcs (int source ) { 
  clear();
  bool decoded = false;
  bool found   = false;
  int sourceID  ;
  if(m_getRaw)getCaloBanksFromRaw();
  if( NULL == m_banks || 0 == m_banks->size() ){
    Error("The banks container is empty").ignore();
  }else{
    for( std::vector<LHCb::RawBank*>::const_iterator itB = m_banks->begin(); 
         itB != m_banks->end() ; ++itB ) {
      sourceID       = (*itB)->sourceID();
      if( source >= 0 && source != sourceID )continue;
      found = true;
      decoded = getData ( *itB );
      if( !decoded )error() << " Error when decoding bank " << sourceID  << " -> incomplete data - May be corrupted" <<endreq;
    } 
  }
  if( !found ){
    std::stringstream s("");
    s<< source;
    Error("rawBank sourceID : " + s.str() + " has not been found").ignore();
  }
  return m_data ;
}

//=========================================================================
//  Decode the adcs of a single bank (given by bank pointer)
//=========================================================================
std::vector<LHCb::L0CaloAdc>& CaloTriggerAdcsFromRaw::adcs ( LHCb::RawBank* bank ){
  clear();
  if( !getData( bank ))clear();
  return m_data ;
}

//=============================================================================
// Main method to decode the rawBank - fill m_data vector
//=============================================================================

bool CaloTriggerAdcsFromRaw::getData ( LHCb::RawBank* bank ){
  if(NULL == bank)return false;
  unsigned int* data = bank->data();
  int size           = bank->size()/4;  // in bytes in the header
  int version        = bank->version();
  int sourceID       = bank->sourceID();
  int lastData = 0;

  if(0 == size)m_status.addStatus(sourceID,LHCb::RawBankReadoutStatus::Empty );

  if ( msgLevel( MSG::DEBUG) )debug() << "Decode bank " << bank << " source " << sourceID 
                                      << "version " << version << " size " << size << endreq;
  
  // -----------------------------------------------
  // skip detector specific header line 
  if(m_extraHeader){
    ++data ; 
    --size;
  }
  // -----------------------------------------------
  
  //=== Offline coding
  if ( 2 > version ) {
    while ( 0 < size ) {
      int lastID   = (*data) >> 16;
      int adc1     = (*data)>>8 & 0xFF;
      int adc2     = (*data)    & 0xFF;
      ++data;
      --size;

      LHCb::CaloCellID id1( lastID );
      if ( 0 != adc1 ) {
        LHCb::L0CaloAdc dum( id1, adc1 );
        if( m_pinArea != id1.area() ){
          m_data.push_back( dum );
        }else{
          m_pinData.push_back( dum );
        }
      }

      //event dump
      if ( msgLevel( MSG::DEBUG) ) {
        debug() << " |  SourceID : " << sourceID
                << " |  FeBoard : " << m_calo->cardNumber(id1)
                << " |  CaloCell " << id1
                << " |  valid ? " << m_calo->valid(id1)
                << " |  ADC value = " << adc1 << endreq;
      }

      LHCb::CaloCellID id2( ++lastID );
      if ( 0 != adc2 ) {
        LHCb::L0CaloAdc dum( id2, adc2 );
        if( m_pinArea != id2.area() ){
          m_data.push_back( dum );
        }else{
          m_pinData.push_back( dum );
        }
      }
 
      //event dump
      if ( msgLevel( MSG::DEBUG) ) {
        debug() << " |  SourceID : " << sourceID
                << " |  FeBoard : " << m_calo->cardNumber(id2)
                << " |  CaloCell " << id2
                << " |  valid ? " << m_calo->valid(id2)
                << " |  ADC value = " << adc2 << endreq;
      }
      
    }
  } else if ( 2 == version ) {
    //==== Codage for 1 MHz
    // Get the FE-Cards associated to that bank (via condDB)
    std::vector<int> feCards = m_calo->tell1ToCards( sourceID );
    int nCards = feCards.size();
    if ( msgLevel( MSG::DEBUG) )debug() << nCards << " FE-Cards are expected to be readout : " 
                                        << feCards << " in Tell1 bank " << sourceID << endreq;
    int lenAdc   = 0;
    int lenTrig  = 0;

    int prevCard = -1;
    while ( 0 < size ) {
      int word = *data++;
      size--;
      lenTrig = word & 0x3F;
      lenAdc  = (word >> 7 ) & 0x3F;
      int code  = (word >> 14 ) & 0x1FF;
      int ctrl    = (word >> 23) &  0x1FF;
      if ( msgLevel( MSG::DEBUG) )debug()<< "Control word :" << ctrl << endreq;
      if( 0 != 0x1& ctrl || 0 != 0x20& ctrl || 0 != 0x40& ctrl){
        warning() << "Tell1 error bits have been detected in data" << endreq;
        if( 0 != 0x1  & ctrl)m_status.addStatus(sourceID,LHCb::RawBankReadoutStatus::Tell1Error );
        if( 0 != 0x20 & ctrl)m_status.addStatus(sourceID,LHCb::RawBankReadoutStatus::Tell1Sync  );      
        if( 0 != 0x40 & ctrl)m_status.addStatus(sourceID,LHCb::RawBankReadoutStatus::Tell1Link  );
      }
      // access chanID via condDB
      std::vector<LHCb::CaloCellID> chanID  ;
      int card = findCardbyCode(feCards,code);
      if( 0 <= card ){
        chanID = m_calo->cardChannels( feCards[card] );
        feCards.erase(feCards.begin()+card);
      }else{
        error() << " FE-Card w/ [code : " << code << "] not associated with TELL1 bank " << sourceID
                << " in condDB :  Cannot read that bank" << endreq;
        error() << "Warning : previous data may be corrupted" << endreq;
        if(m_cleanCorrupted)cleanData(prevCard);
        m_status.addStatus( sourceID,   LHCb::RawBankReadoutStatus::Corrupted || LHCb::RawBankReadoutStatus::Incomplete);
        return false;
      }
      prevCard = card;

      // Start readout of the FE-board (trigger data)
      if ( 0 < lenTrig ) {
        int pattern  = *data++;
        int offset   = 0;
        lastData  = *data++;
        size -= 2;
        for (unsigned  int bitNum = 0 ; 32 > bitNum ; bitNum++ ) {
          if ( 0 != (pattern & (1<<bitNum)) ) {
            if ( 31 < offset ) {
              offset   = 0;
              lastData = *data++;
              size--;
            }


            LHCb::CaloCellID id = LHCb::CaloCellID();
            if(bitNum < chanID.size())id= chanID[ bitNum ];

            int adc = ( lastData >> offset ) & 0xFF;

            // event dump
            if ( msgLevel( MSG::DEBUG) ) {
              debug() << " |  SourceID : " << sourceID
                      << " |  FeBoard : " << code 
                      << " |  Channel : " << bitNum
                      << " |  CaloCell " << id
                      << " |  valid ? " << m_calo->valid(id)
                      << " |  ADC value = " << adc << endreq;
            }
            
            if ( 0 != id.index() ) {
              LHCb::L0CaloAdc temp( id, adc );
              if( m_pinArea != id.area() ){
                m_data.push_back( temp );
              }else{
                m_pinData.push_back( temp );
              }
            }
            offset += 8;
          }
        }
      }
      // Skip ADC data
      int nSkip = (lenAdc+3)/4;  //== length in byte, skip words
      data     += nSkip;
        size     -= nSkip;
    } // another card
    // Check All cards have been read
    if(!checkCards(nCards,feCards))m_status.addStatus(sourceID, LHCb::RawBankReadoutStatus::Incomplete); 
  } // version 
  return true;
}
//=============================================================================
