// $Id: $
// Include files
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "OfflineRateLimiter.h"
//-----------------------------------------------------------------------------
// Implementation file for class : OfflineRateLimiter
//
// 2011-08-05 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( OfflineRateLimiter );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OfflineRateLimiter::OfflineRateLimiter( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : OfflineDeterministicPrescaler ( name , pSvcLocator )
  , m_glp(0)
  , m_tck(0)
{
  declareProperty("HltLimiter",m_hltLimiter = "Hlt1MBNoBiasODINFilter",
                  "Reference Hlt rate limiter");
  declareProperty("Rate",m_rate = 1.,"Rate to achieve");
}
//=============================================================================
// Destructor
//=============================================================================
OfflineRateLimiter::~OfflineRateLimiter() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode OfflineRateLimiter::initialize() {
  StatusCode sc = OfflineDeterministicPrescaler::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by OfflineDeterministicPrescaler

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_glp = tool<IGetLumiParameters>("GetLumiParameters",this); // make it private
  m_glp->init( "PropertyConfigSvc",m_hltLimiter, true );
  if (m_rate<0) Exception("Negative Rate requested");
  
  info() << "Will attempt to reduce events from " << m_hltLimiter 
         << " to a rate of " << m_rate << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Not an Incident
//=============================================================================
void OfflineRateLimiter::handle() {  
  if (m_tck == m_glp->getTCK()) return ;
  
  boost::uint32_t savedAcc = m_acc;
  m_tck = m_glp->getTCK() ;
  double randomRate = m_glp->HLTRandomRate() ;
  if ( randomRate > 0) {
    m_accFrac = m_rate/randomRate ;
  } else {
    warning() << "Rate of " << m_hltLimiter << " in TCK " 
              << m_tck << " is " 
              << randomRate << endmsg ;
    m_accFrac = 0 ;
  }
  update();
  if (msgLevel(MSG::DEBUG) || (savedAcc!=m_acc )) {
    info() << "Rate of " << m_hltLimiter << " in TCK " 
           << m_tck << " is " 
           << randomRate
           << " -> Need to reduce by factor " << m_accFrac << endmsg ;
  }
  return ;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode OfflineRateLimiter::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute "  << endmsg;
  handle() ; // update TCK
  bool acc = accept();
  setFilterPassed(acc);
  *m_counter += acc;
  if (msgLevel(MSG::DEBUG)) debug() << (acc?"Accepted":"Rejected") << endmsg ;
  return StatusCode::SUCCESS;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode OfflineRateLimiter::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return OfflineDeterministicPrescaler::finalize();  // must be called after all other actions
}

