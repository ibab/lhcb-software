#ifndef     GIGA_GiGaStepAction
#define     GIGA_GiGaStepAction 1 

///
/// from STL
///
#include <vector> 

///
/// base class 
///
#include "GiGa/IGiGaStepAction.h" 
#include "GiGa/GiGaBase.h" 


/** @class GiGaStepAction GiGaStepAction.h GiGa/GiGaStepAction.h
    
    Base class for implementation of concrete Steping Action for GiGa
    
    @author  Vanya Belyaev
    @date    23/01/2001
*/

class GiGaStepAction: virtual public IGiGaStepAction   ,
		       public  GiGaBase
{
  ///
 protected:
  /// constructor 
  GiGaStepAction( const std::string& , ISvcLocator*); 
  /// virtual destructor 
  virtual ~GiGaStepAction();
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
  GiGaStepAction           ( const GiGaStepAction& ); /// no copy constructor!
  GiGaStepAction& operator=( const GiGaStepAction& ); /// no assignment! 
  ///
 private:
  ///
};

 
#endif   // GIGA_GiGaStepAction














