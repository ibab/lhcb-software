// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.1  2001/07/26 16:09:23  ibelyaev
/// GiGaRunActions budiness
/// 
// ============================================================================
#ifndef GIGA_GIGARUNACTIONSEQUENCE_H 
#define GIGA_GIGARUNACTIONSEQUENCE_H 1
// ============================================================================
/// STD & STL 
#include <string>
#include <vector> 
/// GiGa
#include "GiGa/GiGaRunActionBase.h"

/** @class GiGaRunActionSequence GiGaRunActionSequence.h
 *  
 *  Concrete component of GiGarun Action 
 *  It is just a sequence(chain) of other run actions 
 *
 *  @author Ivan Belyaev
 *  @date   26/07/2001
 */

class GiGaRunActionSequence: public GiGaRunActionBase
{
  /// friend factory for instantiation 
  friend class GiGaRunActionFactory<GiGaRunActionSequence>;
  ///  
public:
  
  /// useful typedefs 
  typedef  std::vector<std::string>      MEMBERS;
  typedef  std::vector<IGiGaRunAction*>  ACTIONS;
  
  /** initialization method 
   *  @return status code 
   */
  virtual StatusCode         initialize  ()        ;
  
  /** finalization method 
   *  @return status code 
   */
  virtual StatusCode         finalize    ()        ;
  
  /** perform begin-of-run action
   *  @param run  pointer to Geant4 Run object
   */
  virtual void BeginOfRunAction ( const G4Run* run );
  
  /** perform end-of-run action
   *  @param run  pointer to Genat4 Run object
   */
  virtual void EndOfRunAction   ( const G4Run* run );
  
protected :
  
  /** standard constructor
   *  @param Name name of this concrete instance of Run Action Object 
   *  @param Loc  pointer to Service Locator 
   */ 
  GiGaRunActionSequence( const std::string Name ,
                     ISvcLocator*      Loc  ); 
 /// destructor 
  virtual ~GiGaRunActionSequence();
  
  /** finalise, delete and clear actions container 
   *  @return status code 
   */
  StatusCode   actionsReset() ;
  
private: 
  
  /// no default constructor 
  GiGaRunActionSequence();
  ///  no copy constructor 
  GiGaRunActionSequence( const GiGaRunActionSequence& );
  /// no assignment 
  GiGaRunActionSequence& operator=( const GiGaRunActionSequence& );
  
private:

  MEMBERS   m_members ;
  ACTIONS   m_actions ;

};

// ============================================================================
#endif ///< GIGA_GIGARUNACTIONSEQUENCE_H
// ============================================================================
