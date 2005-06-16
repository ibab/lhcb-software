// $Id: RichG4ReconFlatMirr.cpp,v 1.4 2005-06-16 11:39:59 seaso Exp $
// Include files


#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IMessageSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/TabulatedProperty.h"

#include <CLHEP/Geometry/Point3D.h>
#include <CLHEP/Geometry/Vector3D.h>
#include <CLHEP/Geometry/Transform3D.h>

// local
#include "RichG4SvcLocator.h"


#include "RichG4ReconFlatMirr.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4ReconFlatMirr
//
// 2003-09-11 : Sajan EASO
//-----------------------------------------------------------------------------
//

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4ReconFlatMirr::RichG4ReconFlatMirr(  )
  :    m_RichNumFlatMirrorOrientationParam(4),
       m_RichNumFlatMirrorLocationTypes(4),
       m_RichFlatMirrorNominalOrientation(m_RichNumFlatMirrorLocationTypes,
                                          std::vector<double>(m_RichNumFlatMirrorOrientationParam))
{

  IDataProviderSvc* detSvc = RichG4SvcLocator::RichG4detSvc();
  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream RichG4ReconFlatMirrlog( msgSvc,"RichG4ReconFlatMirr");
  //  RichG4ReconFlatMirrlog << MSG::INFO <<
  //  "Now creating RichG4ReconFlatMirr "<<endreq;



  // Now for the orientation of the flat mirror.
  // for the array   m_RichFlatMirrorNominalOrientation
  // the first element 0->3 correspond to the flat mirrors
  // at Rich1Top, Rich1Bottom, Rich2Left, Rich2Right respectively.
  // The second element 0->3 is the three direction cosines
  // of the plane of a flatmirror and the distance of the
  // flat mirror plane from the origin so that one can
  // write the equation of the plane as ax+by+cz+d=0.



  SmartDataPtr<DetectorElement> Rich1DE(detSvc, "/dd/Structure/LHCb/Rich1");
  SmartDataPtr<DetectorElement> Rich2DE(detSvc, "/dd/Structure/LHCb/Rich2");

  if(Rich1DE) {
    setRich1FlatMirrorParam();

  }
  if(Rich2DE){

    setRich2FlatMirrorParam();

  }


}
RichG4ReconFlatMirr::~RichG4ReconFlatMirr(  ) {
}



void RichG4ReconFlatMirr::setRich1FlatMirrorParam( )
{

  IDataProviderSvc* detSvc = RichG4SvcLocator::RichG4detSvc();
  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream RichG4ReconFlatMirrlog( msgSvc,"RichG4ReconFlatMirr");

  // modif in the following to be compatible with recent Detdesc. 
  // SE 16-6-2005.

  SmartDataPtr<DetectorElement> Rich1DE(detSvc, "/dd/Structure/LHCb/Rich1");
  // modif in the following to be compatible with the xml userparameter name.
  // SE 10-5-2005.
  if( Rich1DE) {

    double r1m2A =
      Rich1DE->param<double>("Rich1NominalNorX");
    //      Rich1DE->userParameterAsDouble("Rich1Mirror2NominalNorXCosX");

    double r1m2B =
      Rich1DE->param<double>("Rich1NominalNorY");
    //      Rich1DE->userParameterAsDouble("Rich1Mirror2NominalNorYCosY");

    double r1m2C =
      Rich1DE->param<double>("Rich1NominalNorZ");
    //      Rich1DE->userParameterAsDouble("Rich1Mirror2NominalNorZCosZ");

    double r1m2D =
      Rich1DE->param<double>("Rich1DParam");
    //      Rich1DE->userParameterAsDouble("Rich1Mirror2DParamDistance");


    //    double r1m2A =
    //  Rich1DE->userParameterAsDouble("Rich1NominalNorX");
    //      Rich1DE->userParameterAsDouble("Rich1Mirror2NominalNorXCosX");

    //    double r1m2B =
    //  Rich1DE->userParameterAsDouble("Rich1NominalNorY");
    //      Rich1DE->userParameterAsDouble("Rich1Mirror2NominalNorYCosY");

    //  double r1m2C =
    //  Rich1DE->userParameterAsDouble("Rich1NominalNorZ");
    //      Rich1DE->userParameterAsDouble("Rich1Mirror2NominalNorZCosZ");

    //  double r1m2D =
    //  Rich1DE->userParameterAsDouble("Rich1DParam");
    //      Rich1DE->userParameterAsDouble("Rich1Mirror2DParamDistance");


    m_RichFlatMirrorNominalOrientation[0][0]= r1m2A;
    m_RichFlatMirrorNominalOrientation[0][1]= r1m2B;
    m_RichFlatMirrorNominalOrientation[0][2]= r1m2C;
    m_RichFlatMirrorNominalOrientation[0][3]= r1m2D;

    m_RichFlatMirrorNominalOrientation[1][0]= r1m2A;
    m_RichFlatMirrorNominalOrientation[1][1]= -1.0*r1m2B;
    m_RichFlatMirrorNominalOrientation[1][2]= r1m2C;
    m_RichFlatMirrorNominalOrientation[1][3]= r1m2D;


    //      RichG4ReconFlatMirrlog << MSG::INFO
    //           << "Flat Mirr param in rich1  "
    //                         << r1m2A<<"  "<<r1m2B
    //                         <<"   "<<r1m2C
    //                         <<"   "<<r1m2D<< endreq;

  }




}

void RichG4ReconFlatMirr::setRich2FlatMirrorParam( )
{
  IDataProviderSvc* detSvc = RichG4SvcLocator::RichG4detSvc();
  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream RichG4ReconFlatMirrlog( msgSvc,"RichG4ReconFlatMirr");


  SmartDataPtr<DetectorElement> Rich2DE(detSvc, "/dd/Structure/LHCb/Rich2");

  if(Rich2DE) {

    double r2m2A =
      Rich2DE->param<double>("Rich2NominalNorX");
    //        Rich2DE->userParameterAsDouble("Rich2NominalNormalCosX");

    double r2m2B =
      Rich2DE->param<double>("Rich2NominalNorY");
    //      Rich2DE->userParameterAsDouble("Rich2NominalNormalCosY");

    double r2m2C =
      Rich2DE->param<double>("Rich2NominalNorZ");
    //      Rich2DE->userParameterAsDouble("Rich2NominalNormalCosZ");

    double r2m2D =
      Rich2DE->param<double>("Rich2DParam");
    //      Rich2DE->userParameterAsDouble("Rich2DParameter");


    // double r2m2A =
    //  Rich2DE->userParameterAsDouble("Rich2NominalNorX");
    //        Rich2DE->userParameterAsDouble("Rich2NominalNormalCosX");

    // double r2m2B =
    //  Rich2DE->userParameterAsDouble("Rich2NominalNorY");
    //      Rich2DE->userParameterAsDouble("Rich2NominalNormalCosY");

    // double r2m2C =
    //  Rich2DE->userParameterAsDouble("Rich2NominalNorZ");
    //      Rich2DE->userParameterAsDouble("Rich2NominalNormalCosZ");

    // double r2m2D =
    //  Rich2DE->userParameterAsDouble("Rich2DParam");
    //      Rich2DE->userParameterAsDouble("Rich2DParameter");

    m_RichFlatMirrorNominalOrientation[2][0]= r2m2A;
    m_RichFlatMirrorNominalOrientation[2][1]= r2m2B;
    m_RichFlatMirrorNominalOrientation[2][2]= r2m2C;
    m_RichFlatMirrorNominalOrientation[2][3]= r2m2D;

    m_RichFlatMirrorNominalOrientation[3][0]= -1.0*r2m2A;
    m_RichFlatMirrorNominalOrientation[3][1]= r2m2B;
    m_RichFlatMirrorNominalOrientation[3][2]= r2m2C;
    m_RichFlatMirrorNominalOrientation[3][3]= r2m2D;

    RichG4ReconFlatMirrlog << MSG::INFO
                           << "Flat Mirr param in rich2  "
                           << r2m2A<<"  "<<r2m2B
                           <<"   "<<r2m2C
                           <<"   "<<r2m2D<< endreq;

  }


}

HepPoint3D  RichG4ReconFlatMirr::FlatMirrorReflect(const HepPoint3D & HitCoordQw ,
                                                   int FlatMirrorType)
{

  const HepTransform3D aFlatMirrRefl =
    HepReflect3D(m_RichFlatMirrorNominalOrientation[FlatMirrorType][0],
                 m_RichFlatMirrorNominalOrientation[FlatMirrorType][1],
                 m_RichFlatMirrorNominalOrientation[FlatMirrorType][2],
                 m_RichFlatMirrorNominalOrientation[FlatMirrorType][3]);

  return  aFlatMirrRefl*HitCoordQw ;
}


//=============================================================================
