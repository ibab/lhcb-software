// $Id: GiGaStackActionEmpty.h,v 1.6 2002-05-07 12:21:36 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef       GIGA_GiGaStackActionEmpty_H
#define       GIGA_GiGaStackActionEmpty_H 1 
// ============================================================================
// GiGa 
#include "GiGa/GiGaStackActionBase.h"
// forward declaration
template <class TYPE> class GiGaFactory;

/** @class GiGaStackActionEmpty GiGaStackActionEmpty.h
 *    
 *  Example of "primitive" implementation of stacking action class
 *  It is just empty!
 * 
 *  @author  Vanya Belyaev
 *  @date    23/01/2001
 */

class GiGaStackActionEmpty: virtual public GiGaStackActionBase
{
  /// friend factory for instantiation
  friend class GiGaFactory<GiGaStackActionEmpty>;
  
protected:
  
  /** standard constructor 
   *  @see GiGaSensDetBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaStackActionEmpty
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;

  ///  destructor (protected and virtual)
  virtual ~GiGaStackActionEmpty();

public:

  /** G4:
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
  virtual G4ClassificationOfNewTrack ClassifyNewTrack ( const G4Track* ) ; 

  /** G4:
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
  virtual void                       NewStage         ();

  /** G4:
      This method is called by G4StackManager at the begining of
      each event.
      
      Be careful that the urgentStack and the waitingStack of 
      G4StackManager are empty at this moment, because this method
      is called before accepting primary particles. Also, note that
      the postponeStack of G4StackManager may have some postponed
      tracks.
  */ 
  virtual void                       PrepareNewEvent  ();

 private:

  GiGaStackActionEmpty() ; ///< no default constructor
  GiGaStackActionEmpty( const GiGaStackActionEmpty& ); ///< no copy  
  GiGaStackActionEmpty& operator=( const GiGaStackActionEmpty& ) ; ///< no = 

};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif  ///<  GIGA_GiGaStackActionEmpty_H
// ============================================================================
