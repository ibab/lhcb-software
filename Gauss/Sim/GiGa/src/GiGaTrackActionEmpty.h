#ifndef       GIGA_GiGaTrackActionEmpty_H
#define       GIGA_GiGaTrackActionEmpty_H 1 



#include "GiGa/GiGaTrackAction.h"

template <class TA> class GiGaTrackActionFactory;

/** @class GiGaTrackActionEmpty GiGaTrackActionEmpty.h GiGaTrackActionEmpty.h
    
    Example of "primitive" implementation of Tracking action class
    It is just empty!
 
    @author  Vanya Belyaev
    @date    23/01/2001
*/


class GiGaTrackActionEmpty: virtual public GiGaTrackAction
{
  ///
  friend class GiGaTrackActionFactory<GiGaTrackActionEmpty>;
  ///
 protected:
  ///
  GiGaTrackActionEmpty( const std::string& name , ISvcLocator* Loc );
  ///  
  virtual ~GiGaTrackActionEmpty();
  ////
 public: 
  ///
  virtual StatusCode initialize () ; 
  virtual StatusCode finalize   () ;
  /// 
 private:
  ///
  GiGaTrackActionEmpty()                                         ; // no default constructor
  GiGaTrackActionEmpty( const GiGaTrackActionEmpty& )            ; // no copy constructor 
  GiGaTrackActionEmpty& operator=( const GiGaTrackActionEmpty& ) ; // no assignment 
  ///
 private:
  ///
};
///
///



#endif  //    GIGA_GiGaTrackActionEmpty_H
