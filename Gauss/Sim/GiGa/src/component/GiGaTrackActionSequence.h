/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
#ifndef COMPONENT_GIGATRACKACTIONSEQUENCE_H 
#define COMPONENT_GIGATRACKACTIONSEQUENCE_H 1
/// ===========================================================================
/// STD & STL 
#include <string>
#include <vector>
/// GiGa 
#include "GiGa/GiGaTrackActionBase.h"
/// forward decalrations 
template <class TA>
class GiGaTrackActionFactory;

/** @class GiGaTrackActionSequence GiGaTrackActionSequence.h
 *
 *  A concrete implementation of IGiGaTrackAction interface
 *  Simple action - just a chain/sequence of other Track Actions   
 *
 *  @author Ivan Belyaev
 *  @date   24/07/2001
 */

class GiGaTrackActionSequence: public GiGaTrackActionBase
{
  /// friend factory
  friend class GiGaTrackActionFactory<GiGaTrackActionSequence>;
  ///
public:
  
  typedef  std::vector<std::string>        MEMBERS ;
  typedef  std::vector<IGiGaTrackAction*>  ACTIONS ; 
  
  /** initialise the action object 
   *  @return status code
   */ 
  virtual StatusCode initialize () ; 
  
  /** finalize the action object 
   *  @return status code
   */ 
  virtual StatusCode finalize   () ;
  
  /** perform the pre-action
   *  @param track pointer to Geant4 track object 
   */ 
  virtual void PreUserTrackingAction  ( const G4Track* track );
  
  /** perform the post-action
   *  @param track pointer to Geant4 track object 
   */ 
  virtual void PostUserTrackingAction ( const G4Track* track );
  
protected:

  /** standard constructor
   *  @param Name name of this concrete instance 
   *  @param Loc  pointer to Service Locator 
   */
  GiGaTrackActionSequence( const std::string& Name ,
                           ISvcLocator*       Loc  ); 
  
  /// destructor 
  virtual ~GiGaTrackActionSequence();

  /** reset all members(actions)
   *  @return status code 
   */
  virtual StatusCode actionsReset() ;
  
private:

  /// no default constructor
  GiGaTrackActionSequence();
  /// no copy constructor 
  GiGaTrackActionSequence( const GiGaTrackActionSequence& );
  /// no assignment
  GiGaTrackActionSequence& operator=( const GiGaTrackActionSequence& );
  
private:
  
  MEMBERS   m_members;
  ACTIONS   m_actions;
  
};

/// ===========================================================================
#endif ///< COMPONENT_GIGATRACKACTIONSEQUENCE_H
/// ===========================================================================
