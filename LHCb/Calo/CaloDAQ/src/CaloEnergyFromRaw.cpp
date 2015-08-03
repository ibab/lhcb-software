// Include files
// local
#include "CaloEnergyFromRaw.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloEnergyFromRaw
//
// 2005-01-10 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( CaloEnergyFromRaw )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloEnergyFromRaw::CaloEnergyFromRaw( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : CaloReadoutTool ( type, name , parent )
{
  declareInterface<ICaloEnergyFromRaw>(this);

  // set default detectorName
  int index = name.find_last_of(".") +1 ; // return 0 if '.' not found --> OK !!

  m_detectorName = name.substr( index, 4 );
  if ( name.substr(index,3) == "Prs" ) m_detectorName = "Prs";
  clear();
}
//=============================================================================
// Destructor
//=============================================================================
CaloEnergyFromRaw::~CaloEnergyFromRaw() {}


//=========================================================================
//  Initialisation, according to the name -> detector
//=========================================================================
StatusCode CaloEnergyFromRaw::initialize ( ) {
  StatusCode sc = CaloReadoutTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "==> Initialize " << name() << " Det = "<< m_detectorName << endmsg;

  if ( "Ecal" == m_detectorName ) {
    m_calo     = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
    m_packedType = LHCb::RawBank::EcalPacked;
    m_shortType  = LHCb::RawBank::EcalE;
    m_errorType = LHCb::RawBank::EcalPackedError;
  } else if ( "Hcal" == m_detectorName ) {
    m_calo     = getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal );
    m_packedType = LHCb::RawBank::HcalPacked;
    m_shortType  = LHCb::RawBank::HcalE;
    m_errorType = LHCb::RawBank::HcalPackedError;
  } else if ( "Prs" == m_detectorName ) {
    m_calo     = getDet<DeCalorimeter>( DeCalorimeterLocation::Prs );
    m_packedType = LHCb::RawBank::PrsPacked;
    m_shortType  = LHCb::RawBank::PrsE;
    m_errorType = LHCb::RawBank::PrsPackedError;
  } else {
    error() << "Unknown detector name " << m_detectorName << endmsg;
    return StatusCode::FAILURE;
  }

  long nCells = m_calo->numberOfCells();
  long nPins  = m_calo->numberOfPins();
  m_pinData.reserve( nPins );
  m_data.reserve( nCells );
  m_digits.reserve( nCells );
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "Got detector element for " << m_detectorName << endmsg;
  return StatusCode::SUCCESS;
}


//-------------------------------------
void CaloEnergyFromRaw::clear( ) {
  m_data.clear();
  m_digits.clear();
  m_pinData.clear();
  m_readSources.clear();
}

//-------------------------------------
void CaloEnergyFromRaw::cleanData(int feb ) {
  if(feb<0)return;
  std::vector<LHCb::CaloAdc>   tempP;
  std::vector<LHCb::CaloAdc>   temp;
  std::vector<LHCb::CaloDigit>  tempD;

  if(m_calo->isPinCard(feb)){
    for(std::vector<LHCb::CaloAdc>::iterator iadc = m_pinData.begin();iadc!=m_pinData.end();++iadc){
      if( m_calo->cellParam( (*iadc).cellID() ).cardNumber() == feb)continue;
      tempP.push_back( *iadc );
    }
    m_pinData.clear();
    for(std::vector<LHCb::CaloAdc>::iterator iadc = tempP.begin();iadc!=tempP.end();++iadc){
      m_pinData.push_back( *iadc );
    }

  }else{
    for(std::vector<LHCb::CaloAdc>::iterator iadc = m_data.begin();iadc!=m_data.end();++iadc){
      if( m_calo->cellParam( (*iadc).cellID() ).cardNumber() == feb)continue;
      temp.push_back( *iadc );
    }
    for(std::vector<LHCb::CaloDigit>::iterator idig = m_digits.begin();idig!=m_digits.end();++idig){
      if( m_calo->cellParam( (*idig).cellID() ).cardNumber() == feb)continue;
      tempD.push_back( *idig );
    }
    m_data.clear();
    m_digits.clear();
    for(std::vector<LHCb::CaloAdc>::iterator iadc = temp.begin();iadc!=temp.end();++iadc){
      m_data.push_back( *iadc );
    }
    for(std::vector<LHCb::CaloDigit>::iterator idig = tempD.begin();idig!=tempD.end();++idig){
      m_digits.push_back( *idig );
    }
  }
}



//=========================================================================
//  Return the specific ADCs for PIN diode
//  Warning : it requires a decoding adcs(...) method to be executed before
//=========================================================================
const std::vector<LHCb::CaloAdc>& CaloEnergyFromRaw::pinAdcs () {
  return m_pinData;
}
//=========================================================================
//  Decode the adcs for ALL banks of one event
//=========================================================================
const std::vector<LHCb::CaloAdc>& CaloEnergyFromRaw::adcs () {
  return adcs(-1);
}
//=========================================================================
//  Decode the adcs for a single bank (given by sourceID)
//  Decode ALL banks if source < 0
//=========================================================================
const std::vector<LHCb::CaloAdc>& CaloEnergyFromRaw::adcs (int source) {
  clear();
  int sourceID   ;
  bool decoded = false;
  bool found   = false;
  if(m_getRaw)getBanks();
  if( NULL == m_banks || 0 == m_banks->size() ){
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << "The banks container is empty"<< endmsg;
  }else{
    for( std::vector<LHCb::RawBank*>::const_iterator itB = m_banks->begin();
         itB != m_banks->end() ; ++itB ) {
      sourceID       = (*itB)->sourceID();
      if( source >= 0 && source != sourceID )continue;

      found = true;

      if(checkSrc( sourceID ))continue;

      decoded = getData ( *itB );
      if( !decoded ){
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
          debug() <<"Error when decoding bank " << Gaudi::Utils::toString(sourceID)
                  << " -> incomplete data - May be corrupted"<< endmsg;
      }
    }
  }
  if( !found ){
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() <<"rawBank sourceID : " << Gaudi::Utils::toString(source) << " has not been found"<<endmsg;
  }

  return m_data ;
}
//=========================================================================
//  Decode the adcs of a single bank (given by bank pointer)
//=========================================================================
const std::vector<LHCb::CaloAdc>& CaloEnergyFromRaw::adcs ( LHCb::RawBank* bank ){
  clear();
  if( !getData( bank ) )clear();
  return m_data ;
}


//=========================================================================
//  Decode the adcs of a single bank and convert to digit (given sourceID)
//=========================================================================
const std::vector<LHCb::CaloDigit>&  CaloEnergyFromRaw::digits ( int source ) {
  adcs( source );
  if( !getDigits() ) m_digits.clear();
  return m_digits ;
}

//=========================================================================
//  Decode the adcs of a single bank and convert to digit (given by bank pointer)
//=========================================================================
const std::vector<LHCb::CaloDigit>&  CaloEnergyFromRaw::digits ( LHCb::RawBank* bank ) {
  adcs( bank );
  if( !getDigits() ) m_digits.clear();
  return m_digits ;
}

//=========================================================================
//  Decode the adcs for ALL banks bank and convert to digit - return m_digits
//=========================================================================
const std::vector<LHCb::CaloDigit>&  CaloEnergyFromRaw::digits ( ) {
  adcs();
  if( !getDigits() ) m_digits.clear();
  return m_digits;
}



//=============================================================================
// Main method to decode the rawBank - fill m_data vector
//=============================================================================
bool CaloEnergyFromRaw::getData ( LHCb::RawBank* bank ){
  if(NULL == bank)return false;
  if( LHCb::RawBank::MagicPattern != bank->magic() )return false;// do not decode when MagicPattern is bad
  // Get bank info
  unsigned int* data = bank->data();
  int size           = bank->size()/4;  // Bank size is in bytes
  int version        = bank->version();
  int sourceID       = bank->sourceID();


  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "Decode bank " << bank << " source " << sourceID
            << " version " << version << " size " << size << endmsg;


  if(0 == size)m_status.addStatus(sourceID,LHCb::RawBankReadoutStatus::Empty );

  // -----------------------------------------------
  // skip detector specific header line
  if(m_extraHeader){
    ++data ;
    --size;
  }
  // -----------------------------------------------


  if ( 1 > version || 3 < version ) {
    warning() << "Bank type " << bank->type() << " sourceID " << sourceID
              << " has version " << version
              << " which is not supported" << endmsg;

  } else if ( 1 == version ) {
    //******************************************************************
    //**** Simple coding, ID + adc in 32 bits.
    //******************************************************************
    while( 0 != size ) {
      int id   = ((*data) >> 16) & 0xFFFF;
      int adc  =  (*data) & 0xFFFF;
      if ( 32767 < adc ) adc |= 0xFFFF0000;  //= negative value
      LHCb::CaloCellID cellId( id );
      LHCb::CaloAdc temp( cellId, adc);

      //event dump
      if ( msgLevel( MSG::VERBOSE) ) {
        verbose() << " |  SourceID : " << sourceID
                  << " |  FeBoard : " << m_calo->cardNumber(cellId)
                  << " |  CaloCell " << cellId
                  << " |  valid ? " << m_calo->valid(cellId)
                  << " |  ADC value = " << adc << endmsg;
      }

      if ( 0 != cellId.index() ) {
        if( !cellId.isPin() ){
          m_data.push_back( temp );
        }else{
          m_pinData.push_back( temp );
        }
      }

      ++data;
      --size;
    }

  } else if ( 2 == version ) {
    //******************************************************************
    //**** 1 MHz compression format, Ecal and Hcal
    //******************************************************************
    // Get the FE-Cards associated to that bank (via condDB)
    std::vector<int> feCards = m_calo->tell1ToCards( sourceID );
    int nCards = feCards.size();
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << nCards << " FE-Cards are expected to be readout : "
              << feCards << " in Tell1 bank " << sourceID << endmsg;
    int prevCard = -1;
    while( 0 != size ) {
      // Skip
      unsigned int word = *data++;
      size--;
      // Read bank header
      int lenTrig = word & 0x7F;
      int code    = (word >> 14 ) & 0x1FF;
      int ctrl    = (word >> 23) &  0x1FF;
      checkCtrl( ctrl,sourceID );
      // access chanID via condDB
      std::vector<LHCb::CaloCellID> chanID  ;
      // look for the FE-Card in the Tell1->cards vector
      int card = findCardbyCode(feCards,code);
      if( 0 <= card ){
        chanID = m_calo->cardChannels( feCards[card] );
        feCards.erase(feCards.begin()+card);
      }else{
        Error(" FE-Card w/ [code : " +  Gaudi::Utils::toString(code) + " ] is not associated with TELL1 bank sourceID : "
              + Gaudi::Utils::toString(sourceID)
              + " in condDB :  Cannot read that bank").ignore();
        Error("Warning : previous data may be corrupted").ignore();
        if(m_cleanCorrupted)cleanData(prevCard);
        m_status.addStatus( sourceID,   LHCb::RawBankReadoutStatus::Incomplete);
        m_status.addStatus( sourceID,   LHCb::RawBankReadoutStatus::Corrupted );
        return false;
      }
      prevCard = card;

      // Start readout of the FE-board
      // First skip trigger bank ...
      int nSkip = (lenTrig+3)/4;   //== is in bytes, with padding
      data     += nSkip;
      size     -= nSkip;
      unsigned int pattern  = *data++;
      int offset   = 0;
      unsigned int lastData = *data++;
      size -= 2;
      // ... and readout data
      for (unsigned  int bitNum = 0; 32 > bitNum; bitNum++ ) {
        int adc;
        if ( 31 < offset ) {
          offset  -= 32;
          lastData =  *data++;
          size--;
        }
        if ( 0 == ( pattern & (1<<bitNum) ) ) {  //.. short coding
          adc = ( ( lastData >> offset ) & 0xF ) - 8;
          offset += 4;
        } else {
          adc =  ( ( lastData >> offset ) & 0xFFF );
          if ( 24 == offset ) adc &= 0xFF;
          if ( 28 == offset ) adc &= 0xF;  //== clean-up extra bits
          offset += 12;
          if ( 32 < offset ) {  //.. get the extra bits on next word
            lastData = *data++;
            size--;
            offset -= 32;
            int temp = (lastData << (12-offset) ) & 0xFFF;
            adc += temp;
          }
          adc -= 256;
        }


        LHCb::CaloCellID id = LHCb::CaloCellID();
        if(bitNum < chanID.size())id= chanID[ bitNum ];


        // event dump
        if ( msgLevel( MSG::VERBOSE) ) {
          verbose() << " |  SourceID : " << sourceID
                    << " |  FeBoard : " << m_calo->cardNumber(id)
                    << " |  Channel : " << bitNum
                    << " |  CaloCell " << id
                    << " |  valid ? " << m_calo->valid(id)
                    << " |  ADC value = " << adc << endmsg;
        }

        //== Keep only valid cells
        if ( 0 != id.index() ) {
          LHCb::CaloAdc temp( id, adc);
          if( !id.isPin() ){
              m_data.push_back( temp );
          }else{
            m_pinData.push_back( temp );
          }
        }
      }
    }
    // Check All cards have been read
    if(!checkCards(nCards,feCards))m_status.addStatus(sourceID, LHCb::RawBankReadoutStatus::Incomplete);

  } else if ( 3 == version ) {
    //******************************************************************
    //**** 1 MHz compression format, Preshower + SPD
    //******************************************************************

    // Get the FE-Cards associated to that bank (via condDB)
    std::vector<int> feCards = m_calo->tell1ToCards( sourceID );
    int nCards = feCards.size();
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << nCards << " FE-Cards are expected to be readout : "
              << feCards << " in Tell1 bank " << sourceID << endmsg;
    int prevCard = -1;
    while( 0 != size ) {
      // Skip
      unsigned int word = *data++;
      size--;
      // Read bank header
      int lenTrig = word & 0x7F;
      int lenAdc  = (word >> 7 ) & 0x7F;
      int code  = (word >>14 ) & 0x1FF;
      int ctrl    = (word >> 23) &  0x1FF;
      checkCtrl( ctrl,sourceID );
      // access chanID via condDB
      std::vector<LHCb::CaloCellID> chanID  ;
      // look for the FE-Card in the Tell1->cards vector
      int card = findCardbyCode(feCards,code);
      if( 0 <= card ){
        chanID = m_calo->cardChannels( feCards[card] );
        feCards.erase(feCards.begin()+card);
      }else{
        Error(" FE-Card w/ [code : " + Gaudi::Utils::toString( code ) + " ] is not associated with TELL1 bank sourceID : "
              +Gaudi::Utils::toString(sourceID)
              + " in condDB :  Cannot read that bank").ignore();
        Error("Warning : previous data may be corrupted").ignore();
        if(m_cleanCorrupted)cleanData(prevCard);
        m_status.addStatus( sourceID,   LHCb::RawBankReadoutStatus::Corrupted || LHCb::RawBankReadoutStatus::Incomplete);
        return false;
      }
      prevCard = card;

      // Read the FE-Board
      // skip the trigger bits
      int nSkip = (lenTrig+3)/4;  //== Length in byte, with padding
      size -= nSkip;
      data     += nSkip;


      // read data
      int offset   = 32;  //== force read the first word, to have also the debug for it.
      unsigned int lastData = 0;

      while ( 0 < lenAdc ) {
        if ( 32 == offset ) {
          lastData =  *data++;
          size--;
          offset = 0;
        }
        int adc = ( lastData >> offset ) & 0x3FF;
        unsigned int num = ( lastData >> (offset+10) ) & 0x3F;

        LHCb::CaloCellID id = LHCb::CaloCellID();
        if(num < chanID.size())id= chanID[ num ];
        LHCb::CaloAdc temp( id, adc );

        // event dump
        if ( msgLevel( MSG::VERBOSE) ) {
          verbose() << " |  SourceID : " << sourceID
                    << " |  FeBoard : " << m_calo->cardNumber(id)
                    << " |  Channel : " << num
                    << " |  CaloCell " << id
                    << " |  valid ? " << m_calo->valid(id)
                    << " |  ADC value = " << adc << endmsg;
        }



        if ( 0 != id.index() ) {
          if( !id.isPin() ){
            m_data.push_back( temp );
          }else{
            m_pinData.push_back( temp );
          }
        }

        lenAdc--;
        offset += 16;
      }
    } //== DataSize
    // Check All cards have been read
    if(!checkCards(nCards,feCards))m_status.addStatus(sourceID, LHCb::RawBankReadoutStatus::Incomplete);
  } //== versions

  return true;
}

//=========================================================================
//  Main method to convert ADCs to CaloDigits
//  This is just a gain calibration (+ped shift) of the adc values.
//=========================================================================
bool CaloEnergyFromRaw::getDigits ( ) {
  m_digits.clear();
  if( 0 == m_data.size() )return true;
  double pedShift = m_calo->pedestalShift();
  for ( std::vector<LHCb::CaloAdc>::const_iterator itAdc = m_data.begin();
        m_data.end() != itAdc; ++itAdc ) {
    LHCb::CaloCellID id = (*itAdc).cellID();
    int adc = (*itAdc).adc();
    double e = ( double(adc) - pedShift ) * m_calo->cellGain( id );
    LHCb::CaloDigit dig( id, e );
    m_digits.push_back( dig );
  }
  return true;
}
