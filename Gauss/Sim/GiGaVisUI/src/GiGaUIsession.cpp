// $Id: GiGaUIsession.cpp,v 1.2 2003-02-18 08:14:04 ranjard Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/12/12 14:46:26  witoldp
// new package containing GiGa vis and UI
//
// Revision 1.2  2002/12/07 14:41:45  ibelyaev
//  add new Calo stuff
//
// Revision 1.1  2002/12/04 21:17:38  ibelyaev
//  add UI and Vis stuff
// 
// ============================================================================
// Include files
/// GaudiKernel
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/System.h"
// GiGa 
#include "GiGa/GiGaMACROs.h"
// G4 
#ifdef      G4UI_USE_WO
#include   "G4UIWo.hh"   
#endif //   G4UI_USE_WO 
/// G4 
#ifdef      G4UI_USE_XM
#include   "G4UIXm.hh"  
#endif //   G4UI_USE_XM 
/// G4 
#ifdef      G4UI_USE_XAW
#include   "G4UIXaw.hh"  
#endif //   G4UI_USE_XAW
/// G4 
#include    "G4UIterminal.hh"             
#include    "G4UItcsh.hh"             
#include    "G4UIcsh.hh"             
#include    "G4UIGAG.hh" 

// local
#include "GiGaUIsession.h"


// ============================================================================
/** @file 
 *  Implementation file for class : GiGaUIsession
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 2002-12-04
 */
// ============================================================================

// ============================================================================
/// factory business 
// ============================================================================
IMPLEMENT_GiGaFactory( GiGaUIsession ) ;
// ============================================================================

// ============================================================================
/** standard constructor 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GiGaUIsession::GiGaUIsession
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaBase   ( type, name , parent )
  , m_sessions (   ) 
  , m_session  ( 0 ) 
{
  // the default ordered list of sessions 
  m_sessions.push_back( "tcsh"     ) ;
  m_sessions.push_back( "Xm"       ) ; 
  m_sessions.push_back( "Wo"       ) ;
  m_sessions.push_back( "GAG"      ) ;
  m_sessions.push_back( "Xaw"      ) ;
  m_sessions.push_back( "terminal" ) ;
  /// declare new interface 
  declareInterface <IGiGaUIsession> (this);
  /// declare the property
  declareProperty( "Sessions" , m_sessions ); 
};
// ============================================================================

// ============================================================================
/// destructor (virtual and protected)
// ============================================================================
GiGaUIsession::~GiGaUIsession() {};
// ============================================================================

// ============================================================================
/** initialization method 
 *  @see GiGaBase 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaUIsession::initialize  () 
{
  // initialize the base class  
  StatusCode sc = GiGaBase::initialize();
  if( sc.isFailure() ){ return Error("Error from base class GiGaBase" , sc ) ; }
  
  for( Sessions::const_iterator session = m_sessions.begin() ;
       m_sessions.end() != session && 0 == m_session ; ++session )
    { 
      if      ( "Wo"        == *session ) 
        { 
#ifdef G4UI_USE_WO
          m_session = new G4UIWo  ( System::argc() , System::argv() ) ; 
#endif ///< G4UI_USE_WO
        }
      else if ( "Xm"        == *session  ) 
        {
#ifdef G4UI_USE_XM
          m_session = new G4UIXm  ( System::argc() , System::argv() ) ; 
#endif ///< G4UI_USE_XM 
        }
      else if ( "Xaw"       == *session  ) 
        {
#ifdef G4UI_USE_XAW
          m_session = new G4UIXaw ( System::argc() , System::argv() ) ; 
#endif ///< G4UI_USE_XAW
        }
      else if ( "GAG"       == *session  )    
        { m_session          = new G4UIGAG                     () ; }
      else if ( "tcsh"      == *session  ) 
        { m_session          = new G4UIterminal( new G4UItcsh ()  ) ; }
      else if ( "csh"       == *session  ) 
        { m_session          = new G4UIterminal( new G4UIcsh ()  ) ; }
      else if ( "terminal"  == *session  ) 
        { m_session          = new G4UIterminal                 () ; }
    }
  
  if( 0 == m_session ) { Warning("UI session is not created!"); }
  
  return StatusCode::SUCCESS ;
};

// ============================================================================
/** get the pointer to G4 user interface
 *  @see IGiGaUIsession
 *  @return pointer to G4 user interface
 */
// ============================================================================
G4UIsession* GiGaUIsession::session () const 
{
  if( 0 == m_session ){ Warning("G4UISession* points to NULL!") ; }
  return m_session ;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
