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
  GiGaMagFieldBase( const std::string& , ISvcLocator* ) ; 
  /// virtual destructor 
  virtual ~GiGaMagFieldBase();
  ///
public:
  /// ident 
  virtual const std::string&  name           () const { return GiGaBase::name() ; }; 
  /// initialize 
  virtual StatusCode   initialize     () ; 
  /// finalize 
  virtual StatusCode   finalize       () ; 
  /// Query Interface
  virtual StatusCode   queryInterface ( const InterfaceID& , void** ) ;
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

#endif  // GIGA_GIGAMagFieldBase_H











