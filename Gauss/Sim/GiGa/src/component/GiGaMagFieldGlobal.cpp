// $Id: GiGaMagFieldGlobal.cpp,v 1.7 2002-03-13 15:36:25 ibelyaev Exp $ 
// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.6  2001/08/12 15:42:51  ibelyaev
/// improvements with Doxygen comments
///
/// Revision 1.5  2001/07/27 17:03:19  ibelyaev
/// improved printout
///
/// Revision 1.4  2001/07/23 13:12:26  ibelyaev
/// the package restructurisation(II)
/// 
// ============================================================================
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Units/SystemOfUnits.h"
///
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/MsgStream.h"
///
#include "GiGa/GiGaMagFieldFactory.h"
/// local 
#include "GiGaMagFieldGlobal.h"

/** implementation of class GiGaMagFieldGlobal
 *
 *  @author Vanya Belyaev 
 */

// ============================================================================
// ============================================================================
static const  GiGaMagFieldFactory<GiGaMagFieldGlobal>         s_Factory ;
const        IGiGaMagFieldFactory&GiGaMagFieldGlobalFactory = s_Factory ; 

// ============================================================================
// ============================================================================
GiGaMagFieldGlobal::GiGaMagFieldGlobal( const std::string& nick , 
                                        ISvcLocator* loc) 
  : GiGaMagFieldBase( nick , loc ) , m_field() {};

// ============================================================================
// ============================================================================
GiGaMagFieldGlobal::~GiGaMagFieldGlobal(){};

// ============================================================================
/** initialization of the object
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaMagFieldGlobal::initialize()
{
  /// initialize the base class 
  StatusCode sc = GiGaMagFieldBase::initialize();
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class",sc);}
  ///
  if( 0 == mfSvc() ) 
    { return Error("The Magnetic field service is not located!");}
  ///
  Print("initialized succesfully") ;
  ///
  return StatusCode::SUCCESS;  
};

// ============================================================================
/** finalization of the object
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaMagFieldGlobal::finalize  ()
{
  ///
  Print("finalization") ;
  ///
  return GiGaMagFieldBase::finalize();
};

// ============================================================================
// ============================================================================
void GiGaMagFieldGlobal::GetFieldValue ( const double Point[4], 
                                         double *B  ) const 
{
  ///
  if( 0 == Point ) { Error("GetFieldValue: Point = 0 !"); return ; }
  ///
  const HepPoint3D point( Point[0] , Point[1] , Point[2] );
  StatusCode sc = mfSvc()->fieldVector( point , m_field );
  if( sc.isFailure() ) 
    { 
      Error("GetFieldValue, error status code from IMagneticFieldSvc", sc ) ; 
      MsgStream log( msgSvc() , name() ); 
      log << MSG::ERROR 
          << " Position[cm]=(" 
          << point.x() / cm << ","  
          << point.y() / cm << ","  
          << point.z() / cm << ")"
          << " Field[tesla]=(" 
          << point.x() / tesla << ","  
          << point.y() / tesla << ","  
          << point.z() / tesla << ")" << endreq ; 
    }
  ///
  if( 0 == B     ) { Error("GetFieldValue: B     = 0 !"); return ; }
  ///
  *(B+0) = m_field.x();
  *(B+1) = m_field.y();
  *(B+2) = m_field.z();
  ///
};

// ============================================================================














