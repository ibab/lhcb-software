// $Id: GiGaMagFieldGlobal.cpp,v 1.7 2008-05-28 16:22:38 gcorti Exp $
// Include files

// CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Units/SystemOfUnits.h"

// GaudiKernel
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

// local
#include "GiGaMagFieldGlobal.h"

/** @file
 *
 *  implementation of class GiGaMagFieldGlobal
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GiGaMagFieldGlobal )

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
{}


// ============================================================================
/// destructor
// ============================================================================
GiGaMagFieldGlobal::~GiGaMagFieldGlobal(){}


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
}


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
  m_field = HepGeom::Vector3D< double >( field.x() , field.y() ,
                                         field.z() ) ;

  if( sc.isFailure() )
    {
      using CLHEP::cm;
      using CLHEP::tesla;
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
}


// ============================================================================
// The END
// ============================================================================














