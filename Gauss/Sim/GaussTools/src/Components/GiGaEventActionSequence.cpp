// $Id: GiGaEventActionSequence.cpp,v 1.3 2003-08-15 12:46:11 witoldp Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2003/01/23 09:36:56  ibelyaev
//  few fixes for Win2K platform
//
// Revision 1.1  2002/12/12 15:19:31  witoldp
// major repackaging
//
// Revision 1.2  2002/12/07 14:41:44  ibelyaev
//  add new Calo stuff
//
// Revision 1.1  2002/09/26 18:10:52  ibelyaev
//  repackageing: add all concrete implementations from GiGa
//
// Revision 1.7  2002/05/07 12:21:34  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// ============================================================================
/// STD & STL 
#include<functional> 
#include<algorithm>
/// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
/// GiGa
#include "GiGa/GiGaUtil.h"
#include "GiGa/IGiGaSvc.h"
#include "GiGa/GiGaMACROs.h"
/// local 
#include "GiGaEventActionSequence.h"

// ============================================================================
/** @file
 *
 *  implementation of GiGaEventActionSequence
 *
 *  @author Vanya  Belyaev
 *  @date 24/07/2001 
 */
// ============================================================================

// ============================================================================
// factory business 
// ============================================================================
IMPLEMENT_GiGaFactory( GiGaEventActionSequence ) ;
// ============================================================================

// ============================================================================
/** standard constructor 
 *  @see GiGaEventActionBase
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GiGaEventActionSequence::GiGaEventActionSequence
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaEventActionBase ( type , name  , parent ) 
  , m_members      ()  ///< default empty list
  , m_actions      ()
{ declareProperty( "Members" , m_members ); };
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GiGaEventActionSequence::~GiGaEventActionSequence(){};
// ============================================================================

// ============================================================================
/** initialize the event action object
 *  @see GiGaEventActionBase 
 *  @see GiGaBase 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaEventActionSequence::initialize() 
{
  /// initialize the base class 
  StatusCode sc = GiGaEventActionBase::initialize();
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class!");}
  if( m_members.empty() ) { Print("The sequence is empty."); }
  // instantiate members 
  std::string Type, Name;
  for( MEMBERS::const_iterator member = m_members.begin() ;
       m_members.end() != member ; ++member )
    {
      IGiGaEventAction* action = tool( *member , action , this  );
      if( 0 == action    ) { return Error("Could not create IGiGaEventAction '" 
                                          + *member + "'"       ) ; }
      m_actions.push_back( action );
    }       
  //
  return Print("Initialized successfully" , 
               StatusCode::SUCCESS        , MSG::VERBOSE );
};
// ============================================================================

// ============================================================================
/** finalize the object
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaEventActionSequence::finalize() 
{
  Print("Finalization", StatusCode::SUCCESS , MSG::VERBOSE );
  // release  all members 
  for( ACTIONS::iterator iaction = m_actions.begin() ; 
       m_actions.end() != iaction ; ++iaction )
    {
      IInterface*  action = *iaction ;
      if( 0 != action ) { action->release() ; }
      *iaction = 0 ;
    }
  m_actions.clear();
  //
  return GiGaEventActionBase::finalize();
};
// ============================================================================

// ============================================================================
/** perform begin-of-event action
 *  @param event pointer to Geant4 event 
 */
// ============================================================================
void GiGaEventActionSequence::BeginOfEventAction ( const G4Event* event )
{
  // stepping actions of all members  
  for( ACTIONS::iterator iaction = m_actions.begin() ; 
       m_actions.end() != iaction ; ++iaction )
    {
      IGiGaEventAction* action = *iaction ;
      if( 0 != action ) { action->BeginOfEventAction( event ) ; }
    }
};
// ============================================================================

// ============================================================================
/** perform end-of-event action
 *  @param event pointer to Geant4 event 
 */
// ============================================================================
void GiGaEventActionSequence::EndOfEventAction ( const G4Event* event )
{
  // stepping actions of all members  
  for( ACTIONS::iterator iaction = m_actions.begin() ; 
       m_actions.end() != iaction ; ++iaction )
    {
      IGiGaEventAction* action = *iaction ;
      if( 0 != action ) { action->EndOfEventAction( event ) ; }
    }
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
