// $Id: GiGaMagFieldUniform.h,v 1.7 2002-03-13 16:22:12 ibelyaev Exp $ 
// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.6  2001/08/12 15:42:51  ibelyaev
/// improvements with Doxygen comments
///
/// Revision 1.5  2001/07/27 17:03:20  ibelyaev
/// improved printout
///
/// Revision 1.4  2001/07/23 13:12:27  ibelyaev
/// the package restructurisation(II)
/// 
// ============================================================================
#ifndef    GIGA_GIGAMagFieldUniform_H
#define    GIGA_GIGAMagFieldUniform_H 1 
// ============================================================================

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

  virtual void GetFieldValue ( const double [4], double *B  ) const ;
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

// ============================================================================
#endif  // GIGA_GIGAMagFieldUniform_H
// ============================================================================
