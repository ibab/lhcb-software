// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.5  2001/07/27 17:03:19  ibelyaev
/// improved printout
///
/// Revision 1.4  2001/07/23 13:12:27  ibelyaev
/// the package restructurisation(II)
/// 
// ============================================================================
#ifndef    GIGA_GIGAMagFieldGLOBAL_H
#define    GIGA_GIGAMagFieldGLOBAL_H 1 
// ============================================================================

#include "GiGa/GiGaMagFieldBase.h"

template <class MagField> 
class GiGaMagFieldFactory;

/** @class GiGaMagFieldGlobal GiGaMagFieldGlobal.h
 *
 *   Implemenation of "Global Magnetic Field" object.
 *  It is just the delegation to IMagneticSvc 
 *
 *  @author Vanya Belyaev
 */

class GiGaMagFieldGlobal: public GiGaMagFieldBase 
{
  ///
  friend class GiGaMagFieldFactory<GiGaMagFieldGlobal>;
  ///
 protected:

  /** standard constructor 
   *  @param Name name of the instance
   *  @param Loc pointer to servcie locator 
   */
  GiGaMagFieldGlobal( const std::string& Name , 
                      ISvcLocator* Loc ); 
  /// virtual destructor 
  virtual ~GiGaMagFieldGlobal();
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
  
  /** get the field value 
   *  @param  point 
   *  @param   B field 
   */
  virtual void GetFieldValue ( const double [3], double *B  ) const ;
  ///
private:
  ///
  GiGaMagFieldGlobal(); ///< no default constructor! 
  GiGaMagFieldGlobal( const GiGaMagFieldGlobal& ) ; ///< no copy constructor!
  GiGaMagFieldGlobal& operator=( const GiGaMagFieldGlobal& ) ; ///< no =  
  ///
private:
  ///
  mutable HepVector3D  m_field;
  ///
};
///



// ============================================================================
#endif  ///< GIGA_GIGAMagFieldGLOBAL_H
// ============================================================================
