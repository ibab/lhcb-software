// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.1  2001/07/23 20:53:46  ibelyaev
/// reorganization of GiGaUtil namespace
/// 
// ============================================================================
#ifndef GIGA_GIGASTEPACTIONSEQUENCE_H 
#define GIGA_GIGASTEPACTIONSEQUENCE_H 1
// ============================================================================
/// STD & STL 
#include <string>
#include <vector>
/// GiGa 
#include "GiGa/GiGaStepActionBase.h"
/// forward declarations 
template <class SA>
class GiGaStepActionFactory;


/** @class GiGaStepActionSequence GiGaStepActionSequence.h
 *  
 *  The specialized Step Action. It is just an ordered 
 *  sequence of other Step Actions.  
 *
 *  @author Ivan Belyaev
 *  @date   23/07/2001
 */

class GiGaStepActionSequence: public GiGaStepActionBase
{
  /// the friend class    
  friend class GiGaStepActionFactory<GiGaStepActionSequence> ;
  ///
public:
  
  typedef std::vector<std::string>       MEMBERS;
  typedef std::vector<IGiGaStepAction*>  ACTIONS;
  
  /** initialize the object
   *  @return status code 
   */
  virtual StatusCode  initialize () ;
  
  /** finalize the object
   *  @return status code 
   */
  virtual StatusCode  finalize   () ;
  
  /** perform the stepping action
   *  @param pointer to current Geant4 step 
   */
  virtual void UserSteppingAction ( const G4Step* step );
  
protected:
  
  /** the standard constructor
   *  @param  Name name of the instance
   *  @param  Loc  pointer to service locator object 
   */
  GiGaStepActionSequence( const std::string& Name ,
                     ISvcLocator*       Loc  );
  
  /// destructor 
  virtual ~GiGaStepActionSequence();
  
  /** finalise, delete and clear actions container 
   *  @return status code 
   */
  StatusCode   actionsReset() ;
  
private:
  
  /// no default constructor!
  GiGaStepActionSequence(); 
  /// no copy constructor!
  GiGaStepActionSequence( const GiGaStepActionSequence& ) ;
  /// no assignment! 
  GiGaStepActionSequence& operator==( const GiGaStepActionSequence& );
  
private:
  
  MEMBERS m_members ; ///< list of member types/names 
  ACTIONS m_actions ; ///< members
  
};

// ============================================================================
#endif ///< GIGA_GIGASTEPACTIONSEQUENCE_H
// ============================================================================
