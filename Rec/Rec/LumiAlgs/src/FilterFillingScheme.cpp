// Include files 
 
// event model
#include "Event/RawEvent.h"
#include "Event/ODIN.h"

// CondDB
#include "DetDesc/Condition.h"
#include "GaudiKernel/IDetDataSvc.h"

// local
#include "FilterFillingScheme.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FilterFillingScheme
//
// 2011-08-09 : Jaap Panman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FilterFillingScheme )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FilterFillingScheme::FilterFillingScheme( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_bxid(0)
  , m_condMagnet(0)
  , m_condFilling(0)
{
  declareProperty( "Beam"             ,  m_beam = "0" );
  declareProperty( "MagnetState"      ,  m_MagnetState = "NONE" );
  declareProperty( "BXOffset"         ,  m_BXOffset = 0 );

}
//=============================================================================
// Destructor
//=============================================================================
FilterFillingScheme::~FilterFillingScheme() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode FilterFillingScheme::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  // get the detectorDataSvc
  m_dds = detSvc();
  if (m_dds == NULL) {
    error() << "No detector data service" << endmsg ;
    return StatusCode::SUCCESS;
  }

  // register conditions for database acces
  sc = registerDB(); // must be executed first
  if ( sc.isFailure() ) {
    error() << "No db parameters (no registration)" << endmsg ;
  }


  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FilterFillingScheme::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // get ODIN
  LHCb::ODIN* odin;
  if( exist<LHCb::ODIN>(LHCb::ODINLocation::Default) ){
    odin = get<LHCb::ODIN> (LHCb::ODINLocation::Default);
  }else{
    // should remain flagged as an error and stop the run
    return Error("ODIN cannot be loaded", StatusCode::FAILURE);
  }
  // obtain the BCID from ODIN

  m_bxid = odin->bunchId();
  if ( msgLevel(MSG::DEBUG) ) debug() << "ODIN BCID: " << m_bxid << endmsg;

  // combine all input for a decision
  bool decision = processDB();
  if ( msgLevel(MSG::DEBUG) ) debug() << "Accept event : " << m_bxid << " : " << decision << endmsg;

  setFilterPassed( decision );

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FilterFillingScheme::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}


//=============================================================================
// DB access
//=============================================================================
StatusCode FilterFillingScheme::registerDB() {
  // register the DB conditions for the update maganer
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Register DB" << endmsg;

  // register filling scheme data
  std::string lhcFS = "Conditions/Online/LHCb/LHCFillingScheme";
  if (this->existDet<Condition>(lhcFS)) {
    registerCondition(lhcFS, m_condFilling, &FilterFillingScheme::i_cacheFillingData);
    if ( msgLevel(MSG::DEBUG) ) debug() << lhcFS << endmsg;
  }
  else {
    warning() << lhcFS << " not found, fall back to sampling data" << endmsg;
  }

  // register magnet data
  std::string magnet = "Conditions/Online/LHCb/Magnet/Measured";
  if (this->existDet<Condition>(magnet)) {
    registerCondition(magnet, m_condMagnet, &FilterFillingScheme::i_cacheMagnetData);
    if ( msgLevel(MSG::DEBUG) ) debug() << magnet << endmsg;
  }
  else {
    warning() << magnet << " not found, fall back to sampling data" << endmsg;
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
//  Extract data from Filling scheme
//=========================================================================
StatusCode FilterFillingScheme::i_cacheFillingData() {
  //  data from /Conditions/Online/LHCb/LHCFillingScheme
  if ( msgLevel(MSG::DEBUG) ) debug() << "callback Filling:" << endmsg;

  m_B1FillingScheme = (std::string) m_condFilling->param<std::string>("B1FillingScheme");
  m_B2FillingScheme = (std::string) m_condFilling->param<std::string>("B2FillingScheme");

  return StatusCode::SUCCESS;
}


//=========================================================================
//  Extract data from Filling scheme
//=========================================================================
StatusCode FilterFillingScheme::i_cacheMagnetData() {
  //  data from /Conditions/Online/LHCb/Magnet/Measured

  if ( msgLevel(MSG::DEBUG) ) debug() << "callback Magnet:" << endmsg;
 
  if(m_condMagnet->exists("State")) {
    m_parMagnetState = (std::string) m_condMagnet->param<std::string>("State");
  }
  else {
    warning() << "Magnet State condition not implemented for these data" << endmsg;
    m_parMagnetState = "OFF";
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// process DB parameters
//=============================================================================
bool FilterFillingScheme::processDB() {

  // odin data
  if ( msgLevel(MSG::DEBUG) ) debug() << "ODIN BXID :       " << m_bxid << " " << endmsg;
  // magnet
  if ( msgLevel(MSG::DEBUG) ) debug() << "MagnetState :     " << m_parMagnetState << " " << endmsg;
  // filling scheme
  if ( msgLevel(MSG::VERBOSE) ) verbose() << "B1FillingScheme : " << m_B1FillingScheme << " " << endmsg;
  if ( msgLevel(MSG::VERBOSE) ) verbose() << "BeFillingScheme : " << m_B1FillingScheme << " " << endmsg;

  // for some options do not look at the data
  if ( m_beam == "0" || m_MagnetState == "None" ) {
    if ( msgLevel(MSG::DEBUG) ) debug() << "Do not look at the data with these options - return true" << " " << endmsg;
    return true;
  }

  // check if magnet state to be checked
  if ( m_MagnetState != "" && m_MagnetState != m_parMagnetState ) { 
    if ( msgLevel(MSG::DEBUG) ) debug() << "Do not look at the data - magnet state - return true" << " " << endmsg;
    return true;
  }

  // take the filling scheme corresponding to the requested beam
  std::string scheme = "";
  if ( m_beam == "1" ) {
    scheme = m_B1FillingScheme;
  } else if ( m_beam == "2" ) {
    scheme = m_B2FillingScheme;
  } else {
    if ( msgLevel(MSG::DEBUG) ) debug() << "Do not look at the data with this beam flag - return true" << " " << endmsg;
    return true;
  }
  
  long max_buckets = scheme.size();
  if ( max_buckets == 0) {
    if ( msgLevel(MSG::DEBUG) ) debug() << "invalid scheme - return true - size:" << max_buckets << " " << endmsg;
    return true;
  }

  // ODIN starts counting at 1 - so we have to subtract 1 always to index into the string
  long index = (m_BXOffset + m_bxid + max_buckets - 1) % max_buckets;
  if ( scheme[index] == '0' ) {
    if ( msgLevel(MSG::DEBUG) ) debug() << "empty bucket at tested location: " << index << " " << scheme[index] << " - return true " << endmsg;
    return true;
  } else {
    if ( msgLevel(MSG::DEBUG) ) debug() << "filled bucket at tested location: " << index << " " << scheme[index] << " - return false " << endmsg;
    return false;
  }
}

//=============================================================================
