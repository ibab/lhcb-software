#ifndef       GIGA_GiGaStackActionEmpty_H
#define       GIGA_GiGaStackActionEmpty_H 1 



#include "GiGa/GiGaStackAction.h"

template <class SA> class GiGaStackActionFactory;

/** @class GiGaStackActionEmpty GiGaStackActionEmpty.h GiGaStackActionEmpty.h
    
    Example of "primitive" implementation of stacking action class
    It is just empty!
 
    @author  Vanya Belyaev
    @date    23/01/2001
*/


class GiGaStackActionEmpty: virtual public GiGaStackAction
{
  ///
  friend class GiGaStackActionFactory<GiGaStackActionEmpty>;
  ///
 protected:
  ///
  GiGaStackActionEmpty( const std::string& name , ISvcLocator* Loc );
  ///  
  virtual ~GiGaStackActionEmpty();
  ////
 public: 
  ///
  virtual StatusCode initialize () ; 
  virtual StatusCode finalize   () ;
  /// 
 private:
  ///
  GiGaStackActionEmpty()                                ; // no default constructor
  GiGaStackActionEmpty( const GiGaStackActionEmpty& )            ; // no copy constructor 
  GiGaStackActionEmpty& operator=( const GiGaStackActionEmpty& ) ; // no assignment 
  ///
 private:
  ///
};
///
///



#endif  //    GIGA_GiGaStackActionEmpty_H
