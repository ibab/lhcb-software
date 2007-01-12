// ============================================================================
#ifndef       GaussTools_GaussStepAction_H
#define       GaussTools_GaussStepAction_H 1 
// ============================================================================
// include files 
// GiGa
#include "GiGa/GiGaStepActionBase.h"
// forward declarations 
//template <class TYPE> class GiGaFactory;

/** @class GaussStepAction GaussStepAction.h
 *   
 *  @author  Witek Pokorski
 *  @date    1/11/2002
 */

class GaussStepAction: virtual public GiGaStepActionBase
{
  /// friend factory for instantiation
  //  friend class GiGaFactory<GaussStepAction>;

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
  GaussStepAction
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~GaussStepAction();

public:
  
  /** stepping action
   *  @see G4UserSteppingAction
   *  @param step Geant4 step
   */
  virtual void UserSteppingAction ( const G4Step* );
  
private:

  GaussStepAction(); ///< no default constructor
  GaussStepAction( const GaussStepAction& ); ///< no copy  
  GaussStepAction& operator=( const GaussStepAction& ) ; ///< no = 

  bool m_storeHitPoints;
  int m_maxoptsteps;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif  ///<    GaussTools_GaussStepAction_H
// ============================================================================
