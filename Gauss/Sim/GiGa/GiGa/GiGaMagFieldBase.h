// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.6  2001/07/23 13:11:42  ibelyaev
/// the package restructurisation(II)
/// 
// ============================================================================
#ifndef    GIGA_GIGAMagFieldBase_H
#define    GIGA_GIGAMagFieldBase_H 1 
// ============================================================================
#include "GiGa/IGiGaMagField.h"
#include "GiGa/GiGaBase.h"

class IMagneticFieldSvc;

/** @class GiGaMagFieldBase     GiGaMagFieldBase.h GiGa/GiGaMagFieldBase.h
 *
 *  A base class for implementation of Magnetic Field Objects.
 *
 *   @author Vanya Belyaev
 */

class GiGaMagFieldBase: public virtual IGiGaMagField , 
                        public         GiGaBase 
{
  ///
protected:

  /** standard constructor
   *  @param name name of the object
   *  @param loc  pointer to service locator 
   */
  GiGaMagFieldBase( const std::string& name , 
                    ISvcLocator*       loc ) ; 
  /// virtual destructor 
  virtual ~GiGaMagFieldBase();
  ///
public:
  
  /// identification  
  virtual const std::string&  name    () const ;
  
  /** initialize the object 
   *  @return status code 
   */
  virtual StatusCode   initialize     () ; 

  /** finalize the object  
   *  @return status code 
   */
  virtual StatusCode   finalize       () ; 

  /** query the interface
   *  @param id uniqie interface identifier 
   *  @param i  placeholder for returned interface 
   *  @return status code 
   */
  virtual StatusCode   queryInterface ( const InterfaceID& id , 
                                        void**             i  ) ;
  ///
protected:

  /** aceess to Magnetic Field Service
   *  @return pointer to Magnetic Field Service 
   */
  inline IMagneticFieldSvc*  mfSvc() const { return m_mfSvc; }
   
private:
  ///
  GiGaMagFieldBase(); ///< no default constructor! 
  GiGaMagFieldBase( const GiGaMagFieldBase& ); ///< no copy constructor!
  GiGaMagFieldBase& operator=( const GiGaMagFieldBase& ); ///< no assignment! 
  ///
private:
  ///
  std::string          m_nameMFSvc;
  IMagneticFieldSvc*   m_mfSvc;
  ///
};
///

// ============================================================================
#endif  /// GIGA_GIGAMagFieldBase_H
// ============================================================================











