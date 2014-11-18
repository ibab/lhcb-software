// $Id: GiGaTrackActionSequence.cpp,v 1.6 2007-01-12 15:36:57 ranjard Exp $ 
// Include files 

/// STD & STL 
#include <functional>
#include <algorithm>

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/PropertyMgr.h"

// GiGa 
#include "GiGa/IGiGaSvc.h"
#include "GiGa/GiGaUtil.h"

// local 
#include "GiGaTrackActionSequence.h"

// ============================================================================
/** @file 
 *  
 *  Implementation file for class : GiGaTrackActionSequence
 *  @date 24/07/2001 
 *  @author Vanya  Belyaev Ivan.Belyaev@itep.ru
 */
// ============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GiGaTrackActionSequence )

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
{ 
  declareProperty( "Members" , m_members ); 
}


// ============================================================================
/// destructor 
// ============================================================================
GiGaTrackActionSequence::~GiGaTrackActionSequence() {}


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
  if( m_members.empty() ) { Print("The sequence is empty."); }
  // instantiate members 
  m_actions.clear() ;
  std::string Type, Name;
  for( MEMBERS::const_iterator member = m_members.begin() ;
       m_members.end() != member ; ++member )
  {
    IGiGaTrackAction* action = tool<IGiGaTrackAction>( *member , this );
    if( 0 == action    ) { return Error("Could not create IGiGaTrackAction '" 
                                        + *member  + "'"       ) ; }
    m_actions.push_back( action );
  }       
  //
  return Print("Iinitialized successfully" , 
               StatusCode::SUCCESS         , MSG::VERBOSE );
}


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
      for( ACTIONS::iterator iaction = m_actions.begin() ; 
           m_actions.end() != iaction ; ++iaction )
        {
          G4TrackingManager* mgr    = trackMgr();
          IGiGaTrackAction*   action = *iaction ;
          if( 0 != action ) { action->SetTrackingManagerPointer( mgr ) ; }
        }
      m_setMgr = true;
    }
  // tracking actions of all members  
  for( ACTIONS::iterator iaction = m_actions.begin() ; 
       m_actions.end() != iaction ; ++iaction )
    {
      IGiGaTrackAction*   action = *iaction ;
      if( 0 != action ) { action->PreUserTrackingAction( track ) ; }
    }
}


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
      for( ACTIONS::iterator iaction = m_actions.begin() ; 
           m_actions.end() != iaction ; ++iaction )
        {
          G4TrackingManager* mgr    = trackMgr();
          IGiGaTrackAction*   action = *iaction ;
          if( 0 != action ) { action->SetTrackingManagerPointer( mgr ) ; }
        }
      m_setMgr = true;
    }
  // tracking actions of all members  
  for( ACTIONS::iterator iaction = m_actions.begin() ; 
       m_actions.end() != iaction ; ++iaction )
    {
      IGiGaTrackAction*   action = *iaction ;
      if( 0 != action ) { action->PostUserTrackingAction( track ) ; }
    }
}


// ============================================================================
// The END 
// ============================================================================
