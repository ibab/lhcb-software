// $Id: GiGaMagFieldGlobal.cpp,v 1.10 2002-04-25 13:02:05 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
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

/** @file 
 *
 *  implementation of class GiGaMagFieldGlobal
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */

// ============================================================================
// Factory
// ============================================================================
IMPLEMENT_GiGaMagField( GiGaMagFieldGlobal );

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
void GiGaMagFieldGlobal::GetFieldValue 
( const double Point[4], 
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
// The END 
// ============================================================================














