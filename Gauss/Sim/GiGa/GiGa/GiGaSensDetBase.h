#ifndef     GIGA_GiGaSensDetBase_H
#define     GIGA_GiGaSensDetBase_H 1 
/// GiGa
#include "GiGa/IGiGaSensDet.h" 
#include "GiGa/GiGaBase.h" 
///
class IDetectorElement;

/** @class GiGaSensDetBase GiGaSensDetBase.h GiGa/GiGaSensDetBase.h
    
    Base class for implementation of concrete Sensitive Detector for GiGa
    
    @author  Vanya Belyaev
    @date    23/01/2001
*/

class GiGaSensDetBase: virtual public IGiGaSensDet   ,
		       public  GiGaBase
{
  ///
protected:
  /// constructor 
  GiGaSensDetBase( const std::string& , ISvcLocator* );
  /// virtual destructor 
  virtual ~GiGaSensDetBase();
  ///
public:
  /// ident 
  virtual const std::string&    name      () const { return GiGaBase::name() ; }; 
  /// initialize 
  virtual StatusCode    initialize () ; 
  /// finalize 
  virtual StatusCode    finalize   () ; 
  /// Query Interface
  virtual StatusCode    queryInterface ( const InterfaceID& , void** ) ;
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
  GiGaSensDetBase           ( const GiGaSensDetBase& ); /// no copy constructor!
  GiGaSensDetBase& operator=( const GiGaSensDetBase& ); /// no assignment! 
  ///
private:
  ///
  bool                m_init                 ;  /// Init Flag
  bool                m_active               ;  /// Active Flag
  std::string         m_detName              ;  /// Detector Element Name 
  IDetectorElement*   m_det                  ;  /// Detector Element to which the SD refers 
  ///
};
///

#endif   // GIGA_GiGaSensDet_H














