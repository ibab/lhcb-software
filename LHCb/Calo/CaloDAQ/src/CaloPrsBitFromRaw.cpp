// $Id: CaloPrsBitFromRaw.cpp,v 1.1.1.1 2005-01-11 07:51:47 ocallot Exp $
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
  m_itB      = m_banks.end();
  m_dataSize = 0;
  m_lastData = 0;
  m_lastID   = 0;
}
//=============================================================================
// Destructor
//=============================================================================
CaloPrsBitFromRaw::~CaloPrsBitFromRaw() {}; 

//=========================================================================
//  Prepare for a new event
//=========================================================================
void CaloPrsBitFromRaw::prepare ( int type ) {
  RawBuffer* rawBuf = get<RawBuffer>( RawBufferLocation::Default );
  RawEvent rawEvt( *rawBuf );
  m_banks    = rawEvt.banks( type );
  m_itB      = m_banks.begin() - 1;
  m_dataSize = 0;
  m_lastData = 0;
}

//=========================================================================
//  Decode the Prs word (lower 8 bits)
//=========================================================================
StatusCode CaloPrsBitFromRaw::nextCell ( CaloCellID& id, int& adc ) {
  //== Ended last bank ? Try to get a new, non empty one.
  while ( 0 == m_lastData && 0 == m_dataSize ) {
    if ( m_banks.end() > ++m_itB ) {
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
