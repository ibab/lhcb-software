// $Id: CaloTrackAlg.cpp,v 1.1 2004-10-26 20:35:58 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/MsgStream.h" 
// ============================================================================
// Event 
// ============================================================================
#include "Event/TrStoredTrack.h"
// ============================================================================
// CaloKernel
// ============================================================================
#include "CaloKernel/CaloPrint.h"
// ============================================================================
// local
// ============================================================================
#include "CaloTrackAlg.h"
// ============================================================================

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
  : CaloAlgorithm ( name , svcloc ) 
  //
  , m_uniqueOnly   ( false )  // match ALL tracks, including clones 
  , m_error        ( false )  // USE ERROR TRACKS ALSO  
  // 
  , m_isLong       ( true  ) 
  , m_isUpstream   ( true  ) 
  , m_isDownstream ( true  ) 
  , m_isVelotrack  ( true  ) 
  , m_isBackward   ( true  ) 
  , m_isTtrack     ( true  ) 
  //
  , m_velo         ( true  )
  , m_seed         ( true  )
  , m_match        ( true  ) 
  , m_forward      ( true  ) 
  , m_follow       ( true  ) 
  , m_veloTT       ( true  )
  , m_veloBack     ( true  ) 
  , m_ksTrack      ( true  ) 
  //
{
  // 
  declareProperty ( "UseUniqueOnly" , m_uniqueOnly ) ;
  declareProperty ( "UseErrorAlso"  , m_error      ) ;
  //
  declareProperty ( "isLong"       , m_isLong       ) ;
  declareProperty ( "isUpstream"   , m_isUpstream   ) ;
  declareProperty ( "isDownstream" , m_isDownstream ) ;
  declareProperty ( "isVelotrack"  , m_isVelotrack  ) ;
  declareProperty ( "isBackward"   , m_isBackward   ) ;
  declareProperty ( "isTtrack"     , m_isTtrack     ) ;
  //
  declareProperty ( "velo"         , m_velo         ) ;
  declareProperty ( "seed"         , m_seed         ) ;
  declareProperty ( "match"        , m_match        ) ;
  declareProperty ( "forward"      , m_forward      ) ;
  declareProperty ( "follow"       , m_follow       ) ;
  declareProperty ( "veloTT"       , m_veloTT       ) ;
  declareProperty ( "veloBack"     , m_veloBack     ) ;
  declareProperty ( "ksTrack"      , m_ksTrack      ) ;
  //
};
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
CaloTrackAlg::~CaloTrackAlg() {};
// ============================================================================

// ============================================================================
/** standard algorithm initialization 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloTrackAlg::initialize()
{
  StatusCode sc = CaloAlgorithm::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  
  std::string msg =  "Rejected Track categories : " ;
  if ( !m_isLong       ) { msg += "'isLong' "       ; }
  if ( !m_isUpstream   ) { msg += "'isUpstream' "   ; }
  if ( !m_isDownstream ) { msg += "'isDownstream' " ; }
  if ( !m_isVelotrack  ) { msg += "'isVelotrack' "  ; }
  if ( !m_isBackward   ) { msg += "'isBackward' "   ; }
  if ( !m_isTtrack     ) { msg += "'isTtrack' "     ; }
  info() << msg << endreq ;
  
  Warning ( msg , StatusCode::SUCCESS , 0 ) ;
  
  msg             =  "Rejected Track algorithms : " ;
  if ( !m_velo         ) { msg += "'velo' "         ; }
  if ( !m_seed         ) { msg += "'seed' "         ; }
  if ( !m_match        ) { msg += "'match' "        ; }
  if ( !m_forward      ) { msg += "'forward' "      ; }
  if ( !m_follow       ) { msg += "'follow' "       ; }
  if ( !m_veloTT       ) { msg += "'veloTT' "       ; }
  if ( !m_veloBack     ) { msg += "'veloBack' "     ; }
  if ( !m_ksTrack      ) { msg += "'ksTrack' "      ; }
  info() << msg << endreq ;
  Warning ( msg , StatusCode::SUCCESS , 0 ) ;
  
  msg             =  "Rejected Track types      : " ;
  if ( m_uniqueOnly ) { msg += "'clone' "           ; }
  if ( !m_error     ) { msg += "'error' "           ; }
  info() << msg << endreq ;
  Warning ( msg , StatusCode::SUCCESS , 0 ) ;
  
  return StatusCode::SUCCESS ;  
};
// ============================================================================

// ============================================================================
/** standard algorithm finalization 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloTrackAlg::finalize  () { return CaloAlgorithm::finalize() ; }
// ============================================================================

// ============================================================================
/** check if the track to be used 
 *  @param track track object 
 *  @return decision
 */
// ============================================================================
bool CaloTrackAlg::use ( const TrStoredTrack* track ) const 
  {
    // check the argument 
    if ( 0 == track ) { return false ; }
    
    // unique ?
    if ( m_uniqueOnly     && !track->unique()           ) { return false ; }
    
    // use error tracks ? 
    if ( !m_error         && 0 != track -> errorFlag () ) { return false ; }
    
    // categories ?
    if ( !m_isLong        && track -> isLong         () ) { return false ; }
    if ( !m_isUpstream    && track -> isUpstream     () ) { return false ; }
    if ( !m_isDownstream  && track -> isDownstream   () ) { return false ; }
    if ( !m_isVelotrack   && track -> isVelotrack    () ) { return false ; }
    if ( !m_isBackward    && track -> isBackward     () ) { return false ; }
    if ( !m_isTtrack      && track -> isTtrack       () ) { return false ; }
    
    // algorihtms?
    if ( !m_velo          && track -> velo           () ) { return false ; }
    if ( !m_seed          && track -> seed           () ) { return false ; }
    if ( !m_match         && track -> match          () ) { return false ; }
    if ( !m_forward       && track -> forward        () ) { return false ; }
    if ( !m_follow        && track -> follow         () ) { return false ; }
    if ( !m_veloTT        && track -> veloTT         () ) { return false ; }
    if ( !m_veloBack      && track -> veloBack       () ) { return false ; }
    if ( !m_ksTrack       && track -> ksTrack        () ) { return false ; }
    
    return true ;
  };
// ============================================================================

// ============================================================================
/** make 'bit' representation of track categories/types/algorithms
 *  @param trObj the track object
 *  @return 'bit'-representation of the track (useful for debugging)
 */
// ============================================================================
std::string CaloTrackAlg::bits 
( const TrStoredTrack* trObj ) const  
{
  if ( 0 == trObj ) { return std::string("<invalid>") ; }
  
  CaloPrint print;  
  std::string msg( "bits: ") ;
  msg +=  "E:" + print ( (int) trObj -> errorFlag     () ) ;
  msg += "/U:" + print ( (int) trObj -> unique        () ) ;
  msg += "/H:" + print ( (int) trObj -> history       () ) ;
  //
  msg += "/L:" + print ( (int) trObj -> isLong        () ) ;
  msg += "/U:" + print ( (int) trObj -> isUpstream    () ) ;
  msg += "/D:" + print ( (int) trObj -> isDownstream  () ) ;
  msg += "/V:" + print ( (int) trObj -> isVelotrack   () ) ;
  msg += "/B:" + print ( (int) trObj -> isBackward    () ) ;
  msg += "/T:" + print ( (int) trObj -> isTtrack      () ) ;
  //
  msg += "/v:" + print ( (int) trObj -> velo          () ) ;
  msg += "/s:" + print ( (int) trObj -> seed          () ) ;
  msg += "/m:" + print ( (int) trObj -> match         () ) ;
  msg += "/f:" + print ( (int) trObj -> forward       () ) ;
  msg += "/f:" + print ( (int) trObj -> follow        () ) ;
  msg += "/v:" + print ( (int) trObj -> veloTT        () ) ;
  msg += "/b:" + print ( (int) trObj -> veloBack      () ) ;
  msg += "/k:" + print ( (int) trObj -> ksTrack       () ) ;
  //
  return msg ;
};
// ============================================================================
 
// ============================================================================
// The End 
// ============================================================================
