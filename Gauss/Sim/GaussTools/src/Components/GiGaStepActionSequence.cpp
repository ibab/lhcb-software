// $Id: GiGaStepActionSequence.cpp,v 1.3 2003-08-15 12:46:11 witoldp Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2003/01/23 09:36:56  ibelyaev
//  few fixes for Win2K platform
//
// Revision 1.1  2002/12/12 15:19:33  witoldp
// major repackaging
//
// Revision 1.2  2002/12/07 14:41:44  ibelyaev
//  add new Calo stuff
//
// Revision 1.1  2002/09/26 18:10:54  ibelyaev
//  repackageing: add all concrete implementations from GiGa
//
// Revision 1.8  2002/05/07 12:21:36  ibelyaev
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
#include "GiGa/GiGaMACROs.h"
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

// ============================================================================
/// factories 
// ============================================================================
IMPLEMENT_GiGaFactory( GiGaStepActionSequence );

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
{ declareProperty("Members" , m_members ); };
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GiGaStepActionSequence::~GiGaStepActionSequence()
{
  m_actions.clear();
  m_members.clear();
};
// ============================================================================

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
  std::string Type, Name;
  for( MEMBERS::const_iterator member = m_members.begin() ;
       m_members.end() != member ; ++member )
    {
      IGiGaStepAction* action = tool( *member , action , this );
      if( 0 == action    ) { return Error("Could not create IGiGaStepAction '" 
                                          + *member + "'"       ) ; }
      m_actions.push_back( action );
    }       
  //
  return Print("Iinitialized successfully" , 
               StatusCode::SUCCESS         , MSG::VERBOSE);
};
// ============================================================================

// ============================================================================
/** finalize the object
 *  @see GiGaStepActionBase 
 *  @see GiGaBase 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaStepActionSequence::finalize() 
{
  Print("Finalization" , StatusCode::SUCCESS , MSG::VERBOSE );
  // finalize all members
  for( ACTIONS::iterator iaction = m_actions.begin() ; 
       m_actions.end() != iaction ; ++iaction )
    {
      IInterface*  action = *iaction ;
      if( 0 != action ) { action->release() ; }
      *iaction = 0 ;
    }
  m_actions.clear();
  // finalize base class 
  return GiGaStepActionBase::finalize();
};
// ============================================================================

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
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================


