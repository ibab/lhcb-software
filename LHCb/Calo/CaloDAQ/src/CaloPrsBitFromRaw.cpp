// $Id: CaloPrsBitFromRaw.cpp,v 1.3 2005-09-06 14:50:01 ocallot Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "CaloPrsBitFromRaw.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloPrsBitFromRaw
//
// 2005-01-06 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<CaloPrsBitFromRaw>          s_factory ;
const        IToolFactory& CaloPrsBitFromRawFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloPrsBitFromRaw::CaloPrsBitFromRaw( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ICaloTriggerFromRaw>(this);
  m_dataSize = 0;
  m_lastData = 0;
  m_lastID   = 0;
}
//=============================================================================
// Destructor
//=============================================================================
CaloPrsBitFromRaw::~CaloPrsBitFromRaw() {}; 


//=========================================================================
//  Initialisation
//=========================================================================
StatusCode CaloPrsBitFromRaw::initialize ( ) {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTool

  debug() << "==> Initialize" << endmsg;

  m_roTool   = tool<CaloReadoutTool>( "CaloReadoutTool/PrsReadoutTool" );

  return StatusCode::SUCCESS;

}
//=========================================================================
//  Prepare for a new event
//=========================================================================
void CaloPrsBitFromRaw::prepare ( int type ) {
  RawEvent*  rawEvt = get<RawEvent>( RawEventLocation::Default );
  m_banks    = &rawEvt->banks( type );
  m_itB      = m_banks->begin() - 1;
  if ( 0 == m_banks->size() ) {
    debug() << "No bank of the requested type. Try the 1MHz type" << endreq;
    m_banks = &rawEvt->banks( RawBuffer::PrsE );
    m_itB   = m_banks->begin();
    debug() << "  Found " << m_banks->size() << " Prs banks." << endreq;
  }
  m_dataSize = 0;
  m_lastData = 0;
  m_version  = (*m_itB).version();
  m_results.clear();

  //==== Codage for 1 MHz
  if ( 3 == m_version ) {
    while ( m_banks->end() != m_itB ) {
      m_data     = (*m_itB).data();
      m_dataSize = (*m_itB).dataSize();
      m_version  = (*m_itB).version();
      m_sourceID = (*m_itB).bankSourceID();
      m_lastData = 0;
      m_itB++;
      m_offset   = 0;
      m_cardNum  = 0;
      m_lenAdc   = 0;
      m_lenTrig  = 0;
      while( 0 != m_dataSize ) {
        int word = *m_data++;
        m_dataSize--;
        m_lenTrig = word & 0x7F;
        m_lenAdc  = (word >> 7 ) & 0x7F;
        if ( msgLevel( MSG::DEBUG) ) {
          debug() << format( "  Header data %8x size %4d lenAdc%3d lenTrig%3d", 
                             word, m_dataSize, m_lenAdc, m_lenTrig )
                  << endreq;
        }      
        int code  = (word >>14 ) & 0x1FF;
        std::vector<int> feCards = m_roTool->feCardsInTell1( m_sourceID );
        if ( code != m_roTool->cardCode( feCards[m_cardNum] ) ) {
          info() << format( "** Error card code tell1 %2d card %2d # %4d code found %4x expected %4x",
                            m_sourceID, m_cardNum, feCards[m_cardNum], code,  
                            m_roTool->cardCode( feCards[m_cardNum] ) )
                 << endreq;
        }
        if ( msgLevel( MSG::DEBUG) ) {
          debug() << format( "Processing Tell1 %2d card %2d#%3d code %3d lenAdc%3d lenTrig%3d",
                             m_sourceID, m_cardNum, feCards[m_cardNum], code, m_lenAdc, m_lenTrig )
                  << endreq;
        }      
        m_chanID   = m_roTool->cellInFECard( feCards[m_cardNum] );
        m_cardNum++;
        m_offset   = 32;
        
        while ( 0 < m_lenAdc ) {
          if ( 32 == m_offset ) {
            m_lastData =  *m_data++;
            m_dataSize--;
            m_offset = 0;
          }
          m_lenAdc--;
          m_offset += 16;
        }
        while ( 0 < m_lenTrig ) {
          if ( 32 == m_offset ) {
            m_lastData = *m_data++;
            m_dataSize--;
            m_offset = 0;
          }
          int num = ( m_lastData >> m_offset ) & 0x3F;
          int isSet = (  m_lastData >> (m_offset +6)) & 1;
          if ( msgLevel( MSG::DEBUG) ) {
            debug() << format( "  data %8x len %3d offset %2d num %2d prs %1d",
                               m_lastData, m_lenTrig, m_offset, num, isSet )
                    << endreq;
          }
          m_offset += 8;
          m_lenTrig--;
          if ( 0 != isSet ) {
            CaloCellID id =  m_chanID[ num ];
            CaloCellID spdId( 0, id.area(), id.row(), id.col() );
            m_results.push_back( spdId );
          }
        }      
      } //== DataSize
    } //== while on banks
  } //== versions
  m_resultsIt = m_results.begin();
}

//=========================================================================
//  Decode the Prs word (lower 8 bits)
//=========================================================================
StatusCode CaloPrsBitFromRaw::nextCell ( CaloCellID& id, int& adc ) {
  if ( 1 < m_version ) {
    if ( m_results.end() != m_resultsIt ) {
      id  = *m_resultsIt;
      adc = 1;
      m_resultsIt++;
      return StatusCode::SUCCESS;
    }
    return StatusCode::FAILURE;
  }

  if ( 0 == m_banks ) return StatusCode::FAILURE;

  //== Ended last bank ? Try to get a new, non empty one.
  while ( 0 == m_lastData && 0 == m_dataSize ) {
    if ( m_banks->end() > ++m_itB ) {
      m_data     = (*m_itB).data();
      m_dataSize = (*m_itB).dataSize();
      m_lastData = 0;
    } else {
      return StatusCode::FAILURE;
    }
  }

  //== No data left -> get next word, re-call if bank is finished.

  if ( 0 == m_lastData ) {
    while ( 0 == m_lastData && 0 != m_dataSize ) {
      m_lastID   = (*m_data) >> 16;
      m_lastData = (*m_data) & 0xFF;
      ++m_data;
      --m_dataSize;
    }
    //== If end of bank, re-call itself to get the first data from a new bank
    if ( 0 == m_lastData ) return nextCell( id, adc );
  }

  while ( 0 == (m_lastData&1) ) {
    m_lastData = m_lastData >> 1;
    m_lastID++;
  }
  adc = 1;
  id = CaloCellID( m_lastID++ );
  m_lastData = m_lastData >> 1;
  return StatusCode::SUCCESS;
}
//=============================================================================
