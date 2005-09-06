// $Id: CaloFillPrsSpdRawBuffer.cpp,v 1.3 2005-09-06 14:50:01 ocallot Exp $
// Include files 
// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/CaloDigit.h"
#include "Event/L0PrsSpdHit.h"

// local
#include "CaloFillPrsSpdRawBuffer.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloFillPrsSpdRawBuffer
//
// 2005-01-04 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<CaloFillPrsSpdRawBuffer>          s_factory ;
const        IAlgFactory& CaloFillPrsSpdRawBufferFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloFillPrsSpdRawBuffer::CaloFillPrsSpdRawBuffer( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  m_inputBank        = CaloDigitLocation::Prs;
  m_prsBank          = L0PrsSpdHitLocation::Prs;
  m_spdBank          = L0PrsSpdHitLocation::Spd;
  m_bankType         = RawBuffer::PrsE;
  m_triggerBankType  = RawBuffer::PrsTrig;
  m_numberOfBanks    = 8;

  declareProperty( "DataCodingType",   m_dataCodingType = 1 );
  declareProperty( "EnergyScale",      m_energyScale    = 0.1 * MeV );

}
//=============================================================================
// Destructor
//=============================================================================
CaloFillPrsSpdRawBuffer::~CaloFillPrsSpdRawBuffer() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CaloFillPrsSpdRawBuffer::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_calo = getDet<DeCalorimeter>( "/dd/Structure/LHCb/Prs" );
  m_roTool = tool<CaloReadoutTool>( "CaloReadoutTool/PrsReadoutTool" );

  if ( 3 == m_dataCodingType ) {
    m_numberOfBanks =  m_roTool->nbTell1();
  }  

  m_nbEvents    = 0;
  m_totDataSize = 0;
  m_totTrigSize = 0;

  std::vector<raw_int> a;
  a.reserve(500);
  for ( int kk = 0 ; m_numberOfBanks > kk ; kk++ ) {
    m_banks.push_back( a );
    m_trigBanks.push_back( a );
    m_dataSize.push_back( 0 );
  }

  info() << "Data coding type " << m_dataCodingType 
         << " energy scale " << m_energyScale/MeV << " MeV"
         << endreq;

  if ( 3 < m_dataCodingType || 0 > m_dataCodingType ) {
    Error( "Invalid Data coding type", StatusCode::FAILURE );
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloFillPrsSpdRawBuffer::execute() {

  debug() << "==> Execute" << endmsg;
 
  for ( int kk = 0 ; m_numberOfBanks > kk ; kk++ ) {
    m_banks[kk].clear( );
    m_trigBanks[kk].clear( );
  }

  //== Build the data banks

  if ( 0 == m_dataCodingType ) {
    fillDataBank( );
  } else if ( 1 == m_dataCodingType ) {
    fillDataBankShort( );
  } else if ( 2 == m_dataCodingType ) {
  } else if ( 3 == m_dataCodingType ) {
    fillPackedBank( );
  }
  
  //== Build the trigger banks

  if ( 2 > m_dataCodingType ) fillTriggerBank( );
  
  int totDataSize = 0;
  int totTrigSize = 0;

  RawBuffer* rawBuffer = get<RawBuffer>( RawBufferLocation::Default );
  raw_int board = 0;
  for ( unsigned int kk = 0; m_banks.size() > kk; kk++ ) {
    rawBuffer->addBank( board, m_bankType, m_banks[kk], m_dataCodingType );
    totDataSize += m_banks[kk].size();
    m_dataSize[kk] += m_banks[kk].size();
    if ( 2 > m_dataCodingType ) {
      rawBuffer->addBank( board, m_triggerBankType, m_trigBanks[kk] );
      totTrigSize += m_trigBanks[kk].size();
    } 
    board++;
  }

  m_totDataSize += totDataSize;
  m_totTrigSize += totTrigSize;
  m_nbEvents++;

  if ( MSG::DEBUG >= msgLevel() ) {
    board = 0;
    debug() << "Bank sizes: ";
    for ( unsigned int kk = 0; m_banks.size() > kk; kk++ ) {
      debug() << format( "%2d:%4d+%4d ", board, m_banks[kk].size(),
                         m_trigBanks[kk].size() );
      board++;
    }
    debug() << endreq << "Total Data bank size " << totDataSize
            << " + trigger " << totTrigSize << endreq;
  }

  if ( MSG::VERBOSE >= msgLevel() ) {
    board = 0;
    for ( unsigned int kk = 0; m_banks.size() > kk; kk++ ) {
      verbose() << "DATA bank : " << board << endreq;
      int kl = 0;
      std::vector<raw_int>::const_iterator itW;
      
      for ( itW = m_banks[kk].begin(); m_banks[kk].end() != itW; itW++ ){
        verbose() << format ( " %8x %11d   ", (*itW), (*itW) );
        kl++;
        if ( 0 == kl%4 ) verbose() << endreq;
      }
      
      verbose() << endreq <<  "TRIGGER bank size=" << m_trigBanks[kk].size() << "  "
                << endreq;
      kl = 0;
      for ( itW = m_trigBanks[kk].begin(); m_trigBanks[kk].end() != itW; itW++ ){
        verbose() << format ( " %8x ", (*itW) );
        kl++;
        if ( 0 == kl%8 ) verbose() << endreq;
      }
      verbose() << endreq;
      board++;
    }
  }
 
  return StatusCode::SUCCESS;
};

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
                       meanSize, maxSize ) << endreq;
  }
  
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
//=========================================================================
//  Fill the calorimeter data bank, cluster structure
//=========================================================================
void CaloFillPrsSpdRawBuffer::fillDataBank ( ) {
  int prevIndx   = -1;
  int nextIndex  = -1;

  //== Store the ADC data. Format type  clusters, 32 bits per adc value.

  int headIndex  = -1;
  int clusLength = 0;
  int bufIndx    = -1;
  CaloDigits* digs = get<CaloDigits>( m_inputBank );
  CaloDigits::const_iterator itD;
  for ( itD = digs->begin(); digs->end() != itD; ++itD ){
    CaloCellID id = (*itD)->cellID();
    int adc = int( floor( (*itD)->e() / m_energyScale + .5 ) ); 
    int cellIndex = id.raw();
    
    bufIndx = bufferNumber( id );
      
    if ( bufIndx != prevIndx ) { nextIndex = -1; }
      
    if ( cellIndex != nextIndex ) {
      if ( 0 <= headIndex ) {
        m_banks[prevIndx][headIndex] += clusLength;
      }
      clusLength = 0;
      headIndex  = m_banks[bufIndx].size();
      m_banks[bufIndx].push_back( cellIndex << 16 );
    }
    m_banks[bufIndx].push_back( adc );
    nextIndex = cellIndex+1;
    prevIndx = bufIndx;
    clusLength++;
  }
  if ( 0 <= headIndex ) {
    m_banks[bufIndx][headIndex] += clusLength;
  }
}

//=========================================================================
//  Fill the calorimeter data bank, simple structure: ID (upper 16 bits) + ADC
//=========================================================================
void CaloFillPrsSpdRawBuffer::fillDataBankShort ( ) {
  CaloDigits* digs = get<CaloDigits>( m_inputBank );
  CaloDigits::const_iterator itD;
  for ( itD = digs->begin(); digs->end() != itD; ++itD ){
    CaloCellID id = (*itD)->cellID();
    //== find back the ADC content, is on 10 bits by construction
    int adc = int( floor( (*itD)->e() / m_calo->cellGain(id) + .5 ) );
    int cellIndex = id.raw();
    raw_int word = (cellIndex << 16) + (adc & 0xFFFF );
    int bufIndx = bufferNumber( id );
    m_banks[bufIndx].push_back( word );
    
    if ( MSG::VERBOSE >= msgLevel() ) {
      verbose() << id << format( "energy %7.2f gain %8.4f adc %4d",
                                 (*itD)->e(), m_calo->cellGain(id), adc ) 
                << endreq;
    }
  }
}

//=========================================================================
//  Fill the Prs/Spd trigger banks
//=========================================================================
void CaloFillPrsSpdRawBuffer::fillTriggerBank ( ) {

  L0PrsSpdHits* prs = get<L0PrsSpdHits>( m_prsBank );
  L0PrsSpdHits* spd = get<L0PrsSpdHits>( m_spdBank );
  std::vector<int> words( 1536, 0 );  //== 3 area, 512*8 = 4096 max per area

  L0PrsSpdHits::const_iterator itT;
  for ( itT = prs->begin(); prs->end() != itT; ++itT ) {
    CaloCellID id = (*itT)->cellID();
    int cellIndex = (id.raw() & 0x3FF8 ) >> 3;  // Word index, per group of 8
    int bitNum = id.raw() & 0x7;
    words[cellIndex] |= ( 1 << bitNum );

    if ( MSG::VERBOSE >= msgLevel() ) {
      verbose() << format( "Set PRS bit %2d in word %4d for id ", bitNum, cellIndex )
                << id << endreq;
    }
  }

  for ( itT = spd->begin(); spd->end() != itT; ++itT ) {
    CaloCellID id = (*itT)->cellID();
    int cellIndex = (id.raw() & 0x3FF8 ) >> 3;  // Word index, per group of 8
    int bitNum = (id.raw() & 0x7 ) + 8;
    verbose() << format( "Set SPD bit %2d in word %4d for id ", bitNum, cellIndex )
              << id << endreq;
    words[cellIndex] |= ( 1 << bitNum );
  }
  for ( std::vector<int>::const_iterator itW = words.begin() ; 
        words.end() != itW ; ++itW ) {
    if ( 0 == (*itW) ) continue;
    CaloCellID id( 8 * (itW-words.begin() ) );
    raw_int word = (id.raw() << 16 ) + (*itW);
    int bufIndx = bufferNumber( id );
    verbose() << format( "Store %8x in bank %2d for id ", word, bufIndx )
              << id << endreq;
    m_trigBanks[bufIndx].push_back( word );
  }
}

//=========================================================================
//  Packed data format, trigger and data in the same bank. Process ALL digits
//=========================================================================
void CaloFillPrsSpdRawBuffer::fillPackedBank ( ) {
  CaloDigits*  digs = get<CaloDigits>( m_inputBank );
  L0PrsSpdHits* prs = get<L0PrsSpdHits>( m_prsBank );
  L0PrsSpdHits* spd = get<L0PrsSpdHits>( m_spdBank );
  
  for ( int kTell1 = 0 ; m_numberOfBanks > kTell1 ; kTell1++ ) {
    std::vector<int> feCards = m_roTool->feCardsInTell1( kTell1 );
    for ( std::vector<int>::iterator iFe = feCards.begin(); feCards.end() != iFe; ++iFe ) {
      int cardNum = *iFe;
      int sizeIndex  = m_banks[kTell1].size();
      m_banks[kTell1].push_back( m_roTool->cardCode( cardNum ) << 14 );
      int sizeAdc = 0;
      int sizeTrig = 0;

      std::vector<CaloCellID> ids = m_roTool->cellInFECard( cardNum );
      int num = 0;
      int word   = 0;
      
      for ( std::vector<CaloCellID>::const_iterator itId = ids.begin();
            ids.end() != itId; ++itId ) {
        CaloCellID id = *itId;
        CaloDigit* dig = digs->object( id );
        if ( 0 != dig ) {
          int adc = int( floor( dig->e() / m_calo->cellGain(id) + .5 ) );
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

      //== Now the trigger bits

      num = 0;
      int offset = 0;
      if ( 0 != word ) offset = 16;
      int nTrigWord = 0;
      
      for ( std::vector<CaloCellID>::const_iterator itId = ids.begin();
            ids.end() != itId; ++itId ) {
        CaloCellID id = *itId;
        int  mask = 0;
        L0PrsSpdHits::const_iterator itT;
        for ( itT = prs->begin(); prs->end() != itT; ++itT ) {
          if ( id == (*itT)->cellID() ) {
            mask |= 0x40;
            break;
          }
        }
        CaloCellID id2( 0, id.area(), id.row(), id.col() );
        for ( itT = spd->begin(); spd->end() != itT; ++itT ) {
          if ( id2 == (*itT)->cellID() ) {
            mask |= 0x80;
            break;
          }
        }
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
      m_banks[kTell1][sizeIndex] |= (sizeAdc << 7) + sizeTrig;
      m_totTrigSize += nTrigWord;
    }
  }  
}

//=============================================================================
