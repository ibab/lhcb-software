// Include files
#include "Event/RawEvent.h"

// from Gaudi
#include "Event/CaloAdc.h"
#include "Event/L0PrsSpdHit.h"

// local
#include "CaloFillPrsSpdRawBuffer.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloFillPrsSpdRawBuffer
//
// 2005-01-04 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( CaloFillPrsSpdRawBuffer )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloFillPrsSpdRawBuffer::CaloFillPrsSpdRawBuffer( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_calo(NULL)
  , m_totDataSize(0)
  , m_totTrigSize(0)
  , m_nbEvents(0)
{
  m_inputBank        = LHCb::CaloAdcLocation::Prs;
  m_prsBank          = LHCb::L0PrsSpdHitLocation::Prs;
  m_spdBank          = LHCb::L0PrsSpdHitLocation::Spd;
  m_bankType         = LHCb::RawBank::PrsE;
  m_triggerBankType  = LHCb::RawBank::PrsTrig;
  m_numberOfBanks    = 1;

  declareProperty( "DataCodingType",   m_dataCodingType = 2 );
}
//=============================================================================
// Destructor
//=============================================================================
CaloFillPrsSpdRawBuffer::~CaloFillPrsSpdRawBuffer() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode CaloFillPrsSpdRawBuffer::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;

  m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Prs );

  if ( 3 == m_dataCodingType ) {
    m_numberOfBanks =  m_calo->nTell1s();
    m_bankType      = LHCb::RawBank::PrsPacked;
  }

  m_nbEvents    = 0;
  m_totDataSize = 0;
  m_totTrigSize = 0;

  std::vector<unsigned int> a;
  a.reserve(500);
  for ( int kk = 0 ; m_numberOfBanks > kk ; kk++ ) {
    m_banks.push_back( a );
    m_trigBanks.push_back( a );
    m_dataSize.push_back( 0 );
  }

  info() << "Data coding type " << m_dataCodingType << endmsg;

  if ( 3 < m_dataCodingType || 1 > m_dataCodingType ) {
    Error( "Invalid Data coding type", StatusCode::FAILURE );
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloFillPrsSpdRawBuffer::execute() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Execute" << endmsg;

  for ( int kk = 0 ; m_numberOfBanks > kk ; kk++ ) {
    m_banks[kk].clear( );
    m_trigBanks[kk].clear( );
  }

  //== Build the data banks

  if ( 3 == m_dataCodingType ) {
    fillPackedBank( );
  } else {
    fillDataBankShort( );
  }

  //== Build the trigger banks

  if ( 1 == m_dataCodingType ) {
    fillTriggerBank( );
  } else if ( 2 == m_dataCodingType ) {
    fillTriggerBankShort( );
  }

  int totDataSize = 0;
  int totTrigSize = 0;

  LHCb::RawEvent* rawEvent = get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );
  unsigned int board = 0;
  for ( unsigned int kk = 0; m_banks.size() > kk; kk++ ) {
    int version = m_dataCodingType;
    if ( 2 == version ) version = 1;
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << "Data bank coding type : " << m_dataCodingType << endmsg;
    rawEvent->addBank( board, m_bankType, version, m_banks[kk] );
    totDataSize += m_banks[kk].size();
    m_dataSize[kk] += m_banks[kk].size();
    if ( 3 > m_dataCodingType ) {
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
        debug() << "Trigger bank coding type : " << m_dataCodingType << endmsg;
      rawEvent->addBank( board, m_triggerBankType, m_dataCodingType , m_trigBanks[kk] );
      totTrigSize += m_trigBanks[kk].size();
    }
    board++;
  }

  m_totDataSize += totDataSize;
  m_totTrigSize += totTrigSize;
  m_nbEvents++;

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) {
    board = 0;
    debug() << "Bank sizes: ";
    for ( unsigned int kk = 0; m_banks.size() > kk; kk++ ) {
      debug() << format( "%2d:%4d+%4d ", board, m_banks[kk].size(),
                         m_trigBanks[kk].size() );
      board++;
    }
    debug() << endmsg << "Total Data bank size " << totDataSize
            << " + trigger " << totTrigSize << endmsg;
  }

  if( UNLIKELY( msgLevel(MSG::VERBOSE) ) ) {
    board = 0;
    for ( unsigned int kk = 0; m_banks.size() > kk; kk++ ) {
      verbose() << "DATA bank : " << board << endmsg;
      int kl = 0;
      std::vector<unsigned int>::const_iterator itW;

      for ( itW = m_banks[kk].begin(); m_banks[kk].end() != itW; itW++ ){
        verbose() << format ( " %8x %11d   ", (*itW), (*itW) );
        kl++;
        if ( 0 == kl%4 ) verbose() << endmsg;
      }

      verbose() << endmsg <<  "TRIGGER bank size=" << m_trigBanks[kk].size() << "  "
                << endmsg;
      kl = 0;
      for ( itW = m_trigBanks[kk].begin(); m_trigBanks[kk].end() != itW; itW++ ){
        verbose() << format ( " %8x ", (*itW) );
        kl++;
        if ( 0 == kl%8 ) verbose() << endmsg;
      }
      verbose() << endmsg;
      board++;
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CaloFillPrsSpdRawBuffer::finalize() {

  if ( 0 < m_nbEvents ) {
    m_totDataSize /= m_nbEvents;
    m_totTrigSize /= m_nbEvents;
    info() << "Average event size : "
           << format( "%7.1f words, %7.1f for trigger", m_totDataSize, m_totTrigSize );
    double meanSize = 0.;
    double maxSize  = 0.;
    for ( unsigned int kk = 0; m_dataSize.size() > kk ; ++kk ) {
      m_dataSize[kk] /= m_nbEvents;
      meanSize += m_dataSize[kk];
      if ( maxSize < m_dataSize[kk] ) maxSize = m_dataSize[kk];
    }
    meanSize /= m_dataSize.size();
    info() << format ( "  Mean bank size %7.1f, maximum size %7.1f",
                       meanSize, maxSize ) << endmsg;
  }

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
//=========================================================================
//  Fill the calorimeter data bank, simple structure: ID (upper 16 bits) + ADC
//=========================================================================
void CaloFillPrsSpdRawBuffer::fillDataBankShort ( ) {
  LHCb::CaloAdcs* digs = get<LHCb::CaloAdcs>( m_inputBank );
  LHCb::CaloAdcs::const_iterator itD;
  for ( itD = digs->begin(); digs->end() != itD; ++itD ){
    LHCb::CaloCellID id = (*itD)->cellID();
    //== find back the ADC content, is on 10 bits by construction
    int adc = (*itD)->adc();
    int cellIndex = id.all();
    unsigned int word = (cellIndex << 16) + (adc & 0xFFFF );
    m_banks[0].push_back( word );

    if ( MSG::VERBOSE >= msgLevel() ) {
      verbose() << id << format( "adc %4d", adc ) << endmsg;
    }
  }
}

//=========================================================================
//  Fill the Prs/Spd trigger banks
//=========================================================================
void CaloFillPrsSpdRawBuffer::fillTriggerBank ( ) {

  LHCb::L0PrsSpdHits* prs = get<LHCb::L0PrsSpdHits>( m_prsBank );
  LHCb::L0PrsSpdHits* spd = get<LHCb::L0PrsSpdHits>( m_spdBank );
  std::vector<int> words( 1536, 0 );  //== 3 area, 512*8 = 4096 max per area

  LHCb::L0PrsSpdHits::const_iterator itT;
  for ( itT = prs->begin(); prs->end() != itT; ++itT ) {
    LHCb::CaloCellID id = (*itT)->cellID();
    int cellIndex = (id.all() & 0x3FF8 ) >> 3;  // Word index, per group of 8
    int bitNum = id.all() & 0x7;
    words[cellIndex] |= ( 1 << bitNum );

    if ( MSG::VERBOSE >= msgLevel() ) {
      verbose() << format( "Set PRS bit %2d in word %4d for id ", bitNum, cellIndex )
                << id << endmsg;
    }
  }

  for ( itT = spd->begin(); spd->end() != itT; ++itT ) {
    LHCb::CaloCellID id = (*itT)->cellID();
    int cellIndex = (id.all() & 0x3FF8 ) >> 3;  // Word index, per group of 8
    int bitNum = (id.all() & 0x7 ) + 8;
    if ( MSG::VERBOSE >= msgLevel() ) {
      verbose() << format( "Set SPD bit %2d in word %4d for id ", bitNum, cellIndex )
                << id << endmsg;
    }
    words[cellIndex] |= ( 1 << bitNum );
  }
  for ( std::vector<int>::const_iterator itW = words.begin() ;
        words.end() != itW ; ++itW ) {
    if ( 0 == (*itW) ) continue;
    LHCb::CaloCellID id( 8 * (itW-words.begin() ) );
    unsigned int word = (id.all() << 16 ) + (*itW);
    info() << format( "data index %4d value %4x", itW-words.begin(), *itW ) << endmsg;
    m_trigBanks[0].push_back( word );
  }
}

//=========================================================================
//  Fill the Prs/Spd trigger banks
//=========================================================================
void CaloFillPrsSpdRawBuffer::fillTriggerBankShort ( ) {

  LHCb::L0PrsSpdHits* prs = get<LHCb::L0PrsSpdHits>( m_prsBank );
  LHCb::L0PrsSpdHits* spd = get<LHCb::L0PrsSpdHits>( m_spdBank );

  std::vector<int> tag( 3*4096, 0 );  //== 3 area, 4096 max per area

  LHCb::L0PrsSpdHits::const_iterator itT;
  for ( itT = prs->begin(); prs->end() != itT; ++itT ) {
    LHCb::CaloCellID id = (*itT)->cellID();
    int cellIndex = id.all() & 0x3FFF;
    tag[cellIndex] |= 1;
  }

  for ( itT = spd->begin(); spd->end() != itT; ++itT ) {
    LHCb::CaloCellID id = (*itT)->cellID();
    int cellIndex = id.all() & 0x3FFF;
    tag[cellIndex] |= 2;
  }

  unsigned int word = 0;

  for ( std::vector<int>::const_iterator itW = tag.begin() ;
        tag.end() != itW ; ++itW ) {
    if ( 0 == (*itW) ) continue;
    int temp = ( (itW-tag.begin()) << 2 ) + (*itW);
    if ( 0 == word ) {
      word = temp ;
    } else {
      word |= (temp<<16);
      m_trigBanks[0].push_back( word );
      word = 0;
    }
  }
  if( 0 != word )m_trigBanks[0].push_back( word );  // OD : add last word when incomplete
}

//=========================================================================
//  Packed data format, trigger and data in the same bank. Process ALL digits
//=========================================================================
void CaloFillPrsSpdRawBuffer::fillPackedBank ( ) {
  LHCb::CaloAdcs*  digs   = get<LHCb::CaloAdcs>( m_inputBank );
  LHCb::L0PrsSpdHits* prs = get<LHCb::L0PrsSpdHits>( m_prsBank );
  LHCb::L0PrsSpdHits* spd = get<LHCb::L0PrsSpdHits>( m_spdBank );


  for ( int kTell1 = 0 ; m_numberOfBanks > kTell1 ; kTell1++ ) {
    std::vector<int> feCards = m_calo->tell1ToCards( kTell1 );
    for ( std::vector<int>::iterator iFe = feCards.begin(); feCards.end() != iFe; ++iFe ) {
      int cardNum = *iFe;
      int sizeIndex  = m_banks[kTell1].size();
      m_banks[kTell1].push_back( m_calo->cardCode( cardNum ) << 14 );
      int sizeAdc = 0;
      int sizeTrig = 0;

      std::vector<LHCb::CaloCellID> ids = m_calo->cardChannels( cardNum );

      //== First the trigger bits

      int num = 0;
      int offset = 0;
      int nTrigWord = 0;
      int word   = 0;

      for ( std::vector<LHCb::CaloCellID>::const_iterator itId = ids.begin();
            ids.end() != itId; ++itId ) {
        LHCb::CaloCellID id = *itId;
        LHCb::CaloCellID id2( 0, id.area(), id.row(), id.col() );
        int  mask = 0;
        if ( 0 != prs->object( id.all()  ) ) mask |= 0x40;
        if ( 0 != spd->object( id2.all() ) ) mask |= 0x80;
        if ( 0 != mask ) {
          mask |= num;
          mask = mask << offset;
          word |= mask;
          offset += 8;
          if ( 32 == offset ) {
            m_banks[kTell1].push_back( word );
            word = 0;
            offset = 0;
            nTrigWord++;
          }
          sizeTrig += 1;
        }
        num++;
      }

      if ( 0 != word ) {
        m_banks[kTell1].push_back( word );
        word = 0;
        nTrigWord++;
      }

      // Then the Prs ADCs

      num = 0;
      for ( std::vector<LHCb::CaloCellID>::const_iterator itId = ids.begin();
            ids.end() != itId; ++itId ) {
        LHCb::CaloCellID id = *itId;
        LHCb::CaloAdc* adcEntry = digs->object( id );
        if ( 0 != adcEntry &&  0 != adcEntry->adc()) {
          int adc = adcEntry->adc();
          adc = ( adc & 0x3FF ) | ( num << 10 );
          if ( 0 == word ) {
            word = adc;
          } else {
            word |= ( adc<<16);
            m_banks[kTell1].push_back( word );
            word = 0;
          }
          sizeAdc += 1;
        }
        num++;
      }
      if ( 0 != word ) {
        m_banks[kTell1].push_back( word );
        word = 0;
      }

      //== Adjust the header of the FE card.

      m_banks[kTell1][sizeIndex] |= (sizeAdc << 7) + sizeTrig;
      m_totTrigSize += nTrigWord;
    }
  }
}

//=============================================================================
