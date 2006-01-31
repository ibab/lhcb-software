// $Id: GiGaMagFieldGlobal.cpp,v 1.4 2006-01-31 11:26:44 gcorti Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2003/10/30 17:00:27  witoldp
// G4cout replaced by std::cout
//
// Revision 1.2  2003/10/23 08:49:44  witoldp
// command track action added
//
// Revision 1.1  2002/09/26 18:10:56  ibelyaev
//  repackageing: add all concrete implementations from GiGa
//
// Revision 1.11  2002/05/07 12:21:35  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// ============================================================================
// CLHEP 
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"

// LHCb (MathCore)
#include "Kernel/CLHEP_Units_defs.h"
#include "Kernel/Point3DTypes.h"
#include "Kernel/Vector3DTypes.h"
#include "ClhepTools/MathCore2Clhep.h"

// GaudiKernel
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/MsgStream.h"
// GiGa 
#include "GiGa/GiGaMACROs.h"

// local 
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
IMPLEMENT_GiGaFactory( GiGaMagFieldGlobal );
// ============================================================================

// ============================================================================
/** standard constructor 
 *  @see GiGaMagFieldBase
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GiGaMagFieldGlobal::GiGaMagFieldGlobal
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaMagFieldBase( type , name , parent  ) 
  , m_field() 
{};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GiGaMagFieldGlobal::~GiGaMagFieldGlobal(){};
// ============================================================================

// ============================================================================
/** initialization of the object
 *  @see GiGaMagFieldBase 
 *  @see GiGaBase 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaMagFieldGlobal::initialize()
{
  // initialize the base class 
  StatusCode sc = GiGaMagFieldBase::initialize();
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class",sc);}
  //
  if( 0 == mfSvc() ) 
    { return Error("The Magnetic Field Service is not located!");}
  //
  return Print("Initialized succesfully" , 
               StatusCode::SUCCESS       , MSG::VERBOSE ) ;
};
// ============================================================================

// ============================================================================
/** get the field value 
 *  @see G4MagneticField
 *  @param  point 
 *  @param   B field 
 */
// ============================================================================
void GiGaMagFieldGlobal::GetFieldValue 
( const double Point[4] , 
  double*      B        ) const 
{
  // check 
  if( 0 == Point ) { Error("GetFieldValue: Point = 0 !"); return ; }
  if( 0 == B     ) { Error("GetFieldValue: B     = 0 !"); return ; }

  // point 
  //const HepPoint3D point( Point[0] , Point[1] , Point[2] );
  const Gaudi::XYZPoint point( Point[0] , Point[1] , Point[2] );
  Gaudi::XYZVector field(0.,0.,0.);
  StatusCode sc = mfSvc()->fieldVector( point , field );
  m_field = LHCb::math2clhep::vector3D( field );

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
          << m_field.x() / tesla << ","  
          << m_field.y() / tesla << ","  
          << m_field.z() / tesla << ")" << endreq ; 
    }
  ///
  *(B+0) = m_field.x();
  *(B+1) = m_field.y();
  *(B+2) = m_field.z();
  ///
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================














