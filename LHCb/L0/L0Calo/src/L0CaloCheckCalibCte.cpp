// Include files 

// local
#include "L0CaloCheckCalibCte.h"

// From CaloDet
#include "CaloDet/DeCalorimeter.h"
#include "CaloDAQ/ICaloDataProvider.h"
#include "CaloDAQ/ICaloL0DataProvider.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0CaloCheckCalibCte
//
// 2009-11-21 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0CaloCheckCalibCte )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0CaloCheckCalibCte::L0CaloCheckCalibCte( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : Calo2Dview( name , pSvcLocator )
  , m_calo(NULL)
  , m_daq(NULL)
  , m_l0daq(NULL)
{
  declareProperty( "Detector"      , m_detectorName = "Ecal" ) ;
  declareProperty( "ReadoutTool"   , m_readoutTool = "CaloDataProvider" ) ;
  declareProperty( "L0ReadoutTool" , m_l0readoutTool = "CaloL0DataProvider" ) ;
  
}

//=============================================================================
// Destructor
//=============================================================================
L0CaloCheckCalibCte::~L0CaloCheckCalibCte() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0CaloCheckCalibCte::initialize() {
  StatusCode sc = Calo2Dview::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc; 

  if ( "Ecal" == m_detectorName ) 
    m_calo = getDet< DeCalorimeter >( DeCalorimeterLocation::Ecal ) ;
  else if ( "Hcal" == m_detectorName ) 
    m_calo = getDet< DeCalorimeter >( DeCalorimeterLocation::Hcal ) ;
  else {
    error() << "Unknown detector name " << m_detectorName << endmsg ;
    return StatusCode::FAILURE ;
  }

  m_daq = tool< ICaloDataProvider >( m_readoutTool , 
                                     m_detectorName + "ReadoutTool" , this ) ;
  m_l0daq = tool< ICaloL0DataProvider >( m_l0readoutTool , 
                                         m_detectorName + "L0ReadoutTool" , 
                                         this ) ;

  // Book 2D histograms
  bookCalo2D( "L0CteComp" + m_detectorName , 
              "L0Constant Mismatch " + m_detectorName , m_detectorName ) ;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0CaloCheckCalibCte::execute() {

  if ( ! m_l0daq -> ok() ) {
    Warning( "No L0Bank" , StatusCode::SUCCESS ).ignore() ;
    return StatusCode::SUCCESS ;
  }

  std::map< LHCb::CaloCellID , int > adcMap ;
  std::map< LHCb::CaloCellID , int >::iterator itAdcMap ;
  
  CaloVector< LHCb::CaloAdc >::const_iterator iadc ;

  const CaloVector< LHCb::CaloAdc >& caloAdcs = (m_daq -> adcs( -1 ) ) ;
  for ( iadc = caloAdcs.begin() ; iadc != caloAdcs.end() ; ++iadc ) {
    LHCb::CaloCellID cellID = (*iadc).cellID() ;
    if ( ! ( cellID.isPin() ) ) adcMap[ cellID ] = (*iadc).adc() ;
  }

  // now check the L0Calo ADC
  for ( itAdcMap = adcMap.begin() ; adcMap.end() != itAdcMap ; ++itAdcMap ) {
    // compute expected L0ADC 
    if ( l0adcFromAdc( (*itAdcMap).second , (*itAdcMap).first ) != 
         m_l0daq -> l0Adc( (*itAdcMap).first ) ) { 
      LHCb::CaloCellID id = (*itAdcMap).first ;
      if( msgLevel(MSG::DEBUG) ) {
        debug() << "Mismatch ADC / L0ADC " 
                << (*itAdcMap).first 
                << " " << m_l0daq -> l0Adc( (*itAdcMap).first )
                << " " << (*itAdcMap).second 
                << " " << l0adcFromAdc((*itAdcMap).second , (*itAdcMap).first )
                << endmsg ;
        debug() << "Calib constant should be = " 
                << m_calo -> cellParam( id ).l0Constant()
                << " Row = " << m_calo -> cellParam( id ).cardRow()
                << " Column = " << m_calo -> cellParam( id ).cardColumn()
                << " Crate = " 
                << m_calo -> cardCrate( m_calo -> cellParam( id ).cardNumber() ) 
                << " Slot = " 
                << m_calo -> cardSlot( m_calo -> cellParam( id ).cardNumber() ) 
                << endmsg ;
      }
      
      // Plot the location of the error
      fillCalo2D( "L0CteComp" + m_detectorName ,(*itAdcMap).first , 1. ,
                  "L0Constant Mismatch " + m_detectorName ) ;
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// L0ADC from ADC
//=============================================================================
int L0CaloCheckCalibCte::l0adcFromAdc( const int adc , 
                                       const LHCb::CaloCellID & id) const {
  if ( adc < 0 ) return 0 ;

  unsigned long calibCte = m_calo -> cellParam( id ).l0Constant() ;

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
