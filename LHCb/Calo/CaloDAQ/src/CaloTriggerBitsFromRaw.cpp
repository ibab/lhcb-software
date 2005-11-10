// $Id: CaloTriggerBitsFromRaw.cpp,v 1.1 2005-11-10 16:43:22 ocallot Exp $
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
static const  ToolFactory<CaloTriggerBitsFromRaw>          s_factory ;
const        IToolFactory& CaloTriggerBitsFromRawFactory = s_factory ;


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
  m_runNum = -1;
  m_evtNum = -1;
}
//=============================================================================
// Destructor
//=============================================================================
CaloTriggerBitsFromRaw::~CaloTriggerBitsFromRaw() {
  std::cout << "Destroy CaloTriggerBitsFromRaw" << std::endl;
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
//  Test if the event is a new one.
//=========================================================================
bool CaloTriggerBitsFromRaw::eventHasChanged ( ) {
  RawEvent* rawEvt = get<RawEvent> ( RawEventLocation::Default );
  const std::vector<RawBank>& data = rawEvt->banks( RawBuffer::DAQ );
  raw_int* evhData = (*(data.begin())).data();
  long evtNum = (*evhData++);
  long runNum = (*evhData++);
  bool changed = (m_evtNum != evtNum || m_runNum != runNum );
  m_evtNum = evtNum;
  m_runNum = runNum;
  return changed;
}

//=========================================================================
//  Unpack a new event if needed, and return the appropriate container.
//=========================================================================
std::vector<CaloCellID>& CaloTriggerBitsFromRaw::firedCells ( bool isPrs ) {
  if ( eventHasChanged() ) {
    RawEvent*  rawEvt = get<RawEvent>( RawEventLocation::Default );
    const std::vector<RawBank>* banks = &rawEvt->banks( RawBuffer::PrsPacked );
    if ( 0 == banks->size() ) {
      banks = &rawEvt->banks( RawBuffer::PrsE );
      debug() << "  Found " << banks->size() << " Prs banks." << endreq;
    }
    m_prsCells.clear();
    m_spdCells.clear();

    std::vector<RawBank>::const_iterator itB = banks->begin();
    while ( banks->end() != itB ) {
      raw_int* data = (*itB).data();
      int dataSize  = (*itB).dataSize();
      int version   = (*itB).version();
      int sourceID  = (*itB).bankSourceID();
      int lastData  = 0;
      itB++;
      //=== Offline coding: a CellID, 8 SPD bits, 8 Prs bits
      if ( 1 == version ) {
        while ( 0 != dataSize ) {
          int spdData = (*data >> 8 ) & 0xFF;
          int prsData = (*data) & 0xFF;
          int lastID  = (*data) >> 16;
          ++data;
          --dataSize;
          for ( unsigned int kk = 0; 8 > kk; ++kk ) {
            CaloCellID id( lastID+kk );
            if ( spdData & 1 ) m_spdCells.push_back( id );
            if ( prsData & 1 ) m_prsCells.push_back( id );
            spdData = spdData >> 1;
            prsData = prsData >> 1;
          }
        }
      //=== Compact coding: a CellID, and its Prs/SPD bits
      } else if ( 2 == version ) {
        while ( 0 != dataSize ) {
          int word = *data;
          while ( 0 != word ) {
            int item = word & 0xFFFF;
            word = ( word >> 16 ) & 0xFFFF;
            if ( 0 != item & 0x8000 ) {
              CaloCellID id ( item&0x3FFF );   // SPD
              m_spdCells.push_back( id );
            }
            if ( 0 != item & 04000 ) {
             CaloCellID id ( item&0x3FFF + 0x4000 );   // Prs
             m_prsCells.push_back( id );
            }
          }
          ++data;
          --dataSize;
        }
        //==== Codage for 1 MHz
      } else if ( 3 == version ) {
        int offset   = 0;
        int cardNum  = 0;
        int lenAdc   = 0;
        int lenTrig  = 0;
        while( 0 != dataSize ) {
          int word = *data++;
          dataSize--;
          lenTrig = word & 0x7F;
          lenAdc  = (word >> 7 ) & 0x7F;
          if ( msgLevel( MSG::DEBUG) ) {
            debug() << format( "  Header data %8x size %4d lenAdc%3d lenTrig%3d",
                               word, dataSize, lenAdc, lenTrig )
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
          std::vector<CaloCellID> chanID   = m_roTool->cellInFECard( feCards[cardNum] );
          cardNum++;
          offset   = 32;

          while ( 0 < lenTrig ) {
            if ( 32 == offset ) {
              lastData = *data++;
              dataSize--;
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
            CaloCellID id =  chanID[ num ];
            if ( 0 != isPrs ) m_prsCells.push_back( id );
            if ( 0 != isSpd ) {
              CaloCellID spdId( 0, id.area(), id.row(), id.col() );
              m_spdCells.push_back( spdId );
            }
          }
          int nSkip = (lenAdc+1 ) / 2;  // Length in number of words
          dataSize -= nSkip;
          data     += nSkip;
        } //== DataSize
      } //== versions
    } //== while on banks
  } //== re-compute this event

  if ( isPrs ) {
    return m_prsCells;
  } else {
    return m_spdCells;
  }
}
//=============================================================================
