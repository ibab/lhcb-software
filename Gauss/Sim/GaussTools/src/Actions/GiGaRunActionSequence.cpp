// $Id: GiGaRunActionSequence.cpp,v 1.1 2002-09-26 18:10:52 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.7  2002/05/07 12:21:35  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
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
  std::string Type ; // Member Type 
  std::string Name ; // Member Name
  for( MEMBERS::const_iterator member = m_members.begin() ;
       m_members.end() != member ; ++member ) 
    {
      sc = GiGaUtil::SplitTypeAndName( *member , Type , Name );
      if( sc.isFailure() )
        { return Error("Member Type/Name '"+(*member)+"' is unparsable",sc);}
      IGiGaRunAction* action = 0 ;
      sc = toolSvc()->retrieveTool( Type , Name , action , gigaSvc() );
      if( sc.isFailure() ) 
        { return Error("Could not create IGiGaRunAction '" 
                       + Type + "'/'" + Name + "'" , sc  ) ; }
      if( 0 == action    ) 
        { return Error("Could not create IGiGaRunAction '" 
                       + Type + "'/'" + Name + "'"       ) ; }
      action->addRef();
      m_actions.push_back( action );
    }       
  ///
  return Print("initialized successfully" , 
               StatusCode::SUCCESS , MSG::VERBOSE );
};
// ============================================================================

// ============================================================================
/** finalization method 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaRunActionSequence::finalize()
{
  Print("Finalization", StatusCode::SUCCESS , MSG::VERBOSE );
  // release all members 
  std::for_each  ( m_actions.begin () , 
                   m_actions.end   () ,
                   std::mem_fun(&IGiGaRunAction::release) );
  m_actions.clear();
  // finalize teh base class 
  return GiGaRunActionBase::finalize();
};

// ============================================================================
/** perform begin-of-run action
 *  @param run  pointer to Geant4 Run object
 */
// ============================================================================
void GiGaRunActionSequence::BeginOfRunAction ( const G4Run* run )
{
  // run actions of all members  
  std::for_each ( m_actions.begin () , m_actions.end   () ,
                  std::bind2nd( std::mem_fun1(&IGiGaRunAction::
                                              BeginOfRunAction) , run ) );
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
  std::for_each ( m_actions.begin () , m_actions.end   () ,
                  std::bind2nd( std::mem_fun1(&IGiGaRunAction::
                                              EndOfRunAction) , run ) );
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================


