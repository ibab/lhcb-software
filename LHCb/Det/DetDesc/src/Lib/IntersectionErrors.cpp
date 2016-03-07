// $Id: IntersectionErrors.cpp,v 1.1 2007-12-19 09:42:40 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// DetDesk
// ============================================================================
#include "DetDesc/ILVolume.h"
#include "DetDesc/IntersectionErrors.h"
#include "DetDesc/VolumeIntersectionIntervals.h"
#include "DetDesc/IGeometryErrorSvc.h"
// ============================================================================
/** @file 
 *  Implementation file for class DetDesc::IntersectionErrors
 *  @date 2007-12-12 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
// initialize the static variable
StatusCode DetDesc::IntersectionErrors::s_code = 
StatusCode ( StatusCode::SUCCESS , true ) ;
// ============================================================================
// initialize the static variable 
DetDesc::IGeometryErrorSvc* DetDesc::IntersectionErrors::s_service = 0 ;
// ============================================================================
// initialize the static variable
unsigned long DetDesc::IntersectionErrors::s_errors = 0 ;
// ============================================================================
// initialize the static variable
bool DetDesc::IntersectionErrors::s_recovery = false ;
// ============================================================================
// set the static code 
// ============================================================================
void  DetDesc::IntersectionErrors::setCode 
( const StatusCode& sc     , 
  const ILVolume*   volume ) 
{
  s_code = sc ; 
  if ( s_code.isFailure() )
  {
    if ( 0 != s_service ) { s_service -> setCode ( s_code , volume ) ; }
    else                  { ++s_errors ; } /// increment number of errors 
  }
}
// ============================================================================
// get the total number of uncatched errors 
// ============================================================================
unsigned long DetDesc::IntersectionErrors::errors() { return s_errors ; }
// ============================================================================
// get the static error code 
// ============================================================================
const StatusCode& DetDesc::IntersectionErrors::code() { return s_code ; }
// ============================================================================
// set the static pointer to Geoemtry Error Service 
// ============================================================================
void DetDesc::IntersectionErrors::setService 
( DetDesc::IGeometryErrorSvc* svc) { s_service = svc ; }
// ============================================================================
// get the static pointer to Geoemtry Error Service 
// ============================================================================
DetDesc::IGeometryErrorSvc* DetDesc::IntersectionErrors::service() 
{ return s_service ; }
// ============================================================================
// set the static flag for recovery
void DetDesc::IntersectionErrors::setRecovery ( const bool value ) 
{ s_recovery = value ; }
// ============================================================================
// get the static flag for recovery
// ============================================================================
bool DetDesc::IntersectionErrors::recovery() { return s_recovery ; }
// ============================================================================
// inspect the potential error in intersectionsa
// ============================================================================
void  DetDesc::IntersectionErrors::inspect
( const ILVolume*                volume , 
  const Gaudi::XYZPoint&         pnt    , 
  const Gaudi::XYZVector&        vect   , 
  const ILVolume::Intersections& cnt    ) 
{
  if ( 0 != s_service ) 
  {
    s_service -> inspect( volume , pnt , vect , cnt ) ;
    return ;
  }
  // increment number of improcessed errors 
  ++s_errors ;
}
// ============================================================================
/* report the recovered action in intersections 
 *  @param  volume    the problematic volume 
 *  @aram   material1 the affected material 
 *  @aram   material2 the affected material 
 *  @param  delta    the problematic delta  (non-negative!)
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-12-14
 */
// ============================================================================
void DetDesc::IntersectionErrors::recovered
( const ILVolume* volume    , 
  const Material* material1 ,
  const Material* material2 ,
  const double    delta     ) 
{
  if ( 0 != s_service ) 
  { 
    s_service -> recovered ( volume , material1 , material2 , delta ) ;   
    return ;
  }
  // increment number of improcessed errors 
  ++s_errors ;
}
// ============================================================================
/* report the skipped intersection 
 *  @param  volume   the problematic volume 
 *  @aram   material the affected material 
 *  @param  delta    the problematic delta  (non-negative!)
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-12-14
 */
// ===========================================================================
void DetDesc::IntersectionErrors::skip
( const ILVolume* volume   , 
  const Material* material ,
  const double    delta    ) 
{
  if ( 0 != s_service ) 
  { 
    s_service -> skip ( volume , material , delta ) ; 
    return ;
  }
  // increment number of improcessed errors 
  ++s_errors ;
}
// ============================================================================
// The END 
// ============================================================================
