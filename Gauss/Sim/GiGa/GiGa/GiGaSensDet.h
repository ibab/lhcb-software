#ifndef     GIGA_GiGaSensDet
#define     GIGA_GiGaSensDet 1 

///
/// from STL
///
#include <vector> 

///
/// base class 
///
#include "GiGa/IGiGaSensDet.h" 
#include "GiGa/GiGaBase.h" 


///
/// forward declarations  
class IDetectorElement;


/** @class GiGaSensDet GiGaSensDet.h GiGa/GiGaSensDet.h
    
    Base class for implementation of concrete Sensitive Detector for GiGa
    
    @author  Vanya Belyaev
    @date    23/01/2001
*/

class GiGaSensDet: virtual public IGiGaSensDet   ,
	      public  GiGaBase
{
  ///
 protected:
  /// constructor 
  GiGaSensDet( const std::string& , ISvcLocator*); 
  /// virtual destructor 
  virtual ~GiGaSensDet();
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
  virtual StreamBuffer& serialize( StreamBuffer& )       ;
  /// serialization for writing
  virtual StreamBuffer& serialize( StreamBuffer& ) const ;
  ///
 protected:
  ///
  inline  IDetectorElement*   det     () const { return m_det     ; };  
  /// 
 protected:
  ///
  GiGaSensDet           ( const GiGaSensDet& ); /// no copy constructor!
  GiGaSensDet& operator=( const GiGaSensDet& ); /// no assignment! 
  ///
 private:
  ///
  bool                m_init                 ;  /// Init Flag
  bool                m_active               ;  /// Active Flag
  std::string         m_detName              ;  /// Detector Element Name 
  IDetectorElement*   m_det                  ;  /// Detector Element to which the SD refers 
  ///
};

 
#endif   // GIGA_GiGaSensDet














