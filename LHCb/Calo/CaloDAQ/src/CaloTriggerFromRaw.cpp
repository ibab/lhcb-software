// $Id: CaloTriggerFromRaw.cpp,v 1.2 2005-05-09 06:38:53 ocallot Exp $
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
//  Prepare for a new event
//=========================================================================
void CaloTriggerFromRaw::prepare ( int type ) {
  RawEvent* rawEvt = get<RawEvent>( RawEventLocation::Default );
  m_banks    = &rawEvt->banks( type );
  m_itB      = m_banks->begin();
  m_dataSize = 0;
  m_lastData = 0;
}

//=========================================================================
//  Get the next entry, protect if no more entries
//=========================================================================
StatusCode CaloTriggerFromRaw::nextCell ( CaloCellID& id, int& adc ) {
  if ( 0 == m_banks ) return StatusCode::FAILURE;
  
  //== Ended last bank ? Try to get a new, non empty one.
  while ( 0 == m_dataSize ) {
    if ( m_banks->end() != m_itB ) {
      m_data     = (*m_itB).data();
      m_dataSize = (*m_itB).dataSize();
      m_lastData = 0;
      m_itB++;
    } else {
      return StatusCode::FAILURE;
    }
  }

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
  return StatusCode::SUCCESS;
}
//=============================================================================
