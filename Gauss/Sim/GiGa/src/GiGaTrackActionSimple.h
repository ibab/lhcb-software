#ifndef       GIGA_GiGaTrackActionSimple_H
#define       GIGA_GiGaTrackActionSimple_H 1 
/// GiGa
#include "GiGa/GiGaTrackActionBase.h"
///
template <class TA> 
class GiGaTrackActionFactory;
class G4Track;


/** @class GiGaTrackActionSimple GiGaTrackActionSimple.h GiGaTrackActionSimple.h
    
    Example of "simple" implementation of Tracking Action Class
    It uses GiGaTrajectory, and thats why the results(trajectories) 
    could be converted into Gaudi Transient Store.  
    It is just "Simple"!
    
 
    @author  Vanya Belyaev
    @date    23/01/2001
*/


class GiGaTrackActionSimple: virtual public GiGaTrackActionBase
{
  ///
  friend class GiGaTrackActionFactory<GiGaTrackActionSimple>;
  ///
 protected:
  ///
  GiGaTrackActionSimple( const std::string& , ISvcLocator* );
  ///  
  virtual ~GiGaTrackActionSimple();
  ////
 public: 
  ///
  virtual StatusCode initialize () ; 
  virtual StatusCode finalize   () ;
  ///  
  virtual void PreUserTrackingAction  ( const G4Track* ) ;
  virtual void PostUserTrackingAction ( const G4Track* ) ;
  ///
protected:
  ///
  inline G4TrackingManager* trackMgr() const 
  { return G4UserTrackingAction::fpTrackingManager; } 
  ///
private:
  ///
  GiGaTrackActionSimple()                                         ; // no default constructor
  GiGaTrackActionSimple( const GiGaTrackActionSimple& )            ; // no copy constructor 
  GiGaTrackActionSimple& operator=( const GiGaTrackActionSimple& ) ; // no assignment 
  ///
 private:
  ///
};
///


#endif  //    GIGA_GiGaTrackActionSimple_H
