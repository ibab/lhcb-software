// $Id: GiGa.cpp,v 1.5 2002-05-07 12:21:33 ibelyaev Exp $ 
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
// $Log: not supported by cvs2svn $
// ===========================================================================
/// GaudiKernel
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/System.h"
/// GiGa 
#include "GiGa/GiGa.h"
// Local
#include "GiGaVisManager.h"
/// G4 
#include "G4RunManager.hh"
#include "G4VVisManager.hh"
#include "G4UIsession.hh"
/// G4 
#ifndef     G4UI_NONE
#ifdef      G4UI_USE_WO
#include   "G4UIWo.hh"   
#endif ///< G4UI_USE_WO 
/// G4 
#ifdef      G4UI_USE_GAG
#include   "G4UIGAG.hh" 
#endif ///< G4UI_USE_GAG
/// G4 
#ifdef      G4UI_USE_XM
#include   "G4UIXm.hh"  
#endif ///< G4UI_USE_XM 
/// G4 
#ifdef      G4UI_USE_XAW
#include   "G4UIXaw.hh"  
#endif ///< G4UI_USE_XAW
/// G4 
#ifdef      G4UI_USE_TERMINAL
#include   "G4UIterminal.hh"             
#include   "G4UItcsh.hh"             
#include   "G4UIcsh.hh"             
#endif ///< G4UI_USE_TERMINAL 
/// G4
#include    "G4UIterminal.hh"             
#include    "G4UItcsh.hh"             
#include    "G4UIcsh.hh"             
#include    "G4UIGAG.hh" 
#endif   ///<G4UI_NONE
 
// ============================================================================
/** @file GiGa.cpp
 * Implementation of methods from namespace GiGa
 * 
 *  @author Vanya Belyaev
 *  @date  31/07/2001 
 */
// ============================================================================

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
#ifndef G4VIS_NONE
  G4VisManager* visMgr = new GiGaVisManager();
  visMgr->Initialize();
  s_visMgr  = visMgr ;
#endif 
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
  // static variable 
  static G4UIsession*  s_uiSession = 0 ;
  // return the existing session
  if( 0 != s_uiSession ) { return s_uiSession ; }
  ///
#ifndef G4UI_NONE
  if      ( "Wo"        == session ) 
    { 
#ifdef G4UI_USE_WO
      s_uiSession = new G4UIWo  ( System::argc() , System::argv() ) ; 
#endif ///< G4UI_USE_WO
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
  else if ( "GAG"       == session  )    
    { s_uiSession = new G4UIGAG () ; }
  else if ( "tcsh"  == session  ) 
    { s_uiSession = new G4UIterminal( new G4UIcsh ()  ) ; }
  else if ( "csh"  == session  ) 
    { s_uiSession = new G4UIterminal( new G4UIcsh ()  ) ; }
  else if ( "terminal"  == session  ) 
    { s_uiSession = new G4UIterminal () ; }
#endif   ///< G4UI_NONE
  ///
  return s_uiSession ;
  ///
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
