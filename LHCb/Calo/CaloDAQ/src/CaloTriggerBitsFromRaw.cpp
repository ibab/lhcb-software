// Include files

// local
#include "CaloTriggerBitsFromRaw.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloTriggerBitsFromRaw
//
// 2005-01-06 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CaloTriggerBitsFromRaw )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloTriggerBitsFromRaw::CaloTriggerBitsFromRaw( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : CaloReadoutTool ( type, name , parent )
{
  declareInterface<ICaloTriggerBitsFromRaw>(this);
  clear();
}


//=============================================================================
// Destructor
//=============================================================================
CaloTriggerBitsFromRaw::~CaloTriggerBitsFromRaw() {}

//=========================================================================
//  Initialisation
//=========================================================================
StatusCode CaloTriggerBitsFromRaw::initialize ( ) {
  StatusCode sc = CaloReadoutTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTool

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "==> Initialize " << name() << endmsg;

  m_calo     = getDet<DeCalorimeter>( DeCalorimeterLocation::Prs );
  m_packedType = LHCb::RawBank::PrsPacked;
  m_shortType  = LHCb::RawBank::PrsTrig;
  m_errorType = LHCb::RawBank::PrsPackedError;

  long nCells = m_calo->numberOfCells();
  (m_data.first).reserve( nCells );
  (m_data.second).reserve( nCells );
  return StatusCode::SUCCESS;
}

//-------------------------------------
void CaloTriggerBitsFromRaw::clear( ) {
  (m_data.first).clear();
  (m_data.second).clear();
  m_readSources.clear();
}

void CaloTriggerBitsFromRaw::cleanData(int feb ) {
  if(feb<0)return;
  LHCb::Calo::FiredCells& prs = m_data.first;
  LHCb::Calo::FiredCells& spd = m_data.second;
  LHCb::Calo::FiredCells tempPrs ;
  LHCb::Calo::FiredCells tempSpd ;
  for(LHCb::Calo::FiredCells::iterator ispd = spd.begin();ispd != spd.end();++ispd){
    if( m_calo->cellParam( *ispd ).cardNumber() == feb)continue;
    tempSpd.push_back( *ispd );
  }
  for(LHCb::Calo::FiredCells::iterator iprs = prs.begin();iprs!=prs.end();++iprs){
    if( m_calo->cellParam( *iprs ).cardNumber() == feb)continue;
    tempPrs.push_back( *iprs );
  }
  clear();
  m_data = std::make_pair(tempPrs,tempSpd);
}




//=========================================================================
//  Return prs or spd cells independently
//  Warning if both method are invoqued consecutively
//  the FULL decoding is run twice
//  --> in this case it's better to invoque prsSpdCells() method
//=========================================================================
const LHCb::Calo::FiredCells& CaloTriggerBitsFromRaw::prsCells () {
  const LHCb::Calo::PrsSpdFiredCells& allCells = prsSpdCells();
  return allCells.first;
}
const LHCb::Calo::FiredCells& CaloTriggerBitsFromRaw::spdCells () {
  const LHCb::Calo::PrsSpdFiredCells& allCells = prsSpdCells();
  return allCells.second;
}
//=========================================================================
//  Decode the fired Cells for ALL banks of one event
//=========================================================================
const LHCb::Calo::PrsSpdFiredCells& CaloTriggerBitsFromRaw::prsSpdCells () {
  return prsSpdCells( -1 );
}
//=========================================================================
//  Decode the TriggerBit for a single bank (given by sourceID)
//  Decode ALL banks if source < 0
//=========================================================================
const LHCb::Calo::PrsSpdFiredCells& CaloTriggerBitsFromRaw::prsSpdCells (int source ) {

  clear();
  bool decoded = false;
  bool found   = false;
  int sourceID     ;
  if(m_getRaw)getBanks();
  if( NULL == m_banks || 0 == m_banks->size() ){
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << "The banks container is empty" << endmsg;
  }else{
    for( std::vector<LHCb::RawBank*>::const_iterator itB = m_banks->begin(); itB != m_banks->end() ; ++itB ) {
      sourceID       = (*itB)->sourceID();
      if( source >= 0 && source != sourceID )continue;
      found = true;
      if(checkSrc( sourceID ))continue;
      decoded = getData ( *itB );
      if( !decoded ){
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
          debug() << "Error when decoding bank " << Gaudi::Utils::toString(sourceID)
                  << " -> incomplete data - May be corrupted" << endmsg;
      }
    }
  }
  if( !found ){
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << "rawBank sourceID : " << Gaudi::Utils::toString(source) << " has not been found"<<endmsg;
  }
  return m_data;
}

//=========================================================================
//  Return appropriate containers (prs.or.spd) for a single bank (given by pointer)
//=========================================================================

const LHCb::Calo::PrsSpdFiredCells& CaloTriggerBitsFromRaw::prsSpdCells (  LHCb::RawBank* bank ) {
  clear();
  if( ! getData ( bank ))clear();
  return m_data;
}

//=========================================================================
//  Main decoding method fill both m_prsCells and m_spdCells containers.
//=========================================================================
bool CaloTriggerBitsFromRaw::getData(  LHCb::RawBank* bank ) {
  if(NULL == bank)return false;
  if( LHCb::RawBank::MagicPattern != bank->magic() )return false;// do not decode when MagicPattern is bad
  unsigned int* data = bank->data();
  int size           = bank->size()/4;  // size in byte
  int version        = bank->version();
  int sourceID       = bank->sourceID();
  int lastData       = 0;

  if(0 == size)m_status.addStatus( sourceID, LHCb::RawBankReadoutStatus::Empty);

  if ( msgLevel( MSG::DEBUG) )debug() << "Decode bank " << bank << " source " << sourceID
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

          if ( spdData & 1 ) m_data.second.push_back( spdId);
          if ( prsData & 1 ) m_data.first.push_back( id );


          //event dump
          if ( msgLevel( MSG::VERBOSE) ) {
            verbose() << " |  SourceID : " << sourceID
                    << " |  FeBoard : " << m_calo->cardNumber(id)
                    << " |  CaloCell " << id
                    << " |  valid ? " << m_calo->valid(id)
                    << " |  Prs/Spd  = " << (prsData & 1) << "/" << (spdData & 1) << endmsg;
          }
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
        if ( msgLevel( MSG::VERBOSE) ) {
          verbose() << " |  SourceID : " << sourceID
                    << " |  FeBoard : " << m_calo->cardNumber( prsId )
                    << " |  CaloCell " << prsId
                    << " |  valid ? " << m_calo->valid( prsId )
                    << " |  Prs/Spd  = " << (item&1) << "/" << (item&2) << endmsg;
        }


        if( 0 != spdId ){
          if ( 0 != (item & 2) ) {
            LHCb::CaloCellID id ( spdId );   // SPD
            m_data.second.push_back( id );
          }
          if ( 0 != (item & 1) ) {
            m_data.first.push_back( prsId );
          }
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
    if ( msgLevel( MSG::DEBUG) )debug() << nCards << " FE-Cards are expected to be readout : "
                                        << feCards << " in Tell1 bank " << sourceID << endmsg;

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
      checkCtrl(ctrl,sourceID);

      if ( msgLevel( MSG::DEBUG) )debug() << "Read FE-board ["<< code << "] linked to TELL1 bank " << sourceID << endmsg;
      // access chanID via condDB
      std::vector<LHCb::CaloCellID> chanID  ;
      // look for the FE-Card in the Tell1->cards vector
      int card = findCardbyCode(feCards,code);
      if( 0 <= card ){
        chanID = m_calo->cardChannels( feCards[card] );
        feCards.erase(feCards.begin()+card);
      }else{
        Error(" FE-Card w/ [code : " + Gaudi::Utils::toString(code) + " ] is not associated with TELL1 bank sourceID : "
              +Gaudi::Utils::toString(sourceID)
              + " in condDB :  Cannot read that bank").ignore();
        Error("Warning : previous data may be corrupted").ignore();
        if(m_cleanCorrupted)cleanData(prevCard);
        m_status.addStatus( sourceID,   LHCb::RawBankReadoutStatus::Incomplete);
        m_status.addStatus( sourceID,   LHCb::RawBankReadoutStatus::Corrupted );
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
        if ( msgLevel( MSG::VERBOSE) ) {
          verbose() << " |  SourceID : " << sourceID
                    << " |  FeBoard : " << code
                    << " |  Channel : " << num
                    << " |  CaloCell " << id
                    << " |  valid ? " << m_calo->valid(id)
                    << " |  Prs/Spd  = " << isPrs << "/" << isSpd << endmsg;
        }


        if ( 0 != id.index() ){
          if ( 0 != isPrs ) m_data.first.push_back( id );
          if ( 0 != isSpd ) {
            LHCb::CaloCellID spdId( 0, id.area(), id.row(), id.col() );
            m_data.second.push_back( spdId );
          }
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
//=============================================================================
