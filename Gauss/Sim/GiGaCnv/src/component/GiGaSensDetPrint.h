// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGaCnv/src/component/GiGaSensDetPrint.h,v 1.1.1.1 2001-04-23 08:34:15 ibelyaev Exp $
#ifndef       GIGA_GiGaSensDetPRINT_H
#define       GIGA_GiGaSensDetPRINT_H 1 
/// GiGaCnv 
#include "GiGaCnv/GiGaSensDetBase.h"
// forward 
template <class SD> 
class GiGaSensDetFactory;

/** @class GiGaSensDetPrint GiGaSensDetPrint.h GiGaSensDetPrint.h
    
    Example of "primitive" implementation of sensitive detector
    just print some information inside G4 sensitive volume, no 
    real hit creation
 
    @author  Vanya Belyaev
    @date    23/01/2001
*/


class GiGaSensDetPrint: virtual public GiGaSensDetBase
{
  ///
  friend class GiGaSensDetFactory<GiGaSensDetPrint>;
  ///
 protected:
  ///
  GiGaSensDetPrint( const std::string&  , ISvcLocator* );
  virtual ~GiGaSensDetPrint();
  ////
 public: 
  ///
  virtual StatusCode initialize () ; 
  virtual StatusCode finalize   () ;
  /// 
 protected:
  ///
  virtual bool ProcessHits( G4Step*,G4TouchableHistory* ) ; /// "the main" method :-))
  ///
 private:
  ///
  GiGaSensDetPrint()                                ; // no default constructor
  GiGaSensDetPrint( const GiGaSensDetPrint& )            ; // no copy constructor 
  GiGaSensDetPrint& operator=( const GiGaSensDetPrint& ) ; // no assignment 
  ///
 private:
  ///
};
///
///



#endif  //    GIGA_GiGaSensDetPRINT_H
