/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.4  2001/07/23 13:12:27  ibelyaev
/// the package restructurisation(II)
/// 
/// ===========================================================================
#ifndef    GIGA_GIGAMagFieldUniform_H
#define    GIGA_GIGAMagFieldUniform_H 1 
/// ===========================================================================

#include "GiGa/GiGaMagFieldBase.h"

template <class MagField> class GiGaMagFieldFactory;


/** @class GiGaMagFieldUniform  GiGaMagFieldUniform.h
 *
 *  Implemenation of "Uniform Magnetic Field" object.
 *   
 *
 *  @author Vanya Belyaev
*/

class GiGaMagFieldUniform: public GiGaMagFieldBase 
{
  ///
  friend class GiGaMagFieldFactory<GiGaMagFieldUniform>;
  ///
 protected:
  /// constructor 
  GiGaMagFieldUniform( const std::string& , ISvcLocator*); 
  /// virtual destructor 
  virtual ~GiGaMagFieldUniform();
  ///
 public:

  /** initialization of the object
   *  @return status code 
   */
  virtual StatusCode initialize();
  
  /** finalization of the object
   *  @return status code 
   */
  virtual StatusCode finalize  ();

  virtual void GetFieldValue ( const double [3], double *B  ) const ;
  ///
 private:
  ///
  GiGaMagFieldUniform(); ///< no default constructor! 
  GiGaMagFieldUniform( const GiGaMagFieldUniform& ) ; ///< no copy 
  GiGaMagFieldUniform& operator=( const GiGaMagFieldUniform& ) ; ///< no = 
  ///
 private:
  ///
  float    m_Bx ; 
  float    m_By ; 
  float    m_Bz ; 
  ///
};
///

/// ===========================================================================
#endif  // GIGA_GIGAMagFieldUniform_H
/// ===========================================================================
