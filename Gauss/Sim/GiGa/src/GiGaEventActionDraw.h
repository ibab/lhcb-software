// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGa/src/GiGaEventActionDraw.h,v 1.1 2001-03-18 14:56:14 ibelyaev Exp $ 
#ifndef       GIGA_GiGaEventActionDraw_H
#define       GIGA_GiGaEventActionDraw_H 1 


#include "GiGa/GiGaEventActionBase.h"

template <class EA> class GiGaEventActionFactory;

/** @class GiGaEventActionDraw GiGaEventActionDraw.h GiGaEventActionDraw.h
    
    Example of "primitive" implementation of Event Action class
    It is just Draw!
 
    @author  Vanya Belyaev
    @date    17/03/2001
*/


class GiGaEventActionDraw: virtual public GiGaEventActionBase
{
  ///
  friend class GiGaEventActionFactory<GiGaEventActionDraw>;
  ///
 protected:
  ///
  GiGaEventActionDraw( const std::string& name , ISvcLocator* Loc );
  ///  
  virtual ~GiGaEventActionDraw();
  ////
 public: 
  ///
  virtual StatusCode initialize () ; 
  virtual StatusCode finalize   () ;
  ///
  virtual void EndOfEventAction   ( const G4Event* event );
  /// 
 private:
  ///
  GiGaEventActionDraw()                                ; // no default constructor
  GiGaEventActionDraw( const GiGaEventActionDraw& )            ; // no copy constructor 
  GiGaEventActionDraw& operator=( const GiGaEventActionDraw& ) ; // no assignment 
  ///
 private:
  ///
};
///
///



#endif  //    GIGA_GiGaEventActionDraw_H
