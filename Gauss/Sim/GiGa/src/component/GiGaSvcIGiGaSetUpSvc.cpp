// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.7  2001/08/12 15:42:55  ibelyaev
/// improvements with Doxygen comments
///
/// Revision 1.6  2001/08/01 09:42:24  ibelyaev
/// redesign and reimplementation of GiGaRunManager class
///
/// Revision 1.5  2001/07/23 13:12:29  ibelyaev
/// the package restructurisation(II)
///
/// Revision 1.4  2001/07/15 20:54:36  ibelyaev
/// package restructurisation
/// 
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
#include "GiGaSvc.h"


// ============================================================================
/**  Implementation of class GiGaSvc  
 *   all methods from abstract interface IGiGaSetUpSvc 
 *
 *   @author: Vanya Belyaev 
 *   @date xx/xx/xxxx
 */
// ============================================================================

// ============================================================================
#define ___GIGA_MACRO_TRICK1___ \
        { const std::string _name( System::typeinfoName( typeid( obj ) ) ) ; \
        ___GIGA_TRY___ { \
        StatusCode _sc( StatusCode::SUCCESS ); \
        if( 0 == m_GiGaRunManager ){ _sc = createGiGaRunManager() ;} \
        Assert( 0 != m_GiGaRunManager," Unable to create GiGaRunManager "); \
        Assert( _sc.isSuccess() ," Unable to create GiGaRunManager! ", _sc ); \
        _sc = m_GiGaRunManager->declare( obj ); \
        Assert( _sc.isSuccess() ," Unable to declare " + _name, _sc ); \
        } ___GIGA_CATCH_AND_THROW___( "GiGaSvc::operator<<" +_name , \
        "::declare()" ); \
        return *this; };

// ============================================================================
#define ___GIGA_MACRO_TRICK2___ \
        { StatusCode sc( StatusCode::FAILURE ); \
          ___GIGA_TRY___ { \
          *this << obj ; \
          } ___GIGA_CATCH_PRINT_AND_RETURN___(name()," ",msgSvc(), \
          chronoSvc(),sc); \
          return StatusCode::SUCCESS ; }

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
IGiGaSetUpSvc& GiGaSvc::operator << ( G4VUserDetectorConstruction   * obj )
{
  try
    {
      StatusCode sc = StatusCode::SUCCESS;
      if( 0 == runMgr  () ) { sc = createGiGaRunManager(); }
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
};

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
IGiGaSetUpSvc& GiGaSvc::operator << ( G4VPhysicalVolume             * obj )
{ 
  try
    {
      StatusCode sc = StatusCode::SUCCESS;
      if( 0 == runMgr  () ) { sc = createGiGaRunManager(); }
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
}; 

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
IGiGaSetUpSvc& GiGaSvc::operator << ( G4VUserPrimaryGeneratorAction * obj )
{ 
  try
    {
      StatusCode sc = StatusCode::SUCCESS;
      if( 0 == runMgr  () ) { sc = createGiGaRunManager(); }
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
}; 

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
IGiGaSetUpSvc& GiGaSvc::operator << ( G4VUserPhysicsList            * obj )
{
  try
    {
      StatusCode sc = StatusCode::SUCCESS;
      if( 0 == runMgr  () ) { sc = createGiGaRunManager(); }
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
}; 

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
IGiGaSetUpSvc& GiGaSvc::operator << ( G4UserRunAction               * obj )
{
  try
    {
      StatusCode sc = StatusCode::SUCCESS;
      if( 0 == runMgr  () ) { sc = createGiGaRunManager(); }
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
}; 

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
IGiGaSetUpSvc& GiGaSvc::operator << ( G4UserEventAction             * obj )
{
  try
    {
      StatusCode sc = StatusCode::SUCCESS;
      if( 0 == runMgr  () ) { sc = createGiGaRunManager(); }
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
}; 

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
IGiGaSetUpSvc& GiGaSvc::operator << ( G4UserStackingAction          * obj ) 
{
  try
    {
      StatusCode sc = StatusCode::SUCCESS;
      if( 0 == runMgr  () ) { sc = createGiGaRunManager(); }
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
};

    
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
IGiGaSetUpSvc& GiGaSvc::operator << ( G4UserTrackingAction          * obj ) 
{
  try
    {
      StatusCode sc = StatusCode::SUCCESS;
      if( 0 == runMgr  () ) { sc = createGiGaRunManager(); }
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
};

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
IGiGaSetUpSvc& GiGaSvc::operator << ( G4UserSteppingAction          * obj ) 
{ 
  try
    {
      StatusCode sc = StatusCode::SUCCESS;
      if( 0 == runMgr  () ) { sc = createGiGaRunManager(); }
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
};

// ============================================================================
/** set detector constructon module 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj      pointer to detector construction module  
 *  @return self-reference ot IGiGaSetUpSvc interface 
 */
// ============================================================================
StatusCode GiGaSvc::setConstruction ( G4VUserDetectorConstruction   * obj )
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
};

// ============================================================================
/** set new world wolume 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj    pointer to  new world volume   
 *  @return status code  
 */
// ============================================================================
StatusCode GiGaSvc::setDetector     ( G4VPhysicalVolume             * obj )
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
};

// ============================================================================
/** set new generator 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj        pointer to new generator   
 *  @return status code  
 */
// ============================================================================
StatusCode GiGaSvc::setGenerator    ( G4VUserPrimaryGeneratorAction * obj )
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
};

// ============================================================================
/** set new physics list 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj      pointer to physics list    
 *  @return status code  
 */
// ============================================================================
StatusCode GiGaSvc::setPhysics      ( G4VUserPhysicsList            * obj )
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
};

// ============================================================================
/** set new run action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj     pointer to new run action     
 *  @return status code  
 */
// ============================================================================
StatusCode GiGaSvc::setRunAction    ( G4UserRunAction               * obj )
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
};

// ============================================================================
/** set new event action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj     pointer to new event action     
 *  @return status code  
 */
// ============================================================================
StatusCode GiGaSvc::setEvtAction    ( G4UserEventAction             * obj )
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
};

// ============================================================================
/** set new stacking action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj     pointer to new stacking action     
 *  @return status code  
 */
// ============================================================================
StatusCode GiGaSvc::setStacking     ( G4UserStackingAction          * obj )
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
};

// ============================================================================
/** set new tracking  action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj     pointer to new tracking action     
 *  @return status code  
 */
// ============================================================================
StatusCode GiGaSvc::setTracking     ( G4UserTrackingAction          * obj )
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
};

// ============================================================================
/** set new stepping  action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj     pointer to new stepping action     
 *  @return status code  
 */
// ============================================================================
StatusCode GiGaSvc::setStepping     ( G4UserSteppingAction          * obj )
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
};

// ============================================================================

