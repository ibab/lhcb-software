#ifndef     GIGA_GiGaStackAction
#define     GIGA_GiGaStackAction 1 

///
/// from STL
///
#include <vector> 

///
/// base class 
///
#include "GiGa/IGiGaStackAction.h" 
#include "GiGa/GiGaBase.h" 


/** @class GiGaStackAction GiGaStackAction.h GiGa/GiGaStackAction.h
    
    Base class for implementation of concrete Stacking Action for GiGa
    
    @author  Vanya Belyaev
    @date    23/01/2001
*/

class GiGaStackAction: virtual public IGiGaStackAction   ,
		       public  GiGaBase
{
  ///
 protected:
  /// constructor 
  GiGaStackAction( const std::string& , ISvcLocator*); 
  /// virtual destructor 
  virtual ~GiGaStackAction();
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
  /// serialization for reading
  virtual StreamBuffer&       serialize( StreamBuffer& )       ;
  /// serialization for writing
  virtual StreamBuffer&       serialize( StreamBuffer& ) const ;
  ///
 protected:
  ///
  GiGaStackAction           ( const GiGaStackAction& ); /// no copy constructor!
  GiGaStackAction& operator=( const GiGaStackAction& ); /// no assignment! 
  ///
 private:
  ///
};

 
#endif   // GIGA_GiGaStackAction














