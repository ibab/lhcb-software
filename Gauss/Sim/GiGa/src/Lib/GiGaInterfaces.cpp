// $Id: GiGaInterfaces.cpp,v 1.2 2004-04-20 04:26:06 ibelyaev Exp $
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $
// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2004/02/20 18:58:20  ibelyaev
//  update in IGiGaXXX and IIDIGiGaXXX
//
// ===========================================================================
// Include files
// ===========================================================================
// GiGa
// ===========================================================================
#include "GiGa/IGiGaEventAction.h"
#include "GiGa/IGiGaFieldMgr.h"
#include "GiGa/IGiGaGeoSrc.h"
#include "GiGa/IGiGaInterface.h"
#include "GiGa/IGiGaMagField.h"
#include "GiGa/IGiGaPhysConstructor.h"
#include "GiGa/IGiGaPhysicsConstructor.h"
#include "GiGa/IGiGaPhysicsList.h"
#include "GiGa/IGiGaPhysList.h"
#include "GiGa/IGiGaRunAction.h"
#include "GiGa/IGiGaRunManager.h"
#include "GiGa/IGiGaSensDet.h"
#include "GiGa/IGiGaSetUpSvc.h"
#include "GiGa/IGiGaStackAction.h"
#include "GiGa/IGiGaStepAction.h"
#include "GiGa/IGiGaSvc.h"
#include "GiGa/IGiGaTool.h"
#include "GiGa/IGiGaTrackAction.h"
#include "GiGa/IGiGaUIsession.h"
#include "GiGa/IGiGaVisManager.h"
// ===========================================================================a
// Local
// ===========================================================================
#include "IIDIGiGaEventAction.h"
#include "IIDIGiGaFieldMgr.h"
#include "IIDIGiGaGeoSrc.h"
#include "IIDIGiGaInterface.h"
#include "IIDIGiGaMagField.h"
#include "IIDIGiGaPhysConstructor.h"
#include "IIDIGiGaPhysicsConstructor.h"
#include "IIDIGiGaPhysicsList.h"
#include "IIDIGiGaPhysList.h"
#include "IIDIGiGaRunAction.h"
#include "IIDIGiGaRunManager.h"
#include "IIDIGiGaSensDet.h"
#include "IIDIGiGaSetUpSvc.h"
#include "IIDIGiGaStackAction.h"
#include "IIDIGiGaStepAction.h"
#include "IIDIGiGaSvc.h"
#include "IIDIGiGaTool.h"
#include "IIDIGiGaTrackAction.h"
#include "IIDIGiGaUIsession.h"
#include "IIDIGiGaVisManager.h"
// ===========================================================================

/** @file
 *
 *  implement empty destructors for all GiGa interfaces
 *
 *  @date 2004-02-20
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */

// ===========================================================================
IGiGaEventAction          :: ~IGiGaEventAction          ()   {}
IGiGaFieldMgr             :: ~IGiGaFieldMgr             ()   {}
IGiGaGeoSrc               :: ~IGiGaGeoSrc               ()   {}
IGiGaInterface            :: ~IGiGaInterface            ()   {}
IGiGaMagField             :: ~IGiGaMagField             ()   {}
IGiGaPhysConstructor      :: ~IGiGaPhysConstructor      ()   {}
IGiGaPhysicsConstructor   :: ~IGiGaPhysicsConstructor   ()   {}
IGiGaPhysicsList          :: ~IGiGaPhysicsList          ()   {}
IGiGaPhysList             :: ~IGiGaPhysList             ()   {}
IGiGaRunAction            :: ~IGiGaRunAction            ()   {}
IGiGaRunManager           :: ~IGiGaRunManager           ()   {}
IGiGaSensDet              :: ~IGiGaSensDet              ()   {}
IGiGaSetUpSvc             :: ~IGiGaSetUpSvc             ()   {}
IGiGaStackAction          :: ~IGiGaStackAction          ()   {}
IGiGaStepAction           :: ~IGiGaStepAction           ()   {}
IGiGaSvc                  :: ~IGiGaSvc                  ()   {}
IGiGaTool                 :: ~IGiGaTool                 ()   {}
IGiGaTrackAction          :: ~IGiGaTrackAction          ()   {}
IGiGaUIsession            :: ~IGiGaUIsession            ()   {}
IGiGaVisManager           :: ~IGiGaVisManager           ()   {}
// ===========================================================================


// ===========================================================================
const InterfaceID& IGiGaEventAction           :: interfaceID ()
{ return       IID_IGiGaEventAction           ; }
// ===========================================================================
const InterfaceID& IGiGaFieldMgr              :: interfaceID ()
{ return       IID_IGiGaFieldMgr              ; }
// ===========================================================================
const InterfaceID& IGiGaGeoSrc                :: interfaceID ()
{ return       IID_IGiGaGeoSrc                ; }
// ===========================================================================
const InterfaceID& IGiGaInterface             :: interfaceID ()
{ return       IID_IGiGaInterface             ; }
// ===========================================================================
const InterfaceID& IGiGaMagField              :: interfaceID ()
{ return       IID_IGiGaMagField              ; }
// ===========================================================================
const InterfaceID& IGiGaPhysConstructor       :: interfaceID ()
{ return       IID_IGiGaPhysConstructor       ; }
// ===========================================================================
const InterfaceID& IGiGaPhysicsConstructor    :: interfaceID ()
{ return       IID_IGiGaPhysicsConstructor    ; }
// ===========================================================================
const InterfaceID& IGiGaPhysicsList           :: interfaceID ()
{ return       IID_IGiGaPhysicsList           ; }
// ===========================================================================
const InterfaceID& IGiGaPhysList              :: interfaceID ()
{ return       IID_IGiGaPhysList              ; }
// ===========================================================================
const InterfaceID& IGiGaRunAction             :: interfaceID ()
{ return       IID_IGiGaRunAction             ; }
// ===========================================================================
const InterfaceID& IGiGaRunManager            :: interfaceID ()
{ return       IID_IGiGaRunManager            ; }
// ===========================================================================
const InterfaceID& IGiGaSensDet               :: interfaceID ()
{ return       IID_IGiGaSensDet               ; }
// ===========================================================================
const InterfaceID& IGiGaSetUpSvc              :: interfaceID ()
{ return       IID_IGiGaSetUpSvc              ; }
// ===========================================================================
const InterfaceID& IGiGaStackAction           :: interfaceID ()
{ return       IID_IGiGaStackAction           ; }
// ===========================================================================
const InterfaceID& IGiGaStepAction            :: interfaceID ()
{ return       IID_IGiGaStepAction            ; }
// ===========================================================================
const InterfaceID& IGiGaSvc                   :: interfaceID ()
{ return       IID_IGiGaSvc                   ; }
// ===========================================================================
const InterfaceID& IGiGaTool                  :: interfaceID ()
{ return       IID_IGiGaTool                  ; }
// ===========================================================================
const InterfaceID& IGiGaTrackAction           :: interfaceID ()
{ return       IID_IGiGaTrackAction           ; }
// ===========================================================================
const InterfaceID& IGiGaUIsession             :: interfaceID ()
{ return       IID_IGiGaUIsession             ; }
// ===========================================================================
const InterfaceID& IGiGaVisManager            :: interfaceID ()
{ return       IID_IGiGaVisManager            ; }
// ===========================================================================



// ===========================================================================
IGiGaSensDet::IGiGaSensDet ()
  : G4VSensitiveDetector( "NotYetDefined" ){}
// ===========================================================================

IGiGaRunManager* IGiGaRunManager::s_manager = 0 ;
IGiGaRunManager* IGiGaRunManager::instance() { return s_manager ; }
StatusCode       IGiGaRunManager::setInstance( IGiGaRunManager* value ) 
{ s_manager = value ; return StatusCode::SUCCESS ; }
StatusCode       IGiGaRunManager::destroy() 
{
  IGiGaRunManager* mgr = IGiGaRunManager::instance() ;
  if( 0 != mgr ) { delete mgr ; }
  IGiGaRunManager::setInstance( (IGiGaRunManager*)0 );
  return StatusCode::SUCCESS ;
}

// ===========================================================================
// The END
// ===========================================================================
