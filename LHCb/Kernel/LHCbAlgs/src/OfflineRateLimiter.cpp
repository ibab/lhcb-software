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
DECLARE_ALGORITHM_FACTORY( OfflineRateLimiter )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OfflineRateLimiter::OfflineRateLimiter( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : OfflineDeterministicPrescaler ( name , pSvcLocator )
  , m_tckReader(0)
  , m_tck(0)
  ,  m_condTriggerTool(0)
{
  declareProperty("HltLimiter",m_hltLimiter = "Hlt1MBNoBiasODINFilter",
                  "Reference Hlt rate limiter");
  declareProperty("Rate",m_rate = 1.,"Rate to achieve [Hz]");
  declareProperty("ConditionRate", m_conditionRate = "NoBiasRate");
  declareProperty("UseCondition", m_useCondDB = false);
  declareProperty("FallBack", m_fallback = false);
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
  if ( m_rate<0) Exception("Negative Rate requested");
  if ( m_useCondDB ){
    m_condTriggerTool = tool<RateFromCondDB>("RateFromCondDB",this);
    if (!m_condTriggerTool) return StatusCode::FAILURE;
    if (!m_condTriggerTool->initializeCondDB()){
      if (m_fallback) {
        info() << "Failed to get rate from condition database. This is normal for <2012." << endmsg ;
        info() << "Fill fallback on TCK" << endmsg ;
        m_condTriggerTool->finalize();
        m_useCondDB = false ;
      } else{
        Exception("Failed to get rate from condition database and no fallback") ;
      }
    }
  }
  // this one I need anyway as fallback
  if ( !m_useCondDB || m_fallback ){
    m_tckReader = tool<IRateFromTCK>("RateFromTCK",this); // make it private
    if (!m_tckReader) return StatusCode::FAILURE;
  }
  info() << "Will attempt to reduce events from " << m_hltLimiter 
         << " to a rate of " << m_rate << " Hz" << endmsg;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Not an Incident
//=============================================================================
void OfflineRateLimiter::handleTCK() { 
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> handleTCK "  << endmsg;
  if (0==m_tckReader) Exception("TCK Reader is NULL in handleTCK");
  unsigned int newTCK = m_tckReader->getTCK() ;
  if (m_tck == newTCK ) return ;
  
  if ( msgLevel(MSG::DEBUG) ) debug() << " changed TCK from " << m_tck << " to " <<  newTCK << endmsg;
  boost::uint32_t savedAcc = m_acc;
  m_tck = newTCK ;
  double randomRate = m_tckReader->rateFromTCK(m_hltLimiter) ;
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
           << " Hz -> Need to reduce by factor " << m_accFrac << endmsg ;
  }
  return ;
}
//=============================================================================
// Not an Incident
//=============================================================================
void OfflineRateLimiter::handleCondDB() { 
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> handleCondDB "  << endmsg;
  if (0==m_condTriggerTool) Exception("CondDB Reader is NULL in handleCondDB");
  
  boost::uint32_t savedAcc = m_acc;
  double randomRate = m_condTriggerTool->getRate() ;
  if ( randomRate > 0) {
    m_accFrac = m_rate/randomRate ;
    update();
  } else {
    warning() << "Rate of Line is" << randomRate << endmsg ;
    if ( m_tckReader ) handleTCK() ;
  }
  if (msgLevel(MSG::DEBUG) || (savedAcc!=m_acc )) {
    info() << "Rate of Line is " 
           << randomRate
           << " Hz -> Need to reduce by factor " << m_accFrac << endmsg ;
  }
  return ;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode OfflineRateLimiter::execute() {

  //  somewhere here get rate

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute "  << endmsg;
  if (m_useCondDB) handleCondDB() ;    // Try via CondDB
  else handleTCK() ;                   // Try via TCK
  bool acc = accept();
  setFilterPassed(acc);
  *m_counter += acc;
  if (msgLevel(MSG::DEBUG)) debug() << (acc?"Accepted":"Rejected") << endmsg ;
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode OfflineRateLimiter::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return OfflineDeterministicPrescaler::finalize();  // must be called after all other actions
}

