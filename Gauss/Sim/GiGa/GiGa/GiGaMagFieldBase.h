#ifndef    GIGA_GIGAMagFieldBase_H
#define    GIGA_GIGAMagFieldBase_H 1 

#include "GiGa/IGiGaMagField.h"
#include "GiGa/GiGaBase.h"


/** @class GiGaMagFieldBase     GiGaMagFieldBase.h GiGa/GiGaMagFieldBase.h

    A base class for implementation of Magnetic Field Objects.

    @author Vanya Belyaev
*/


class GiGaMagFieldBase: public virtual IGiGaMagField , 
			public          GiGaBase 
{
  ///
  protected:
  /// constructor 
  inline GiGaMagFieldBase( const std::string& , ISvcLocator* ) ; 
  /// virtual destructor 
  virtual inline ~GiGaMagFieldBase();
  ///
public:
  /// ident 
  virtual const std::string&  name           () const { return GiGaBase::name() ; }; 
  /// initialize 
  virtual inline StatusCode   initialize     () ; 
  /// finalize 
  virtual inline StatusCode   finalize       () ; 
  /// Query Interface
  virtual inline StatusCode   queryInterface ( const InterfaceID& , void** ) ;
  ///
private:
  ///
  GiGaMagFieldBase           ()                         ; /// no default constructor! 
  GiGaMagFieldBase           ( const GiGaMagFieldBase& ); /// no copy constructor!
  GiGaMagFieldBase& operator=( const GiGaMagFieldBase& ); /// no assignment! 
  ///
private:
  ///
};
///
#include "GiGa/GiGaMagFieldBase.icpp"
///

#endif  // GIGA_GIGAMagFieldBase_H











