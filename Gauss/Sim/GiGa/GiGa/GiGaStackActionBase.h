// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.7  2001/07/23 13:11:43  ibelyaev
/// the package restructurisation(II)
///
// ============================================================================
#ifndef     GIGA_GiGaStackActionBase_H
#define     GIGA_GiGaStackActionBase_H 1 
// ============================================================================
// base class 
#include "GiGa/IGiGaStackAction.h" 
#include "GiGa/GiGaBase.h" 
//

/** @class GiGaStackActionBase GiGaStackActionBase.h
 *    
 *  Base class for implementation of concrete Stacking Action for GiGa
 *  
 *  @author  Vanya Belyaev
 *  @date    23/01/2001
 */

class GiGaStackActionBase: virtual public IGiGaStackAction   ,
                           public  GiGaBase
{
  ///
protected:
  /// constructor 
  GiGaStackActionBase( const std::string& , ISvcLocator* );
  /// virtual destructor 
  virtual ~GiGaStackActionBase();
  ///
public:
  /// ident 
  virtual const std::string& name       () const { return GiGaBase::name() ; }; 
  /// initialize 
  virtual StatusCode  initialize () ; 
  /// finalize 
  virtual StatusCode  finalize   () ; 
  /// Query Interface
  virtual StatusCode  queryInterface ( const InterfaceID& , void** ) ;
  ///
public:
  ///      
  /**     Reply G4ClassificationOfNewTrack determined by the
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
  virtual G4ClassificationOfNewTrack ClassifyNewTrack ( const G4Track* ) ; 
  ///
  /**     This method is called by G4StackManager when the urgentStack
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
  virtual void                       NewStage         ();
  ///
  /**     This method is called by G4StackManager at the begining of
	  each event.
	  
	  Be careful that the urgentStack and the waitingStack of 
	  G4StackManager are empty at this moment, because this method
	  is called before accepting primary particles. Also, note that
	  the postponeStack of G4StackManager may have some postponed
	  tracks.
  */ 
  virtual void                       PrepareNewEvent  ();
  ///
private:
  ///
  GiGaStackActionBase () ; ///< no default constructor!                   
  GiGaStackActionBase           ( const GiGaStackActionBase& ); ///< no copy
  GiGaStackActionBase& operator=( const GiGaStackActionBase& ); ///< no = ! 
  ///
private:
  ///
};
///

// ============================================================================
#endif   ///< GIGA_GiGaStackActionBase_H
// ============================================================================















