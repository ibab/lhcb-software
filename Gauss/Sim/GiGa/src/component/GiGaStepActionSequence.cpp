// $Id: GiGaStepActionSequence.cpp,v 1.7 2002-04-25 13:02:05 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
/// STD & STL 
#include <functional>
#include <algorithm>
/// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
/// GiGa
#include "GiGa/IGiGaStepAction.h"
#include "GiGa/GiGaStepActionFactory.h"
#include "GiGa/GiGaUtil.h"
/// Local 
#include "GiGaStepActionSequence.h"

/** @file
 * 
 * Implementation of GiGaStepActionSequence
 *
 *  @author Vanya Belyaev
 *  @date 23/07/2001 
 */

// ============================================================================
/// factories 
// ============================================================================
IMPLEMENT_GiGaStepAction( GiGaStepActionSequence );

// ============================================================================
/** the standard constructor
 *  @param  Name name of the instance
 *  @param  Loc  pointer to service locator object 
 */
// ============================================================================
GiGaStepActionSequence::GiGaStepActionSequence( const std::string& Name ,
                                                ISvcLocator*       Loc  )
  : GiGaStepActionBase( Name , Loc )
  , m_members    () ///< empty default vector! 
  , m_actions    ()
{
  declareProperty("Members" , m_members );
};

// ============================================================================
/** finalise, delete and clear actions container 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaStepActionSequence::actionsReset() 
{
  /// finalize all members 
  std::for_each  ( m_actions.begin () , 
                   m_actions.end   () ,
                   std::mem_fun(&IGiGaStepAction::finalize) );
  /// delete all members

  std::transform ( m_actions.begin  () , 
                   m_actions.end    () ,
                   m_actions.begin  () ,
                   GiGaUtil::Delete () );
  /// clear the coinatiner 
  m_actions.clear() ;
  ///
  return StatusCode::SUCCESS ;
};

// ============================================================================
/// destructor 
// ============================================================================
GiGaStepActionSequence::~GiGaStepActionSequence()
{
  /// delete all actions 
  std::transform( m_actions.begin () ,  
                  m_actions.end   () , 
                  m_actions.begin () ,  
                  GiGaUtil::Delete() );
  ///
  m_actions.clear();
  m_members.clear();
  ///
}


// ============================================================================
/** initialize the object
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaStepActionSequence::initialize() 
{
  /// initialize the base class 
  StatusCode sc = GiGaStepActionBase::initialize();
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class!");}
  if( m_members.empty() ) { Warning("The sequence is empty!"); }
  /// create the creator
  GiGaUtil::StepActionCreator  creator( objMgr() , svcLoc() );
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
      IGiGaStepAction* stepAction = creator( Type , Name ) ;
      if( 0 == stepAction ) 
        {
          Error("Could not create Type/Name=" + Type + "/" +  Name , sc ) ;
          actionsReset() ;
          return  StatusCode::FAILURE ;                      /// < RETURN
        }
      /// initialize the member 
      sc = stepAction->initialize() ;
      if( sc.isFailure() ) 
        { 
          Error("Could not initialize Type/Name=" + Type + "/" + Name , sc ) ;
          actionsReset();
          return  sc ;                                      /// < RETURN 
        }
      ///
      m_actions.push_back( stepAction );
      Print("Member '"+Type+"'/'"+Name+"' is added to the sequence");
    }
  /// 
  Print("initialized successfully");
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
/** finalize the object
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaStepActionSequence::finalize() 
{
  ///
  Print("finalization");
  /// finalize all members 
  std::for_each  ( m_actions.begin () , 
                   m_actions.end   () ,
                   std::mem_fun(&IGiGaStepAction::finalize) );
  ///
  return GiGaStepActionBase::finalize();
};

// ============================================================================
/** perform the stepping action
 *  @param pointer to current Geant4 step 
 */
// ============================================================================
void GiGaStepActionSequence::UserSteppingAction ( const G4Step* step )
{
  /// stepping actions of all members  
  std::for_each  ( m_actions.begin () , 
                   m_actions.end   () ,
                   std::bind2nd( std::mem_fun1(&IGiGaStepAction::
                                               UserSteppingAction) , step ) );
  ///  
};

// ============================================================================
// The END 
// ============================================================================


