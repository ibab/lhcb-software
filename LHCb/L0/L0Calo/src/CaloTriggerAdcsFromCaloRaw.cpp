// Include files 

// From Calo
#include "CaloDAQ/ICaloDataProvider.h"

// local
#include "CaloTriggerAdcsFromCaloRaw.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloTriggerAdcsFromCaloRaw
//
// 2014-03-04 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CaloTriggerAdcsFromCaloRaw )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloTriggerAdcsFromCaloRaw::CaloTriggerAdcsFromCaloRaw( const std::string& type,
                                                        const std::string& name,
                                                        const IInterface* parent )
  : GaudiTool( type, name , parent )
{
  declareInterface<ICaloTriggerAdcsFromRaw>(this);
  declareProperty( "DoubleScale" , m_doubleScale = false ) ;
  int index = name.find_first_of(".",0) +1 ;
  m_detectorName = name.substr( index, 4 );
  m_data.clear() ;
}
//=============================================================================
// Destructor
//=============================================================================
CaloTriggerAdcsFromCaloRaw::~CaloTriggerAdcsFromCaloRaw() {} 

//=============================================================================
// Initialize function
//=============================================================================
StatusCode CaloTriggerAdcsFromCaloRaw::initialize() {
  StatusCode sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) return sc ;

  if ( "Ecal" == m_detectorName ) {
    m_calo = getDet< DeCalorimeter >( DeCalorimeterLocation::Ecal ) ;
    m_adcs = 
      tool< ICaloDataProvider >( "CaloDataProvider" , 
                                 "EcalReadoutTool" , this ) ;
  } else if ( "Hcal" == m_detectorName ) {
    m_calo = getDet< DeCalorimeter >( DeCalorimeterLocation::Hcal ) ;
    m_adcs = 
      tool< ICaloDataProvider >( "CaloDataProvider" , 
                                 "HcalReadoutTool" , this ) ;
  } else {
    error() << "Unknown detector name " << m_detectorName << endmsg ;
    return StatusCode::FAILURE ;
  }

  long nCells = m_calo -> numberOfCells() ;
  m_data.reserve( nCells ) ;
  m_data.clear() ;
  return sc ;
}

//=============================================================================
const std::vector< LHCb::L0CaloAdc > & CaloTriggerAdcsFromCaloRaw::adcs( ) 
{
  m_data.clear() ;

  const CaloVector< LHCb::CaloAdc >& adcs = m_adcs -> adcs( -1 ) ;
  for ( CaloVector< LHCb::CaloAdc >::const_iterator itAdc = adcs.begin() ; 
        adcs.end() != itAdc ; ++itAdc ) {
    LHCb::CaloCellID id = (*itAdc).cellID() ;
    int adc = l0adcFromAdc( (*itAdc).adc() , id ) ;
    LHCb::L0CaloAdc tAdc( id , adc ) ;
    m_data.push_back( tAdc ) ;
  }
  
  return m_data ;
}

//=============================================================================
const std::vector< LHCb::L0CaloAdc > & CaloTriggerAdcsFromCaloRaw::adcs( int ) 
{
  return adcs() ;
}

//=============================================================================
const std::vector< LHCb::L0CaloAdc > & CaloTriggerAdcsFromCaloRaw::adcs( LHCb::RawBank * )
{
  return adcs( ) ;
}

const std::vector< LHCb::L0CaloAdc > & CaloTriggerAdcsFromCaloRaw::pinAdcs( )
{
  return adcs( ) ;
}

int CaloTriggerAdcsFromCaloRaw::l0adcFromAdc( const int adc , 
                                              const LHCb::CaloCellID & id  ) const {
  if ( adc < 0 ) return 0 ;

  unsigned long calibCte = m_calo -> cellParam( id ).l0Constant() ;

  if ( m_doubleScale ) calibCte = calibCte / 2 ;

  int theAdc = adc ;

  if ( calibCte > 255 ) { 
    theAdc = 4 * adc ;
    if ( ( adc & 0xC00 ) != 0 ) 
      theAdc = 0xFFF ;
    calibCte = calibCte / 4 ;
  }
  
  unsigned long v1, v2, v3, v4, v5, v6, v7, v8 ;
  v1 = theAdc ;
  v2 = ( theAdc << 1 ) ;
  v3 = ( theAdc << 2 ) ;
  v4 = ( theAdc << 3 ) ;
  v5 = ( theAdc << 4 ) ;
  v6 = ( theAdc << 5 ) ;
  v7 = ( theAdc << 6 ) ;
  v8 = ( theAdc << 7 ) ;
  
  unsigned long s1, s2, s3, s4, s5, s6, s7, s8 ;
  s1 = calibCte & 0x1 ;
  s2 = ( calibCte >> 1 ) & 0x1 ;
  s3 = ( calibCte >> 2 ) & 0x1 ;
  s4 = ( calibCte >> 3 ) & 0x1 ;
  s5 = ( calibCte >> 4 ) & 0x1 ;
  s6 = ( calibCte >> 5 ) & 0x1 ;
  s7 = ( calibCte >> 6 ) & 0x1 ;
  s8 = ( calibCte >> 7 ) & 0x1 ;

  unsigned long R0, R1, R2, R3 ;
  R0 = ( ( v7 & 0x3FF00 ) * s7 + ( v8 & 0x7FF00 ) * s8 ) & 0xFFF00 ;
  R1 = ( ( v5 & 0xFFC0  ) * s5 + ( v6 & 0x1FFC0 ) * s6 ) & 0xFFF00 ;
  R2 = ( ( v3 & 0x3FF0  ) * s3 + ( v4 & 0x7FF0  ) * s4 ) & 0xFFF00 ;
  R3 = ( ( v1 & 0xFFC   ) * s1 + ( v2 & 0x1FFC  ) * s2 ) & 0xFFF00 ;
  
  unsigned long result = R0 + R1 + R2 + R3 ;
  unsigned long trig ;
  trig = ( ( ( result & 0x3FC00 ) >> 10 ) & 0xFF ) ;
  if ( ( 0 != ( result & 0x200 ) ) && ( 0xFF != trig ) ) trig++ ;
  if ( 0 != ( result & 0xC0000 ) ) trig = 0xFF ;
  
  return trig ;
}
