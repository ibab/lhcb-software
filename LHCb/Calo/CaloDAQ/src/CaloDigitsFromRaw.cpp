// Include files

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// local
#include "CaloDigitsFromRaw.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloDigitsFromRaw
//
// 2003-11-18 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( CaloDigitsFromRaw )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloDigitsFromRaw::CaloDigitsFromRaw( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_spdTool(0), m_energyTool(0), m_calo(0)
{
  m_detectorNum = CaloCellCode::CaloNumFromName( name ) ;
  if ( m_detectorNum < 0 || m_detectorNum >= (int) CaloCellCode::CaloNums )
  { m_detectorNum = CaloCellCode::CaloNumFromName( "Ecal" ) ; }

  declareProperty( "Extension"               ,  m_extension = "" );
  declareProperty( "OutputType"              ,  m_outputType = "Digits"  ) ;
  declareProperty( "PinContainer"            ,  m_pinContainerName );
  declareProperty( "DigitsContainer"         ,  m_outputDigits );
  declareProperty( "AdcsContainer"           ,  m_outputADCs);
  declareProperty( "StatusOnTES"             ,  m_statusOnTES = true);


  m_digitOnTES =false  ;
  m_adcOnTES =false  ;


  if( 2 == m_detectorNum ) {
    m_outputDigits     = LHCb::CaloDigitLocation::Ecal  ;
    m_outputADCs       = LHCb::CaloAdcLocation::Ecal    ;
    m_pinContainerName = LHCb::CaloAdcLocation::EcalPin ;
  }
  else if( 3 == m_detectorNum ) {
    m_outputDigits     = LHCb::CaloDigitLocation::Hcal  ;
    m_outputADCs       = LHCb::CaloAdcLocation::Hcal    ;
    m_pinContainerName = LHCb::CaloAdcLocation::HcalPin ;
  }
  else if( 1 == m_detectorNum ) {
    m_outputDigits     = LHCb::CaloDigitLocation::Prs ;
    m_outputADCs       = LHCb::CaloAdcLocation::Prs ;
    m_pinContainerName = "None";
   }
  else if( 0 == m_detectorNum ) {
    m_outputDigits     = LHCb::CaloDigitLocation::Spd ;
    m_outputADCs       = LHCb::CaloAdcLocation::Spd ;
    m_pinContainerName = "None";
  }

}

//=============================================================================
// Destructor
//=============================================================================
CaloDigitsFromRaw::~CaloDigitsFromRaw() {}


//=========================================================================
//  Initialization
//=========================================================================
StatusCode CaloDigitsFromRaw::initialize ( ) {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
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
    std::ostringstream os;
    os << "Invalid detector Num =  " << m_detectorNum;
    return( Error( os.str() ) );
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
    error()<< "CaloDigitsFromRaw configured to produce ** NO ** output (outputType = '" << m_outputType <<"')" << endmsg;
    return StatusCode::FAILURE;
  }
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) {
    if( m_digitOnTES )
      debug() <<  "CaloDigitsFromRaw will produce CaloDigits on TES at "
              << rootInTES() + m_outputDigits + m_extension << endmsg;
    if( m_adcOnTES )
      debug() <<  "CaloDigitsFromRaw will produce CaloAdcs on TES at "
              << rootInTES() + m_outputADCs + m_extension << endmsg;
  }

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloDigitsFromRaw::execute() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Execute" << endmsg;

  if       ( 0 == m_detectorNum ) {
    convertSpd ( 3.2 * Gaudi::Units::MeV );
  } else {
    convertCaloEnergies ( );
  }
  return StatusCode::SUCCESS;
}

//=========================================================================
//  Convert the SPD trigger bits to CaloDigits
//=========================================================================
void CaloDigitsFromRaw::convertSpd ( double energyScale ) {

  LHCb::Calo::FiredCells spdCells = m_spdTool->spdCells( );

  if(m_digitOnTES){
    LHCb::CaloDigits* digits = new LHCb::CaloDigits();
    put( digits, m_outputDigits + m_extension );
    for ( std::vector<LHCb::CaloCellID>::const_iterator itD = spdCells.begin();
          spdCells.end() != itD; ++itD ) {
      LHCb::CaloDigit* dig = new LHCb::CaloDigit( *itD, energyScale );

      try{
        digits->insert( dig );
      }catch(GaudiException &exc){
        counter("Duplicate Spd 'digit'") += 1;
        std::ostringstream os("");
        os << "Duplicate digit for channel " << *itD;
        Warning(os.str(),StatusCode::SUCCESS).ignore();
        int card =  m_spdTool->deCalo()->cardNumber( *itD );
        int tell1=  m_spdTool->deCalo()->cardToTell1( card);
        LHCb::RawBankReadoutStatus& status = m_spdTool->status();
        status.addStatus( tell1 ,LHCb::RawBankReadoutStatus::DuplicateEntry);
        delete dig;
      }

    }
    std::stable_sort ( digits->begin(), digits->end(),
                       CaloDigitsFromRaw::IncreasingByCellID() );
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << m_outputDigits + m_extension << " CaloDigit container size " << digits->size() << endmsg;
  }

  if(m_adcOnTES){
    LHCb::CaloAdcs* adcs = new LHCb::CaloAdcs();
    put( adcs ,  m_outputADCs + m_extension );
    for ( std::vector<LHCb::CaloCellID>::const_iterator itD = spdCells.begin();
          spdCells.end() != itD; ++itD ) {
      LHCb::CaloAdc* adc = new LHCb::CaloAdc( *itD, 1 );

      try{
        adcs->insert( adc );
      }catch(GaudiException &exc){
        counter("Duplicate Spd 'ADC'") += 1;
        std::ostringstream os("");
        os << "Duplicate ADC for channel " << *itD;
        Warning(os.str(),StatusCode::SUCCESS).ignore();
        int card =  m_spdTool->deCalo()->cardNumber( *itD );
        int tell1=  m_spdTool->deCalo()->cardToTell1( card);
        LHCb::RawBankReadoutStatus& status = m_spdTool->status();
        status.addStatus( tell1 ,LHCb::RawBankReadoutStatus::DuplicateEntry);
        delete adc;
      }

    }
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() <<  m_outputADCs + m_extension << " CaloAdc container size " << adcs->size() << endmsg;
  }

  if(m_statusOnTES)m_spdTool->putStatusOnTES();


}

//=========================================================================
//  Converts the standard calorimeter adc-energy
//=========================================================================
void CaloDigitsFromRaw::convertCaloEnergies ( ) {


  if(m_digitOnTES){

    LHCb::CaloDigits* digits = new LHCb::CaloDigits();
    put( digits, m_outputDigits+ m_extension );
    const std::vector<LHCb::CaloDigit>& allDigits = m_energyTool->digits( );

    for ( std::vector<LHCb::CaloDigit>::const_iterator itD = allDigits.begin();
          allDigits.end() != itD; ++itD ) {
      LHCb::CaloDigit* dig = (*itD).clone();

      try{
        digits->insert( dig );
      }catch(GaudiException &exc){
        counter("Duplicate CaloDigit") += 1;
        std::ostringstream os("");
        os << "Duplicate digit for channel " << itD->cellID();
        Warning(os.str(),StatusCode::SUCCESS).ignore();
        int card =  m_energyTool->deCalo()->cardNumber( itD->cellID() );
        int tell1=  m_energyTool->deCalo()->cardToTell1( card);
        LHCb::RawBankReadoutStatus& status = m_energyTool->status();
        status.addStatus( tell1 ,LHCb::RawBankReadoutStatus::DuplicateEntry);
        delete dig;
      }

      if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
        verbose() << "ID " << dig->cellID() << " energy " << dig->e() << endmsg;
    }
    std::stable_sort ( digits->begin(), digits->end(),
                       CaloDigitsFromRaw::IncreasingByCellID() );
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << m_outputDigits+ m_extension << " CaloDigit container size " << digits->size() << endmsg;
    if(m_statusOnTES)m_energyTool->putStatusOnTES();

  }

  if(m_adcOnTES){
    // Channel ADC
    LHCb::CaloAdcs* adcs = new LHCb::CaloAdcs();
    put( adcs ,  m_outputADCs+ m_extension );
    const std::vector<LHCb::CaloAdc>& allAdcs = m_energyTool->adcs( );
    if(m_statusOnTES)m_energyTool->putStatusOnTES();
    for ( std::vector<LHCb::CaloAdc>::const_iterator itA = allAdcs.begin();
          allAdcs.end() != itA; ++itA ) {
      LHCb::CaloAdc* adc = new LHCb::CaloAdc( (*itA).cellID(), (*itA).adc() ); // 'clone'

      try{
        adcs->insert(adc);
      }catch(GaudiException &exc){
        counter("Duplicate CaloDigit") += 1;
        std::ostringstream os("");
        os << "Duplicate digit for channel " << itA->cellID();
        Warning(os.str(),StatusCode::SUCCESS).ignore();
        int card =  m_energyTool->deCalo()->cardNumber( itA->cellID() );
        int tell1=  m_energyTool->deCalo()->cardToTell1( card);
        LHCb::RawBankReadoutStatus& status = m_energyTool->status();
        status.addStatus( tell1 ,LHCb::RawBankReadoutStatus::DuplicateEntry);
        delete adc;
      }


      if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
        verbose() << "ID " << adc->cellID() << " ADC value " << adc->adc() << endmsg;
    }
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << " CaloAdc container '"  << m_outputADCs+ m_extension  << "' -> size = " << adcs->size() << endmsg;


    // PinDiode ADC (possibly in a different container)
    // MUST BE AFTER STANDARD ADCs decoding
    const std::vector<LHCb::CaloAdc>& allPinAdcs = m_energyTool->pinAdcs( );
    if( "None" != m_pinContainerName && 0 !=allPinAdcs.size() ){
      LHCb::CaloAdcs*  pinAdcs;
      if(m_pinContainerName == m_outputDigits || m_pinContainerName == "SAME"|| m_pinContainerName == "Same"){
        pinAdcs = adcs ;
      }else{
        pinAdcs = new LHCb::CaloAdcs();
        put(pinAdcs , m_pinContainerName + m_extension );
      }
      for ( std::vector<LHCb::CaloAdc>::const_iterator itA = allPinAdcs.begin();
            allPinAdcs.end() != itA; ++itA ) {
        LHCb::CaloAdc* pinAdc = new LHCb::CaloAdc( (*itA).cellID(), (*itA).adc() ); // 'clone'
        pinAdcs->insert( pinAdc );

        try{
          pinAdcs->insert(pinAdc);
        }catch(GaudiException &exc){
          counter("Duplicate CaloDigit") += 1;
          std::ostringstream os("");
          os << "Duplicate digit for channel " << itA->cellID();
          Warning(os.str(),StatusCode::SUCCESS).ignore();
          int card =  m_energyTool->deCalo()->cardNumber( itA->cellID() );
          int tell1=  m_energyTool->deCalo()->cardToTell1( card);
          LHCb::RawBankReadoutStatus& status = m_energyTool->status();
          status.addStatus( tell1 ,LHCb::RawBankReadoutStatus::DuplicateEntry);
          delete pinAdc;
        }

        if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
          verbose() << "Pin-diode : ID " << pinAdc->cellID() << " ADC value " << pinAdc->adc() << endmsg;
      }
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
        debug() << " Adding PIN-Diode CaloAdc to container '" << m_pinContainerName + m_extension
                << "' -> size = " << pinAdcs->size() << endmsg;
    }
    if(m_statusOnTES)m_energyTool->putStatusOnTES();
  }



}
//=============================================================================
