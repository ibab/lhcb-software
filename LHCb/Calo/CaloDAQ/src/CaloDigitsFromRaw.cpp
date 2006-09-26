// $Id: CaloDigitsFromRaw.cpp,v 1.7 2006-09-26 12:42:02 odescham Exp $
// Include files 

#include "Kernel/SystemOfUnits.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "CaloDigitsFromRaw.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloDigitsFromRaw
//
// 2003-11-18 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<CaloDigitsFromRaw>          s_factory ;
const        IAlgFactory& CaloDigitsFromRawFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloDigitsFromRaw::CaloDigitsFromRaw( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) 
{  
  m_calo = CaloCellCode::CaloNumFromName( name ) ;
  if ( m_calo < 0 || m_calo >= (int) CaloCellCode::CaloNums ) 
  { m_calo = CaloCellCode::CaloNumFromName( "Ecal" ) ; }
  
  declareProperty( "Extension"  ,  m_extension = "" );
  declareProperty( "OutputType" , m_outputType = "Digits"  ) ;
  m_digitOnTES =false  ;
  m_adcOnTES =false  ;
}

//=============================================================================
// Destructor
//=============================================================================
CaloDigitsFromRaw::~CaloDigitsFromRaw() {}; 


//=========================================================================
//  Initialization 
//=========================================================================
StatusCode CaloDigitsFromRaw::initialize ( ) {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  //
  if ( 0 == m_calo ) {

    m_spdTool = tool<ICaloTriggerBitsFromRaw>( "CaloTriggerBitsFromRaw",  name() + "Tool",this );

  } else {
    m_energyTool = tool<ICaloEnergyFromRaw>( "CaloEnergyFromRaw",  name() + "Tool",this );

    // 
    if( m_outputType == "Digits" || 
        m_outputType == "CaloDigits" || 
        m_outputType == "Both")m_digitOnTES = true;
    if( m_outputType == "ADCs" ||
        m_outputType == "CaloAdcs" || 
        m_outputType == "Both")m_adcOnTES = true;
    if( !m_adcOnTES && !m_digitOnTES ){
      error()<< "CaloDigitsFromRaw configured to produce ** NO ** output" << endreq;
      return StatusCode::FAILURE;
    }
    if( m_digitOnTES )debug() <<  "CaloDigitsFromRaw will produce CaloDigits on TES" 
                              << endreq;
    if( m_adcOnTES )debug() <<  "CaloDigitsFromRaw will produce CaloAdcs on TES" 
                            << endreq;
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloDigitsFromRaw::execute() {

  debug() << "==> Execute" << endreq;

  if       ( 0 == m_calo ) {
    convertSpd ( LHCb::CaloDigitLocation::Spd + m_extension, 3.2 * MeV );
  } else if  ( 1 == m_calo ) {
    if(m_digitOnTES)convertCaloEnergies ( LHCb::CaloDigitLocation::Prs + m_extension );
    if(m_adcOnTES  )convertCaloEnergies ( LHCb::CaloAdcLocation::Prs   + m_extension );
  } else if  ( 2 == m_calo ) {
    if(m_digitOnTES)convertCaloEnergies ( LHCb::CaloDigitLocation::Ecal + m_extension );
    if(m_adcOnTES  )convertCaloEnergies ( LHCb::CaloAdcLocation::Ecal   + m_extension );
  } else if  ( 3 == m_calo ) {
    if(m_digitOnTES)convertCaloEnergies ( LHCb::CaloDigitLocation::Hcal + m_extension );
    if(m_adcOnTES  )convertCaloEnergies ( LHCb::CaloAdcLocation::Hcal   + m_extension );
  }
  return StatusCode::SUCCESS;
};

//=========================================================================
//  Convert the SPD trigger bits to CaloDigits
//=========================================================================
void CaloDigitsFromRaw::convertSpd ( std::string containerName,
                                     double energyScale ) {

  LHCb::CaloDigits* digits = new LHCb::CaloDigits();
  put( digits, containerName );

  std::vector<LHCb::CaloCellID>& spdCells = m_spdTool->firedCells( false );

  LHCb::CaloCellID id;
  for ( std::vector<LHCb::CaloCellID>::const_iterator itD = spdCells.begin();
        spdCells.end() != itD; ++itD ) {
    LHCb::CaloDigit* dig = new LHCb::CaloDigit( *itD, energyScale );
    digits->insert( dig );
  }
  
  std::stable_sort ( digits->begin(), digits->end(), 
                     CaloDigitsFromRaw::IncreasingByCellID() );

  debug() << "SPD size : " << digits->size() << endreq;
}

//=========================================================================
//  Converts the standard calorimeter adc-energy
//=========================================================================
void CaloDigitsFromRaw::convertCaloEnergies ( std::string containerName ) {


  if(m_digitOnTES){

    LHCb::CaloDigits* digits = new LHCb::CaloDigits();
    put( digits, containerName );
    std::vector<LHCb::CaloDigit>& allDigits = m_energyTool->digits( );

    for ( std::vector<LHCb::CaloDigit>::const_iterator itD = allDigits.begin();
          allDigits.end() != itD; ++itD ) {
      LHCb::CaloDigit* dig = (*itD).clone();
      digits->insert( dig );
      debug() << "ID " << dig->cellID() << " energy " << dig->e() << endreq;
    }
    std::stable_sort ( digits->begin(), digits->end(), 
                       CaloDigitsFromRaw::IncreasingByCellID() );
    debug() << containerName << " CaloDigit container size " << digits->size() << endreq;

  }
  
  if(m_adcOnTES){
  
    LHCb::CaloAdcs* adcs = new LHCb::CaloAdcs();
    put( adcs , containerName );
    std::vector<LHCb::CaloAdc>& allAdcs = m_energyTool->adcs( );
    
    for ( std::vector<LHCb::CaloAdc>::const_iterator itA = allAdcs.begin();
          allAdcs.end() != itA; ++itA ) {
      LHCb::CaloAdc* adc = new LHCb::CaloAdc( (*itA).cellID(), (*itA).adc() ); // 'clone'
      adcs->insert( adc );
      debug() << "ID " << adc->cellID() << " ADC value " << adc->adc() << endreq;
    }
    debug() << containerName << " CaloAdc container size " << adcs->size() << endreq;
  }
  
    
  

}
//=============================================================================
