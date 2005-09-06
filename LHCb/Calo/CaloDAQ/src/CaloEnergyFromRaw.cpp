// $Id: CaloEnergyFromRaw.cpp,v 1.4 2005-09-06 14:50:01 ocallot Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

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
  m_dataSize = 0;
  m_lastData = 0;
  m_lastID   = 0;
  m_version  = 0;
}
//=============================================================================
// Destructor
//=============================================================================
CaloEnergyFromRaw::~CaloEnergyFromRaw() {};


//=========================================================================
//  
//=========================================================================
StatusCode CaloEnergyFromRaw::initialize ( ) {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  std::string det = name().substr( 8, 4 );
  if ( "Ecal" == det ) {
    m_calo   = getDet<DeCalorimeter>( "/dd/Structure/LHCb/Ecal" );
    m_roTool = tool<CaloReadoutTool>( "CaloReadoutTool/EcalReadoutTool" );
  } else if ( "Hcal" == det ) {
    m_calo   = getDet<DeCalorimeter>( "/dd/Structure/LHCb/Hcal" );
    m_roTool = tool<CaloReadoutTool>( "CaloReadoutTool/HcalReadoutTool" );
  } else {
    m_calo = getDet<DeCalorimeter>( "/dd/Structure/LHCb/Prs" );
    m_roTool = tool<CaloReadoutTool>( "CaloReadoutTool/PrsReadoutTool" );
  }
  debug() << "Got detector element for " << det << endreq;
  return StatusCode::SUCCESS;
}
//=========================================================================
//  Store the scale for decoding, version 0.
//=========================================================================
void CaloEnergyFromRaw::setScaleAndShift ( double scale, double shift ) {
  m_scale    = scale;
  m_pedShift = shift;
}
//=========================================================================
//  Prepare the decoding of one event
//=========================================================================
void CaloEnergyFromRaw::prepare ( int type ) {
  RawEvent* rawEvt = get<RawEvent> ( RawEventLocation::Default );
  m_banks    = &rawEvt->banks( type );
  m_itB      = m_banks->begin();
  m_dataSize = 0;
  m_lastData = 0;
  m_version  = (*m_itB).version();
  if ( 1 < m_version ) {
    //== Prepare the results NOW, to be fast later.
    m_results.clear();
    while  ( m_banks->end() != m_itB ) {
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
      if ( 2 == m_version ) {
        //******************************************************************
        //**** 1 MHz coding proposal, Ecal and Hcal
        //******************************************************************
        while( 0 != m_dataSize ) {
          int word = *m_data++;
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
          m_chanID   = m_roTool->cellInFECard( feCards[m_cardNum] );
          m_pattern  = *m_data++;
          m_offset   = 0;
          m_bitNum   = 0;
          m_lastData = *m_data++;
          m_dataSize -= 3;
          m_cardNum++;
          for ( m_bitNum = 0; 32 > m_bitNum; m_bitNum++ ) {
            int adc;
            if ( 31 < m_offset ) {
              m_offset -= 32;
              m_lastData =  *m_data++;
              m_dataSize--;
            }
            if ( 0 == ( m_pattern & (1<<m_bitNum) ) ) {  //.. short coding
              adc = ( ( m_lastData >> m_offset ) & 0xF ) - 8;
              m_offset += 4;
            } else {
              adc =  ( ( m_lastData >> m_offset ) & 0xFFF ); 
              if ( 24 == m_offset ) adc &= 0xFF;
              if ( 28 == m_offset ) adc &= 0xF;  //== clean-up extra bits
              m_offset += 12;
              if ( 32 < m_offset ) {  //.. get the extra bits on next word
                m_lastData = *m_data++;
                m_dataSize--;
                m_offset -= 32;        
                int temp = (m_lastData << (12-m_offset) ) & 0xFFF;
                adc += temp;
              }
              adc -= 8;
            }
            CaloCellID id = m_chanID[ m_bitNum ];
            //== Keep only valid cells
            if ( 0 != id.index() ) {
              double energy = ( double(adc) - m_pedShift ) * m_calo->cellGain( id );
              std::pair<CaloCellID,double> temp( id, energy);
              m_results.push_back( temp );
            }
          }
          m_data     += m_lenTrig;
          m_dataSize -= m_lenTrig;
        }
      } else if ( 3 == m_version ) {
        //******************************************************************
        //**** 1 MHz coding proposal, Preshower + SPD
        //******************************************************************
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
              if ( msgLevel( MSG::DEBUG) ) {
                debug() << format( "  data %8x size %4d lenAdc%3d", m_lastData, m_dataSize, m_lenAdc )
                        << endreq;
              }
              m_offset = 0;
            }
            int adc = ( m_lastData >> m_offset ) & 0x3FF;
            int num = ( m_lastData >> (m_offset+10) ) & 0x3F;
            CaloCellID id = m_chanID[ num ];
            double energy = ( double(adc) - m_pedShift ) * m_calo->cellGain( id );
            std::pair<CaloCellID,double> temp( id, energy );
            m_results.push_back( temp );      
            m_lenAdc--;
            m_offset += 16;
          }
          if ( 16 == m_offset ) m_lenTrig -= 2;
          while ( 0 < m_lenTrig ) {        
            m_lastData = *m_data++;
            m_dataSize--;
            m_lenTrig -= 4;
            if ( msgLevel( MSG::DEBUG) ) {
              debug() << format( "  skip data %8x size %4d lenAdc%3d lenTrig%3d", 
                                 m_lastData, m_dataSize, m_lenAdc, m_lenTrig )
                      << endreq;
            }
          }
        } //== DataSize
      } //== versions
    } //== while on banks
    m_resultsIt = m_results.begin();
  }
}

//=========================================================================
//   Get the next entry, protect if no more entries
//=========================================================================
StatusCode CaloEnergyFromRaw::nextCell ( CaloCellID& id, double& energy ) {

  if ( 1 < m_version ) {
    if ( m_results.end() != m_resultsIt ) {
      id = (*m_resultsIt).first;
      energy = (*m_resultsIt).second;
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

      //== force m_dataSize to not go to zero too early
      if ( 2 == m_version || 3 == m_version ) m_dataSize += 1;
    } else {
      return StatusCode::FAILURE;
    }
  }

  if ( 0 == m_version ) {
    if ( 0 == m_lastData ) {
      m_lastID   = ((*m_data) >> 16) & 0xFFFF;
      m_lastData = (*m_data) & 0xFFFF;
      ++m_data;
      --m_dataSize;
    }
    energy = (*m_data) * m_scale;
    ++m_data;
    --m_lastData;
    --m_dataSize;
    id = CaloCellID( m_lastID++ );

  } else if ( 1 == m_version ) {
    m_lastID   = ((*m_data) >> 16) & 0xFFFF;
    m_lastData =  (*m_data) & 0xFFFF;
    if ( 32767 < m_lastData ) m_lastData |= 0xFFFF0000;  //= negative value
    id = CaloCellID( m_lastID );
    energy = ( double(m_lastData) - m_pedShift ) * m_calo->cellGain( id );
    ++m_data;
    --m_dataSize;
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
