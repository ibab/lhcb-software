/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
#define GIGA_GIGASVCIGIGASETUPSVC_CPP 1 
/// ===========================================================================
/// include files 
/// STD & ATL 
#include <string>
#include <typeinfo> 
/// GaudiKernel 
#include  "GaudiKernel/Kernel.h"
#include  "GaudiKernel/StatusCode.h"
#include  "GaudiKernel/System.h" 
#include  "GaudiKernel/MsgStream.h" 
/// GiGa
#include "GiGa/GiGaException.h"
#include "GiGa/GiGaRunManager.h"
/// local 
#include "GiGaSvc.h"


/// ===========================================================================
/**  Implementation of class GiGaSvc  
 *   all methods from abstract interface IGiGaSetUpSvc 
 *
 *   @author: Vanya Belyaev 
 *   @date xx/xx/xxxx
 */
/// ===========================================================================

/// ===========================================================================
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

/// ===========================================================================
#define ___GIGA_MACRO_TRICK2___ \
        { StatusCode sc( StatusCode::FAILURE ); \
          ___GIGA_TRY___ { \
          *this << obj ; \
          } ___GIGA_CATCH_PRINT_AND_RETURN___(name()," ",msgSvc(), \
          chronoSvc(),sc); \
          return StatusCode::SUCCESS ; }

/// ===========================================================================
/** set detector constructon module 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  NB: errors are reported through exception thrown 
 *
 *  @param  obj      pointer to detector construction module  
 *  @return self-reference ot IGiGaSetUpSvc interface 
 */
/// ===========================================================================
IGiGaSetUpSvc& GiGaSvc::operator << ( G4VUserDetectorConstruction   * obj )
{  ___GIGA_MACRO_TRICK1___; };

/// ===========================================================================
/** set new world wolume 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  NB: errors are reported through exception thrown 
 * 
 *  @param  obj    pointer to  new world volume   
 *  @return self-reference ot IGiGaSetUpSvc interface 
 */
/// ===========================================================================
IGiGaSetUpSvc& GiGaSvc::operator << ( G4VPhysicalVolume             * obj )
{  ___GIGA_MACRO_TRICK1___; }; 

/// ===========================================================================
/** set new generator 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  NB: errors are reported throw exception
 * 
 *  @param  obj        pointer to new generator   
 *  @return self-reference ot IGiGaSetUpSvc interface 
 */
/// ===========================================================================
IGiGaSetUpSvc& GiGaSvc::operator << ( G4VUserPrimaryGeneratorAction * obj )
{  ___GIGA_MACRO_TRICK1___; }; 

/// ===========================================================================
/** set new physics list 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  NB: errors are reported through  exception thrown 
 * 
 *  @param  obj      pointer to physics list    
 *  @return self-reference ot IGiGaSetUpSvc interface 
 */
/// ===========================================================================
IGiGaSetUpSvc& GiGaSvc::operator << ( G4VUserPhysicsList            * obj )
{  ___GIGA_MACRO_TRICK1___; }; 

/// ===========================================================================
/** set new run action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  NB: errors are reported through exception thrown 
 * 
 *  @param  obj     pointer to new run action     
 *  @return self-reference ot IGiGaSetUpSvc interface 
 */
/// ===========================================================================
IGiGaSetUpSvc& GiGaSvc::operator << ( G4UserRunAction               * obj )
{  ___GIGA_MACRO_TRICK1___; }; 

/// ===========================================================================
/** set new event action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  NB: errors are reported through exception thrown 
 * 
 *  @param  obj     pointer to new event action     
 *  @return self-reference ot IGiGaSetUpSvc interface 
 */
/// ===========================================================================
IGiGaSetUpSvc& GiGaSvc::operator << ( G4UserEventAction             * obj )
{  ___GIGA_MACRO_TRICK1___; }; 

/// ===========================================================================
/** set new stacking action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  NB: errors are reported through  exception thrown 
 * 
 *  @param  obj     pointer to new stacking action     
 *  @return self-reference ot IGiGaSetUpSvc interface 
 */
/// ===========================================================================
IGiGaSetUpSvc& GiGaSvc::operator << ( G4UserStackingAction          * obj ) 
{  ___GIGA_MACRO_TRICK1___; }; 

/// ===========================================================================
/** set new tracking  action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  NB: errors are reported through exception thrown 
 * 
 *  @param  obj     pointer to new tracking action     
 *  @return self-reference ot IGiGaSetUpSvc interface 
 */
/// ===========================================================================
IGiGaSetUpSvc& GiGaSvc::operator << ( G4UserTrackingAction          * obj ) 
{  ___GIGA_MACRO_TRICK1___; }; 

/// ===========================================================================
/** set new stepping  action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  NB: errors are reported through exception thrown 
 * 
 *  @param  obj     pointer to new stepping action     
 *  @return self-reference ot IGiGaSetUpSvc interface 
 */
/// ===========================================================================
IGiGaSetUpSvc& GiGaSvc::operator << ( G4UserSteppingAction          * obj ) 
{  ___GIGA_MACRO_TRICK1___; }; 

/// ===========================================================================
/** set new visualization manager  
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  NB: errors are reported through exception thrown 
 * 
 *  @param  obj            pointer to new visualization manager      
 *  @return self-reference ot IGiGaSetUpSvc interface 
 */
/// ===========================================================================
IGiGaSetUpSvc& GiGaSvc::operator << ( G4VisManager                  * obj ) 
{  ___GIGA_MACRO_TRICK1___; }; 

/// ===========================================================================
/** set detector constructon module 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj      pointer to detector construction module  
 *  @return self-reference ot IGiGaSetUpSvc interface 
 */
/// ===========================================================================
StatusCode GiGaSvc::setConstruction ( G4VUserDetectorConstruction   * obj )
{  ___GIGA_MACRO_TRICK2___; }; 

/// ===========================================================================
/** set new world wolume 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj    pointer to  new world volume   
 *  @return status code  
 */
/// ===========================================================================
StatusCode GiGaSvc::setDetector     ( G4VPhysicalVolume             * obj )
{  ___GIGA_MACRO_TRICK2___; }; 

/// ===========================================================================
/** set new generator 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj        pointer to new generator   
 *  @return status code  
 */
/// ===========================================================================
StatusCode GiGaSvc::setGenerator    ( G4VUserPrimaryGeneratorAction * obj )
{  ___GIGA_MACRO_TRICK2___; }; 

/// ===========================================================================
/** set new physics list 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj      pointer to physics list    
 *  @return status code  
 */
/// ===========================================================================
StatusCode GiGaSvc::setPhysics      ( G4VUserPhysicsList            * obj )
{  ___GIGA_MACRO_TRICK2___; }; 

/// ===========================================================================
/** set new run action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj     pointer to new run action     
 *  @return status code  
 */
/// ===========================================================================
StatusCode GiGaSvc::setRunAction    ( G4UserRunAction               * obj )
{  ___GIGA_MACRO_TRICK2___; }; 

/// ===========================================================================
/** set new event action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj     pointer to new event action     
 *  @return status code  
 */
/// ===========================================================================
StatusCode GiGaSvc::setEvtAction    ( G4UserEventAction             * obj )
{  ___GIGA_MACRO_TRICK2___; }; 

/// ===========================================================================
/** set new stacking action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj     pointer to new stacking action     
 *  @return status code  
 */
/// ===========================================================================
StatusCode GiGaSvc::setStacking     ( G4UserStackingAction          * obj )
{  ___GIGA_MACRO_TRICK2___; }; 

/// ===========================================================================
/** set new tracking  action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj     pointer to new tracking action     
 *  @return status code  
 */
/// ===========================================================================
StatusCode GiGaSvc::setTracking     ( G4UserTrackingAction          * obj )
{  ___GIGA_MACRO_TRICK2___; }; 

/// ===========================================================================
/** set new stepping  action 
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj     pointer to new stepping action     
 *  @return status code  
 */
/// ===========================================================================
StatusCode GiGaSvc::setStepping     ( G4UserSteppingAction          * obj )
{  ___GIGA_MACRO_TRICK2___; }; 

/// ===========================================================================
/** set new visualization manager  
 *               implementation of IGiGaSetUpSvc abstract interface 
 *
 *  @param  obj            pointer to new visualization manager      
 *  @return status code  
 */
/// ===========================================================================
StatusCode GiGaSvc::setVisManager   ( G4VisManager                  * obj )
{  ___GIGA_MACRO_TRICK2___; }; 

/// ===========================================================================

