// ============================================================================
#ifndef       GaussTools_CutsStepAction_H
#define       GaussTools_CutsStepAction_H 1 
// ============================================================================
// include files 
// GiGa
#include "GiGa/GiGaStepActionBase.h"
// forward declarations 
//template <class TYPE> class GiGaFactory;

/** @class CutsStepAction CutsStepAction.h
 *   
 *  @author  Witek Pokorski
 *  @date    1/11/2002
 */

class CutsStepAction: virtual public GiGaStepActionBase
{
  /// friend factory for instantiation
  //  friend class GiGaFactory<CutsStepAction>;

  //protected:
public:

  /** standard constructor 
   *  @see GiGaStepActionBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  CutsStepAction
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~CutsStepAction();

public:
  
  /** stepping action
   *  @see G4UserSteppingAction
   *  @param step Geant4 step
   */
  virtual void UserSteppingAction ( const G4Step* );
  
private:
  CutsStepAction(); ///< no default constructor
  CutsStepAction( const CutsStepAction& ); ///< no copy  
  CutsStepAction& operator=( const CutsStepAction& ) ; ///< no = 

  double m_trcuteg;
  double m_trcuthadr;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif  ///<    GaussTools_CutsStepAction_H
// ============================================================================
