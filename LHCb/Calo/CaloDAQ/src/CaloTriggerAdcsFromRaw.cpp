// $Id: CaloTriggerAdcsFromRaw.cpp,v 1.8 2006-11-23 13:38:32 cattanem Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
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
  : GaudiTool ( type, name , parent )
{
  declareInterface<ICaloTriggerAdcsFromRaw>(this);
  int index = name.find_first_of(".",0) +1 ; // return -1+1=0 if '.' not found --> OK !!
  m_detectorName = name.substr( index, 4 );

  declareProperty( "DetectorName", m_detectorName );
  declareProperty( "PackedIsDefault", m_packedIsDefault = false);
  m_adcs.clear();
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

  debug() << "==> Initialize" << endmsg;
  
  if ( "Ecal" == m_detectorName ) {
    m_roTool  = tool<CaloReadoutTool>( "CaloReadoutTool/EcalReadoutTool" );
    m_packedType = LHCb::RawBank::EcalPacked;
    m_shortType  = LHCb::RawBank::EcalTrig;
  } else if ( "Hcal" == m_detectorName ) {
    m_roTool  = tool<CaloReadoutTool>( "CaloReadoutTool/HcalReadoutTool" );
    m_packedType = LHCb::RawBank::HcalPacked;
    m_shortType  = LHCb::RawBank::HcalTrig;
  } else {
    error() << "Unknown detector name '" << m_detectorName 
            << "'. Set it by option 'DetectorName', should be Ecal or Hcal" << endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
//=========================================================================
//  Unpack a new event if needed, and return the vector of L0CaloAdc
//=========================================================================
std::vector<LHCb::L0CaloAdc>& CaloTriggerAdcsFromRaw::adcs ( ) {
  LHCb::RawEvent* rawEvt = get<LHCb::RawEvent>( rootOnTES() + LHCb::RawEventLocation::Default );

  const std::vector<LHCb::RawBank*>*  banks = 0;
  if( !m_packedIsDefault)banks= &rawEvt->banks( m_packedType );
  if ( 0 == banks || 0 == banks->size() ) {
    banks = &rawEvt->banks( m_shortType );
    debug() << "Found " << banks->size() << " banks of short type " << m_shortType << endreq;
    if ( 0 == banks->size() )warning() << " Requested packed Banks has not been found "<< endreq;
  } else {
    debug() << "Found " << banks->size() << " banks of packed type " << m_packedType << endreq;
  }
  m_adcs.clear();
  std::vector<LHCb::RawBank*>::const_iterator itB = banks->begin();
  while ( banks->end() != itB ) {
    unsigned int* data = (*itB)->data();
    int size           = (*itB)->size()/4;  // in bytes in the header
    int version        = (*itB)->version();
    int sourceID       = (*itB)->sourceID();
    int lastData = 0;
    itB++;
    //=== Offline coding
    if ( 2 > version ) {
      debug() << "Starting new bank, version " << version << " size " << size << endreq;
      while ( 0 < size ) {
        int lastID   = (*data) >> 16;
        int adc1     = (*data)>>8 & 0xFF;
        int adc2     = (*data)    & 0xFF;
        ++data;
        --size;
        if ( 0 != adc1 ) {
          LHCb::CaloCellID id( lastID );
          LHCb::L0CaloAdc dum( id, adc1 );
          m_adcs.push_back( dum );
        }
        if ( 0 != adc2 ) {
          LHCb::CaloCellID id( ++lastID );
          LHCb::L0CaloAdc dum( id, adc2 );
          m_adcs.push_back( dum );
        }
      }
    } else if ( 2 == version ) {
      //==== Codage for 1 MHz
      int cardNum  = 0;
      int lenAdc   = 0;
      int lenTrig  = 0;
      debug() << "Starting new bank, version " << version << " size " << size << endreq;
      while ( 0 < size ) {
        int word = *data++;
        size--;
        lenTrig = word & 0x3F;
        lenAdc  = (word >> 7 ) & 0x3F;
        int code  = (word >> 14 ) & 0x1FF;
        std::vector<int> feCards = m_roTool->feCardsInTell1( sourceID );
        if ( code != m_roTool->cardCode( feCards[cardNum] ) ) {
          info() << format( "** Error card code tell1 %2d card %2d # %4d code found %4x expected %4x",
                            sourceID, cardNum, feCards[cardNum], code,
                            m_roTool->cardCode( feCards[cardNum] ) )
                 << endreq;
        }
        debug() << " Starting new card code " << code << " lenAdc " << lenAdc
                << " lenTrig " << lenTrig << " size " << size
                << endreq;

        std::vector<LHCb::CaloCellID>& chanID = m_roTool->cellInFECard( feCards[cardNum] );
        cardNum++;
        if ( 0 < lenTrig ) {
          int pattern  = *data++;
          int offset   = 0;
          debug() << format( " pattern %8x lenTrig %2d", pattern, lenTrig ) << endreq;
          lastData  = *data++;
          size -= 2;
          for ( int bitNum = 0 ; 32 > bitNum ; bitNum++ ) {
            if ( 0 != (pattern & (1<<bitNum)) ) {
              if ( 31 < offset ) {
                offset   = 0;
                lastData = *data++;
                size--;
              }
              LHCb::CaloCellID id = chanID[ bitNum ];
              if ( 0 != id.index() ) {
                int adc = ( lastData >> offset ) & 0xFF;
                LHCb::L0CaloAdc temp( id, adc );
                m_adcs.push_back( temp );
              }
              offset += 8;
            }
          }
        }
        int nSkip = (lenAdc+3)/4;  //== length in byte, skip words
        data     += nSkip;
        size     -= nSkip;
      } // another card ?
    } // another bank ?
  } // 1 MHz coding ?
  return m_adcs;
}
//=============================================================================
