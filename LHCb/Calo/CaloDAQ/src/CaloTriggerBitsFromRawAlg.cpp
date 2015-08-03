// Include files 

#include "Event/L0PrsSpdHit.h"

// local
#include "CaloTriggerBitsFromRawAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloTriggerBitsFromRawAlg
//
// 2006-04-07 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CaloTriggerBitsFromRawAlg )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloTriggerBitsFromRawAlg::CaloTriggerBitsFromRawAlg( const std::string& name,
                                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_l0BitTool(NULL)
{
  declareProperty("OutputData"  , m_outputData  );  
  declareProperty( "Extension"  ,  m_extension = "" );
  declareProperty( "StatusOnTES"   , m_statusOnTES = true);
  

  m_toolType  = "CaloTriggerBitsFromRaw";
  m_toolName = name + "Tool";
  m_isPrs = false;
  if ( "Prs" == name.substr( 0 , 3 ) ) {
    m_outputData = LHCb::L0PrsSpdHitLocation::Prs + m_extension;
    m_isPrs = true;
  } else if ( "Spd" == name.substr( 0 , 3 ) ) {
    m_outputData = LHCb::L0PrsSpdHitLocation::Spd + m_extension;
  }

}
//=============================================================================
// Destructor
//==============h===============================================================
CaloTriggerBitsFromRawAlg::~CaloTriggerBitsFromRawAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CaloTriggerBitsFromRawAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "==> Initialize " << name() << endmsg;

  m_l0BitTool = tool<ICaloTriggerBitsFromRaw>( m_toolType, m_toolName , this);
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloTriggerBitsFromRawAlg::execute() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Execute"  <<endmsg;
  //*** create the output container
  LHCb::L0PrsSpdHits* newL0Bits = new LHCb::L0PrsSpdHits();
  put( newL0Bits , m_outputData );
  //*** get the input data from Raw and fill the output container
  LHCb::Calo::FiredCells l0Cells ;
  if( m_isPrs){    
    l0Cells = m_l0BitTool->prsCells();
  }
  else {
    l0Cells = m_l0BitTool->spdCells();
  }
  

  std::vector<LHCb::CaloCellID>::const_iterator iCell;
  for( iCell = l0Cells.begin(); l0Cells.end() != iCell ; ++iCell ) {
    LHCb::L0PrsSpdHit* l0Bit = new LHCb::L0PrsSpdHit( *iCell );

    // protect against SEU
    try{
      newL0Bits->insert( l0Bit ) ;
    }catch(GaudiException &exc) { 
      counter("Duplicate l0Bit") += 1;
      std::ostringstream os("");
      os << "Duplicate l0Bit for channel " << *iCell << " " << std::endl;
      Warning(os.str(),StatusCode::SUCCESS).ignore();
      int card =  m_l0BitTool->deCalo()->cardNumber( *iCell );
      int tell1=  m_l0BitTool->deCalo()->cardToTell1( card);
      LHCb::RawBankReadoutStatus& status = m_l0BitTool->status();
      status.addStatus( tell1 ,LHCb::RawBankReadoutStatus::DuplicateEntry);
      delete l0Bit;
    }     
  }
  
  if(m_statusOnTES)m_l0BitTool->putStatusOnTES();
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << " L0PrsSpdHits container size " << newL0Bits->size() << endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
