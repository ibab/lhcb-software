 /// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// ===========================================================================
#ifndef       GIGA_GiGaTrackActionEmpty_H
#define       GIGA_GiGaTrackActionEmpty_H 1 
/// ===========================================================================
// GiGa
#include "GiGa/GiGaTrackActionBase.h"
//
template <class TA> 
class GiGaTrackActionFactory;

/** @class GiGaTrackActionEmpty GiGaTrackActionEmpty.h 
 *
 *  Example of "primitive" implementation of Tracking action class
 *  It is just empty!
 *
 *  @author  Vanya Belyaev
 *  @date    23/01/2001
 */

class GiGaTrackActionEmpty: virtual public GiGaTrackActionBase
{
  ///
  friend class GiGaTrackActionFactory<GiGaTrackActionEmpty>;
  ///
protected:
  ///
  GiGaTrackActionEmpty( const std::string& , ISvcLocator* );
  ///  
  virtual ~GiGaTrackActionEmpty();
  ////
public: 
  ///
  virtual StatusCode initialize () ; 
  virtual StatusCode finalize   () ;
  ///   
public:
  ///  
  virtual void PreUserTrackingAction  ( const G4Track* );
  virtual void PostUserTrackingAction ( const G4Track* );
  /// 
private:
  ///
  GiGaTrackActionEmpty(); ///< no default constructor
  GiGaTrackActionEmpty( const GiGaTrackActionEmpty& ); ///< no copy  
  GiGaTrackActionEmpty& operator=( const GiGaTrackActionEmpty& ) ; ///< no = 
  ///
 private:
  ///
};
///


/// ===========================================================================
#endif  ///< GIGA_GiGaTrackActionEmpty_H
/// ===========================================================================
