// $Id: GiGaEventActionSequence.cpp,v 1.6 2007-01-12 15:36:45 ranjard Exp $
// Include files 

/// STD & STL 
#include<functional> 
#include<algorithm>
// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/PropertyMgr.h"
/// GiGa
#include "GiGa/GiGaUtil.h"
#include "GiGa/IGiGaSvc.h"
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

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GiGaEventActionSequence )

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
{ 
  declareProperty( "Members" , m_members ); 
}


// ============================================================================
/// destructor 
// ============================================================================
GiGaEventActionSequence::~GiGaEventActionSequence(){}


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
  m_actions.clear();
  // instantiate members 
  std::string Type, Name;
  for( MEMBERS::const_iterator member = m_members.begin() ;
       m_members.end() != member ; ++member )
  {
    IGiGaEventAction* action = 
      tool<IGiGaEventAction>( *member , this  );
    if( 0 == action    ) { return Error("Could not create IGiGaEventAction '" 
                                        + *member + "'"       ) ; }
    m_actions.push_back( action );
  }       
  //
  return StatusCode::SUCCESS ;
}


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
}


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
}


// ============================================================================
// The END 
// ============================================================================
