#ifndef     GIGA_GiGaTrackAction
#define     GIGA_GiGaTrackAction 1 

///
/// from STL
///
#include <vector> 

///
/// base class 
///
#include "GiGa/IGiGaTrackAction.h" 
#include "GiGa/GiGaBase.h" 


/** @class GiGaTrackAction GiGaTrackAction.h GiGa/GiGaTrackAction.h
    
    Base class for implementation of concrete Tracking Action for GiGa
    
    @author  Vanya Belyaev
    @date    23/01/2001
*/

class GiGaTrackAction: virtual public IGiGaTrackAction   ,
		       public  GiGaBase
{
  ///
 protected:
  /// constructor 
  GiGaTrackAction( const std::string& , ISvcLocator*); 
  /// virtual destructor 
  virtual ~GiGaTrackAction();
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
  GiGaTrackAction           ( const GiGaTrackAction& ); /// no copy constructor!
  GiGaTrackAction& operator=( const GiGaTrackAction& ); /// no assignment! 
  ///
 private:
  ///
};

 
#endif   // GIGA_GiGaTrackAction














