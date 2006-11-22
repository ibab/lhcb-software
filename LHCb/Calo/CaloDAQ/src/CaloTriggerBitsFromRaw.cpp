// $Id: CaloTriggerBitsFromRaw.cpp,v 1.11 2006-11-22 12:29:33 ocallot Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "CaloTriggerBitsFromRaw.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloTriggerBitsFromRaw
//
// 2005-01-06 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CaloTriggerBitsFromRaw );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloTriggerBitsFromRaw::CaloTriggerBitsFromRaw( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ICaloTriggerBitsFromRaw>(this);
  m_prsCells.clear();
  m_spdCells.clear();
  declareProperty( "PackedIsDefault", m_packedIsDefault = false);
}
//=============================================================================
// Destructor
//=============================================================================
CaloTriggerBitsFromRaw::~CaloTriggerBitsFromRaw() {

};
//=========================================================================
//  Initialisation
//=========================================================================
StatusCode CaloTriggerBitsFromRaw::initialize ( ) {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTool

  debug() << "==> Initialize" << endmsg;

  m_roTool   = tool<CaloReadoutTool>( "CaloReadoutTool/PrsReadoutTool" );

  return StatusCode::SUCCESS;
}

//=========================================================================
//  Unpack a new event if needed, and return the appropriate container.
//=========================================================================
std::vector<LHCb::CaloCellID>& CaloTriggerBitsFromRaw::firedCells ( bool iWantPrs ) {

  LHCb::RawEvent*  rawEvt = get<LHCb::RawEvent>( rootOnTES() + LHCb::RawEventLocation::Default );

  const std::vector<LHCb::RawBank*>* banks = 0;  
  if(!m_packedIsDefault)banks= &rawEvt->banks( LHCb::RawBank::PrsTrig );
  if ( 0 == banks || 0 == banks->size() ) {
    banks = &rawEvt->banks( LHCb::RawBank::PrsPacked );
    debug() << "  Found " << banks->size() 
            << " banks of packed type at "
            << LHCb::RawBank::PrsPacked
            << " -  extracting Prs (1) or Spd (0) bit ? "  << iWantPrs << endreq;
    if ( 0 == banks->size() )warning() << " Requested packed Banks has not been found "<< endreq;
  }else{
    debug() << "  Found " << banks->size() 
            << " banks of short type at "
            << LHCb::RawBank::PrsTrig
            << " -  extracting Prs (1) or Spd (0) bit ? "  << iWantPrs << endreq;
  }
  
  
  m_prsCells.clear();
  m_spdCells.clear();
  
  std::vector<LHCb::RawBank*>::const_iterator itB = banks->begin();
  while ( banks->end() != itB ) {
    unsigned int* data = (*itB)->data();
    int size           = (*itB)->size()/4;  // size in byte
    int version        = (*itB)->version();
    int sourceID       = (*itB)->sourceID();
    int lastData       = 0;

    itB++;
    //=== Offline coding: a CellID, 8 SPD bits, 8 Prs bits
    if ( 1 == version ) {
      while ( 0 != size ) {
        int spdData = (*data >> 8 ) & 0xFF;
        int prsData = (*data) & 0xFF;
        debug() << "SpdData = "<< spdData << " - PrsData = " << prsData << endreq;
        int lastID  = (*data) >> 16;
        ++data;
        --size;
        for ( unsigned int kk = 0; 8 > kk; ++kk ) {
          LHCb::CaloCellID id( lastID+kk );
          if ( spdData & 1 ) m_spdCells.push_back( id );
          if ( prsData & 1 ) m_prsCells.push_back( id );
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
          if ( 0 != (item & 2) ) {
            LHCb::CaloCellID id ( spdId );   // SPD
            m_spdCells.push_back( id );
          }
          if ( 0 != (item & 1) ) {
            LHCb::CaloCellID id ( spdId + 0x4000 );   // Prs
            m_prsCells.push_back( id );
          }
        }
        ++data;
        --size;
      }
      //==== Codage for 1 MHz
    } else if ( 3 == version ) {
      int offset   = 0;
      int cardNum  = 0;
      int lenAdc   = 0;
      int lenTrig  = 0;
      while( 0 != size ) {
        int word = *data++;
        size--;
        lenTrig = word & 0x7F;
        lenAdc  = (word >> 7 ) & 0x7F;
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
        offset   = 32;
        
        while ( 0 < lenTrig ) {
          if ( 32 == offset ) {
            lastData = *data++;
            size--;
            offset = 0;
          }
          int num   = ( lastData >> offset ) & 0x3F;
          int isPrs = ( lastData >> (offset +6)) & 1;
          int isSpd = ( lastData >> (offset +7)) & 1;
          if ( msgLevel( MSG::DEBUG) ) {
            debug() << format( "  data %8x len %3d offset %2d num %2d Prs %1d SPD %1d",
                               lastData, lenTrig, offset, num, isPrs, isSpd )
                    << endreq;
          }
          offset += 8;
          lenTrig--;
          LHCb::CaloCellID id =  chanID[ num ];
          if ( 0 != isPrs ) m_prsCells.push_back( id );
          if ( 0 != isSpd ) {
            LHCb::CaloCellID spdId( 0, id.area(), id.row(), id.col() );
            m_spdCells.push_back( spdId );
          }
        }
        int nSkip = (lenAdc+1 ) / 2;  // Length in number of words
        size     -= nSkip;
        data     += nSkip;
      } //== DataSize
    } //== versions
  } //== while on banks

  if ( iWantPrs ) {
    return m_prsCells;
  } else {
    return m_spdCells;
  }
}
//=============================================================================
