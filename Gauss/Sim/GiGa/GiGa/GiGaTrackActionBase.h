// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGa/GiGa/GiGaTrackActionBase.h,v 1.4 2001-03-18 14:56:13 ibelyaev Exp $ 
#ifndef     GIGA_GiGaTrackActionBase_H
#define     GIGA_GiGaTrackActionBase_H 1 

// from STL
#include <vector> 
// base class 
#include "GiGa/IGiGaTrackAction.h" 
#include "GiGa/GiGaBase.h" 
//
class G4Track;
//

/** @class GiGaTrackActionBase GiGaTrackActionBase.h GiGa/GiGaTrackActionBase.h
    
    Base class for implementation of concrete Tracking Action for GiGa
    
    @author  Vanya Belyaev
    @date    23/01/2001
*/

class GiGaTrackActionBase: virtual public IGiGaTrackAction   ,
                           public  GiGaBase
{
  ///
protected:
  /// constructor 
  GiGaTrackActionBase( const std::string& , ISvcLocator* );
  virtual ~GiGaTrackActionBase();
  ///
public:
  /// ident 
  virtual const std::string&  name       () const { return GiGaBase::name() ; }; 
  /// initialize 
  virtual StatusCode   initialize () ; 
  /// finalize 
  virtual StatusCode   finalize   () ; 
  /// Query Interface
  virtual StatusCode   queryInterface ( const InterfaceID& , void** ) ;
  ///
public:
  ///
  virtual void PreUserTrackingAction  ( const G4Track* );
  virtual void PostUserTrackingAction ( const G4Track* );
  ///
private:
  ///
  GiGaTrackActionBase ();                                       /// no default constructor 
  GiGaTrackActionBase           ( const GiGaTrackActionBase& ); /// no copy constructor!
  GiGaTrackActionBase& operator=( const GiGaTrackActionBase& ); /// no assignment! 
  ///
private:
  ///
};
///
 
#endif   // GIGA_GiGaTrackActionBase_H 














