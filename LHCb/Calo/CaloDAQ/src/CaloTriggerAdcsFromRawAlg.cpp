// Include files 

#include "Event/L0CaloAdc.h" 

// local
#include "CaloTriggerAdcsFromRawAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloTriggerAdcsFromRawAlg
//
// 2006-04-07 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CaloTriggerAdcsFromRawAlg )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloTriggerAdcsFromRawAlg::CaloTriggerAdcsFromRawAlg( const std::string& name,
                                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ), m_l0AdcTool(0)
{
  declareProperty("OutputData"  , m_outputData  );  
  declareProperty( "Extension"  ,  m_extension = "" );
  declareProperty( "PinContainer"  ,  m_pinContainer );
  declareProperty( "StatusOnTES"   , m_statusOnTES = true);
  

  m_toolType = "CaloTriggerAdcsFromRaw";  
  m_toolName  = name + "Tool";  
  if ( "Ecal" == name.substr( 0 , 4 ) ) {
    m_outputData   = LHCb::L0CaloAdcLocation::Ecal    + m_extension;    
    m_pinContainer = LHCb::L0CaloAdcLocation::EcalPin + m_extension; 
  } else if ( "Hcal" == name.substr( 0 , 4 ) ) {
    m_outputData   = LHCb::L0CaloAdcLocation::Hcal    + m_extension;
    m_pinContainer = LHCb::L0CaloAdcLocation::HcalPin + m_extension; 
  }

}
//=============================================================================
// Destructor
//==============h===============================================================
CaloTriggerAdcsFromRawAlg::~CaloTriggerAdcsFromRawAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CaloTriggerAdcsFromRawAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" <<  endmsg;
  m_l0AdcTool = tool<ICaloTriggerAdcsFromRaw>( m_toolType , m_toolName, this);
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloTriggerAdcsFromRawAlg::execute() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Execute"  <<endmsg;

  //*** create the output container
  LHCb::L0CaloAdcs* newL0Adcs = new LHCb::L0CaloAdcs();
  put( newL0Adcs, m_outputData );

  //*** get the input data from Raw and fill the output container
  const std::vector<LHCb::L0CaloAdc>& l0Adcs = m_l0AdcTool->adcs( );


  std::vector<LHCb::L0CaloAdc>::const_iterator il0Adc;
  for( il0Adc = l0Adcs.begin(); l0Adcs.end() != il0Adc ; ++il0Adc ) {
    LHCb::L0CaloAdc* adc = new LHCb::L0CaloAdc( (*il0Adc).cellID(), (*il0Adc).adc() );    
    try{
      newL0Adcs->insert( adc ) ;
    }
    catch(GaudiException &exc) { 
      counter("Duplicate L0ADC") += 1;
      std::ostringstream os("");
      os << "Duplicate l0ADC for channel " << il0Adc->cellID() << std::endl;
      Warning(os.str(),StatusCode::SUCCESS).ignore();
      int card =  m_l0AdcTool->deCalo()->cardNumber( il0Adc->cellID() );
      int tell1=  m_l0AdcTool->deCalo()->cardToTell1( card);
      LHCb::RawBankReadoutStatus& status = m_l0AdcTool->status();
      status.addStatus( tell1 ,LHCb::RawBankReadoutStatus::DuplicateEntry);
      delete adc;
    }     
  }
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << " L0CaloAdcs container" << m_outputData 
            <<" size " << newL0Adcs->size() << endmsg;
  
  //*** get the PinDiode data and fill the output container
  // MUST BE AFTER STANDARD ADCs
  const std::vector<LHCb::L0CaloAdc>& l0PinAdcs = m_l0AdcTool->pinAdcs( );
  if( "None" != m_pinContainer && 0 != l0PinAdcs.size() ){
    LHCb::L0CaloAdcs* newL0PinAdcs = new LHCb::L0CaloAdcs();
    put( newL0PinAdcs, m_pinContainer );
    std::vector<LHCb::L0CaloAdc>::const_iterator il0PinAdc;
    for( il0PinAdc = l0PinAdcs.begin(); l0PinAdcs.end() != il0PinAdc ; ++il0PinAdc ) {
      LHCb::L0CaloAdc* pinAdc = new LHCb::L0CaloAdc( (*il0PinAdc).cellID(), (*il0PinAdc).adc() );

    try{
      newL0PinAdcs->insert( pinAdc ) ;
    }
    catch(GaudiException &exc) { 
      counter("Duplicate PIN L0ADC") += 1;
      std::ostringstream os("");
      os << "Duplicate PIN l0ADC for channel " << il0Adc->cellID() << std::endl;
      Warning(os.str(),StatusCode::SUCCESS).ignore();
      int card =  m_l0AdcTool->deCalo()->cardNumber( il0Adc->cellID() );
      int tell1=  m_l0AdcTool->deCalo()->cardToTell1( card);
      LHCb::RawBankReadoutStatus& status = m_l0AdcTool->status();
      status.addStatus( tell1 ,LHCb::RawBankReadoutStatus::DuplicateEntry);
      delete pinAdc;
    }

    }
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << " PinDiode : L0CaloAdcs container " << m_pinContainer 
              << " size " << newL0Adcs->size() << endmsg;
  }

  if(m_statusOnTES)m_l0AdcTool->putStatusOnTES();  
  return StatusCode::SUCCESS;
}
//=============================================================================
