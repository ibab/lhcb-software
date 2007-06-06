// $Id: CaloTriggerAdcsFromRawAlg.cpp,v 1.8 2007-06-06 14:25:11 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/L0CaloAdc.h" 

// local
#include "CaloTriggerAdcsFromRawAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloTriggerAdcsFromRawAlg
//
// 2006-04-07 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CaloTriggerAdcsFromRawAlg );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloTriggerAdcsFromRawAlg::CaloTriggerAdcsFromRawAlg( const std::string& name,
                                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty("OutputData"  , m_outputData  );  
  declareProperty( "Extension"  ,  m_extension = "" );
  declareProperty( "PinContainer"  ,  m_pinContainer );

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

  debug() << "==> Initialize" <<  endreq;  
  m_l0AdcTool = tool<ICaloTriggerAdcsFromRaw>( m_toolType , m_toolName, this);
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloTriggerAdcsFromRawAlg::execute() {

  debug() << "==> Execute"  <<endmsg;

  //*** create the output container
  LHCb::L0CaloAdcs* newL0Adcs = new LHCb::L0CaloAdcs();
  put( newL0Adcs, m_outputData );

  //*** get the input data from Raw and fill the output container
  std::vector<LHCb::L0CaloAdc>& l0Adcs = m_l0AdcTool->adcs( );
  std::vector<LHCb::L0CaloAdc>::const_iterator il0Adc;
  for( il0Adc = l0Adcs.begin(); l0Adcs.end() != il0Adc ; ++il0Adc ) {
    LHCb::L0CaloAdc* adc = new LHCb::L0CaloAdc( (*il0Adc).cellID(), (*il0Adc).adc() );
    newL0Adcs->insert( adc ) ;
  }
  debug() << " L0CaloAdcs container" << m_outputData 
          <<" size " << newL0Adcs->size() << endreq;
  
  //*** get the PinDiode data and fill the output container
  // MUST BE AFTER STANDARD ADCs
  std::vector<LHCb::L0CaloAdc>& l0PinAdcs = m_l0AdcTool->pinAdcs( );
  if( "None" != m_pinContainer && 0 != l0PinAdcs.size() ){
    LHCb::L0CaloAdcs* newL0PinAdcs = new LHCb::L0CaloAdcs();
    put( newL0PinAdcs, m_pinContainer );
    std::vector<LHCb::L0CaloAdc>::const_iterator il0PinAdc;
    for( il0PinAdc = l0PinAdcs.begin(); l0PinAdcs.end() != il0PinAdc ; ++il0PinAdc ) {
      LHCb::L0CaloAdc* pinAdc = new LHCb::L0CaloAdc( (*il0PinAdc).cellID(), (*il0PinAdc).adc() );
      newL0PinAdcs->insert( pinAdc ) ;
    }
    debug() << " PinDiode : L0CaloAdcs container " << m_pinContainer 
            << " size " << newL0Adcs->size() << endreq;
  }




  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CaloTriggerAdcsFromRawAlg::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
