// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGa/src/GiGaStepActionDraw.h,v 1.1 2001-03-18 14:56:15 ibelyaev Exp $
#ifndef       GIGA_GiGaStepActionDraw_H
#define       GIGA_GiGaStepActionDraw_H 1 
// GiGa 
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
  ///
  GiGaStepActionDraw( const std::string& , ISvcLocator* );
  virtual ~GiGaStepActionDraw();
  ////
 public: 
  ///
  virtual StatusCode initialize () ; 
  virtual StatusCode finalize   () ;
  ///
  virtual void UserSteppingAction( const G4Step* aStep );
  /// 
 private:
  ///
  GiGaStepActionDraw()                                ; // no default constructor
  GiGaStepActionDraw( const GiGaStepActionDraw& )            ; // no copy constructor 
  GiGaStepActionDraw& operator=( const GiGaStepActionDraw& ) ; // no assignment 
  ///
 private:
  ///
};
///
///



#endif  //    GIGA_GiGaStepActionDraw_H
