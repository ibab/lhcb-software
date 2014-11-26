// $Id: GiGaUIsession.cpp,v 1.5 2007-01-12 15:48:35 ranjard Exp $
// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/System.h"
// GiGa 
//#include "GiGa/GiGaMACROs.h"
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
#ifndef WIN32            
#include    "G4UItcsh.hh"             
#include    "G4UIcsh.hh"             
#include    "G4UIGAG.hh" 
#endif // not WIN32

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

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GiGaUIsession )

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
#ifndef WIN32
  m_sessions.push_back( "tcsh"     ) ;
#endif
  m_sessions.push_back( "Xm"       ) ; 
  m_sessions.push_back( "Wo"       ) ;
#ifndef WIN32
  m_sessions.push_back( "GAG"      ) ;
#endif
  m_sessions.push_back( "Xaw"      ) ;
  m_sessions.push_back( "terminal" ) ;
  /// declare new interface 
  declareInterface <IGiGaUIsession> (this);
  /// declare the property
  declareProperty( "Sessions" , m_sessions ); 
}


// ============================================================================
/// destructor (virtual and protected)
// ============================================================================
GiGaUIsession::~GiGaUIsession() {}


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
#ifndef WIN32
      else if ( "GAG"       == *session  )    
        { m_session          = new G4UIGAG                     () ; }
      else if ( "tcsh"      == *session  ) 
        { m_session          = new G4UIterminal( new G4UItcsh ()  ) ; }
      else if ( "csh"       == *session  )
        { m_session          = new G4UIterminal( new G4UIcsh ()  ) ; }
#endif ///< WIN32 
      else if ( "terminal"  == *session  ) 
        { m_session          = new G4UIterminal                 () ; }
    }
  
  if( 0 == m_session ) { Warning("UI session is not created!"); }
  
  return StatusCode::SUCCESS ;
}

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
}


// ============================================================================
// The END 
// ============================================================================
