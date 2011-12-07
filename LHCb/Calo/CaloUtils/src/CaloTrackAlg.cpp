// ============================================================================
// Include files
// ============================================================================
#include "Event/Track.h"
#include "CaloUtils/CaloAlgUtils.h"
#include "CaloUtils/CaloTrackAlg.h"
// ============================================================================
/** @file 
 * 
 *  Implementation file for class CaloCluster2TrackAlg
 *  @see CaloCluster2TrackAlg
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 02/11/2001
 */
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @param   name   algorithm name 
 *  @param   svcloc pointer to service locator 
 */
// ============================================================================
CaloTrackAlg::CaloTrackAlg
( const std::string& name   ,
  ISvcLocator*       svcloc )
  : GaudiAlgorithm ( name , svcloc ) 
  , m_use () 
{
  //
  StatusCode sc = m_use.declareProperties ( this ) ;
  if(!sc.isSuccess())warning() <<" TrackUse::declareProperties  FAILED" << endmsg;
  //
  _setProperty ( "CheckTracks" , "true" ) ;
  std::vector<int> stat;
  stat.push_back( LHCb::Track::Fitted);
  if( LHCb::CaloAlgUtils::hltContext(context()) )stat.push_back( LHCb::Track::FitStatusUnknown);
  setProperty ( "AcceptedFitStatus" , stat).ignore();
}
// ============================================================================
/// destructor
// ============================================================================
CaloTrackAlg::~CaloTrackAlg() {}
// ============================================================================
/// standard algorithm initialization 
// ============================================================================
StatusCode CaloTrackAlg::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  //
  if ( propsPrint() || msgLevel ( MSG::DEBUG ) || m_use.check() ) 
  { info () << m_use << endmsg ; } ;
  //
  return StatusCode::SUCCESS ;  
}
// ============================================================================
/// standard algorithm finalization 
StatusCode CaloTrackAlg::finalize  () { return GaudiAlgorithm::finalize() ; }
// ============================================================================


void CaloTrackAlg::_setProperty(const std::string& p ,const std::string& v){
  StatusCode sc = setProperty(p,v);
  if(sc.isSuccess()){
    if( msgLevel(MSG::DEBUG) ) debug() << " setting Property "<<p<< " to " << v <<endmsg ;
  }
  else{
    warning() << " setting Property "<<p<< " to " << v << " FAILED" <<endmsg ;
  }
}


