// $Id: CaloTriggerFromRaw.cpp,v 1.3 2005-09-06 14:50:01 ocallot Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "CaloTriggerFromRaw.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloTriggerFromRaw
//
// 2005-01-05 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<CaloTriggerFromRaw>          s_factory ;
const        IToolFactory& CaloTriggerFromRawFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloTriggerFromRaw::CaloTriggerFromRaw( const std::string& type,
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
CaloTriggerFromRaw::~CaloTriggerFromRaw() {}; 

//=========================================================================
//  Initialise
//=========================================================================
StatusCode CaloTriggerFromRaw::initialize ( ) {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTool

  debug() << "==> Initialize" << endmsg;

  m_ecalRoTool  = tool<CaloReadoutTool>( "CaloReadoutTool/EcalReadoutTool" );
  m_hcalRoTool  = tool<CaloReadoutTool>( "CaloReadoutTool/HcalReadoutTool" );

  return StatusCode::SUCCESS;
}

//=========================================================================
//  Prepare for a new event
//=========================================================================
void CaloTriggerFromRaw::prepare ( int type ) {
  RawEvent* rawEvt = get<RawEvent>( RawEventLocation::Default );
  m_banks    = &rawEvt->banks( type );
  if ( 0 == m_banks->size() ) {
    debug() << "No bank of the requested type. Try the 1MHz type" << endreq;
    if ( RawBuffer::EcalTrig == type ) {
      m_banks = &rawEvt->banks( RawBuffer::EcalE );
      m_roTool = m_ecalRoTool;
      debug() << "  Found " << m_banks->size() << " Ecal banks." << endreq;
    } else if (  RawBuffer::HcalTrig == type ) {
      m_banks = &rawEvt->banks( RawBuffer::HcalE );
      m_roTool = m_hcalRoTool;
      debug() << "  Found " << m_banks->size() << " Hcal banks." << endreq;
    }    
  }
  m_itB      = m_banks->begin();
  m_dataSize = 0;
  m_lastData = 0;
  m_version  = (*m_itB).version();
  m_results.clear();

  //==== Codage for 1 MHz
  if ( 2 == m_version ) {
  //== Ended last bank ? Try to get a new, non empty one.
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
      debug() << "Starting new bank, version " << m_version << " size " << m_dataSize << endreq;
      while ( 0 < m_dataSize ) {
        int word = *m_data++;
        m_dataSize--;
        m_lenTrig = word & 0xF;
        m_lenAdc  = (word & 0xF0) >> 4;
        int code  = (word & 0x1FF00) >> 8;
        std::vector<int> feCards = m_roTool->feCardsInTell1( m_sourceID );
        if ( code != m_roTool->cardCode( feCards[m_cardNum] ) ) {
          info() << format( "** Error card code tell1 %2d card %2d # %4d code found %4x expected %4x",
                            m_sourceID, m_cardNum, feCards[m_cardNum], code,  
                            m_roTool->cardCode( feCards[m_cardNum] ) )
                 << endreq;
        }
        debug() << " Starting new card code " << code << " lenAdc " << m_lenAdc 
                << " lenTrig " << m_lenTrig << " dataSize " << m_dataSize
                << endreq;
        
        m_chanID    = m_roTool->cellInFECard( feCards[m_cardNum] );
        m_cardNum++;
        m_data     += m_lenAdc;
        m_dataSize -= m_lenAdc;
        if ( 0 < m_lenTrig ) {
          m_pattern   = *m_data++;
          m_offset    = 0;
          debug() << format( " pattern %8x bit %2d offset %2d lenTrig %2d", 
                             m_pattern, m_bitNum, m_offset, m_lenTrig ) << endreq;
          m_lastData  = *m_data++;
          m_dataSize -= 2;
          for ( m_bitNum = 0 ; 32 > m_bitNum ; m_bitNum++ ) {
            if ( 0 != (m_pattern & (1<<m_bitNum)) ) {
              if ( 31 < m_offset ) {
                m_offset   = 0;
                m_lastData =  *m_data++;
                m_dataSize--;
              }
              CaloCellID id = m_chanID[ m_bitNum ];
              if ( 0 != id.index() ) {
                int adc = ( m_lastData >> m_offset ) & 0xFF;
                std::pair<CaloCellID,int> temp( id, adc );
                m_results.push_back( temp );
              }
              m_offset += 8;
            }
          }
        }
      } // another card ?
    } // another bank ?
  } // 1 MHz coding ?
  m_resultsIt = m_results.begin();
}

//=========================================================================
//  Get the next entry, protect if no more entries
//=========================================================================
StatusCode CaloTriggerFromRaw::nextCell ( CaloCellID& id, int& adc ) {
  if ( 1 < m_version ) {
    if ( m_results.end() != m_resultsIt ) {
      id  = (*m_resultsIt).first;
      adc = (*m_resultsIt).second;
      m_resultsIt++;
      return StatusCode::SUCCESS;
    }
    return StatusCode::FAILURE;
  }

  if ( 0 == m_banks ) return StatusCode::FAILURE;
  
  //== Ended last bank ? Try to get a new, non empty one.
  while ( 0 == m_dataSize ) {
    if ( m_banks->end() != m_itB ) {
      m_data     = (*m_itB).data();
      m_dataSize = (*m_itB).dataSize();
      m_version  = (*m_itB).version();
      m_sourceID = (*m_itB).bankSourceID();
      m_lastData = 0;
      m_itB++;
      m_offset   = 0;
      m_bitNum   = -1;
      m_cardNum  = 0;
      m_lenAdc   = 0;
      m_lenTrig  = 0;
      debug() << "Starting new bank, version " << m_version << " size " << m_dataSize << endreq;
    } else {
      return StatusCode::FAILURE;
    }
  }

  if ( 2 > m_version ) {
    //== Some data left in the last word ? use it.
    if ( 0 != m_lastData ) {
      adc = m_lastData;
      m_lastData = 0;
      m_lastID++;
    } else {
      m_lastID   = (*m_data) >> 16;
      m_lastData = (*m_data)    & 0xFF;
      adc        = (*m_data)>>8 & 0xFF;
      ++m_data;
    }
    if ( 0 == m_lastData ) m_dataSize--;
    id = CaloCellID( m_lastID );

  }
  return StatusCode::SUCCESS;
}
//=============================================================================
