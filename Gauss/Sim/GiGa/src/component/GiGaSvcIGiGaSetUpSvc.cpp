
#include <string>
#include <typeinfo> 

#include  "GaudiKernel/Kernel.h"
#include  "GaudiKernel/StatusCode.h"
#include  "GaudiKernel/System.h" 
#include  "GaudiKernel/MsgStream.h" 


#include "GiGa/GiGaException.h"
#include "GiGa/GiGaRunManager.h"


///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///                                                                             ///
///    class GiGaSvc : implementation of abstract Interfaces IGiGaSvc           ///      
///                    (for event-by-event communications with Geant4)          ///
///                    and IGiGaSetUpSvc (for configuration of Geant4)          ///
///                                                                             ///
///    Author: Vanya Belyaev                                                    ///
///                                                                             ///
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
/// *             implementation of IGiGaSetUpSvc interface                   * ///
/// *************************************************************************** /// 
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
/// *             set detector construction                                   * ///
/// *************************************************************************** ///
/// *             define primary generator action                             * ///
/// *************************************************************************** ///
/// *             define Physics List                                         * ///
/// *************************************************************************** ///
/// *             define Run Action                                           * ///
/// *************************************************************************** ///
/// *             define Event Action                                         * ///
/// *************************************************************************** ///
/// *             define Stacking Action                                      * ///
/// *************************************************************************** ///
/// *             define Tracking Action                                      * ///
/// *************************************************************************** ///
/// *             define Stepping Action                                      * ///
/// *************************************************************************** ///
/// *             define Visualisation Manager                                * ///
/// *************************************************************************** ///
/// *             define User Interface Session                               * ///
/// *************************************************************************** /// 
///////////////////////////////////////////////////////////////////////////////////


#define ___GIGA_MACRO_TRICK1___ \
        { const std::string _name( System::typeinfoName( typeid( obj ) ) ) ; \
        ___GIGA_TRY___ { \
        StatusCode _sc( StatusCode::SUCCESS ); \
        if( 0 == m_GiGaRunManager ){ _sc = createGiGaRunManager() ;} \
        Assert( 0 != m_GiGaRunManager," Unable to create GiGaRunManager "); \
        Assert( _sc.isSuccess() ," Unable to create GiGaRunManager! ", _sc ); \
        _sc = m_GiGaRunManager->declare( obj ); \
        Assert( _sc.isSuccess() ," Unable to declare " + _name, _sc ); \
        } ___GIGA_CATCH_AND_THROW___( "GiGaSvc::operator<<" +_name , "::declare()" ); \
        return *this; };


#define ___GIGA_MACRO_TRICK2___ \
        { StatusCode sc( StatusCode::FAILURE ); \
          ___GIGA_TRY___ { \
          *this << obj ; \
          } ___GIGA_CATCH_PRINT_AND_RETURN___(name()," ",msgSvc(),chronoSvc(),sc); \
          return StatusCode::SUCCESS ; }


/// local 
#include "GiGaSvc.h"

IGiGaSetUpSvc& GiGaSvc::operator << ( G4VUserDetectorConstruction   * obj )
{  ___GIGA_MACRO_TRICK1___; }; 
///////////////////////////////////////////////////////////////////////////////////
IGiGaSetUpSvc& GiGaSvc::operator << ( G4VPhysicalVolume             * obj )
{  ___GIGA_MACRO_TRICK1___; }; 
///////////////////////////////////////////////////////////////////////////////////
IGiGaSetUpSvc& GiGaSvc::operator << ( G4VUserPrimaryGeneratorAction * obj )
{  ___GIGA_MACRO_TRICK1___; }; 
///////////////////////////////////////////////////////////////////////////////////
IGiGaSetUpSvc& GiGaSvc::operator << ( G4VUserPhysicsList            * obj )
{  ___GIGA_MACRO_TRICK1___; }; 
///////////////////////////////////////////////////////////////////////////////////
IGiGaSetUpSvc& GiGaSvc::operator << ( G4UserRunAction               * obj )
{  ___GIGA_MACRO_TRICK1___; }; 
///////////////////////////////////////////////////////////////////////////////////
IGiGaSetUpSvc& GiGaSvc::operator << ( G4UserEventAction             * obj )
{  ___GIGA_MACRO_TRICK1___; }; 
///////////////////////////////////////////////////////////////////////////////////
IGiGaSetUpSvc& GiGaSvc::operator << ( G4UserStackingAction          * obj ) 
{  ___GIGA_MACRO_TRICK1___; }; 
///////////////////////////////////////////////////////////////////////////////////
IGiGaSetUpSvc& GiGaSvc::operator << ( G4UserTrackingAction          * obj ) 
{  ___GIGA_MACRO_TRICK1___; }; 
///////////////////////////////////////////////////////////////////////////////////
IGiGaSetUpSvc& GiGaSvc::operator << ( G4UserSteppingAction          * obj ) 
{  ___GIGA_MACRO_TRICK1___; }; 
///////////////////////////////////////////////////////////////////////////////////
IGiGaSetUpSvc& GiGaSvc::operator << ( G4VisManager                  * obj ) 
{  ___GIGA_MACRO_TRICK1___; }; 
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaSvc::setConstruction ( G4VUserDetectorConstruction   * obj )
{  ___GIGA_MACRO_TRICK2___; }; 
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaSvc::setDetector     ( G4VPhysicalVolume             * obj )
{  ___GIGA_MACRO_TRICK2___; }; 
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaSvc::setGenerator    ( G4VUserPrimaryGeneratorAction * obj )
{  ___GIGA_MACRO_TRICK2___; }; 
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaSvc::setPhysics      ( G4VUserPhysicsList            * obj )
{  ___GIGA_MACRO_TRICK2___; }; 
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaSvc::setRunAction    ( G4UserRunAction               * obj )
{  ___GIGA_MACRO_TRICK2___; }; 
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaSvc::setEvtAction    ( G4UserEventAction             * obj )
{  ___GIGA_MACRO_TRICK2___; }; 
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaSvc::setStacking     ( G4UserStackingAction          * obj )
{  ___GIGA_MACRO_TRICK2___; }; 
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaSvc::setTracking     ( G4UserTrackingAction          * obj )
{  ___GIGA_MACRO_TRICK2___; }; 
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaSvc::setStepping     ( G4UserSteppingAction          * obj )
{  ___GIGA_MACRO_TRICK2___; }; 
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaSvc::setVisManager   ( G4VisManager                  * obj )
{  ___GIGA_MACRO_TRICK2___; }; 
///////////////////////////////////////////////////////////////////////////////////
