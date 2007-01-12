// $Id: GiGaStepActionSequence.h,v 1.3 2007-01-12 15:36:57 ranjard Exp $ 
#ifndef GIGA_GIGASTEPACTIONSEQUENCE_H 
#define GIGA_GIGASTEPACTIONSEQUENCE_H 1

/// STD & STL 
#include <string>
#include <vector>
/// GiGa 
#include "GiGa/GiGaStepActionBase.h"
/// forward declarations 
//template <class TYPE> class GiGaFactory;


/** @class GiGaStepActionSequence GiGaStepActionSequence.h
 *  
 *  The specialized Step Action. It is just an ordered 
 *  sequence of other Step Actions.  
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   23/07/2001
 */

class GiGaStepActionSequence: public GiGaStepActionBase
{
  /// the friend factory for instantiation   
  //friend class GiGaFactory<GiGaStepActionSequence> ;

public:
  
  typedef std::vector<std::string>       MEMBERS;
  typedef std::vector<IGiGaStepAction*>  ACTIONS;
  
  /** initialize the object
   *  @see GiGaStepActionBase 
   *  @see GiGaBase 
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode  initialize () ;
  
  /** perform the stepping action
   *  @see G4UserSteppingAction
   *  @param pointer to current Geant4 step 
   */
  virtual void UserSteppingAction ( const G4Step* step );
  
  //protected:
  
  /** standard constructor 
   *  @see GiGaStepActionBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaStepActionSequence
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~GiGaStepActionSequence();
  
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

// ============================================================================
// The END 
// ============================================================================
#endif ///< GIGA_GIGASTEPACTIONSEQUENCE_H
// ============================================================================
