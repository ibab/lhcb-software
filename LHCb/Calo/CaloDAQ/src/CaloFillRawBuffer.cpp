// $Id: CaloFillRawBuffer.cpp,v 1.3 2005-09-06 14:50:01 ocallot Exp $
// Include files 
// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/CaloDigit.h"
#include "Event/L0CaloAdc.h"

// local
#include "CaloFillRawBuffer.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloFillRawBuffer
//
// 2004-12-17 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<CaloFillRawBuffer>          s_factory ;
const        IAlgFactory& CaloFillRawBufferFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloFillRawBuffer::CaloFillRawBuffer( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  //=== Default values according to the name of the algorithm !
  if ( "Ecal" == name.substr( 0, 4 ) ) {
    m_detectorName     = "Ecal";
    m_inputBank        = CaloDigitLocation::Ecal;
    m_triggerBank      = L0CaloAdcLocation::Ecal;
    m_bankType         = RawBuffer::EcalE;
    m_triggerBankType  = RawBuffer::EcalTrig;
    m_numberOfBanks    = 10;
  } else if ("Hcal" == name.substr( 0, 4 ) ) {
    m_detectorName     = "Hcal";
    m_inputBank        = CaloDigitLocation::Hcal;
    m_triggerBank      = L0CaloAdcLocation::Hcal;
    m_bankType         = RawBuffer::HcalE;
    m_triggerBankType  = RawBuffer::HcalTrig;
    m_numberOfBanks    = 4;
  }

  declareProperty( "DataCodingType",   m_dataCodingType = 1 );
  declareProperty( "EnergyScale",      m_energyScale    = 1. * MeV );
}
//=============================================================================
// Destructor
//=============================================================================
CaloFillRawBuffer::~CaloFillRawBuffer() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CaloFillRawBuffer::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_calo = getDet<DeCalorimeter>( "/dd/Structure/LHCb/" + m_detectorName );

  std::string toolName = "CaloReadoutTool/" + m_detectorName + "ReadoutTool";
  m_roTool = tool<CaloReadoutTool>( toolName );

  if ( 2 == m_dataCodingType ) {
    m_numberOfBanks =  m_roTool->nbTell1();
    info() << "Processing " << m_roTool->nbFECards() 
           << " FE Cards and " << m_roTool->nbTell1() << " TELL1"
           << endreq;

    if ( "Ecal" == m_detectorName ) {
      m_inputBank = CaloDigitLocation::FullEcal;
    } else if ( "Hcal" == m_detectorName ) {
      m_inputBank = CaloDigitLocation::FullHcal;
    }
  }
  
  m_nbEvents    = 0;
  m_totDataSize = 0;
  m_totTrigSize = 0;

  std::vector<raw_int> a;
  a.reserve(500);
  for ( int kk = 0 ; m_numberOfBanks > kk ; kk++ ) {
    m_banks.push_back( a );
    m_trigBanks.push_back( a );
    m_dataSize.push_back( 0. );
  }

  info() << "Data coding type " << m_dataCodingType 
         << " energy scale " << m_energyScale/MeV << " MeV"
         << endreq;

  if ( 2 < m_dataCodingType || 0 > m_dataCodingType ) {
    Error( "Invalid Data coding type", StatusCode::FAILURE );
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloFillRawBuffer::execute() {

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
    if ( 2 >m_dataCodingType ) {
      rawBuffer->addBank( board, m_triggerBankType, m_trigBanks[kk] );
      totTrigSize += m_trigBanks[kk].size();
    }
    board++;
  }

  m_totDataSize += totDataSize;
  m_totTrigSize += totTrigSize;
  m_nbEvents++;

  if ( msgLevel( MSG::DEBUG ) ) {
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
StatusCode CaloFillRawBuffer::finalize() {

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
void CaloFillRawBuffer::fillDataBank ( ) {
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
void CaloFillRawBuffer::fillDataBankShort ( ) {
  CaloDigits* digs = get<CaloDigits>( m_inputBank );
  CaloDigits::const_iterator itD;
  for ( itD = digs->begin(); digs->end() != itD; ++itD ){
    CaloCellID id = (*itD)->cellID();
    //== find back the ADC content, is on 12 bits by construction
    //== As there is a pedestal shift, add one before int rounding.
    int adc = int( floor( (*itD)->e() / m_calo->cellGain(id) + 1. ) );
    int cellIndex = id.raw();
    raw_int word = (cellIndex << 16) + (adc & 0xFFFF );
    int bufIndx = bufferNumber( id );
    m_banks[bufIndx].push_back( word );
  }
}

//=========================================================================
//  Packed data format, trigger and data in the same bank. Process ALL digits
//=========================================================================
void CaloFillRawBuffer::fillPackedBank ( ) {
  CaloDigits* digs = get<CaloDigits>( m_inputBank );
  L0CaloAdcs* trigAdcs = get<L0CaloAdcs>( m_triggerBank );
  
  for ( int kTell1 = 0 ; m_numberOfBanks > kTell1 ; kTell1++ ) {
    std::vector<int> feCards = m_roTool->feCardsInTell1( kTell1 );
    for ( std::vector<int>::iterator iFe = feCards.begin(); feCards.end() != iFe; ++iFe ) {
      int cardNum = *iFe;
      int sizeIndex  = m_banks[kTell1].size();
      m_banks[kTell1].push_back( m_roTool->cardCode( cardNum ) << 8 );
      int patternIndex = m_banks[kTell1].size();
      int pattern      = 0;
      m_banks[kTell1].push_back( pattern );
      int word = 0;
      int offset = 0;

      std::vector<CaloCellID> ids = m_roTool->cellInFECard( cardNum );
      int bNum = 0;
      for ( std::vector<CaloCellID>::const_iterator itId = ids.begin();
            ids.end() != itId; ++itId ) {
        CaloCellID id = *itId;
        CaloDigit* dig = digs->object( id );
        int adc = 8;        //== Default if non existing cell.
        if ( 0 != dig ) {
          //== find back the ADC content, is on 12 bits by construction
          //== As there is a pedestal shift, add one before int rounding.
          adc = int( floor( dig->e() / m_calo->cellGain( id ) + 1. ) ) + 8;
          if (    0 > adc ) adc = 0;
          if ( 4095 < adc ) adc = 4095;
        }

        word |= adc<<offset;
        if ( 16   > adc ) {  //... store short
          offset  += 4;
        } else {             //... store long.
          pattern += (1<<bNum);
          offset  +=12;
        }
        if ( 32 <= offset ) {        //== Have we a full word or more ? Store it
          m_banks[kTell1].push_back( word );
          offset -= 32;
          word    = adc >> (12-offset);  //== upper bits if needed
        }
        bNum++;
      }
      if ( 0 != offset )  m_banks[kTell1].push_back( word );
      
      int sizeAdc = m_banks[kTell1].size() - patternIndex;
      m_banks[kTell1][patternIndex] = pattern;

      //=== Now the trigger part
      patternIndex = m_banks[kTell1].size();
      int patTrig = 0;
      m_banks[kTell1].push_back( patTrig );
      word = 0;
      offset = 0;
      bNum = 0;
      for ( std::vector<CaloCellID>::const_iterator itId = ids.begin();
            ids.end() != itId; ++itId ) {
        CaloCellID id = *itId;
        L0CaloAdc* trig = trigAdcs->object( id.index() );
        if ( 0 != trig ) {
          patTrig |= 1<<bNum;
          int adc = trig->adc();
          if ( 24 < offset ) {
            m_banks[kTell1].push_back( word );
            offset = 0;
            word = 0;
          }
          word |= adc << offset;
          offset += 8;
        }
        bNum++;
      }
      if ( 0 != offset ) {
        m_banks[kTell1].push_back( word );
      }
      int sizeTrig = m_banks[kTell1].size() - patternIndex;
      //== If no trigger at all, remove even the pattern...
      if ( 1 == sizeTrig ) {
        m_banks[kTell1].pop_back();
        sizeTrig = 0;
      } else {
        m_banks[kTell1][patternIndex] = patTrig;
      }
      m_banks[kTell1][sizeIndex] |= (sizeAdc << 4) + sizeTrig;
      m_totTrigSize += sizeTrig;
      
      if ( msgLevel( MSG::DEBUG ) ) {
        debug() << format( "Tell1 %2d card %3d pattern %8x patTrig %8x size Adc %2d Trig %2d",
                           kTell1, cardNum, pattern, patTrig, sizeAdc, sizeTrig )
                << endreq;
      }
    }
  }

}

//=========================================================================
//  Fill the Calorimeter trigger banks
//=========================================================================
void CaloFillRawBuffer::fillTriggerBank ( ) {

  int prevIndx  = -1;
  int nextIndex = -1;
  int word      = -1;

  L0CaloAdcs* trigAdcs = get<L0CaloAdcs>( m_triggerBank );
  L0CaloAdcs::const_iterator itT;
  for ( itT = trigAdcs->begin(); trigAdcs->end() != itT; ++itT ) {
    CaloCellID id = (*itT)->cellID();
    int bufIndx = bufferNumber( id );
    
    if ( bufIndx != prevIndx ) { nextIndex = -1; }
    
    int cellIndex = id.raw();
    if ( cellIndex != nextIndex ) {
      if ( 0 <= prevIndx ) m_trigBanks[prevIndx].push_back( word );
      word = (cellIndex << 16) + ((*itT)->adc() << 8);
      nextIndex = cellIndex+1;
    } else {
      word += (*itT)->adc();
    }
    
    if ( MSG::VERBOSE >= msgLevel() ) {
      verbose() << id << format( "trigValue %3d word %8x ", (*itT)->adc(), word) << endreq;
    }
    
    prevIndx = bufIndx;
  }
  if ( 0 <= prevIndx ) m_trigBanks[prevIndx].push_back( word );

}
//=============================================================================
