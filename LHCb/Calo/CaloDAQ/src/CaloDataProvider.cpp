// Include files

// local
#include "CaloDataProvider.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloDataProvider
//
// 2005-01-10 : Olivier Deschamps
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( CaloDataProvider )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloDataProvider::CaloDataProvider( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : CaloReadoutTool ( type, name , parent )
    ,m_adcs()
    ,m_digits()
    ,m_tell1s(0)
{
  declareInterface<ICaloDataProvider>(this);

  // set default detectorName
  int index = name.find_last_of(".") +1 ; // return 0 if '.' not found --> OK !!
  m_detectorName = name.substr( index, 4 );
  if ( name.substr(index,3) == "Prs" ) m_detectorName = "Prs";
  if ( name.substr(index,3) == "Spd" ) m_detectorName = "Spd";
}
//=============================================================================
// Destructor
//=============================================================================
CaloDataProvider::~CaloDataProvider() {}


//=========================================================================
//  Initialisation, according to the name -> detector
//=========================================================================
StatusCode CaloDataProvider::initialize ( ) {
  StatusCode sc = CaloReadoutTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "==> Initialize " << name() << endmsg;

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
  } else if ( "Spd" == m_detectorName ) {
    m_calo     = getDet<DeCalorimeter>( DeCalorimeterLocation::Spd ); // Prs FE for SPD
    m_packedType = LHCb::RawBank::PrsPacked;
    m_shortType  = LHCb::RawBank::PrsTrig;
    m_errorType = LHCb::RawBank::PrsPackedError;
  } else {
    error() << "Unknown detector name " << m_detectorName << endmsg;
    return StatusCode::FAILURE;
  }

  long nCells = m_calo->numberOfCells();
  long nPins  = m_calo->numberOfPins();
  m_adcs.reserve( nCells + nPins  );
  m_digits.reserve( nCells );
  clear();
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << " Initialisation OK" << endmsg;
  return StatusCode::SUCCESS;
}

//-------------------------------------------------------
// Public methods :
//-------------------------------------------------------



//-------------------------------------
void CaloDataProvider::clear( ) {
  m_adcs.clear();
  m_digits.clear();
  m_tell1s = 0;
  m_readSources.clear();
  m_minADC = LHCb::CaloAdc(LHCb::CaloCellID() , 3840);
  m_minPinADC = LHCb::CaloAdc(LHCb::CaloCellID(), 3840);
  m_maxADC = LHCb::CaloAdc(LHCb::CaloCellID() , -256);
  m_maxPinADC = LHCb::CaloAdc(LHCb::CaloCellID(), -256);
  if ( msgLevel( MSG::DEBUG) )
    debug() << "ALL DATA CLEARED" << endmsg;
}
//-------------------------------------
void CaloDataProvider::cleanData(int feb ) {
  if(feb<0)return;
  CaloVector<LHCb::CaloAdc> temp;
  CaloVector<LHCb::CaloDigit> tempD;
  for(CaloVector<LHCb::CaloAdc>::iterator iadc = m_adcs.begin();iadc!=m_adcs.end();++iadc){
    if( m_calo->cellParam( (*iadc).cellID() ).cardNumber() == feb)continue;
    temp.addEntry( *iadc, (*iadc).cellID() );
  }
  for(CaloVector<LHCb::CaloDigit>::iterator idig = m_digits.begin();idig!=m_digits.end();++idig){
    if( m_calo->cellParam( (*idig).cellID() ).cardNumber() == feb)continue;
    tempD.addEntry( *idig, (*idig).cellID() );
  }
  m_adcs.clear();
  m_digits.clear();
  for(CaloVector<LHCb::CaloAdc>::iterator iadc = temp.begin();iadc!=temp.end();++iadc){
    m_adcs.addEntry(*iadc, (*iadc).cellID() );
  }
  for(CaloVector<LHCb::CaloDigit>::iterator idig = tempD.begin();idig!=tempD.end();++idig){
    m_digits.addEntry(*idig, (*idig).cellID() );
  }
}

//===================
//  Get ADC Container
//===================
const CaloVector<LHCb::CaloAdc>& CaloDataProvider::adcs(int source,bool clean){
  if(clean)clear();
  (!m_packed)? decodeTell1(-1) : decodeTell1(source);
  return m_adcs;
}
const CaloVector<LHCb::CaloAdc>& CaloDataProvider::adcs(std::vector<int> sources,bool clean){
  if(clean)clear();
  if( m_getRaw )getBanks();
  if( !m_packed) return adcs(); // decode the single 'offline' bank
  for(std::vector<int>::iterator i=sources.begin();i!=sources.end();i++){
    decodeTell1(*i);
  }
  return m_adcs;
}
void CaloDataProvider::adc2digit(){
  double pedShift = m_calo->pedestalShift();
  for(CaloVector<LHCb::CaloAdc>::iterator iadc = m_adcs.begin();iadc!=m_adcs.end();++iadc){
    int temp = (*iadc).adc();
    LHCb::CaloCellID id = (*iadc).cellID() ;
    double e = ( double(temp) - pedShift ) * m_calo->cellGain( id );
    LHCb::CaloDigit dig(id,e);
    m_digits.addEntry( dig , id);
  }
}
const CaloVector<LHCb::CaloDigit>& CaloDataProvider::digits(std::vector<int> sources,bool clean){
  if( m_getRaw )getBanks();
  if( !m_packed) return digits(); // decode the single 'offline' bank
  adcs(sources,clean);
  adc2digit();
  return m_digits;
}
const CaloVector<LHCb::CaloDigit>& CaloDataProvider::digits(int source,bool clean){
  adcs(source,clean);
  adc2digit();
  return m_digits;
}
//==========
//  Get data
//==========
double CaloDataProvider::digit (LHCb::CaloCellID id,double def){
  double pedShift = m_calo->pedestalShift();
  if( m_getRaw )getBanks();
  if( 0 >  m_digits.index(id) ){
    int temp = adc(id,-256);
    if( -256 == temp && 0 >  m_adcs.index(id) ) return def; // 0-suppressed data or non-valid CellID
    double e = ( double(temp) - pedShift ) * m_calo->cellGain( id );
    LHCb::CaloDigit dig(id,e);
    m_digits.addEntry( dig , id);
    return e;
  }
  return m_digits[id].e();
}
//-------------------------------------------------------
int CaloDataProvider::adc (LHCb::CaloCellID id, int def){
  if( m_getRaw )getBanks();
  // decode the bank if not already done
  if( 0 >  m_adcs.index(id) )decodeCell( id );
  // 2nd attempt to access the adc
  if( 0 >  m_adcs.index(id) )return def;// 0-suppressed data or non-valid CellID
  return m_adcs[id].adc();
}

//-------------------------------------------------------
// Protected methods :
//-------------------------------------------------------
bool CaloDataProvider::decodeCell(LHCb::CaloCellID id ){
  // for packed banks only (sourceID = Tell1 ID)
  int tell1 = -1;// Decode the whole 0-suppressed bank by default (single bank)
  bool read = false;
  if( m_packed){
    int card = m_calo->cardNumber (id)   ; // Fe-Card from cellId
    if(card<0)return false;
    tell1 = m_calo->cardToTell1(card); // Tell1 from FE-Card
    if(tell1<0)return false;
    for(std::vector<int>::iterator it = m_readSources.begin() ; it != m_readSources.end() ; ++it){
      if( tell1 == *it){
        read = true;
        break;
      }
    }
  }else{
    for(std::vector<int>::iterator it = m_readSources.begin() ; it != m_readSources.end() ; ++it){
      if( 0 == *it){
        read = true;
        break;
      }
    }
  }
  if( read ) return true;
  return decodeTell1( tell1 );
}
//-------------------------------------------------------
bool CaloDataProvider::decodeTell1 (int source) {
  if( source < 0)clear(); // re-init for full decoding
  bool decoded = false;
  bool found  = false;
  if( m_getRaw )getBanks();
  if(NULL == m_banks) return false;
  int sourceID  ;


  for( std::vector<LHCb::RawBank*>::const_iterator itB = m_banks->begin(); itB != m_banks->end() ; ++itB ) {
    sourceID       = (*itB)->sourceID();
    if( source >= 0 && source != sourceID )continue;
    found = true;

    if(checkSrc( sourceID ))continue;

    if( "Spd" == m_detectorName ){
      decoded = decodePrsTriggerBank( *itB);
    }else{
      decoded = decodeBank ( *itB );
    }
    if( !decoded ){
      Error("Error when decoding bank " + Gaudi::Utils::toString( sourceID)  + " -> incomplete data - May be corrupted").ignore();
    }
    m_tell1s++; // count the number of decoded TELL1
  }
  if( !found ){
    Error("rawBank sourceID : " + Gaudi::Utils::toString( source )  + " has not been found").ignore();
  }
  return decoded;
}
//==================================
// Main method to decode the rawBank
//==================================
bool CaloDataProvider::decodeBank( LHCb::RawBank* bank ){
  if(NULL == bank)return false;
  if( LHCb::RawBank::MagicPattern != bank->magic() )return false;// do not decode when MagicPattern is bad
  // Get bank info
  unsigned int* data = bank->data();
  int size           = bank->size()/4;  // Bank size is in bytes
  int version        = bank->version();
  int sourceID       = bank->sourceID();

  if(0 == size)m_status.addStatus( sourceID, LHCb::RawBankReadoutStatus::Empty);


  if ( msgLevel( MSG::DEBUG) )
    debug() << "Decode bank " << bank << " source " << sourceID
            << " version " << version << " size " << size << endmsg;

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

      //event dump
      if(msgLevel(MSG::VERBOSE))verbose() << " |  SourceID : " << sourceID
                                          << " |  FeBoard : " << m_calo->cardNumber(cellId)
                                          << " |  CaloCell " << cellId
                                          << " |  valid ? " << m_calo->valid(cellId)
                                          << " |  ADC value = " << adc << endmsg;

      if ( 0 != cellId.index() )fillAdc( cellId , adc, sourceID);
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
    if ( msgLevel( MSG::DEBUG) )
      debug() << nCards << " FE-Cards are expected to be readout : "
              << feCards << " in Tell1 sourceID : " << sourceID << endmsg;

    int prevCard = -1;
    while( 0 != size ) {
      // Skip
      unsigned int word = *data++;
      size--;
      // Read bank header
      int lenTrig = word & 0x7F;
      int code    = (word >> 14 ) & 0x1FF;
      int ctrl    = (word >> 23) &  0x1FF;
      checkCtrl( ctrl ,sourceID);

      // access chanID via condDB
      std::vector<LHCb::CaloCellID> chanID  ;
      // look for the FE-Card in the Tell1->cards vector
      int card = findCardbyCode(feCards,code);
      if( 0 <= card ){
        chanID = m_calo->cardChannels( feCards[card] );
        feCards.erase(feCards.begin()+card);
      }else{
        Error(" FE-Card w/ [code : " + Gaudi::Utils::toString(code) + " ] is not associated with TELL1 bank sourceID : "
              + Gaudi::Utils::toString( sourceID )
              + " in condDB :  Cannot read that bank").ignore();
        Error("Warning : previous data may be corrupted").ignore();
        if(m_cleanCorrupted)cleanData(prevCard);
        m_status.addStatus(sourceID, LHCb::RawBankReadoutStatus::Incomplete);
        m_status.addStatus(sourceID, LHCb::RawBankReadoutStatus::Corrupted );
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
      for (unsigned int bitNum = 0; 32 > bitNum; bitNum++ ) {
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
        if( msgLevel( MSG::VERBOSE) )verbose() << " |  SourceID : " << sourceID
                                               << " |  FeBoard : " << m_calo->cardNumber(id)
                                               << " |  Channel : " << bitNum
                                               << " |  CaloCell " << id
                                               << " |  valid ? " << m_calo->valid(id)
                                               << " |  ADC value = " << adc << endmsg;


        //== Keep only valid cells
        if ( 0 != id.index() )fillAdc( id , adc,sourceID);
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
    if ( msgLevel( MSG::DEBUG) )
      debug() << nCards << " FE-Cards are expected to be readout : "
              << feCards << " in Tell1 bank sourceID " << sourceID << endmsg;

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
        Error(" FE-Card w/ [code : " + Gaudi::Utils::toString(code) + " ] is not associated with TELL1 bank sourceID : "
              + Gaudi::Utils::toString(sourceID)
              + " in condDB :  Cannot read that bank").ignore();
        Error("Warning : previous data may be corrupted").ignore();
        if(m_cleanCorrupted)cleanData(prevCard);
        m_status.addStatus( sourceID, LHCb::RawBankReadoutStatus::Corrupted || LHCb::RawBankReadoutStatus::Incomplete);
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

        // event dump
        if(msgLevel(MSG::VERBOSE))verbose() << " |  SourceID : " << sourceID
                                            << " |  FeBoard : " << m_calo->cardNumber(id)
                                            << " |  Channel : " << num
                                            << " |  CaloCell " << id
                                            << " |  valid ? " << m_calo->valid(id)
                                            << " |  ADC value = " << adc << endmsg;
        if ( 0 != id.index() )fillAdc( id , adc,sourceID);
        lenAdc--;
        offset += 16;
      }
    } //== DataSize
    // Check All cards have been read
    if(!checkCards(nCards,feCards))m_status.addStatus(sourceID, LHCb::RawBankReadoutStatus::Incomplete);
  } //== versions

  return true;
}



//==================================
bool CaloDataProvider::decodePrsTriggerBank( LHCb::RawBank* bank ) {

  if(NULL == bank)return false;
  if( LHCb::RawBank::MagicPattern != bank->magic() )return false;// do not decode when MagicPattern is bad

  unsigned int* data = bank->data();
  int size           = bank->size()/4;  // size in byte
  int version        = bank->version();
  int sourceID       = bank->sourceID();
  int lastData       = 0;

  if(0 == size)m_status.addStatus( sourceID, LHCb::RawBankReadoutStatus::Empty);


  if ( msgLevel( MSG::DEBUG) )
    debug() << "Decode Prs bank " << bank << " source " << sourceID
            << " version " << version << " size " << size << endmsg;


  // -----------------------------------------------
  // skip detector specific header line
  if(m_extraHeader){
    ++data ;
    --size;
  }
  // -----------------------------------------------

  //=== Offline coding: a CellID, 8 SPD bits, 8 Prs bits
  if ( 1 == version ) {
    while ( 0 != size ) {
      int spdData = (*data >> 8 ) & 0xFF;
      int prsData = (*data) & 0xFF;
      int lastID  = (*data) >> 16;
        ++data;
        --size;
        for ( unsigned int kk = 0; 8 > kk; ++kk ) {

          LHCb::CaloCellID id( lastID+kk );
          LHCb::CaloCellID spdId( (lastID+kk) & 0x3FFF );


          if ( 0 != (spdData & 1) &&  0 != spdId.index() )fillAdc( spdId , 1,sourceID);




          //event dump
          if(msgLevel(MSG::VERBOSE))verbose() << " |  SourceID : " << sourceID
                                              << " |  FeBoard : " << m_calo->cardNumber(id)
                                              << " |  CaloCell " << id
                                              << " |  valid ? " << m_calo->valid(id)
                                              << " |  Prs/Spd  = " << (prsData & 1) << "/" << (spdData & 1)
                                              << endmsg;

          spdData = spdData >> 1;
          prsData = prsData >> 1;
        }
    }
    //=== Compact coding: a CellID, and its Prs/SPD bits
  } else if ( 2 == version ) {
    while ( 0 != size ) {
      int word = *data;
      while ( 0 != word ) {
        int item = word & 0xFFFF;
        word = ( word >> 16 ) & 0xFFFF;
        int spdId =  (item&0xFFFC) >> 2;

        //event dump
        LHCb::CaloCellID prsId( spdId + 0x4000 );   // Prs
        if(msgLevel(MSG::VERBOSE))verbose() << " |  SourceID : " << sourceID
                                            << " |  FeBoard : " << m_calo->cardNumber( prsId )
                                            << " |  CaloCell " << prsId
                                            << " |  valid ? " << m_calo->valid( prsId )
                                            << " |  Prs/Spd  = " << (item&1) << "/" << (item&2)
                                            << endmsg;


        if ( 0 != (item & 2) ) {
          LHCb::CaloCellID id ( spdId );   // SPD
          if ( 0 != id.index() )fillAdc( id , 1,sourceID);
        }
      }
      ++data;
      --size;
    }
    //==== Codage for 1 MHz
  } else if ( 3 == version ) {
    // Get the FE-Cards associated to that bank (via condDB)
    std::vector<int> feCards = m_calo->tell1ToCards( sourceID );
    int nCards = feCards.size();
    if ( msgLevel( MSG::DEBUG) )
      debug() << nCards << " FE-Cards are expected to be readout : "
              << feCards << " in Tell1 bank sourceID : " << sourceID << endmsg;

    int offset   = 0;
    int lenAdc   = 0;
    int lenTrig  = 0;

    int prevCard = -1;
    while( 0 != size ) {
      int word = *data++;
      size--;
      lenTrig = word & 0x7F;
      lenAdc  = (word >> 7 ) & 0x7F;
      if ( msgLevel( MSG::DEBUG) ) {
        debug() << format( "  Header data %8x size %4d lenAdc%3d lenTrig%3d",
                           word, size, lenAdc, lenTrig )
                << endmsg;
      }
      int code  = (word >>14 ) & 0x1FF;
      int ctrl    = (word >> 23) &  0x1FF;
      checkCtrl( ctrl,sourceID );

      if ( msgLevel( MSG::DEBUG) )
        debug() << "Read FE-board ["<< code << "] linked to TELL1 bank sourceID : " << sourceID << endmsg;
      // access chanID via condDB
      std::vector<LHCb::CaloCellID> chanID  ;
      // look for the FE-Card in the Tell1->cards vector
      int card = findCardbyCode(feCards,code);
      if( 0 <= card ){
        chanID = m_calo->cardChannels( feCards[card] );
        feCards.erase(feCards.begin()+card);
      }else{
        Error(" FE-Card w/ [code : " + Gaudi::Utils::toString(code) + " ] is not associated with TELL1 bank sourceID : "
              + Gaudi::Utils::toString(sourceID)
              + " in condDB :  Cannot read that bank").ignore();
        Error("Warning : previous data may be corrupted").ignore();
        if(m_cleanCorrupted)cleanData(prevCard);
        m_status.addStatus( sourceID,   LHCb::RawBankReadoutStatus::Corrupted || LHCb::RawBankReadoutStatus::Incomplete);
        return false;
      }
      prevCard = card;

      // Process FE-data decoding
      offset   = 32;
      while ( 0 < lenTrig ) {
        if ( 32 == offset ) {
          lastData = *data++;
          size--;
          offset = 0;
        }
        unsigned int num   = ( lastData >> offset ) & 0x3F;
        int isPrs = ( lastData >> (offset +6)) & 1;
        int isSpd = ( lastData >> (offset +7)) & 1;
        offset += 8;
        lenTrig--;

        LHCb::CaloCellID id = LHCb::CaloCellID();
        if(num < chanID.size())id= chanID[ num ];


        // event dump
        if(msgLevel(MSG::VERBOSE))verbose() << " |  SourceID : " << sourceID
                                            << " |  FeBoard : " << code
                                            << " |  Channel : " << num
                                            << " |  CaloCell " << id
                                            << " |  valid ? " << m_calo->valid(id)
                                            << " |  Prs/Spd  = " << isPrs << "/" << isSpd
                                            << endmsg;



        if ( 0 != isSpd ) {
          LHCb::CaloCellID spdId( 0, id.area(), id.row(), id.col() );
          if ( 0 != spdId.index()) fillAdc( spdId , 1, sourceID);
        }
      }
      int nSkip = (lenAdc+1 ) / 2;  // Length in number of words
      size     -= nSkip;
      data     += nSkip;
    } //== DataSize
    // Check All cards have been read
    if(!checkCards(nCards,feCards))m_status.addStatus(sourceID, LHCb::RawBankReadoutStatus::Incomplete);
  } //== versions

  return true;
}
