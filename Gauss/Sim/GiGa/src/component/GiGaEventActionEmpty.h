// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGa/src/component/GiGaEventActionEmpty.h,v 1.1 2001-04-23 08:52:05 ibelyaev Exp $ 
#ifndef       GIGA_GiGaEventActionEmpty_H
#define       GIGA_GiGaEventActionEmpty_H 1 



#include "GiGa/GiGaEventActionBase.h"

template <class EA> class GiGaEventActionFactory;

/** @class GiGaEventActionEmpty GiGaEventActionEmpty.h GiGaEventActionEmpty.h
    
    Example of "primitive" implementation of Event Action class
    It is just empty!
 
    @author  Vanya Belyaev
    @date    17/03/2001
*/


class GiGaEventActionEmpty: virtual public GiGaEventActionBase
{
  ///
  friend class GiGaEventActionFactory<GiGaEventActionEmpty>;
  ///
 protected:
  ///
  GiGaEventActionEmpty( const std::string& name , ISvcLocator* Loc );
  ///  
  virtual ~GiGaEventActionEmpty();
  ////
 public: 
  ///
  virtual StatusCode initialize () ; 
  virtual StatusCode finalize   () ;
  ///
  virtual void BeginOfEventAction ( const G4Event* );
  virtual void EndOfEventAction   ( const G4Event* );
  /// 
 private:
  ///
  GiGaEventActionEmpty()                                ; // no default constructor
  GiGaEventActionEmpty( const GiGaEventActionEmpty& )            ; // no copy constructor 
  GiGaEventActionEmpty& operator=( const GiGaEventActionEmpty& ) ; // no assignment 
  ///
 private:
  ///
};
///
///



#endif  //    GIGA_GiGaEventActionEmpty_H
