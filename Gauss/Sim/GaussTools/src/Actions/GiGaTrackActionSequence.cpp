// $Id: GiGaTrackActionSequence.cpp,v 1.2 2002-12-07 14:41:44 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/09/26 18:10:55  ibelyaev
//  repackageing: add all concrete implementations from GiGa
//
// Revision 1.6  2002/05/07 12:21:36  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// ============================================================================
/// STD & STL 
#include <functional>
#include <algorithm>
/// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
/// GiGa 
#include "GiGa/IGiGaSvc.h"
#include "GiGa/GiGaUtil.h"
#include "GiGa/GiGaMACROs.h"
/// local 
#include "GiGaTrackActionSequence.h"

// ============================================================================
/** @file 
 *  
 *  Implementation file for class : GiGaTrackActionSequence
 *  @date 24/07/2001 
 *  @author Vanya  Belyaev Ivan.Belyaev@itep.ru
 */
// ============================================================================

// ============================================================================
/// Factory business 
// ============================================================================
IMPLEMENT_GiGaFactory( GiGaTrackActionSequence ) ;

// ============================================================================
/** standard constructor 
 *  @see GiGaTrackActionBase 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GiGaTrackActionSequence::GiGaTrackActionSequence
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaTrackActionBase( type , name , parent  ) 
  , m_members ()  ///< default empty vertor!
  , m_actions ()
  , m_setMgr  ( false ) 
{ declareProperty( "Members" , m_members ); };
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GiGaTrackActionSequence::~GiGaTrackActionSequence() {};
// ============================================================================

// ============================================================================
/** initialise the action object 
 *  @return status code
 */ 
// ============================================================================
StatusCode GiGaTrackActionSequence::initialize () 
{
  StatusCode sc = GiGaTrackActionBase::initialize();
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class!", sc ); }
  if( m_members.empty() ) { Warning("The sequence is empty!"); }
  // instantiate members 
  std::string Type, Name;
  for( MEMBERS::const_iterator member = m_members.begin() ;
       m_members.end() != member ; ++member )
    {
      IGiGaTrackAction* action = tool( *member , action , this );
      if( 0 == action    ) { return Error("Could not create IGiGaTrackAction '" 
                                          + *member  + "'"       ) ; }
      m_actions.push_back( action );
    }       
  //
  return Print("Iinitialized successfully" , 
               StatusCode::SUCCESS         , MSG::VERBOSE );
};
// ============================================================================

// ============================================================================
/** finalize the action object 
 *  @return status code
 */ 
// ============================================================================
StatusCode GiGaTrackActionSequence::finalize   () 
{
  Print("Finalization" , StatusCode::SUCCESS , MSG::VERBOSE );
  // release all memebrs 
  std::for_each( m_actions.begin () ,
                 m_actions.end   () ,
                 std::mem_fun(&IInterface::release) );         
  m_actions.clear();
  // finalize the base class
  return GiGaTrackActionBase::finalize() ;
};
// ============================================================================

// ============================================================================
/** perform the pre-action
 *  @param track pointer to Geant4 track object 
 */ 
// ============================================================================
void GiGaTrackActionSequence::PreUserTrackingAction  ( const G4Track* track )
{
  // set the tracking manager for all members 
  if( ! m_setMgr ) 
    {
      G4TrackingManager* mgr = trackMgr();
      std::for_each( m_actions.begin () , 
                     m_actions.end   () ,
                     std::bind2nd( std::mem_fun1(&IGiGaTrackAction::
                                                 SetTrackingManagerPointer) , 
                                   mgr ) ) ;
      m_setMgr = true;
    }
  // tracking actions of all members  
  std::for_each( m_actions.begin () , 
                 m_actions.end   () ,
                 std::bind2nd( std::mem_fun1(&IGiGaTrackAction::
                                             PreUserTrackingAction),track ) ) ;
};
// ============================================================================

// ============================================================================
/** perform the post-action
 *  @param track pointer to Geant4 track object 
 */ 
// ============================================================================
void GiGaTrackActionSequence::PostUserTrackingAction  ( const G4Track* track )
{
  // set the tracking manager for all members 
  if( ! m_setMgr ) 
    {
      G4TrackingManager* mgr = trackMgr();
      std::for_each( m_actions.begin () , 
                     m_actions.end   () ,
                     std::bind2nd( std::mem_fun1(&IGiGaTrackAction::
                                                 SetTrackingManagerPointer) , 
                                   mgr ) ) ;
      m_setMgr = true;
    }
  // tracking actions of all members  
  std::for_each( m_actions.begin () , 
                 m_actions.end   () ,
                 std::bind2nd( std::mem_fun1(&IGiGaTrackAction::
                                             PostUserTrackingAction),track ) );
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
