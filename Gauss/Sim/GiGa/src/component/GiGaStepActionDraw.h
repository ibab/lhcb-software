/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
#ifndef       GIGA_GiGaStepActionDraw_H
#define       GIGA_GiGaStepActionDraw_H 1 /
/// ===========================================================================
/// GiGa 
#include "GiGa/GiGaStepActionBase.h"

template <class SA> 
class GiGaStepActionFactory;
class G4Step;

/** @class GiGaStepActionDraw GiGaStepActionDraw.h GiGaStepActionDraw.h
    
    Example of "primitive" implementation of Stepping action class
    It performs visualization of steps!
 
    @author  Vanya Belyaev
    @date    17/03/2001
*/


class GiGaStepActionDraw: virtual public GiGaStepActionBase
{
  ///
  friend class GiGaStepActionFactory<GiGaStepActionDraw>;
  ///
 protected:

  /** standard constructor 
   *  @param name name of this ocncrete instance
   *  @param Loc  pointer to service locator 
   */
  GiGaStepActionDraw( const std::string& Name , 
                      ISvcLocator*       Loc  );
  /// destructor 
  virtual ~GiGaStepActionDraw();
  ////
 public: 

  /** initialization 
   *  @return status code 
   */
  virtual StatusCode initialize () ; 
  
  /** finalization 
   *  @return status code 
   */
  virtual StatusCode finalize   () ;
  
  /** the "main" method
   *  @param aStep  pointer to current step object
   */
  virtual void UserSteppingAction( const G4Step* aStep );
  /// 
private:
  ///
  GiGaStepActionDraw() ; ///< no default constructor
  GiGaStepActionDraw( const GiGaStepActionDraw& ); ///< no copy constructor 
  GiGaStepActionDraw& operator=( const GiGaStepActionDraw& ) ; // no assignment 
  ///
 private:
  ///
};

/// ===========================================================================
#endif  ///<    GIGA_GiGaStepActionDraw_H
/// ===========================================================================
