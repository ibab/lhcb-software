// $Id: CaloFillRawBuffer.cpp,v 1.4 2005-11-10 16:43:22 ocallot Exp $
// Include files 
// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/CaloAdc.h"
#include "Event/L0CaloAdc.h"
#include "Event/RawEvent.h"

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
    m_inputBank        = CaloAdcLocation::Ecal;
    m_triggerBank      = L0CaloAdcLocation::Ecal;
    m_bankType         = RawBuffer::EcalE;
    m_triggerBankType  = RawBuffer::EcalTrig;
    m_numberOfBanks    = 1;
  } else if ("Hcal" == name.substr( 0, 4 ) ) {
    m_detectorName     = "Hcal";
    m_inputBank        = CaloAdcLocation::Hcal;
    m_triggerBank      = L0CaloAdcLocation::Hcal;
    m_bankType         = RawBuffer::HcalE;
    m_triggerBankType  = RawBuffer::HcalTrig;
    m_numberOfBanks    = 1;
  }

  declareProperty( "DataCodingType",   m_dataCodingType = 1 );
  declareProperty( "InputBank",        m_inputBank          );
  declareProperty( "BankType",         m_bankType          );
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
    if ( "Ecal" == m_detectorName ) {
      m_bankType = RawBuffer::EcalPacked;
    } else {
      m_bankType = RawBuffer::HcalPacked;
    }
    info() << "Processing " << m_roTool->nbFECards() 
           << " FE Cards and " << m_roTool->nbTell1() << " TELL1"
           << endreq;

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

  info() << "Data coding type " << m_dataCodingType << endreq;

  if ( 2 < m_dataCodingType || 0 >= m_dataCodingType ) {
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

  if ( 1 == m_dataCodingType ) {
    fillDataBankShort( );
  } else if ( 2 == m_dataCodingType ) {
    fillPackedBank( );
  }
  
  //== Build the trigger banks

  if ( 1 == m_dataCodingType ) fillTriggerBank( );
  
  int totDataSize = 0;
  int totTrigSize = 0;

  RawBuffer* rawBuffer = get<RawBuffer>( RawBufferLocation::Default );
  for ( unsigned int kk = 0; m_banks.size() > kk; kk++ ) {
    rawBuffer->addBank( kk, m_bankType, m_banks[kk], m_dataCodingType );
    totDataSize += m_banks[kk].size();
    m_dataSize[kk] += m_banks[kk].size();
    if ( 1 == m_dataCodingType ) {
      rawBuffer->addBank( kk, m_triggerBankType, m_trigBanks[kk] );
      totTrigSize += m_trigBanks[kk].size();
    }
  }

  m_totDataSize += totDataSize;
  m_totTrigSize += totTrigSize;
  m_nbEvents++;

  if ( msgLevel( MSG::DEBUG ) ) {
    debug() << "Bank sizes: ";
    for ( unsigned int kk = 0; m_banks.size() > kk; kk++ ) {
      debug() << format( "%2d:%4d+%4d ", kk, m_banks[kk].size(),
                         m_trigBanks[kk].size() );
    }
    debug() << endreq << "Total Data bank size " << totDataSize
            << " + trigger " << totTrigSize << endreq;
  }

  if ( MSG::VERBOSE >= msgLevel() ) {
    for ( unsigned int kk = 0; m_banks.size() > kk; kk++ ) {
      verbose() << "DATA bank : " << kk << endreq;
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
    }
  }

  if ( exist<RawEvent>( RawEventLocation::Default ) ) {
    RawEvent* dum = get<RawEvent>( RawEventLocation::Default );
    evtSvc()->unregisterObject( dum );
    delete dum;
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
//  Fill the calorimeter data bank, simple structure: ID (upper 16 bits) + ADC
//=========================================================================
void CaloFillRawBuffer::fillDataBankShort ( ) {
  CaloAdcs* digs = get<CaloAdcs>( m_inputBank );
  CaloAdcs::const_iterator itD;
  for ( itD = digs->begin(); digs->end() != itD; ++itD ){
    CaloCellID id = (*itD)->cellID();
    int adc       = (*itD)->adc();
    int cellIndex = id.raw();
    raw_int word  = (cellIndex << 16) + (adc & 0xFFFF );
    m_banks[0].push_back( word );
  }
}

//=========================================================================
//  Packed data format, trigger and data in the same bank. Process ALL digits
//=========================================================================
void CaloFillRawBuffer::fillPackedBank ( ) {
  CaloAdcs* digs = get<CaloAdcs>( m_inputBank );
  L0CaloAdcs* trigAdcs = get<L0CaloAdcs>( m_triggerBank );
  
  for ( int kTell1 = 0 ; m_numberOfBanks > kTell1 ; kTell1++ ) {
    std::vector<int> feCards = m_roTool->feCardsInTell1( kTell1 );
    for ( std::vector<int>::iterator iFe = feCards.begin(); feCards.end() != iFe; ++iFe ) {
      int cardNum = *iFe;
      int sizeIndex  = m_banks[kTell1].size();
      m_banks[kTell1].push_back( m_roTool->cardCode( cardNum ) << 14 );
      std::vector<CaloCellID> ids = m_roTool->cellInFECard( cardNum );

      //=== The trigger part is first
      int patternIndex = m_banks[kTell1].size();
      int patTrig = 0;
      m_banks[kTell1].push_back( patTrig );
      int word = 0;
      int offset = 0;
      int bNum = 0;
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
      int sizeTrig = 4 * ( m_banks[kTell1].size() - patternIndex ); // in byte
      //== If no trigger at all, remove even the pattern...
      if ( 4 == sizeTrig ) {
        m_banks[kTell1].pop_back();
        sizeTrig = 0;
      } else {
        m_banks[kTell1][patternIndex] = patTrig;
      }

      //=== Now the ADCs
      patternIndex = m_banks[kTell1].size();
      int pattern      = 0;
      m_banks[kTell1].push_back( pattern );
      word = 0;
      offset = 0;
      bNum = 0;

      for ( std::vector<CaloCellID>::const_iterator itId = ids.begin();
            ids.end() != itId; ++itId ) {
        CaloCellID id = *itId;
        CaloAdc* dig = digs->object( id );
        int adc = 256;        //== Default if non existing cell.
        if ( 0 != dig ) {
          adc = dig->adc() + 256;
          if (    0 > adc ) adc = 0;
          if ( 4095 < adc ) adc = 4095;
        }
        if ( 248 <= adc && 264 > adc ) { //... store short
          adc -= 248;
          word |= adc<<offset;
          offset += 4;
        } else {                         //... store long
          word |= adc<<offset;
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
      
      int sizeAdc = 4 * ( m_banks[kTell1].size() - patternIndex );
      m_banks[kTell1][patternIndex] = pattern;

      m_banks[kTell1][sizeIndex] |= (sizeAdc << 7) + sizeTrig;
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
    prevIndx = 0;
  }
  if ( 0 <= prevIndx ) m_trigBanks[prevIndx].push_back( word );

}
//=============================================================================
