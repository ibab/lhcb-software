/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// ===========================================================================
#ifndef       GIGA_GiGaStepActionEmpty_H
#define       GIGA_GiGaStepActionEmpty_H 1 
/// ===========================================================================

#include "GiGa/GiGaStepActionBase.h"

template <class SA> 
class GiGaSensDetFactory;

/** @class GiGaStepActionEmpty GiGaStepActionEmpty.h
 *   
 *  Example of "primitive" implementation of Stepping action class
 *  It is just empty!
 *
 *   @author  Vanya Belyaev
 *  @date    23/01/2001
 */

class GiGaStepActionEmpty: virtual public GiGaStepActionBase
{
  ///
  friend class GiGaStepActionFactory<GiGaStepActionEmpty>;
  ///
 protected:
  ///
  GiGaStepActionEmpty( const std::string& , ISvcLocator* );
  virtual ~GiGaStepActionEmpty();
  ////
 public: 
  ///
  virtual StatusCode initialize () ; 
  virtual StatusCode finalize   () ;
  /// 
 private:
  ///
  GiGaStepActionEmpty(); ///< no default constructor
  GiGaStepActionEmpty( const GiGaStepActionEmpty& ); ///< no copy  
  GiGaStepActionEmpty& operator=( const GiGaStepActionEmpty& ) ; ///< no = 
  ///
 private:
  ///
};


/// ===========================================================================
#endif  ///<    GIGA_GiGaStepActionEmpty_H
/// ===========================================================================
