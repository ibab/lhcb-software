// $Id: CaloEnergyFromRaw.cpp,v 1.8 2006-02-02 16:16:50 ocallot Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

#include "Event/RawEvent.h"

// local
#include "CaloEnergyFromRaw.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloEnergyFromRaw
//
// 2005-01-10 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<CaloEnergyFromRaw>          s_factory ;
const        IToolFactory& CaloEnergyFromRawFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloEnergyFromRaw::CaloEnergyFromRaw( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ICaloEnergyFromRaw>(this);
  m_detectorName = name.substr( 8, 4 );
  if ( name.substr(8,3) == "Prs" ) m_detectorName = "Prs";
  
  declareProperty( "DetectorName", m_detectorName );
  m_adcs.clear();
  m_digits.clear();
}
//=============================================================================
// Destructor
//=============================================================================
CaloEnergyFromRaw::~CaloEnergyFromRaw() {};


//=========================================================================
//  Initialisation, according to the name -> detector
//=========================================================================
StatusCode CaloEnergyFromRaw::initialize ( ) {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  if ( "Ecal" == m_detectorName ) {
    m_calo     = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
    m_roTool   = tool<CaloReadoutTool>( "CaloReadoutTool/EcalReadoutTool" );
    m_pedShift = 0.6;
    m_packedType = LHCb::RawBank::EcalPacked;
    m_shortType  = LHCb::RawBank::EcalE;
  } else if ( "Hcal" == m_detectorName ) {
    m_calo     = getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal );
    m_roTool   = tool<CaloReadoutTool>( "CaloReadoutTool/HcalReadoutTool" );
    m_pedShift = 0.6;
    m_packedType = LHCb::RawBank::HcalPacked;
    m_shortType  = LHCb::RawBank::HcalE;
  } else if ( "Prs" == m_detectorName ) {
    m_calo     = getDet<DeCalorimeter>( DeCalorimeterLocation::Prs );
    m_roTool   = tool<CaloReadoutTool>( "CaloReadoutTool/PrsReadoutTool" );
    m_pedShift = 0.;
    m_packedType = LHCb::RawBank::PrsPacked;
    m_shortType  = LHCb::RawBank::PrsE;
  } else {
    error() << "Unknown detector name " << m_detectorName << endreq;
    return StatusCode::FAILURE;
  }
  
  m_adcs.reserve( m_calo->numberOfCells() );
  m_digits.reserve( m_calo->numberOfCells() );
  debug() << "Got detector element for " << m_detectorName << endreq;
  return StatusCode::SUCCESS;
}

//=========================================================================
//  Return the vector of CaloDigit for the current event. Compute it only if new event
//  This is just a gain calibration (+ped shift) of the adc values.
//=========================================================================
std::vector<LHCb::CaloDigit>&  CaloEnergyFromRaw::digits ( ) {
  adcs( );
  m_digits.clear();
  for ( std::vector<LHCb::CaloAdc>::const_iterator itAdc = m_adcs.begin();
        m_adcs.end() != itAdc; ++itAdc ) {
    LHCb::CaloCellID id = (*itAdc).cellID();
    int adc = (*itAdc).adc();
    double e = ( double(adc) - m_pedShift ) * m_calo->cellGain( id );
    LHCb::CaloDigit dig( id, e );
    m_digits.push_back( dig );
  }
  return m_digits;
}
//=========================================================================
//  Decode the adcs of one event
//=========================================================================
std::vector<LHCb::CaloAdc>& CaloEnergyFromRaw::adcs ( ) {
  m_adcs.clear();
  LHCb::RawEvent* rawEvt = get<LHCb::RawEvent> ( LHCb::RawEventLocation::Default );
  const std::vector<LHCb::RawBank*>* banks = &rawEvt->banks( m_shortType );
  if ( 0 == banks->size() ) {
    banks = &rawEvt->banks( m_packedType );
    debug() << "Found " << banks->size() << " banks of packed type " << m_packedType << endreq;
  } else {
    debug() << "Found " << banks->size() << " banks of short type " << m_shortType << endreq;
  }
  
  std::vector<LHCb::RawBank*>::const_iterator itB = banks->begin();
  
  while  ( banks->end() != itB ) {
    unsigned int* data = (*itB)->data();
    int size           = (*itB)->size()/4;  // Bank size is in bytes
    int version        = (*itB)->version();
    int sourceID       = (*itB)->sourceID();
    itB++;
    int cardNum = 0;
    debug() << "Bank sourceID " << sourceID 
            << " version " << version << " size " << size << endreq;
    if ( 1 > version || 3 < version ) {
      warning() << "Bank type " << (*itB)->type() << " sourceID " << sourceID 
                << " has version " << version 
                << " which is not supported" << endreq;
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
        m_adcs.push_back( temp );
        ++data;
        --size;
      }
    } else if ( 2 == version ) {
      //******************************************************************
      //**** 1 MHz compression format, Ecal and Hcal
      //******************************************************************
      while( 0 != size ) {
        unsigned int word = *data++;
        size--;
        int lenTrig = word & 0x7F;
        int code    = (word >> 14 ) & 0x1FF;
        std::vector<int> feCards = m_roTool->feCardsInTell1( sourceID );
        if ( code != m_roTool->cardCode( feCards[cardNum] ) ) {
          info() << format( "** Error card code tell1 %2d card %2d # %4d code found %4x expected %4x",
                            sourceID, cardNum, feCards[cardNum], code,  
                            m_roTool->cardCode( feCards[cardNum] ) )
                 << endreq;
        }
        std::vector<LHCb::CaloCellID> chanID   = m_roTool->cellInFECard( feCards[cardNum] );
        //== Skip Trigger part, lenth in bytes, includes pattern if present.
        int nSkip = (lenTrig+3)/4;   //== is in bytes, with padding
        data     += nSkip;
        size     -= nSkip;

        unsigned int pattern  = *data++;
        int offset   = 0;
        unsigned int lastData = *data++;
        size -= 2;
        
        cardNum++;
        for ( int bitNum = 0; 32 > bitNum; bitNum++ ) {
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
          LHCb::CaloCellID id = chanID[ bitNum ];
          //== Keep only valid cells
          if ( 0 != id.index() ) {
            LHCb::CaloAdc temp( id, adc);
            m_adcs.push_back( temp );
          }
        }
      }
    } else if ( 3 == version ) {
      //******************************************************************
      //**** 1 MHz compression forma, Preshower + SPD
      //******************************************************************
      while( 0 != size ) {
        unsigned int word = *data++;
        size--;
        int lenTrig = word & 0x7F;
        int lenAdc  = (word >> 7 ) & 0x7F;
        if ( msgLevel( MSG::DEBUG) ) {
          debug() << format( "  Header data %8x size %4d lenAdc%3d lenTrig%3d", 
                             word, size, lenAdc, lenTrig )
                  << endreq;
        }      
        int code  = (word >>14 ) & 0x1FF;
        std::vector<int> feCards = m_roTool->feCardsInTell1( sourceID );
        if ( code != m_roTool->cardCode( feCards[cardNum] ) ) {
          info() << format( "** Error card code tell1 %2d card %2d # %4d code found %4x expected %4x",
                            sourceID, cardNum, feCards[cardNum], code,  
                            m_roTool->cardCode( feCards[cardNum] ) )
                 << endreq;
        }
        if ( msgLevel( MSG::DEBUG) ) {
          debug() << format( "Processing Tell1 %2d card %2d#%3d code %3d lenAdc%3d lenTrig%3d",
                             sourceID, cardNum, feCards[cardNum], code, lenAdc, lenTrig )
                  << endreq;
        }      
        std::vector<LHCb::CaloCellID> chanID = m_roTool->cellInFECard( feCards[cardNum] );
        cardNum++;

        int nSkip = (lenTrig+3)/4;  //== Length in byte, with padding
        size -= nSkip;
        data     += nSkip;
        
        int offset   = 32;  //== force read the first word, to have also the debug for it.
        unsigned int lastData = 0;
        
        while ( 0 < lenAdc ) {
          if ( 32 == offset ) {
            lastData =  *data++;
            size--;
            if ( msgLevel( MSG::DEBUG) ) {
              debug() << format( "  data %8x size %4d lenAdc%3d", lastData, size, lenAdc )
                      << endreq;
            }
            offset = 0;
          }
          int adc = ( lastData >> offset ) & 0x3FF;
          int num = ( lastData >> (offset+10) ) & 0x3F;
          LHCb::CaloCellID id = chanID[ num ];
          LHCb::CaloAdc temp( id, adc );
          m_adcs.push_back( temp );      
          lenAdc--;
          offset += 16;
        }
      } //== DataSize
    } //== versions
  } //== while on banks
  return m_adcs;
}
//=============================================================================
