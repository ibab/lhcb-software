/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $  
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
#ifndef     GIGA_GiGaSensDetBase_H
#define     GIGA_GiGaSensDetBase_H 1 
/// ===========================================================================
/// GiGa
#include "GiGa/IGiGaSensDet.h" 
#include "GiGa/GiGaBase.h" 
///

/** @class GiGaSensDetBase GiGaSensDetBase.h GiGa/GiGaSensDetBase.h
 *
 *  Base class for implementation of concrete Sensitive Detector for GiGa
 *  
 *  @author  Vanya Belyaev
 *  @date    23/01/2001
 */

class GiGaSensDetBase: virtual public IGiGaSensDet ,
                       public          GiGaBase
{
  ///
protected:

  /** standard constructor   
   *  @param name name of this sensitive detector instance 
   *  @param Svc pointer to service locator 
   */
  GiGaSensDetBase( const std::string& name , 
                   ISvcLocator*       Svc  );
  /// virtual destructor 
  virtual ~GiGaSensDetBase();
  ///
public:

  /** ident
   *  @return name of the given sensitive detector instance 
   */
  virtual const std::string& name       () const ;
  
  /** initialize the sensitive detector  
   *  @return status code 
   */
  virtual StatusCode         initialize () ; 

  /** finalize the sensitive detector  
   *  @return status code 
   */
  virtual StatusCode         finalize   () ; 

  /** query the interface
   *  @param ID   uniqie interface identifier 
   *  @param ppI  the placeholder for returned interface 
   *  @return status code 
   */
  virtual StatusCode    queryInterface ( const InterfaceID& ID , 
                                         void** ppI ) ;

  /// serialization for reading
  virtual StreamBuffer& serialize( StreamBuffer& )       ;
  /// serialization for writing
  virtual StreamBuffer& serialize( StreamBuffer& ) const ;
  ///
  /// 
protected:
  ///
  GiGaSensDetBase           ( const GiGaSensDetBase& ); /// no copy constructor!
  GiGaSensDetBase& operator=( const GiGaSensDetBase& ); /// no assignment! 
  ///
private:
  ///
  bool                m_init    ;  ///< Init Flag
  bool                m_active  ;  ///< Active Flag
  std::string         m_detName ;  ///< Det name 
  ///
};
///

#endif   // GIGA_GiGaSensDet_H














