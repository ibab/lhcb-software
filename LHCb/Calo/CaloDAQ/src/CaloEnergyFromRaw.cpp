// $Id: CaloEnergyFromRaw.cpp,v 1.1.1.1 2005-01-11 07:51:47 ocallot Exp $
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
  m_itB      = m_banks.end();
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
  RawBuffer* rawBuf = get<RawBuffer>( RawBufferLocation::Default );
  RawEvent rawEvt( *rawBuf );
  m_banks    = rawEvt.banks( type );
  m_itB      = m_banks.begin();
  m_dataSize = 0;
  m_lastData = 0;
  m_version  = 0;
  std::string det = name().substr( 8, 4 );
  if ( "Ecal" == det ) {
    m_calo = getDet<DeCalorimeter>( "/dd/Structure/LHCb/Ecal" );
  } else if ( "Hcal" == det ) {
    m_calo = getDet<DeCalorimeter>( "/dd/Structure/LHCb/Hcal" );
  } else {
    m_calo = getDet<DeCalorimeter>( "/dd/Structure/LHCb/Prs" );
  }
}

//=========================================================================
//   Get the next entry, protect if no more entries
//=========================================================================
StatusCode CaloEnergyFromRaw::nextCell ( CaloCellID& id, double& energy ) {

  //== Ended last bank ? Try to get a new, non empty one.
  while ( 0 == m_dataSize ) {
    if ( m_banks.end() != m_itB ) {
      m_data     = (*m_itB).data();
      m_dataSize = (*m_itB).dataSize();
      m_version  = (*m_itB).version();
      m_lastData = 0;
      m_itB++;
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
    return StatusCode::SUCCESS;
  } else if ( 1 == m_version ) {
    m_lastID   = ((*m_data) >> 16) & 0xFFFF;
    m_lastData =  (*m_data) & 0xFFFF;
    if ( 32767 < m_lastData ) m_lastData |= 0xFFFF0000;  //= negative value
    id = CaloCellID( m_lastID );
    energy = ( double(m_lastData) - m_pedShift ) * m_calo->cellGain( id );
    ++m_data;
    --m_dataSize;
    return StatusCode::SUCCESS;
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
