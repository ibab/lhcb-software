// $Id: GiGaMagFieldUniform.h,v 1.2 2007-01-12 15:36:51 ranjard Exp $ 
#ifndef    GIGA_GIGAMagFieldUniform_H
#define    GIGA_GIGAMagFieldUniform_H 1 

// include files 
// GiGa 
#include "GiGa/GiGaMagFieldBase.h"
// forward declarations 
//template <class TYPE> class GiGaFactory;

/** @class GiGaMagFieldUniform  GiGaMagFieldUniform.h
 *
 *  Implemenation of "Uniform Magnetic Field" object.
 *   
 *
 *  @author Vanya Belyaev
*/

class GiGaMagFieldUniform: public GiGaMagFieldBase 
{
  /// friend factory for instantiatio 
  //  friend class GiGaFactory<GiGaMagFieldUniform>;
  
public:
  //protected:
  
  /** standard constructor 
   *  @see GiGaMagFieldBase
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaMagFieldUniform
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// virtual destructor (protected)
  virtual ~GiGaMagFieldUniform();
  
public:
  
  /** get the field value 
   *  @see G4MagneticField
   *  @param point point  
   *  @param B   value of magnetic filed in poitnt 
   */
  virtual void GetFieldValue ( const double [4], double *B  ) const ;
  
private:
  
  GiGaMagFieldUniform(); ///< no default constructor! 
  GiGaMagFieldUniform( const GiGaMagFieldUniform& ) ; ///< no copy 
  GiGaMagFieldUniform& operator=( const GiGaMagFieldUniform& ) ; ///< no = 
  
private:

  float    m_Bx ; 
  float    m_By ; 
  float    m_Bz ; 

};
// ============================================================================

// ============================================================================
#endif  // GIGA_GIGAMagFieldUniform_H
// ============================================================================
