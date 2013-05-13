// ============================================================================
// Include files 
// ============================================================================
// Local
// ============================================================================
#include "CaloTrackTool.h"
// ============================================================================

// ============================================================================
/** @file 
 *  Implementation file for class Calo::CaloTrackTool
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-05-28 
 */
// ============================================================================
/// standard constructor
// ============================================================================
Calo::CaloTrackTool::CaloTrackTool
( const std::string& type   , // ?
  const std::string& name   , 
  const IInterface*  parent ) 
  : GaudiTool ( type , name , parent )
  // extrapolator 
  , m_extrapolatorName () // no default value!!!
  , m_extrapolator         ( 0 ) 
  // fast extrapolator 
  , m_fastExtrapolatorName ( "TrackLinearExtrapolator/Linear:PUBLIC") 
  , m_fastExtrapolator     ( 0 )
  // z for "linear" extrapolation 
  , m_fastZ                ( 10.0 * Gaudi::Units::meter  )
  // tolerances for extrapolations to certain planes 
  , m_tolerance            (  2.0 * Gaudi::Units::mm     )
  , m_cosTolerance         ( ::cos( 0.1 * Gaudi::Units::mrad ) ) 
  , m_maxIter              ( 5 )
  // detector 
  , m_calo                 ( 0 ) 
  , m_detectorName         ("") 
  // usage of the tracks 
  , m_use ()
  // local storages 
  , m_pos () 
  , m_mom ()
{
  // regular & fast extrapolator
  declareProperty ( "Extrapolator"         , m_extrapolatorName     ) ;
  declareProperty ( "FastExtrapolator"     , m_fastExtrapolatorName ) ;
  declareProperty ( "zForFastExtrapolator" , m_fastZ                ) ;
  // tolerances for  extrapolation
  declareProperty ( "Tolerance"            , m_tolerance            ) ;
  declareProperty ( "CosTolerance"         , m_cosTolerance         ) ;
  declareProperty ( "MaxPlaneIterations"   , m_maxIter              ) ;
  // detector 
  declareProperty ( "Calorimeter"          , m_detectorName         ) ; 
  // use track types 
  StatusCode sc = m_use.declareProperties ( this ) ;
  if(!sc.isSuccess())warning() <<" TrackUse::declareProperties  FAILED" << endmsg;
  // 
  _setProperty     ( "CheckTracks"          , "false"               ) ;
}
// ============================================================================
/// initialize the tool 
// ============================================================================
StatusCode Calo::CaloTrackTool::initialize () 
{
  StatusCode sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) { return sc ; }                             // RETURN 
  //
  if ( propsPrint() || msgLevel ( MSG::DEBUG ) || m_use.check() ) 
  { info () << m_use << endmsg ; } ;
  //
  if ( !m_detectorName.empty() ) 
  { m_calo = getDet<DeCalorimeter> ( detectorName()  ) ; }
  else { Warning("empty detector name!") ; }
  //
  return StatusCode::SUCCESS ;
}


void Calo::CaloTrackTool::_setProperty(const std::string& p ,const std::string& v){
  StatusCode sc = setProperty(p,v);
  if(sc.isSuccess()){
    if( msgLevel(MSG::DEBUG) ) debug() << " setting Property "<<p<< " to " << v <<endmsg ;
  }
  else{
    warning() << " setting Property "<<p<< " to " << v << " FAILED" <<endmsg ;
  }
}

