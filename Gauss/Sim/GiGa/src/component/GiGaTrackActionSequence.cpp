// $Id: GiGaTrackActionSequence.cpp,v 1.5 2002-04-25 13:02:05 ibelyaev Exp $ 
// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
// ============================================================================
/// STD & STL 
#include <functional>
#include <algorithm>
/// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
/// GiGa 
#include "GiGa/GiGaUtil.h"
#include "GiGa/GiGaTrackActionFactory.h"
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
IMPLEMENT_GiGaTrackAction( GiGaTrackActionSequence ) ;

// ============================================================================
/** standard constructor
 *  @param Name name of this concrete instance 
 *  @param Loc  pointer to Service Locator 
 */
// ============================================================================
GiGaTrackActionSequence::GiGaTrackActionSequence( const std::string& Name ,
                                                  ISvcLocator*       Loc  )
  : GiGaTrackActionBase( Name , Loc ) 
  , m_members ()  ///< default empty vertor!
  , m_actions ()
  , m_setMgr  ( false ) 
{  
  declareProperty( "Members" , m_members );
};

// ============================================================================
/// destructor 
// ============================================================================
GiGaTrackActionSequence::~GiGaTrackActionSequence()
{
  /// delete all actions
  std::transform( m_actions.begin () , 
                  m_actions.end   () ,
                  m_actions.begin () ,
                  GiGaUtil::Delete() );
  ///
};

// ============================================================================
/** reset all members(actions)
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaTrackActionSequence::actionsReset() 
{
  /// finalize all actions 
  std::for_each( m_actions.begin () ,
                 m_actions.end   () ,
                 std::mem_fun(&IGiGaTrackAction::finalize) );         
  /// delete all actions 
  std::transform ( m_actions.begin  () , 
                   m_actions.end    () ,
                   m_actions.begin  () ,
                   GiGaUtil::Delete () );
  /// clear the conatiner 
  m_actions.clear() ;
  ///
  return StatusCode::SUCCESS;
};

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
  /// create the creator
  GiGaUtil::TrackActionCreator  creator( objMgr() , svcLoc() );
  /// instantiate all members using the creator 
  for( MEMBERS::const_iterator it = m_members.begin() ;
       m_members.end() != it ; ++it ) 
    {
      /// get the type and name for the member 
      std::string Type ; /// Member Type 
      std::string Name ; /// Member Name
      StatusCode sc = 
        GiGaUtil::SplitTypeAndName( *it , Type , Name );
      if( sc.isFailure() ) 
        { 
          Error("Could not resolve Type/Name=" + (*it) , sc ) ;
          actionsReset() ;
          return  sc ;                                      /// < RETURN 
        }
      /// create the member 
      IGiGaTrackAction* trackAction = creator( Type , Name ) ;
      if( 0 == trackAction ) 
        {
          Error("Could not create Type/Name=" + Type + "/" +  Name , sc ) ;
          actionsReset() ;
          return  StatusCode::FAILURE ;                      /// < RETURN
        }
      /// initialize the member 
      sc = trackAction->initialize() ;
      if( sc.isFailure() ) 
        { 
          Error("Could not initialize Type/Name=" + Type + "/" + Name , sc ) ;
          actionsReset();
          return  sc ;                                      /// < RETURN 
        }
      ///
      m_actions.push_back( trackAction );
      Print("Member '"+Type+"'/'"+Name+"' is added to the sequence");
    }
  ///
  Print("initialized successfully");
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
/** finalize the action object 
 *  @return status code
 */ 
// ============================================================================
StatusCode GiGaTrackActionSequence::finalize   () 
{
  ///
  Print("finalization");
  /// finalize all actions 
  std::for_each( m_actions.begin () ,
                 m_actions.end   () ,
                 std::mem_fun(&IGiGaTrackAction::finalize) );         
  /// finalize the base class
  return GiGaTrackActionBase::finalize() ;
};

// ============================================================================
/** perform the pre-action
 *  @param track pointer to Geant4 track object 
 */ 
// ============================================================================
void GiGaTrackActionSequence::PreUserTrackingAction  ( const G4Track* track )
{
  /// set the tracking manager for all members 
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
  /// tracking actions of all members  
  std::for_each( m_actions.begin () , 
                 m_actions.end   () ,
                 std::bind2nd( std::mem_fun1(&IGiGaTrackAction::
                                             PreUserTrackingAction),track ) ) ;
};

// ============================================================================
/** perform the post-action
 *  @param track pointer to Geant4 track object 
 */ 
// ============================================================================
void GiGaTrackActionSequence::PostUserTrackingAction  ( const G4Track* track )
{
  /// tracking actions of all members  
  std::for_each( m_actions.begin () , 
                 m_actions.end   () ,
                 std::bind2nd( std::mem_fun1(&IGiGaTrackAction::
                                             PostUserTrackingAction),track ) );
};

// ============================================================================
// The END 
// ============================================================================
