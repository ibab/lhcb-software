// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2001/08/12 13:25:23  ibelyaev
// improvements in Doxygen documentation
//
// Revision 1.1  2001/08/01 09:42:23  ibelyaev
// redesign and reimplementation of GiGaRunManager class
// 
// x===========================================================================
/// GaudiKernel
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/System.h"
/// GiGa 
#include "GiGa/GiGa.h"
#include "GiGa/GiGaRunManager.h"
#include "GiGa/GiGaVisManager.h"
/// G4 
#include "G4RunManager.hh"
#include "G4VVisManager.hh"
#include "G4UIsession.hh"
/// G4 
#ifdef G4UI_USE_WO
#include "G4UIWo.hh"   
#endif ///< G4UI_USE_WO 
/// G4 
#ifdef G4UI_USE_GAG
#include "G4UIGAG.hh" 
#endif ///< G4UI_USE_GAG
/// G4 
#ifdef G4UI_USE_XM
#include "G4UIXm.hh"  
#endif ///< G4UI_USE_XM 
/// G4 
#ifdef G4UI_USE_XAW
#include "G4UIXaw.hh"  
#endif ///< G4UI_USE_XAW
/// G4 
#ifdef G4UI_USE_TERMINAL
#include "G4UIterminal.hh"             
#include "G4UItcsh.hh"             
#include "G4UIcsh.hh"             
#endif ///< G4UI_USE_TERMINAL 
/// G4
#ifdef G4UI_USE
#include "G4UIterminal.hh"             
#include "G4UItcsh.hh"             
#include "G4UIcsh.hh"             
#endif ///< G4UI_USE

// ============================================================================
/** @file GiGa.cpp
 * Implementation of methods from namespace GiGa
 * 
 *  @author Vanya Belyaev
 *  @date  31/07/2001 
 */
// ============================================================================

// ============================================================================
/** create GiGa Run Manager
 *  @param   Name name of run manager 
 *  @param   Loc  pointer to service Locator  
 *  @return  pointer to giga run manager abstract interface 
 */     
// ============================================================================
IGiGaRunManager* GiGa::createRunManager( const std::string& Name , 
                                         ISvcLocator*       Loc  )
{
  /// static variable! 
  static IGiGaRunManager* s_runMgr = 0;         ///< NB !!! STATIC !!!
  /// return the existing reference 
  if( 0 != s_runMgr ) { s_runMgr->addRef() ; return s_runMgr ; }
  /// check for other G4 Run Manager 
  if( 0 != G4RunManager::GetRunManager() )   { return 0; }
  ///
  if( 0 == Loc ) { Loc = Gaudi::svcLocator() ; }
  /// 
  s_runMgr = new GiGaRunManager( Name , Loc );
  ///
  return s_runMgr ;
};

// ============================================================================
/** create (and initialize) GiGa Vis manager
 *  return pointer to Visualization Manager 
 */
// ============================================================================
G4VVisManager* GiGa::createVisManager()
{
  /// static variable 
  static G4VVisManager* s_visMgr = 0;
  /// return existing instance 
  if( 0 != s_visMgr ) { return s_visMgr ; }
  /// check for other G4VVis managers 
  if( 0 != G4VVisManager::GetConcreteInstance() ) { return 0; }
  /// 
  G4VisManager* visMgr = new GiGaVisManager();
  visMgr->Initialize();
  ///
  s_visMgr  = visMgr ;
  ///
  return s_visMgr ;
};

// ============================================================================
/** create  User Interface session
 *  @param  session session type 
 *  @return pointer to user interafce session 
 */
// ============================================================================
G4UIsession* GiGa::createUIsession( const std::string& session )
{
  /// static variable 
  static G4UIsession*  s_uiSession = 0 ;
  /// return the existing session
  if( 0 != s_uiSession ) { return s_uiSession ; }
  ///
  if      ( "Wo"        == session ) 
    { 
#ifdef G4UI_USE_WO
      s_uiSession = new G4UIWo  ( System::argc() , System::argv() ) ; 
#endif ///< G4UI_USE_WO
    }
  else if ( "GAG"       == session  )    
    {
#ifdef G4UI_USE_GAG
      s_uiSession = new G4UIGAG () ; 
#endif ///< G4UI_USE_GAG
    }
  else if ( "Xm"        == session  ) 
    {
#ifdef G4UI_USE_XM
      s_uiSession = new G4UIXm  ( System::argc() , System::argv() ) ; 
#endif ///< G4UI_USE_XM 
    }
  else if ( "Xaw"       == session  ) 
    {
#ifdef G4UI_USE_XAW
      s_uiSession = new G4UIXaw ( System::argc() , System::argv() ) ; 
#endif ///< G4UI_USE_XAW
    }
  else if ( "tcsh"  == session  ) 
    {
#ifdef G4UI_USE 
      s_uiSession = new G4UIterminal( new G4UIcsh ()  ) ;
#endif 
    }
  else if ( "csh"  == session  ) 
    {
#ifdef G4UI_USE 
      s_uiSession = new G4UIterminal( new G4UIcsh ()  ) ;
#endif
    }
  else if ( "terminal"  == session  ) 
    {
#ifdef G4UI_USE 
      s_uiSession = new G4UIterminal () ;           
#endif
    }
  ///
  /// #endif ///< G4UI_USE 
  ///
  return s_uiSession ;
  ///
};

// ============================================================================
