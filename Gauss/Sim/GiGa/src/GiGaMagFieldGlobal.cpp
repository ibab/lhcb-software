///
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


///////////////////////////////////////////////////////////////////////////////////////
static const  GiGaMagFieldFactory<GiGaMagFieldGlobal>           s_GiGaMagFieldGlobalFactory ;
const        IGiGaMagFieldFactory&  GiGaMagFieldGlobalFactory = s_GiGaMagFieldGlobalFactory ; 
///////////////////////////////////////////////////////////////////////////////////////
GiGaMagFieldGlobal::GiGaMagFieldGlobal( const std::string& nick , ISvcLocator* loc) : GiGaMagField ( nick , loc ){};
///////////////////////////////////////////////////////////////////////////////////////
GiGaMagFieldGlobal::~GiGaMagFieldGlobal(){};
///////////////////////////////////////////////////////////////////////////////////////
void GiGaMagFieldGlobal::GetFieldValue ( const double Point[3], double *B  ) const 
{
  ///
  HepPoint3D  point( Point[0] , Point[1] , Point[2] );
  HepVector3D field;
  ///
  {
    StatusCode sc = mfSvc()->fieldVector( point , field );
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
  }
  ///
  *(B+0) = field.x();
  *(B+1) = field.y();
  *(B+2) = field.z();
  ///
};
///////////////////////////////////////////////////////////////////////////////////////




