// $Id: GiGaRunActionSequence.cpp,v 1.4 2004-02-20 19:35:29 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2003/09/22 13:59:33  ibelyaev
//  polishing of addRef/release/releaseTools/finalize
//
// ============================================================================
/// STD & STL 
#include <algorithm>
/// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
/// GiGa 
#include "GiGa/IGiGaSvc.h"
#include "GiGa/GiGaMACROs.h"
#include "GiGa/GiGaUtil.h"
/// local 
#include "GiGaRunActionSequence.h"

// ============================================================================
/** @file
 *
 * Implementation file for class GiGaRunActionSequence
 * 
 * @author Vanya  Belyaev Ivan.Belyaev@itep.ru
 * @date 26/07/2001 
 */
// ============================================================================

// ============================================================================
/// factory business 
// ============================================================================
IMPLEMENT_GiGaFactory( GiGaRunActionSequence ) ;

// ============================================================================
/** standard constructor 
 *  @see GiGaPhysListBase
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GiGaRunActionSequence::GiGaRunActionSequence
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaRunActionBase( type , name , parent )
  , m_members ( ) ///< empty default lst!
  , m_actions ( ) 
{ declareProperty( "Members" , m_members ); };
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GiGaRunActionSequence::~GiGaRunActionSequence() 
{
  m_members.clear () ;
  m_actions.clear () ;
};
// ============================================================================

// ============================================================================
/** initialization method 
 *  @see GiGaRunActionBase 
 *  @see GiGaBase 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaRunActionSequence::initialize  ()
{
  /// initialize the base class 
  StatusCode sc = GiGaRunActionBase::initialize();
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class!");}
  /// instantiate all members using the creator 
  m_actions.clear() ;
  std::string Type ; // Member Type 
  std::string Name ; // Member Name
  for( MEMBERS::const_iterator member = m_members.begin() ;
       m_members.end() != member ; ++member ) 
  {
    IGiGaRunAction* action = tool<IGiGaRunAction>( *member , this );
    if( 0 == action    ) { return Error("Could not create IGiGaRunAction '" 
                                        + *member + "'"       ) ; }
    m_actions.push_back( action );
  }       
  ///
  return StatusCode::SUCCESS ;
  
};
// ============================================================================

// ============================================================================
/** perform begin-of-run action
 *  @param run  pointer to Geant4 Run object
 */
// ============================================================================
void GiGaRunActionSequence::BeginOfRunAction ( const G4Run* run )
{
  // run actions of all members  
  for( ACTIONS::iterator iaction = m_actions.begin() ; 
       m_actions.end() != iaction ; ++iaction )
    {
      IGiGaRunAction* action = *iaction ;
      if( 0 != action ) { action->BeginOfRunAction( run ) ; }
    }
};
// ============================================================================

// ============================================================================
/** perform end-of-run action
 *  @param run  pointer to Geant4 Run object
 */
// ============================================================================
void GiGaRunActionSequence::EndOfRunAction ( const G4Run* run )
{
  // run actions of all members  
  for( ACTIONS::iterator iaction = m_actions.begin() ; 
       m_actions.end() != iaction ; ++iaction )
    {
      IGiGaRunAction* action = *iaction ;
      if( 0 != action ) { action->BeginOfRunAction( run ) ; }
    }
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================


