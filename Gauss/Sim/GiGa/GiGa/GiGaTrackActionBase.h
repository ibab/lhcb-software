#ifndef     GIGA_GiGaTrackActionBase_H
#define     GIGA_GiGaTrackActionBase_H 1 

/// from STL
#include <vector> 
/// base class 
#include "GiGa/IGiGaTrackAction.h" 
#include "GiGa/GiGaBase.h" 


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
  inline GiGaTrackActionBase( const std::string& , ISvcLocator* );
  virtual inline ~GiGaTrackActionBase();
  ///
public:
  /// ident 
  virtual const std::string&  name       () const { return GiGaBase::name() ; }; 
  /// initialize 
  virtual inline StatusCode   initialize () ; 
  /// finalize 
  virtual inline StatusCode   finalize   () ; 
  /// Query Interface
  virtual inline StatusCode   queryInterface ( const InterfaceID& , void** ) ;
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
#include "GiGa/GiGaTrackActionBase.icpp"
///
 
#endif   // GIGA_GiGaTrackActionBase_H 














