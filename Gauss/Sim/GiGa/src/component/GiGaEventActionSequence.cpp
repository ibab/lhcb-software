// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.3  2001/07/27 17:03:19  ibelyaev
/// improved printout
///
/// Revision 1.2  2001/07/27 14:29:00  ibelyaev
/// bug fix
///
/// Revision 1.1  2001/07/24 09:48:14  ibelyaev
/// new component GiGaEventActionSequence
/// 
// ============================================================================
/// STD & STL 
#include<functional> 
#include<algorithm>
/// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
/// GiGa
#include "GiGa/GiGaUtil.h"
#include "GiGa/GiGaEventActionFactory.h"
/// local 
#include "GiGaEventActionSequence.h"

// ============================================================================
/** @class GiGaEventActionSequence
 *
 *  implementation 
 *
 *  @author Vanya  Belyaev
 *  @date 24/07/2001 
 */
// ============================================================================

// ============================================================================
/// factory business 
// ============================================================================
static const GiGaEventActionFactory<GiGaEventActionSequence>         s_Factory;
const       IGiGaEventActionFactory&GiGaEventActionSequenceFactory = s_Factory;

// ============================================================================
  /** standard constructor
   *  @param Name name of the event action object instance
   *  @param Loc  pointer to Service Locator 
   */
// ============================================================================
GiGaEventActionSequence::GiGaEventActionSequence( const std::string& Name ,
                                                  ISvcLocator*       Loc  )
  : GiGaEventActionBase ( Name , Loc ) 
  , m_members      ()  ///< default empty list
  , m_actions      ()
{
  declareProperty( "Members" , m_members );  
};

// ============================================================================
/// destructor 
// ============================================================================
GiGaEventActionSequence::~GiGaEventActionSequence()
{
  /// delete all actions 
  std::transform( m_actions.begin  () ,
                  m_actions.end    () ,
                  m_actions.begin  () ,
                  GiGaUtil::Delete () );
  ///
};

// ============================================================================
/** finalise, delete and clear actions container 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaEventActionSequence::actionsReset() 
{
  /// finalize all members 
  std::for_each  ( m_actions.begin () , 
                   m_actions.end   () ,
                   std::mem_fun(&IGiGaEventAction::finalize) );
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
/** initialize the object
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaEventActionSequence::initialize() 
{
  /// initialize the base class 
  StatusCode sc = GiGaEventActionBase::initialize();
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class!");}
  if( m_members.empty() ) { Warning("The sequence is empty!"); }
  /// create the creator
  GiGaUtil::EventActionCreator  creator( objMgr() , svcLoc() );
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
      IGiGaEventAction* eventAction = creator( Type , Name ) ;
      if( 0 == eventAction ) 
        {
          Error("Could not create Type/Name=" + Type + "/" +  Name , sc ) ;
          actionsReset() ;
          return  StatusCode::FAILURE ;                      /// < RETURN
        }
      /// initialize the member 
      sc = eventAction->initialize() ;
      if( sc.isFailure() ) 
        { 
          Error("Could not initialize Type/Name=" + Type + "/" + Name , sc ) ;
          actionsReset();
          return  sc ;                                      /// < RETURN 
        }
      ///
      m_actions.push_back( eventAction );
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
StatusCode GiGaEventActionSequence::finalize() 
{
  ///
  Print("finalization");
  /// finalize all members 
  std::for_each  ( m_actions.begin () , 
                   m_actions.end   () ,
                   std::mem_fun(&IGiGaEventAction::finalize) );
  ///
  return GiGaEventActionBase::finalize();
};

// ============================================================================
/** perform begin-of-event action
 *  @param event pointer to Geant4 event 
 */
// ============================================================================
void GiGaEventActionSequence::BeginOfEventAction ( const G4Event* event )
{
  /// stepping actions of all members  
  std::for_each ( m_actions.begin () , 
                  m_actions.end   () ,
                  std::bind2nd( std::mem_fun1(&IGiGaEventAction::
                                              BeginOfEventAction) , event ) );
  ///
};

// ============================================================================
/** perform end-of-event action
 *  @param event pointer to Geant4 event 
 */
// ============================================================================
void GiGaEventActionSequence::EndOfEventAction ( const G4Event* event )
{
  /// stepping actions of all members  
  std::for_each ( m_actions.begin () , 
                  m_actions.end   () ,
                  std::bind2nd( std::mem_fun1(&IGiGaEventAction::
                                              EndOfEventAction) , event ) );
  ///
};


// ============================================================================
