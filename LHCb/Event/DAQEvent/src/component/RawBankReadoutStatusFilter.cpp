// $Id: RawBankReadoutStatusFilter.cpp,v 1.4 2007-12-05 15:04:07 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "RawBankReadoutStatusFilter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RawBankReadoutStatusFilter
//
// 2007-12-05 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RawBankReadoutStatusFilter );


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

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RawBankReadoutStatusFilter::execute() {

  debug() << "==> Execute" << endmsg;

    

  setFilterPassed(true); // accept by defaut
  int value = LHCb::RawBankReadoutStatus::OK;

  if(m_type == LHCb::RawBank::LastType){
    warning() << "No BankType requested in RawBankReadoutStatusFilter -> filterPassed = true" << endreq;
    return StatusCode::SUCCESS;
  }
  
  

  LHCb::RawBankReadoutStatus* status = NULL;
  LHCb::RawBankReadoutStatuss*  statuss =get<LHCb::RawBankReadoutStatuss>(LHCb::RawBankReadoutStatusLocation::Default);
  if(NULL != statuss){
    status = statuss->object( LHCb::RawBank::BankType(m_type) );  
  } else {
    warning() << "No Readout status container found at "<< LHCb::RawBankReadoutStatusLocation::Default 
              <<" -> will act as the bank " << m_type << " was Missing !!"  << endreq;    
    value = LHCb::RawBankReadoutStatus::Missing;
  }
  if(NULL != status){
    value = status->status();
  }else{ 
    warning() << "No Readout status found for bankType "<< m_type 
              <<" -> will act as the bank " << m_type << " was Missing !!"  << endreq; 
    value = LHCb::RawBankReadoutStatus::Missing;
  }

  int decision = value && m_mask;  
  if(decision !=0 )setFilterPassed(false); // reject

  debug() << "Status value : " << value << " Mask : " << m_mask << " => " << filterPassed() << endreq;


  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RawBankReadoutStatusFilter::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
