// $Id: GiGaMagFieldBase.h,v 1.8 2002-05-07 12:21:29 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
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
                        public          GiGaBase 
{
protected:
  
  /** standard constructor
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaMagFieldBase 
  ( const std::string& type   , 
    const std::string& name   , 
    const IInterface*  parent ) ; 
  
  /// virtual destructor 
  virtual ~GiGaMagFieldBase();

public:
  
  /** initialize the object 
   *  @see GiGaBase
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode   initialize     () ; 

  /** finalize the object  
   *  @see GiGaBase
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode   finalize       () ; 
  
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

// ============================================================================
// The END 
// ============================================================================
#endif  /// GIGA_GIGAMagFieldBase_H
// ============================================================================











