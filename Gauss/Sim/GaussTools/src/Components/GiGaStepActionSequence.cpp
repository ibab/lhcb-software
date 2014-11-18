// $Id: GiGaStepActionSequence.cpp,v 1.6 2007-01-12 15:36:57 ranjard Exp $ 
// Include files 

/// STD & STL 
#include <functional>
#include <algorithm>

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/PropertyMgr.h"

/// GiGa
#include "GiGa/IGiGaSvc.h"
#include "GiGa/GiGaUtil.h"

/// Local 
#include "GiGaStepActionSequence.h"

/** @file
 * 
 *  Implementation of GiGaStepActionSequence
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 23/07/2001 
 */

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GiGaStepActionSequence )

// ============================================================================
/** standard constructor 
 *  @see GiGaStepActionBase 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GiGaStepActionSequence::GiGaStepActionSequence
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) 
  : GiGaStepActionBase( type , name , parent )
  , m_members    () // empty default vector! 
  , m_actions    ()
{ 
declareProperty("Members" , m_members ); 
}


// ============================================================================
/// destructor 
// ============================================================================
GiGaStepActionSequence::~GiGaStepActionSequence()
{
  m_actions.clear();
  m_members.clear();
}


// ============================================================================
/** initialize the object
 *  @see GiGaStepActionBase 
 *  @see GiGaBase 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaStepActionSequence::initialize() 
{
  /// initialize the base class 
  StatusCode sc = GiGaStepActionBase::initialize();
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class!");}
  if( m_members.empty() ) { Print("The sequence is empty."); }
  // instantiate members 
  m_actions.clear() ;
  std::string Type, Name;
  for( MEMBERS::const_iterator member = m_members.begin() ;
       m_members.end() != member ; ++member )
  {
    IGiGaStepAction* action = tool<IGiGaStepAction> ( *member , this );
    if( 0 == action    ) { return Error("Could not create IGiGaStepAction '" 
                                          + *member + "'"       ) ; }
    m_actions.push_back( action );
  }       
  //
  return Print("Iinitialized successfully" , 
               StatusCode::SUCCESS         , MSG::VERBOSE);
}


// ============================================================================
/** perform the stepping action
 *  @param pointer to current Geant4 step 
 */
// ============================================================================
void GiGaStepActionSequence::UserSteppingAction ( const G4Step* step )
{
  /// stepping actions of all members  
  for( ACTIONS::iterator iaction = m_actions.begin() ; 
       m_actions.end() != iaction ; ++iaction )
    {
      IGiGaStepAction*  action = *iaction ;
      if( 0 != action ) { action->UserSteppingAction( step ) ; }
    }
}


// ============================================================================
// The END 
// ============================================================================


