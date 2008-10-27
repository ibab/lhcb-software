// $Id: CaloDigitsFromRaw.cpp,v 1.13 2008-10-27 18:14:26 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// local
#include "CaloDigitsFromRaw.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloDigitsFromRaw
//
// 2003-11-18 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( CaloDigitsFromRaw );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloDigitsFromRaw::CaloDigitsFromRaw( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) 
{  
  m_detectorNum = CaloCellCode::CaloNumFromName( name ) ;
  if ( m_detectorNum < 0 || m_detectorNum >= (int) CaloCellCode::CaloNums ) 
  { m_detectorNum = CaloCellCode::CaloNumFromName( "Ecal" ) ; }
  
  declareProperty( "Extension"  ,  m_extension = "" );
  declareProperty( "OutputType" , m_outputType = "Digits"  ) ;
  declareProperty( "PinContainer"  ,  m_pinContainerName );
  declareProperty( "DigitsContainer"  ,  m_outputDigits );
  declareProperty( "AdcsContainer"   ,  m_outputADCs);
  declareProperty( "PinContainer"    ,  m_pinContainerName );
  declareProperty( "StatusOnTES"     , m_statusOnTES = true);


  m_digitOnTES =false  ;
  m_adcOnTES =false  ;


  if( 2 == m_detectorNum ) {
    m_outputDigits     = LHCb::CaloDigitLocation::Ecal  + m_extension;
    m_outputADCs       = LHCb::CaloAdcLocation::Ecal    + m_extension;
    m_pinContainerName = LHCb::CaloAdcLocation::EcalPin + m_extension;
  }
  else if( 3 == m_detectorNum ) {
    m_outputDigits     = LHCb::CaloDigitLocation::Hcal  + m_extension;
    m_outputADCs       = LHCb::CaloAdcLocation::Hcal    + m_extension;
    m_pinContainerName = LHCb::CaloAdcLocation::HcalPin + m_extension;
  }
  else if( 1 == m_detectorNum ) {
    m_outputDigits     = LHCb::CaloDigitLocation::Prs + m_extension;
    m_outputADCs       = LHCb::CaloAdcLocation::Prs + m_extension;
    m_pinContainerName = "None";
   }
  else if( 0 == m_detectorNum ) {
    m_outputDigits     = LHCb::CaloDigitLocation::Spd + m_extension;
    m_outputADCs       = LHCb::CaloAdcLocation::Spd + m_extension;
    m_pinContainerName = "None";
  }

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
  debug() << "==> Initialize " << name() << endmsg;
  // get DeCalorimeter
  if( 2 == m_detectorNum ) {
    m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
  }
  else if( 3 == m_detectorNum ) {
    m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal );
  }
  else if( 1 == m_detectorNum ) {
    m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Prs );
   }
  else if( 0 == m_detectorNum ) {
    m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Spd );
  }
  else {
    return( Error( "Invalid detector Num =  " + m_detectorNum ) );
  }


  //
  if( 0 == m_detectorNum ){
    m_spdTool = tool<ICaloTriggerBitsFromRaw>( "CaloTriggerBitsFromRaw",  name() + "Tool",this );
  }else {
    m_energyTool = tool<ICaloEnergyFromRaw>( "CaloEnergyFromRaw",  name() + "Tool",this );
  }
  
  // 
  std::string out( m_outputType );
  std::transform( m_outputType.begin() , m_outputType.end() , out.begin () , ::toupper ) ;
  m_outputType = out;
  if( m_outputType == "DIGITS" ||  m_outputType == "CALODIGITS" || 
      m_outputType == "DIGIT"  ||  m_outputType == "CALODIGIT"  || 
      m_outputType == "BOTH") m_digitOnTES = true;
  if(m_outputType == "ADCS" ||  m_outputType == "CALOADCS" || 
     m_outputType == "ADC"  ||  m_outputType == "CALOADC"  || 
     m_outputType == "BOTH")m_adcOnTES = true;
  if( !m_adcOnTES && !m_digitOnTES ){
    error()<< "CaloDigitsFromRaw configured to produce ** NO ** output (outputType = '" << m_outputType <<"')" << endreq;
    return StatusCode::FAILURE;
  }
  if( m_digitOnTES )debug() <<  "CaloDigitsFromRaw will produce CaloDigits on TES at " 
                            << rootInTES() + m_outputDigits << endreq;
  if( m_adcOnTES )debug() <<  "CaloDigitsFromRaw will produce CaloAdcs on TES at "
                          << rootInTES() + m_outputADCs << endreq;

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloDigitsFromRaw::execute() {

  debug() << "==> Execute" << endreq;

  if       ( 0 == m_detectorNum ) {
    if(m_digitOnTES)convertSpd ( m_outputDigits , 3.2 * Gaudi::Units::MeV );
    if(m_adcOnTES)convertSpd ( m_outputADCs , 0. );
  } else {
    if(m_digitOnTES)convertCaloEnergies ( m_outputDigits );
    if(m_adcOnTES  )convertCaloEnergies ( m_outputADCs );
  }
  return StatusCode::SUCCESS;
};

//=========================================================================
//  Convert the SPD trigger bits to CaloDigits
//=========================================================================
void CaloDigitsFromRaw::convertSpd ( std::string containerName,
                                     double energyScale ) {

  LHCb::Calo::FiredCells spdCells = m_spdTool->spdCells( );
  if(m_statusOnTES)m_spdTool->putStatusOnTES();

  if(m_digitOnTES){
    LHCb::CaloDigits* digits = new LHCb::CaloDigits();
    put( digits, containerName );
    for ( std::vector<LHCb::CaloCellID>::const_iterator itD = spdCells.begin();
          spdCells.end() != itD; ++itD ) {
      LHCb::CaloDigit* dig = new LHCb::CaloDigit( *itD, energyScale );
      digits->insert( dig );
    }
    std::stable_sort ( digits->begin(), digits->end(), 
                       CaloDigitsFromRaw::IncreasingByCellID() );
    debug() << containerName << " CaloDigit container size " << digits->size() << endreq;
  }

  if(m_adcOnTES){
    LHCb::CaloAdcs* adcs = new LHCb::CaloAdcs();
    put( adcs , containerName );
    for ( std::vector<LHCb::CaloCellID>::const_iterator itD = spdCells.begin();
          spdCells.end() != itD; ++itD ) {
      LHCb::CaloAdc* adc = new LHCb::CaloAdc( *itD, 1 );
      adcs->insert( adc );
    }
    debug() << containerName << " CaloAdc container size " << adcs->size() << endreq;
  }


} 

//=========================================================================
//  Converts the standard calorimeter adc-energy
//=========================================================================
void CaloDigitsFromRaw::convertCaloEnergies ( std::string containerName ) {


  if(m_digitOnTES){

    LHCb::CaloDigits* digits = new LHCb::CaloDigits();
    put( digits, containerName );
    std::vector<LHCb::CaloDigit>& allDigits = m_energyTool->digits( );
    if(m_statusOnTES)m_energyTool->putStatusOnTES();



    for ( std::vector<LHCb::CaloDigit>::const_iterator itD = allDigits.begin();
          allDigits.end() != itD; ++itD ) {
      LHCb::CaloDigit* dig = (*itD).clone();
      digits->insert( dig );
      verbose() << "ID " << dig->cellID() << " energy " << dig->e() << endreq;
    }
    std::stable_sort ( digits->begin(), digits->end(), 
                       CaloDigitsFromRaw::IncreasingByCellID() );
    debug() << containerName << " CaloDigit container size " << digits->size() << endreq;

  }
  
  if(m_adcOnTES){
    // Channel ADC
    LHCb::CaloAdcs* adcs = new LHCb::CaloAdcs();
    put( adcs , containerName ); 
    std::vector<LHCb::CaloAdc>& allAdcs = m_energyTool->adcs( );
    for ( std::vector<LHCb::CaloAdc>::const_iterator itA = allAdcs.begin();
          allAdcs.end() != itA; ++itA ) {
      LHCb::CaloAdc* adc = new LHCb::CaloAdc( (*itA).cellID(), (*itA).adc() ); // 'clone'
      adcs->insert(adc);
      verbose() << "ID " << adc->cellID() << " ADC value " << adc->adc() << endreq;
    }
    debug() << " CaloAdc container '"  << containerName  << "' -> size = " << adcs->size() << endreq;


    // PinDiode ADC (possibly in a different container)
    // MUST BE AFTER STANDARD ADCs
    std::vector<LHCb::CaloAdc>& allPinAdcs = m_energyTool->pinAdcs( );
    if( "None" != m_pinContainerName && 0 !=allPinAdcs.size() ){
      LHCb::CaloAdcs*  pinAdcs;
      if(m_pinContainerName == m_outputDigits || m_pinContainerName == "SAME"|| m_pinContainerName == "Same"){
        pinAdcs = adcs ;
      }else{
        pinAdcs = new LHCb::CaloAdcs();
        put(pinAdcs , m_pinContainerName);
      }
      for ( std::vector<LHCb::CaloAdc>::const_iterator itA = allPinAdcs.begin();
            allPinAdcs.end() != itA; ++itA ) {
        LHCb::CaloAdc* pinAdc = new LHCb::CaloAdc( (*itA).cellID(), (*itA).adc() ); // 'clone'
        pinAdcs->insert( pinAdc );
        verbose() << "Pin-diode : ID " << pinAdc->cellID() << " ADC value " << pinAdc->adc() << endreq;
      }
      debug() << " Adding PIN-Diode CaloAdc to container '" << m_pinContainerName 
              << "' -> size = " << pinAdcs->size() << endreq;
    }
  }
}
//=============================================================================
