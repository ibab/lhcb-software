// $Id: GiGaMagFieldGlobal.h,v 1.2 2007-01-12 15:36:51 ranjard Exp $
#ifndef    GIGA_GIGAMagFieldGLOBAL_H
#define    GIGA_GIGAMagFieldGLOBAL_H 1

// include files
// GiGa
#include "GiGa/GiGaMagFieldBase.h"
// forward declarations
//template <class TYPE> class GiGaFactory;

/** @class GiGaMagFieldGlobal GiGaMagFieldGlobal.h
 *
 *   Implemenation of "Global Magnetic Field" object.
 *  It is just the delegation to IMagneticSvc
 *
 *  @author Vanya Belyaev
 */

class GiGaMagFieldGlobal: public GiGaMagFieldBase
{
  /// friend factory for instantiation
  //friend class GiGaFactory<GiGaMagFieldGlobal>;

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
  GiGaMagFieldGlobal
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;


  /// virtual destructor (protected)
  virtual ~GiGaMagFieldGlobal();

public:

  /** inititalizetion of the object
   *  @see GiGaMagFieldBase
   *  @see GiGaBase
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code
   */
  virtual StatusCode initialize ();

  /** get the field value
   *  @see G4MagneticField
   *  @param  point
   *  @param   B field
   */
  virtual void GetFieldValue ( const double [4], double *B  ) const ;

private:

  GiGaMagFieldGlobal(); ///< no default constructor!
  GiGaMagFieldGlobal( const GiGaMagFieldGlobal& ) ; ///< no copy constructor!
  GiGaMagFieldGlobal& operator=( const GiGaMagFieldGlobal& ) ; ///< no =

private:
  ///
  mutable HepGeom::Vector3D<double> m_field;
  ///
};
// ============================================================================

// ============================================================================
// The END
// ============================================================================
#endif  ///< GIGA_GIGAMagFieldGLOBAL_H
// ============================================================================
