/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
/// STD & STL 
#include <algorithm>
/// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
/// GiGa 
#include "GiGa/GiGaRunActionFactory.h"
#include "GiGa/GiGaUtil.h"
/// local 
#include "GiGaRunActionSequence.h"

/// ===========================================================================
/** Implementation file for class : GiGaRunActionSequence
 * 
 * @author Vanya  Belyaev
 * @date 26/07/2001 
 */
/// ===========================================================================

/// ===========================================================================
/// factory business 
/// ===========================================================================
static const GiGaRunActionFactory<GiGaRunActionSequence>         s_Factory;
const       IGiGaRunActionFactory&GiGaRunActionSequenceFactory = s_Factory;

/// ===========================================================================
/** standard constructor
 *  @param Name name of this concrete instance of Run Action Object 
 *  @param Loc  pointer to Service Locator 
 */
/// ===========================================================================
GiGaRunActionSequence::GiGaRunActionSequence( const std::string Name ,
                                              ISvcLocator*      Loc  )
  : GiGaRunActionBase( Name , Loc )
  , m_members ( ) ///< empty default lst!
  , m_actions ( ) 
{ 
  declareProperty( "Members" , m_members );
};

/// ===========================================================================
/// destructor 
/// ===========================================================================
GiGaRunActionSequence::~GiGaRunActionSequence()
{
  /// finalize (if it is not yet done)
  if( init() ) { finalize(); }
  /// delete all actions 
  std::transform( m_actions.begin  () ,
                  m_actions.end    () ,
                  m_actions.begin  () ,
                  GiGaUtil::Delete () );
  ///
};

/// ===========================================================================
/** finalise, delete and clear actions container 
 *  @return status code 
 */
/// ===========================================================================
StatusCode GiGaRunActionSequence::actionsReset() 
{
  /// finalize all members 
  std::for_each  ( m_actions.begin () , 
                   m_actions.end   () ,
                   std::mem_fun(&IGiGaRunAction::finalize) );
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

/// ===========================================================================
/** initialization method 
 *  @return status code 
 */ 
/// ===========================================================================
StatusCode GiGaRunActionSequence::initialize  ()
{
  /// initialize the base class 
  StatusCode sc = GiGaRunActionBase::initialize();
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class!");}
  /// create the creator
  GiGaUtil::RunActionCreator  creator( objMgr() , svcLoc() );
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
      IGiGaRunAction* runAction = creator( Type , Name ) ;
      if( 0 == runAction ) 
        {
          Error("Could not create Type/Name=" + Type + "/" +  Name , sc ) ;
          actionsReset() ;
          return  StatusCode::FAILURE ;                      /// < RETURN
        }
      /// initialize the member 
      sc = runAction->initialize() ;
      if( sc.isFailure() ) 
        { 
          Error("Could not initialize Type/Name=" + Type + "/" + Name , sc ) ;
          actionsReset();
          return  sc ;                                      /// < RETURN 
        }
      ///
      m_actions.push_back( runAction );
    }
  ///
  return StatusCode::SUCCESS; 
};

/// ===========================================================================
/** finalization method 
 *  @return status code 
 */
/// ===========================================================================
StatusCode GiGaRunActionSequence::finalize()
{
  /// finalize all members 
  std::for_each  ( m_actions.begin () , 
                   m_actions.end   () ,
                   std::mem_fun(&IGiGaRunAction::finalize) );
  ///
  return GiGaRunActionBase::finalize();
}

/// ===========================================================================
/** perform begin-of-run action
 *  @param run  pointer to Geant4 Run object
 */
/// ===========================================================================
void GiGaRunActionSequence::BeginOfRunAction ( const G4Run* run )
{
  if( 0 == run ) { return ; }
  /// run actions of all members  
  std::for_each ( m_actions.begin () , m_actions.end   () ,
                  std::bind2nd( std::mem_fun1(&IGiGaRunAction::
                                              BeginOfRunAction) , run ) );
};

/// ===========================================================================
/** perform end-of-run action
 *  @param run  pointer to Geant4 Run object
 */
/// ===========================================================================
void GiGaRunActionSequence::EndOfRunAction ( const G4Run* run )
{
  if( 0 == run ) { return ; }
  /// run actions of all members  
  std::for_each ( m_actions.begin () , m_actions.end   () ,
                  std::bind2nd( std::mem_fun1(&IGiGaRunAction::
                                              EndOfRunAction) , run ) );
};

/// ===========================================================================


