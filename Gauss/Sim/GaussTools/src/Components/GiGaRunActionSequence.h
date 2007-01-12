// $Id: GiGaRunActionSequence.h,v 1.3 2007-01-12 15:36:55 ranjard Exp $ 
#ifndef GIGA_GIGARUNACTIONSEQUENCE_H 
#define GIGA_GIGARUNACTIONSEQUENCE_H 1

/// STD & STL 
#include <string>
#include <vector> 
/// GiGa
#include "GiGa/GiGaRunActionBase.h"
// forward declarations
//template <class TYPE> class GiGaFactory;

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
  //friend class GiGaFactory<GiGaRunActionSequence>;

public:
  
  /// useful typedefs 
  typedef  std::vector<std::string>      MEMBERS;
  typedef  std::vector<IGiGaRunAction*>  ACTIONS;
  
  /** initialization method 
   *  @see GiGaRunActionBase 
   *  @see GiGaBase 
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode         initialize  ()        ;
  
  /** perform begin-of-run action
   *  @see G4UserRunAction
   *  @param run  pointer to Geant4 Run object
   */
  virtual void BeginOfRunAction ( const G4Run* run );
  
  /** perform end-of-run action
   *  @see G4UserRunAction
   *  @param run  pointer to Genat4 Run object
   */
  virtual void EndOfRunAction   ( const G4Run* run );
  
  //protected :
  
  /** standard constructor 
   *  @see GiGaPhysListBase
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaRunActionSequence
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected) 
  virtual ~GiGaRunActionSequence();
  
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

// ============================================================================
// The END 
// ============================================================================
#endif ///< GIGA_GIGARUNACTIONSEQUENCE_H
// ============================================================================
