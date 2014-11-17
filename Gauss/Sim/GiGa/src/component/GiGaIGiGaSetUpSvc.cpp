// $Id: GiGaIGiGaSetUpSvc.cpp,v 1.1 2002-12-07 14:27:52 ibelyaev Exp $ 
// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.9  2002/05/07 12:21:36  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// ============================================================================
#define GIGA_GIGASVCIGIGASETUPSVC_CPP 1 
// ============================================================================
/// include files 
/// STD & ATL 
#include <string>
#include <typeinfo> 
/// GaudiKernel 
#include  "GaudiKernel/Kernel.h"
#include  "GaudiKernel/StatusCode.h"
#include  "GaudiKernel/System.h" 
#include  "GaudiKernel/MsgStream.h" 
/// G4 
#include  "G4VUserDetectorConstruction.hh"
#include  "G4VPhysicalVolume.hh"
#include  "G4VUserPrimaryGeneratorAction.hh"
#include  "G4VUserPhysicsList.hh"
#include  "G4UserRunAction.hh"
#include  "G4UserEventAction.hh"
#include  "G4UserStackingAction.hh"
#include  "G4UserTrackingAction.hh"
#include  "G4UserSteppingAction.hh"

/// GiGa
#include "GiGa/IGiGaRunManager.h"
#include "GiGa/GiGaException.h"
#include "GiGa/GiGaUtil.h"
/// local 
#include "GiGa.h"


// ============================================================================
/**  @file
 * 
 *   Implementation of class GiGa  
 *   all methods from abstract interface IGiGaSetUpSvc 
 *
 *   @author: Vanya Belyaev Ivan.Belyaev@itep.ru
 *   @date xx/xx/xxxx
 */
// ============================================================================

// ============================================================================
/** set detector constructon module 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  NB: errors are reported through exception thrown 
 *
 *  @param  obj      pointer to detector construction module  
 *  @return self-reference ot IGiGaSetUpSvc interface 
 */
// ============================================================================
IGiGaSetUpSvc& GiGa::operator << ( G4VUserDetectorConstruction   * obj )
{
  try
    {
      StatusCode sc = StatusCode::SUCCESS;
      if( 0 == runMgr  () ) { sc = retrieveRunManager()       ; }
      if( sc.isFailure () ) { Exception("Unable to create IGiGaRunManager!");}
      sc = runMgr()->declare( obj ) ;
      if( sc.isFailure () ) { Exception("Unable to declare" +
                                        GiGaUtil::ObjTypeName( obj ) ); }
    }
  catch ( const GaudiException& Excpt ) 
    { Exception( "operator<<(G4VUserDetectorConstruction*)" , Excpt) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "operator<<(G4VUserDetectorConstruction*)" , Excpt) ; } 
  catch(...)                            
    { Exception( "operator<<(G4VUserDetectorConstruction*)"        ) ; }
  ///
  return *this;
}

// ============================================================================
/** set new world wolume 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  NB: errors are reported through exception thrown 
 * 
 *  @param  obj    pointer to  new world volume   
 *  @return self-reference ot IGiGaSetUpSvc interface 
 */
// ============================================================================
IGiGaSetUpSvc& GiGa::operator << ( G4VPhysicalVolume             * obj )
{ 
  try
    {
      StatusCode sc = StatusCode::SUCCESS;
      if( 0 == runMgr  () ) { sc = retrieveRunManager()       ; }
      if( sc.isFailure () ) { Exception("Unable to create IGiGaRunManager!");}
      sc = runMgr()->declare( obj ) ;
      if( sc.isFailure () ) { Exception("Unable to declare" +
                                        GiGaUtil::ObjTypeName( obj ) ); }
    }
  catch ( const GaudiException& Excpt ) 
    { Exception( "operator<<(G4VPhysicalVolume*)" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "operator<<(G4VPhysicalVolume*)" , Excpt ) ; } 
  catch(...)                            
    { Exception( "operator<<(G4VPhysicalVolume*)"         ) ; }
  ///
  return *this;
}

// ============================================================================
/** set new generator 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  NB: errors are reported throw exception
 * 
 *  @param  obj        pointer to new generator   
 *  @return self-reference ot IGiGaSetUpSvc interface 
 */
// ============================================================================
IGiGaSetUpSvc& GiGa::operator << ( G4VUserPrimaryGeneratorAction * obj )
{ 
  try
    {
      StatusCode sc = StatusCode::SUCCESS;
      if( 0 == runMgr  () )
        { sc = Error("operator<< : IGiGaRunManager* points to NULL!");}
      if( sc.isFailure () ) { Exception("Unable to create IGiGaRunManager!");}
      sc = runMgr()->declare( obj ) ;
      if( sc.isFailure () ) { Exception("Unable to declare" +
                                        GiGaUtil::ObjTypeName( obj ) ); }
    }
  catch ( const GaudiException& Excpt ) 
    { Exception( "operator<<(G4VUserPrimaryGenerator*)" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "operator<<(G4VUserPrimaryGenerator*)" , Excpt ) ; } 
  catch(...)                            
    { Exception( "operator<<(G4VUserPrimaryGenerator*)"         ) ; }
  ///
  return *this;
}

// ============================================================================
/** set new physics list 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  NB: errors are reported through  exception thrown 
 * 
 *  @param  obj      pointer to physics list    
 *  @return self-reference ot IGiGaSetUpSvc interface 
 */
// ============================================================================
IGiGaSetUpSvc& GiGa::operator << ( G4VUserPhysicsList            * obj )
{
  try
    {
      StatusCode sc = StatusCode::SUCCESS;
      if( 0 == runMgr  () ) { sc = retrieveRunManager()       ; }
      if( 0 == runMgr  () )
        { sc = Error("operator<< : IGiGaRunManager* points to NULL!");}
      if( sc.isFailure () ) { Exception("Unable to create IGiGaRunManager!");}
      sc = runMgr()->declare( obj ) ;
      if( sc.isFailure () ) { Exception("Unable to declare" +
                                        GiGaUtil::ObjTypeName( obj ) ); }
    }
  catch ( const GaudiException& Excpt ) 
    { Exception( "operator<<(G4VUserPhysicsList*)" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "operator<<(G4VUserPhysicsList*)" , Excpt ) ; } 
  catch(...)                            
    { Exception( "operator<<(G4VUserPhysicsList*)"         ) ; }
  ///
  return *this;
}

// ============================================================================
/** set new run action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  NB: errors are reported through exception thrown 
 * 
 *  @param  obj     pointer to new run action     
 *  @return self-reference ot IGiGaSetUpSvc interface 
 */
// ============================================================================
IGiGaSetUpSvc& GiGa::operator << ( G4UserRunAction               * obj )
{
  try
    {
      StatusCode sc = StatusCode::SUCCESS;
      if( 0 == runMgr  () ) { sc = retrieveRunManager()       ; }
      if( 0 == runMgr  () )
        { sc = Error("operator<< : IGiGaRunManager* points to NULL!");}
      if( sc.isFailure () ) { Exception("Unable to create IGiGaRunManager!");}
      sc = runMgr()->declare( obj ) ;
      if( sc.isFailure () ) { Exception("Unable to declare" +
                                        GiGaUtil::ObjTypeName( obj ) ); }
    }
  catch ( const GaudiException& Excpt ) 
    { Exception( "operator<<(G4UserRunAction*)" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "operator<<(G4UserRunAction*)" , Excpt ) ; } 
  catch(...)                            
    { Exception( "operator<<(G4UserRunAction*)"         ) ; }
  ///
  return *this;
}

// ============================================================================
/** set new event action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  NB: errors are reported through exception thrown 
 * 
 *  @param  obj     pointer to new event action     
 *  @return self-reference ot IGiGaSetUpSvc interface 
 */
// ============================================================================
IGiGaSetUpSvc& GiGa::operator << ( G4UserEventAction             * obj )
{
  try
    {
      StatusCode sc = StatusCode::SUCCESS;
      if( 0 == runMgr  () ) { sc = retrieveRunManager()       ; }
      if( 0 == runMgr  () )
        { sc = Error("operator<< : IGiGaRunManager* points to NULL!");}
      if( sc.isFailure () ) { Exception("Unable to create IGiGaRunManager!");}
      sc = runMgr()->declare( obj ) ;
      if( sc.isFailure () ) { Exception("Unable to declare" +
                                        GiGaUtil::ObjTypeName( obj ) ); }
    }
  catch ( const GaudiException& Excpt ) 
    { Exception( "operator<<(G4UserEventAction*)" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "operator<<(G4UserEventAction*)" , Excpt ) ; } 
  catch(...)                            
    { Exception( "operator<<(G4UserEventAction*)"         ) ; }
  ///
  return *this;
}

// ============================================================================
/** set new stacking action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  NB: errors are reported through  exception thrown 
 * 
 *  @param  obj     pointer to new stacking action     
 *  @return self-reference ot IGiGaSetUpSvc interface 
 */
// ============================================================================
IGiGaSetUpSvc& GiGa::operator << ( G4UserStackingAction          * obj ) 
{
  try
    {
      StatusCode sc = StatusCode::SUCCESS;
      if( 0 == runMgr  () ) { sc = retrieveRunManager()       ; }
      if( sc.isFailure () ) { Exception("Unable to create IGiGaRunManager!");}
      sc = runMgr()->declare( obj ) ;
      if( sc.isFailure () ) { Exception("Unable to declare" +
                                        GiGaUtil::ObjTypeName( obj ) ); }
    }
  catch ( const GaudiException& Excpt ) 
    { Exception( "operator<<(G4UserStackingAction*)" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "operator<<(G4UserStackingAction*)" , Excpt ) ; } 
  catch(...)                            
    { Exception( "operator<<(G4UserStackingAction*)"         ) ; }
  ///
  return *this;
}

    
// ============================================================================
/** set new tracking  action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  NB: errors are reported through exception thrown 
 * 
 *  @param  obj     pointer to new tracking action     
 *  @return self-reference ot IGiGaSetUpSvc interface 
 */
// ============================================================================
IGiGaSetUpSvc& GiGa::operator << ( G4UserTrackingAction          * obj ) 
{
  try
    {
      StatusCode sc = StatusCode::SUCCESS;
      if( 0 == runMgr  () ) { sc = retrieveRunManager()       ; }
      if( sc.isFailure () ) { Exception("Unable to create IGiGaRunManager!");}
      sc = runMgr()->declare( obj ) ;
      if( sc.isFailure () ) { Exception("Unable to declare" +
                                        GiGaUtil::ObjTypeName( obj ) ); }
    }
  catch ( const GaudiException& Excpt ) 
    { Exception( "operator<<(G4UserTrackingAction*)" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "operator<<(G4UserTrackingAction*)" , Excpt ) ; } 
  catch(...)                            
    { Exception( "operator<<(G4UserTrackingAction*)"         ) ; }
  ///
  return *this;
}

// ============================================================================
/** set new stepping  action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  NB: errors are reported through exception thrown 
 * 
 *  @param  obj     pointer to new stepping action     
 *  @return self-reference ot IGiGaSetUpSvc interface 
 */
// ============================================================================
IGiGaSetUpSvc& GiGa::operator << ( G4UserSteppingAction          * obj ) 
{ 
  try
    {
      StatusCode sc = StatusCode::SUCCESS;
      if( 0 == runMgr  () ) { sc = retrieveRunManager()       ; }
      if( sc.isFailure () ) { Exception("Unable to create IGiGaRunManager!");}
      sc = runMgr()->declare( obj ) ;
      if( sc.isFailure () ) { Exception("Unable to declare" +
                                        GiGaUtil::ObjTypeName( obj ) ); }
    }
  catch ( const GaudiException& Excpt ) 
    { Exception( "operator<<(G4UserSteppingAction*)" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "operator<<(G4UserSteppingAction*)" , Excpt ) ; } 
  catch(...)                            
    { Exception( "operator<<(G4UserSteppingAction*)"         ) ; }
  ///
  return *this;
}

// ============================================================================
/** set detector constructon module 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj      pointer to detector construction module  
 *  @return self-reference ot IGiGaSetUpSvc interface 
 */
// ============================================================================
StatusCode GiGa::setConstruction ( G4VUserDetectorConstruction   * obj )
{
  try { *this << obj ; }
  catch ( const GaudiException& Excpt ) 
    { Exception( "setConstruction()" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "setConstruction()" , Excpt ) ; } 
  catch(...)                            
    { Exception( "setConstruction()"         ) ; }
  ///
  return StatusCode::SUCCESS;
}

// ============================================================================
/** set new world wolume 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj    pointer to  new world volume   
 *  @return status code  
 */
// ============================================================================
StatusCode GiGa::setDetector     ( G4VPhysicalVolume             * obj )
{
  try { *this << obj ; }
  catch ( const GaudiException& Excpt ) 
    { Exception( "setDetector()" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "setDetector()" , Excpt ) ; } 
  catch(...)                            
    { Exception( "setDetector()"         ) ; }
  ///
  return StatusCode::SUCCESS;
}

// ============================================================================
/** set new generator 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj        pointer to new generator   
 *  @return status code  
 */
// ============================================================================
StatusCode GiGa::setGenerator    ( G4VUserPrimaryGeneratorAction * obj )
{
  try { *this << obj ; }
  catch ( const GaudiException& Excpt ) 
    { Exception( "setGenerator()" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "setGenerator()" , Excpt ) ; } 
  catch(...)                            
    { Exception( "setGenerator()"         ) ; }
  ///
  return StatusCode::SUCCESS;
}

// ============================================================================
/** set new physics list 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj      pointer to physics list    
 *  @return status code  
 */
// ============================================================================
StatusCode GiGa::setPhysics      ( G4VUserPhysicsList            * obj )
{
  try { *this << obj ; }
  catch ( const GaudiException& Excpt ) 
    { Exception( "setPhysics()" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "setPhysics()" , Excpt ) ; } 
  catch(...)                            
    { Exception( "setPhysics()"         ) ; }
  ///
  return StatusCode::SUCCESS;
}

// ============================================================================
/** set new run action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj     pointer to new run action     
 *  @return status code  
 */
// ============================================================================
StatusCode GiGa::setRunAction    ( G4UserRunAction               * obj )
{
  try { *this << obj ; }
  catch ( const GaudiException& Excpt ) 
    { Exception( "setRunAction()" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "setRunAction()" , Excpt ) ; } 
  catch(...)                            
    { Exception( "setRunAction()"         ) ; }
  ///
  return StatusCode::SUCCESS;
}

// ============================================================================
/** set new event action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj     pointer to new event action     
 *  @return status code  
 */
// ============================================================================
StatusCode GiGa::setEvtAction    ( G4UserEventAction             * obj )
{
  try { *this << obj ; }
  catch ( const GaudiException& Excpt ) 
    { Exception( "setEvtAction()" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "setEvtAction()" , Excpt ) ; } 
  catch(...)                            
    { Exception( "setEvtAction()"         ) ; }
  ///
  return StatusCode::SUCCESS;
}

// ============================================================================
/** set new stacking action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj     pointer to new stacking action     
 *  @return status code  
 */
// ============================================================================
StatusCode GiGa::setStacking     ( G4UserStackingAction          * obj )
{
  try { *this << obj ; }
  catch ( const GaudiException& Excpt ) 
    { Exception( "setStacking()" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "setStacking()" , Excpt ) ; } 
  catch(...)                            
    { Exception( "setStacking()"         ) ; }
  ///
  return StatusCode::SUCCESS;
}

// ============================================================================
/** set new tracking  action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj     pointer to new tracking action     
 *  @return status code  
 */
// ============================================================================
StatusCode GiGa::setTracking     ( G4UserTrackingAction          * obj )
{
  try { *this << obj ; }
  catch ( const GaudiException& Excpt ) 
    { Exception( "setTracking()" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "setTracking()" , Excpt ) ; } 
  catch(...)                            
    { Exception( "setTracking()"         ) ; }
  ///
  return StatusCode::SUCCESS;
}

// ============================================================================
/** set new stepping  action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj     pointer to new stepping action     
 *  @return status code  
 */
// ============================================================================
StatusCode GiGa::setStepping     ( G4UserSteppingAction          * obj )
{
  try { *this << obj ; }
  catch ( const GaudiException& Excpt ) 
    { Exception( "setStepping()" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { Exception( "setStepping()" , Excpt ) ; } 
  catch(...)                            
    { Exception( "setStepping()"         ) ; }
  ///
  return StatusCode::SUCCESS;
}

// ============================================================================
// The END 
// ============================================================================

