#ifndef    GIGA_GIGAMagField_H
#define    GIGA_GIGAMagField_H 1 

#include "GiGa/IGiGaMagField.h"
#include "GiGa/GiGaBase.h"



/** @class GiGaMagField     GiGaMagField.h GiGa/GiGaMagField.h

    A base class for implemenattion of Magnetic Field Objects.

    @author Vanya Belyaev
*/


class GiGaMagField: public virtual IGiGaMagField , 
		    public          GiGaBase 
{
  ///
  protected:
  /// constructor 
  GiGaMagField( const std::string& , ISvcLocator*); 
  /// virtual destructor 
  virtual ~GiGaMagField();
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
    
 private:
  ///
  GiGaMagField           ()                     ; /// no default constructor! 
  GiGaMagField           ( const GiGaMagField& ); /// no copy constructor!
  GiGaMagField& operator=( const GiGaMagField& ); /// no assignment! 
  ///
 private:
  ///
};
///
        

#endif  // GIGA_GIGAMagField_H










