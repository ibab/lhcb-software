// $Id: CaloTrackTool.cpp,v 1.2 2006-06-18 18:32:55 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2006/06/06 11:59:52  ibelyaev
//  new base classes for PIDs & rewritten TrackUse
// 
// ============================================================================
// Include files 
// ============================================================================
// Local
// ============================================================================
#include "CaloUtils/CaloTrackTool.h"
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
  m_use.declareProperties ( this ) ;
  // 
  setProperty     ( "CheckTracks"          , "false"                ) ;
} ;
// ============================================================================
/// initialize the tool 
// ============================================================================
StatusCode Calo::CaloTrackTool::initialize () 
{
  StatusCode sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) { return sc ; }                             // RETURN 
  //
  if ( propsPrint() || msgLevel ( MSG::DEBUG ) || m_use.check() ) 
  { info () << m_use << endreq ; } ;
  //
  if ( !m_detectorName.empty() ) 
  { m_calo = getDet<DeCalorimeter> ( detectorName()  ) ; }
  else { Warning("empty detector name!") ; }
  //
  return StatusCode::SUCCESS ;
} ;
// ============================================================================

// ============================================================================
// The END
// ============================================================================
