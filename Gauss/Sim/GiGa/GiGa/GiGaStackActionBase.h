#ifndef     GIGA_GiGaStackActionBase_H
#define     GIGA_GiGaStackActionBase_H 1 

/// base class 
#include "GiGa/IGiGaStackAction.h" 
#include "GiGa/GiGaBase.h" 


/** @class GiGaStackActionBase GiGaStackActionBase.h GiGa/GiGaStackActionBase.h
    
    Base class for implementation of concrete Stacking Action for GiGa
    
    @author  Vanya Belyaev
    @date    23/01/2001
*/

class GiGaStackActionBase: virtual public IGiGaStackAction   ,
			   public  GiGaBase
{
  ///
protected:
  /// constructor 
  GiGaStackActionBase( const std::string& , ISvcLocator* );
  /// virtual destructor 
  virtual ~GiGaStackActionBase();
  ///
public:
  /// ident 
  virtual const std::string& name       () const { return GiGaBase::name() ; }; 
  /// initialize 
  virtual StatusCode  initialize () ; 
  /// finalize 
  virtual StatusCode  finalize   () ; 
  /// Query Interface
  virtual StatusCode  queryInterface ( const InterfaceID& , void** ) ;
  ///
private:
  ///
  GiGaStackActionBase ()                                      ; /// no default constructor!                   
  GiGaStackActionBase           ( const GiGaStackActionBase& ); /// no copy constructor!
  GiGaStackActionBase& operator=( const GiGaStackActionBase& ); /// no assignment! 
  ///
private:
  ///
};
///

#endif   // GIGA_GiGaStackActionBase_H














