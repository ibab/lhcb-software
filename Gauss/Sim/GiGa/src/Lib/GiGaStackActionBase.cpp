// $Id: GiGaStackActionBase.cpp,v 1.8 2002-12-07 14:27:51 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.7  2002/05/07 12:21:34  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// ============================================================================
// GiGa 
#include "GiGa/GiGaStackActionBase.h"
#include "GiGa/GiGaUtil.h"

/** @file 
 *  implementation of class GiGaStackActionBase
 *
 *  @author Vanya Belyaev 
 */

namespace GiGaStackActionBaseLocal
{
#ifdef GIGA_DEBUG
  /** @var   s_Counter
   *  static instance counter 
   */
  static GiGaUtil::InstanceCounter<GiGaStackActionBase> s_Counter ;
#endif   
};

// ============================================================================
/** standard constructor 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GiGaStackActionBase::GiGaStackActionBase
( const std::string& type   , 
  const std::string& name   , 
  const IInterface*  parent ) 
  : G4UserStackingAction () 
  , GiGaBase             ( type , name , parent ) 
{ 
  declareInterface<IGiGaStackAction> (this) ; 
#ifdef GIGA_DEBUG
  GiGaStackActionBaseLocal::s_Counter.increment () ;
#endif
};
// ============================================================================

// ============================================================================
// Destructor 
// ============================================================================
GiGaStackActionBase::~GiGaStackActionBase()
{
#ifdef GIGA_DEBUG
  GiGaStackActionBaseLocal::s_Counter.decrement () ;
#endif 
};
// ============================================================================

// ============================================================================
/** initialize the stack action  
 *  @see GiGaBase 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaStackActionBase::initialize() 
{
  StatusCode sc = GiGaBase::initialize() ; 
  if( sc.isFailure() ) 
    { return Error("Could not initialize base class GiGaBase"); } 
  ///
  return Print("GiGaStackActionBase initialized successfully",
               StatusCode::SUCCESS                           , MSG::VERBOSE );  
}; 
// ============================================================================

// ============================================================================
/** finalize the stack action
 *  @see GiGaBase 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaStackActionBase::finalize() 
{ 
  Print("GiGaStackActionBase finalization" ,
        StatusCode::SUCCESS                , MSG::VERBOSE ) ;
  // finalize the base class 
  return GiGaBase::finalize();  
};
// ============================================================================

// ============================================================================
/** From G4:
    
    Reply G4ClassificationOfNewTrack determined by the
    newly coming G4Track.  
    
    enum G4ClassificationOfNewTrack
    {
    fUrgent,    // put into the urgent stack
    fWaiting,   // put into the waiting stack
    fPostpone,  // postpone to the next event
    fKill       // kill without stacking
    };  
    
    The parent_ID of the track indicates the origin of it.
    G4int parent_ID = aTrack->get_parentID();
    
    parent_ID = 0 : primary particle
    parent_ID > 0 : secondary particle
    parent_ID < 0 : postponed from the previous event
*/
// ============================================================================
G4ClassificationOfNewTrack 
GiGaStackActionBase::ClassifyNewTrack ( const G4Track* /* track */ )
{ 
  Print("'ClassiftNewTrack' is invoked" , StatusCode::SUCCESS , MSG::DEBUG );
  return fUrgent; 
};
// ============================================================================

// ============================================================================
/** From G4:
    
    This method is called by G4StackManager when the urgentStack
    becomes empty and contents in the waitingStack are transtered
    to the urgentStack.
    
    Note that this method is not called at the begining of each
    event, but "PrepareNewEvent" is called.
    
    In case re-classification of the stacked tracks is needed,
    use the following method to request to G4StackManager.
    
    stackManager->ReClassify();
    
    All of the stacked tracks in the waitingStack will be re-classified 
    by "ClassifyNewTrack" method.
    
    To abort current event, use the following method.
    stackManager->clear();
    
    Note that this way is valid and safe only for the case it is called
    from this user class. The more global way of event abortion is
    
    G4UImanager * UImanager = G4UImanager::GetUIpointer();
    UImanager->ApplyCommand("/event/abort");
*/
// ============================================================================
void GiGaStackActionBase::NewStage         ()
{ Print("'NewStage' is invoked" , StatusCode::SUCCESS , MSG::DEBUG ); };
// ============================================================================

// ============================================================================
/** From G4:
    
    This method is called by G4StackManager at the begining of
    each event.
    
    Be careful that the urgentStack and the waitingStack of 
    G4StackManager are empty at this moment, because this method
    is called before accepting primary particles. Also, note that
    the postponeStack of G4StackManager may have some postponed
    tracks.
*/ 
// ============================================================================
void GiGaStackActionBase::PrepareNewEvent  ()
{ Print("'PrepareNewEvent' is invoked" , StatusCode::SUCCESS , MSG::DEBUG ); };
// ============================================================================

// ============================================================================
// The END 
// ============================================================================














