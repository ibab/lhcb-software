// Include files 

// local
#include "RawBankReadoutStatusFilter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RawBankReadoutStatusFilter
//
// 2007-12-05 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RawBankReadoutStatusFilter )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RawBankReadoutStatusFilter::RawBankReadoutStatusFilter( const std::string& name,
                                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
    , m_type()
    , m_mask()
{
  declareProperty( "BankType", m_type = LHCb::RawBank::LastType);
  declareProperty( "RejectionMask", m_mask = 0x0); // filterPassed = true anyway
  declareProperty( "InvertedFilter" , m_invert = false);
}
//=============================================================================
// Destructor
//=============================================================================
RawBankReadoutStatusFilter::~RawBankReadoutStatusFilter() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode RawBankReadoutStatusFilter::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if(msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RawBankReadoutStatusFilter::execute() {

  if(msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;

  setFilterPassed(!m_invert); // accept by default
  int value = LHCb::RawBankReadoutStatus::OK;

  if(m_type == LHCb::RawBank::LastType){
    return Warning( "No BankType requested in RawBankReadoutStatusFilter -> filterPassed = true", StatusCode::SUCCESS, 0);
  }
  
  

  LHCb::RawBankReadoutStatus* status = NULL;
  LHCb::RawBankReadoutStatuss* statuss = getIfExists<LHCb::RawBankReadoutStatuss>(LHCb::RawBankReadoutStatusLocation::Default);
  if(NULL != statuss){
    status = statuss->object( LHCb::RawBank::BankType(m_type) );  
  } else {
    Warning("No Readout status container found at "+ LHCb::RawBankReadoutStatusLocation::Default 
            + " for the bank " + Gaudi::Utils::toString(m_type),StatusCode::SUCCESS).ignore();    
    value = LHCb::RawBankReadoutStatus::MissingStatus;
  }
  if(NULL != status){
    value = status->status();
  }else{ 
    Warning("No Readout status found for bankType "+ Gaudi::Utils::toString(m_type),StatusCode::SUCCESS).ignore(); 
    value = LHCb::RawBankReadoutStatus::MissingStatus;
  }

  int decision = value & m_mask;  
  if(decision !=0 )setFilterPassed(m_invert); // reject by default
  if(filterPassed())counter("Accepted events")+=1;
  else counter("Rejected events")+=1;

  if(msgLevel(MSG::DEBUG)) debug() << "Status value : " << value << " Mask : "
                                   << m_mask << " => " << filterPassed() << endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RawBankReadoutStatusFilter::finalize() {

  if(msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
