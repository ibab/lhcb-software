#ifndef     GIGA_GiGaStepActionBase_H
#define     GIGA_GiGaStepActionBase_H 1 
/// STL
#include <vector> 
/// GiGa
#include "GiGa/IGiGaStepAction.h" 
#include "GiGa/GiGaBase.h" 


/** @class GiGaStepActionBase GiGaStepActionBase.h GiGa/GiGaStepActionBase.h
    
    Base class for implementation of concrete Steping Action for GiGa
    
    @author  Vanya Belyaev
    @date    23/01/2001
*/

class GiGaStepActionBase: virtual public IGiGaStepAction   ,
                          public  GiGaBase
{
  ///
protected:
  /// constructor 
  GiGaStepActionBase( const std::string& , ISvcLocator* ) ;
  virtual ~GiGaStepActionBase();
  ///
public:
  /// ident 
  virtual const std::string&  name       () const { return GiGaBase::name() ; }; 
  /// initialize 
  virtual StatusCode          initialize () ; 
  /// finalize 
  virtual StatusCode          finalize   () ; 
  /// Query Interface
  virtual StatusCode          queryInterface(const InterfaceID& , void** ) ;
  ///
protected:
  ///
  GiGaStepActionBase ();                                      /// no default constructor!
  GiGaStepActionBase           ( const GiGaStepActionBase& ); /// no copy constructor!
  GiGaStepActionBase& operator=( const GiGaStepActionBase& ); /// no assignment! 
  ///
private:
  ///
};
///

#endif   // GIGA_GiGaStepActionBase_H














