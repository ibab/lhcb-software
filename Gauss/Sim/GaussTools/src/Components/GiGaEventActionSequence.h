// $Id: GiGaEventActionSequence.h,v 1.3 2007-01-12 15:36:45 ranjard Exp $ 
#ifndef GIGA_GIGAEVENTACTIONSEQUENCE_H 
#define GIGA_GIGAEVENTACTIONSEQUENCE_H 1

// STD & STL 
#include <string>
#include <vector>
// GiGa 
#include "GiGa/GiGaEventActionBase.h"
// forward declaration
//template <class TYPE> class GiGaFactory;


/** @class GiGaEventActionSequence GiGaEventActionSequence.h
 *  
 *  A simple concrete Event Action.
 *  It is just a sequence of other Event Actions 
 *
 *  @author Ivan Belyaev
 *  @date   24/07/2001
 */

class GiGaEventActionSequence : public GiGaEventActionBase
{
  /// friend factory
  //friend class GiGaFactory<GiGaEventActionSequence>;
  
public:
  
  /// type definition  for list of type/name 
  typedef  std::vector<std::string>       MEMBERS;
  /// type definiiton for list of sequence members 
  typedef  std::vector<IGiGaEventAction*> ACTIONS;
  
  /** initialize the event action object
   *  @see GiGaEventActionBase 
   *  @see GiGaBase 
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode initialize ();
  
  /** perform begin-of-event action.
   *  @see G4UserEventAction
   *  @param event pointer to Geant4 event 
   */
  virtual void BeginOfEventAction ( const G4Event* event );
  
  /** perform end-of-event action
   *  @see    G4UserEventAction
   *  @param event pointer to Geant4 event 
   */
  virtual void EndOfEventAction   ( const G4Event* event );
  
  //protected:
public:

  /** standard constructor 
   *  @see GiGaEventActionBase
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaEventActionSequence
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~GiGaEventActionSequence( ); 
  
private:
  
  /// no default constructor 
  GiGaEventActionSequence();
  /// no copy    constructor 
  GiGaEventActionSequence( const GiGaEventActionSequence& ) ;
  /// no assignment            
  GiGaEventActionSequence& operator=( const GiGaEventActionSequence& ) ;
  
private:
  
  MEMBERS m_members ;
  ACTIONS m_actions ;
  
};
// ============================================================================

// ============================================================================
// The END
// ============================================================================
#endif ///< GIGA_GIGAEVENTACTIONSEQUENCE_H
// ============================================================================
