// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.1  2001/07/24 09:48:15  ibelyaev
/// new component GiGaEventActionSequence
/// 
// ============================================================================
#ifndef GIGA_GIGAEVENTACTIONSEQUENCE_H 
#define GIGA_GIGAEVENTACTIONSEQUENCE_H 1
// ============================================================================
/// STD & STL 
#include <string>
#include <vector>
/// GiGa 
#include "GiGa/GiGaEventActionBase.h"
/// forward declarations 
template <class EA>
class GiGaEventActionFactory;


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
  friend class GiGaEventActionFactory<GiGaEventActionSequence>;
  
public:

  /// type definition  for list of type/name 
  typedef  std::vector<std::string>       MEMBERS;
  /// type definiiton for list of sequence members 
  typedef  std::vector<IGiGaEventAction*> ACTIONS;
  
  /** initialize the event action object
   *  @return status code 
   */
  virtual StatusCode initialize ();
  
  /** finalize the event action object
   *  @return status code 
   */
  virtual StatusCode finalize   ();

  /** perform begin-of-event action
   *  @param event pointer to Geant4 event 
   */
  virtual void BeginOfEventAction ( const G4Event* event );
  
  /** perform end-of-event action
   *  @param event pointer to Geant4 event 
   */
  virtual void EndOfEventAction   ( const G4Event* event );

protected:
  
  /** standard constructor
   *  @param Name name of the event action object instance
   *  @param Loc  pointer to Service Locator 
   */
  GiGaEventActionSequence( const std::string& Name ,
                           ISvcLocator*       Loc  ); 
  /// destructor 
  virtual ~GiGaEventActionSequence( ); 
  
  /** finalise, delete and clear actions container 
   *  @return status code 
   */
  StatusCode   actionsReset() ;
  
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
#endif ///< GIGA_GIGAEVENTACTIONSEQUENCE_H
// ============================================================================
