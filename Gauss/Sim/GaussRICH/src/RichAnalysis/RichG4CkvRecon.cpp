// $Id: RichG4CkvRecon.cpp,v 1.7 2006-02-13 16:30:43 jonrob Exp $
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
#include "Kernel/Plane3DTypes.h"
#include "Math/VectorUtil.h"

// RichDet
#include "RichDet/DeRich.h"

// local
#include "RichG4SvcLocator.h"
#include "RichG4CkvRecon.h"
#include "RichG4ReconHpd.h"
#include "RichG4ReconTransformHpd.h"
#include "RichG4ReconFlatMirr.h"
#include "RichG4AnalysisConstGauss.h"
#include "RichG4GaussPathNames.h"
#include "RichSolveQuarticEqn.h"
#include <math.h>

// modification made on 30-8-2004 to make windows compatible.

//extern "C" {
//#ifdef WIN32
//  void __stdcall DRTEQ4(double*,double*,double*,double*,double*,double*,int*);
//#else
//  void drteq4_(double*,double*,double*,double*,double*,double*,int*);
//#define DRTEQ4 drteq4_
//#endif
//}
// replaced by GSL compatible  SE 10-5-2005.

//extern "C" void drteq4_(double*,double*,double*,double*,double*,double*,int*);
//-----------------------------------------------------------------------------
// Implementation file for class : RichG4CkvRecon
//
// 2003-09-08 : Sajan Easo
//-----------------------------------------------------------------------------
//
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4CkvRecon::RichG4CkvRecon()
  :m_NumRichDet(2),m_NumHpdRich(std::vector<int> (2)),
   m_HpdTransforms(2,std::vector<RichG4ReconTransformHpd*>(300)),
   m_SphMirrCC(4,std::vector<double>(3)),
   m_SphMirrRad( std::vector<double>(2)),
   m_RichG4ReconHpd(0),
   m_curLocalHitCoord(-10000.,-10000.,-10000.0), m_curEmisPt(0,0,0),
   m_curTkMom(0,0,0),m_curGlobalHitPhCath(0,0,0),
   m_curDetPoint(0,0,0), m_curReflPt(0,0,0) {

  // In the above initialization the 2 and 300 are just
  // dummy parameters, the
  // 2 is the number of richdet and 300 is the number of hpds
  // in each richdet. For the m_SphMirrCCRad , the  4 is for
  // the number of rich sectors
  // in a richdet. ie. top/bottom in rich1 and left/right in rich2
  // correspond to the 0,1,2,3 respectively. For  m_SphMirrCCRad,
  // the 2 is for the
  // the x,y, z coord of the center of curvature of each mirror.




  IDataProviderSvc* detSvc = RichG4SvcLocator::RichG4detSvc();
  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream RichG4CkvReconlog( msgSvc,"RichG4CkvRecon");
  //     RichG4CkvReconlog << MSG::VERBOSE
  //             << "Now creating RichG4CkvRecon "
  //             << endreq;


  SmartDataPtr<DetectorElement> Rich1DE(detSvc, DeRichLocation::Rich1);
  SmartDataPtr<DetectorElement> Rich2DE(detSvc, DeRichLocation::Rich2);

  m_agelnominalrefractiveindex = 1.0339124;
  m_c4f10nominalrefrativeindex = 1.0014069;


  if( !Rich1DE ){
    RichG4CkvReconlog << MSG::ERROR
                      << "Can't retrieve " << DeRichLocation::Rich1 << " for CkvRecon"
                      << endreq;
  }else {



    m_NumRichDet =
      Rich1DE->param<int>("RichNumberOfDetectors");


    m_NumHpdRich[0] =
      Rich1DE->param<int>("Rich1TotNumHpd");

    m_NumHpdRich[1] =
      Rich1DE->param<int>("Rich2TotNumHpd");

    //    m_NumRichDet =
    //  Rich1DE->userParameterAsInt("RichNumberOfDetectors");


    //   m_NumHpdRich[0] =
    //  Rich1DE->userParameterAsInt("Rich1TotNumHpd");

    //   m_NumHpdRich[1] =
    //  Rich1DE->userParameterAsInt("Rich2TotNumHpd");

    m_HpdTransforms [0].resize( m_NumHpdRich[0]);
    m_HpdTransforms [1].resize( m_NumHpdRich[1]);

    m_SphMirrCC [0] [0]=
      Rich1DE->param<double>( "Rich1Mirror1NominalCCX");
    m_SphMirrCC [0] [1]=
      Rich1DE->param<double>( "Rich1Mirror1NominalCCY");
    m_SphMirrCC [0] [2]=
      Rich1DE->param<double>( "Rich1Mirror1NominalCCZ");

    m_SphMirrRad [0] =
      Rich1DE->param<double>( "Rich1Mirror1NominalRadiusC");


    //    m_SphMirrCC [0] [0]=
    //   Rich1DE->userParameterAsDouble( "Rich1Mirror1NominalCCX");
    // m_SphMirrCC [0] [1]=
    //  Rich1DE->userParameterAsDouble( "Rich1Mirror1NominalCCY");
    // m_SphMirrCC [0] [2]=
    //  Rich1DE->userParameterAsDouble( "Rich1Mirror1NominalCCZ");

    //    m_SphMirrRad [0] =
    //  Rich1DE->userParameterAsDouble( "Rich1Mirror1NominalRadiusC");

    m_SphMirrCC [1] [0]= m_SphMirrCC [0] [0];
    m_SphMirrCC [1] [1]= -1.0* m_SphMirrCC [0] [1];
    m_SphMirrCC [1] [2]=   m_SphMirrCC [0] [2];

    m_HpdSiDetThickness =   Rich1DE->
      param<double>("RichHpdSiliconDetectorZSize");

    m_HpdSiPixelXSize =  Rich1DE->
      param<double>( "RichHpdPixelXsize");
    m_HpdSiPixelYSize = Rich1DE->
      param<double>("RichHpdPixelYsize");

    m_HpdSiNumPixelX =  Rich1DE->
      param<int>("RichHpdNumPixelCol");
    m_HpdSiNumPixelY =  Rich1DE->
      param<int>("RichHpdNumPixelRow");


    //    m_HpdSiDetThickness =   Rich1DE->
    //  userParameterAsDouble("RichHpdSiliconDetectorZSize");

    //    m_HpdSiPixelXSize =  Rich1DE->
    //   userParameterAsDouble( "RichHpdPixelXsize");
    //  m_HpdSiPixelYSize = Rich1DE->
    //  userParameterAsDouble("RichHpdPixelYsize");

    // m_HpdSiNumPixelX =  Rich1DE->
    //  userParameterAsInt("RichHpdNumPixelCol");
    //  m_HpdSiNumPixelY =  Rich1DE->
    //  userParameterAsInt("RichHpdNumPixelRow");

  }


  if( !Rich2DE ){
    RichG4CkvReconlog << MSG::ERROR
                      << "Can't retrieve " << DeRichLocation::Rich2 << " for CkvRecon"
                      << endreq;
  } else {

    m_SphMirrCC [2] [0]=
      Rich2DE->param<double>( "Rich2NominalCoCX" );
    m_SphMirrCC [2] [1]=
      Rich2DE->param<double>( "Rich2NominalCoCY");
    m_SphMirrCC [2] [2]=
      Rich2DE->param<double>( "Rich2NominalCoCZ");
    m_SphMirrRad [1]=
      Rich2DE->param<double>( "Rich2SphMirrorRadius");


    //    m_SphMirrCC [2] [0]=
    //  Rich2DE->userParameterAsDouble( "Rich2NominalCoCX" );
    //  m_SphMirrCC [2] [1]=
    //  Rich2DE->userParameterAsDouble( "Rich2NominalCoCY");
    //  m_SphMirrCC [2] [2]=
    //  Rich2DE->userParameterAsDouble( "Rich2NominalCoCZ");
    //  m_SphMirrRad [1]=
    //  Rich2DE->userParameterAsDouble( "Rich2SphMirrorRadius");

    m_SphMirrCC [3] [0]= -1.0* m_SphMirrCC [2] [0];
    m_SphMirrCC [3] [1]= m_SphMirrCC [2] [1];
    m_SphMirrCC [3] [2]= m_SphMirrCC [2] [2];


  }

  RichG4CkvReconlog << MSG::INFO<<
    "Si pixel x size ysize zsize num pix X Y "
                    <<  m_HpdSiPixelXSize <<"  "
                    <<  m_HpdSiPixelYSize<<"  "
                    << m_HpdSiDetThickness<<"  "
                    <<  m_HpdSiNumPixelX<<"   "
                    <<  m_HpdSiNumPixelY<< endreq;


  if( Rich1DE && Rich2DE ) {

    for (int idet=0; idet<m_NumRichDet; idet++){

      //    RichG4CkvReconlog << MSG::INFO
      //           << "Now looping through the rich det for transforms"
      //           << endreq;

      for (int ih=0; ih<m_NumHpdRich[idet] ; ih++ ) {
        //  RichG4CkvReconlog << MSG::INFO
        //         << " Now looping through hpds for transforms  "
        //                << idet <<"  "<<ih << endreq;


        m_HpdTransforms[idet][ih] = new RichG4ReconTransformHpd (idet, ih);

      }

    }




  }


  m_RichG4ReconHpd = new RichG4ReconHpd();

  //  RichG4CkvReconlog << MSG::INFO
  //             << "Num richdet numHpdIn r1 r2  "<<
  //      m_NumRichDet<<"    "<< m_NumHpdRich[0]
  //                      <<"   "<< m_NumHpdRich[1]
  //             << endreq;

}


RichG4CkvRecon::~RichG4CkvRecon(  ) { }



Gaudi::XYZPoint RichG4CkvRecon::GetSiHitCoordFromPixelNum(int aPXNum,
                                                          int aPYNum )
{

  const double zhitc=  m_HpdSiDetThickness/2.0;

  const double xhit= (aPXNum - (m_HpdSiNumPixelX-1)*0.5  ) * m_HpdSiPixelXSize;
  const double yhit= (aPYNum - (m_HpdSiNumPixelY-1)*0.5  ) * m_HpdSiPixelYSize;

  return  Gaudi::XYZPoint(xhit,yhit,zhitc);
}

Gaudi::XYZPoint
RichG4CkvRecon::ReconPhCoordFromLocalCoord (const Gaudi::XYZPoint & aLocalHitCoord )
{

  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream RichG4CkvReconlog( msgSvc,"RichG4CkvRecon");
  //  RichG4CkvReconlog << MSG::INFO
  //                  <<" Now in ReconPhCoordFromLocalCoord "
  //                  <<endreq;


  Gaudi::XYZPoint acurGlobalHitPhCath (0.0,0.0,0.0);

  m_curLocalHitCoord = aLocalHitCoord ;

  //  RichG4CkvReconlog << MSG::INFO
  //                  <<" Local Hit coord on SiDet  "
  //                  <<aLocalHitCoord.x()
  //              <<"   "<<aLocalHitCoord.y()
  //                  <<"   "<<aLocalHitCoord.z()
  //                  <<endreq;


  if(m_curLocalHitCoord.x() == -10000.0 ||
     m_curLocalHitCoord.y() == -10000.0 ||
     m_curLocalHitCoord.z() == -10000.0 ) {

    RichG4CkvReconlog << MSG::ERROR
                      <<" Hpd local Hit coord not set "
                      <<endreq;
  }else {



    // first convert from hit on the Sidet to Hit on
    // Photocathode.
    RichG4ReconHpd* aRichG4ReconHpd = m_RichG4ReconHpd;

    Gaudi::XYZPoint curLocalHitPhCath =
      aRichG4ReconHpd->ReconHitOnPhCathFromLocalHitCoord(m_curLocalHitCoord);


    //   RichG4CkvReconlog << MSG::INFO
    //                  <<" Local Hit coord on PhCathode after mag "
    //                  <<curLocalHitPhCath.x()
    //              <<"   "<<curLocalHitPhCath.y()
    //                 <<"   "<<curLocalHitPhCath.z()
    //                  <<endreq;


    // now convert to the global coord system.
    if( m_CurrentRichDetNum >=0 &&  m_CurrentHpdNum >=0 ) {

      RichG4ReconTransformHpd* CurHpdTransform =
        m_HpdTransforms[m_CurrentRichDetNum] [m_CurrentHpdNum];
      if(CurHpdTransform) {

        Gaudi::Transform3D HpdtoGlobalTransform = CurHpdTransform->HpdLocalToGlobal();

        acurGlobalHitPhCath =
          HpdtoGlobalTransform * curLocalHitPhCath;

        m_curGlobalHitPhCath = acurGlobalHitPhCath;

        //  RichG4CkvReconlog << MSG::VERBOSE
        //                <<" Global Hit coord on PhCathode  "
        //                <<acurGlobalHitPhCath.x()
        //            <<"   "<<acurGlobalHitPhCath.y()
        //               <<"   "<<acurGlobalHitPhCath.z()
        //                <<endreq;

      }



    }

  }



  return   acurGlobalHitPhCath;

}

Gaudi::XYZPoint RichG4CkvRecon::getPhotAgelExitZ( double ex, double ey, double ez,
                                                  RichG4Hit* bHit )
{
  const Gaudi::XYZPoint aPhotTrueEmitPt(ex,ey,ez);

  Hep3Vector TrackStepDir(
                          (m_ChTrackPostStepPosition.x()-m_ChTrackPreStepPosition.x()),
                          (m_ChTrackPostStepPosition.y()-m_ChTrackPreStepPosition.y()),
                          (m_ChTrackPostStepPosition.z()-m_ChTrackPreStepPosition.z()));
  double TrueCkvThetaAtProd=  bHit-> ThetaCkvAtProd();
  double TrueCkvPhiAtProd = bHit-> PhiCkvAtProd();
  double sint = sin(TrueCkvThetaAtProd);
  double sinp = sin(TrueCkvPhiAtProd);
  double cost= cos(TrueCkvThetaAtProd);
  double cosp = cos(TrueCkvPhiAtProd);

  double px= sint*cosp;
  double py= sint*sinp;
  double pz= cost;

  Hep3Vector pDir( px,py,pz);
  pDir.rotateUz( TrackStepDir);
  pDir.unit();

  // aerogel exit plane equation z-1160 =0

  Gaudi::Plane3D AgelExitPlane(0,0,1,-AgelZEndAnalysis);
  //  double distEmisToAgelExit= abs( AgelExitPlane.distance(aPhotTrueEmitPt)) ;
  double distEmisToAgelExit= fabs( AgelExitPlane.Distance(aPhotTrueEmitPt)) ;

  pDir *= distEmisToAgelExit;

  const Gaudi::XYZVector PhotonDir ( pDir.x(), pDir.y(),pDir.z());
  return aPhotTrueEmitPt + PhotonDir;
}

Gaudi::XYZPoint
RichG4CkvRecon::ReconReflectionPointOnSPhMirror (const Gaudi::XYZPoint & aDetectionPoint,
                                                 const Gaudi::XYZPoint & aEmissionPoint,
                                                 const Gaudi::XYZPoint & aQwPoint, G4int aRichDetNum, G4int aFlatMirrNum )
{

  m_curEmisPt=aEmissionPoint;
  m_curDetPoint=aDetectionPoint;

  Gaudi::XYZPoint curFlatMCoC =  m_CurReconFlatMirr-> FlatMirrorCoC(aRichDetNum,aFlatMirrNum);

  Gaudi::XYZPoint aSphReflPt = ReconReflectionPointOnSPhMirrorStdInput() ;
  // for now 5 iterations

  for (int aItr=0; aItr<5 ; ++aItr) {
    Gaudi::XYZPoint aFlatMirrReflPt =  m_CurReconFlatMirr->FlatMirrorIntersection(aSphReflPt,
                                                                                  m_curDetPoint ,
                                                                                  aRichDetNum,
                                                                                  aFlatMirrNum);

    // create a plane at the flat mirr refl point

    const Gaudi::Plane3D aPlane(Gaudi::XYZVector( curFlatMCoC - aFlatMirrReflPt ).unit(), aFlatMirrReflPt);
    // find the detection pt wrt this plane


    double adist = aPlane.Distance(aQwPoint);

    Gaudi::XYZPoint  afrelPt = aQwPoint - 2.0*adist * aPlane.Normal();
    m_curDetPoint =  afrelPt;
    aSphReflPt = ReconReflectionPointOnSPhMirrorStdInput() ;

    ++aItr;


  }

  return  aSphReflPt ;

}


Gaudi::XYZPoint RichG4CkvRecon::ReconReflectionPointOnSPhMirrorStdInput()
{
  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream RichG4CkvReconlog( msgSvc,"RichG4CkvRecon");

  Gaudi::XYZPoint ReflPt = Gaudi::XYZPoint (0.0,0.0,0.0);
  const Gaudi::XYZPoint & aEmisPt = m_curEmisPt;
  const Gaudi::XYZPoint aMirrCC (m_SphMirrCC[m_CurrentRichSector][0],
                                 m_SphMirrCC[m_CurrentRichSector][1],
                                 m_SphMirrCC[m_CurrentRichSector][2]) ;
  const Gaudi::XYZPoint & aDetPt = m_curDetPoint ;

  Gaudi::XYZVector evec =  aEmisPt -  aMirrCC;
  const double e2 = evec.Mag2();
  double e  = pow(e2,0.5);

  Gaudi::XYZVector dvec = aDetPt  - aMirrCC ;
  const  double d2 = dvec.Mag2();
  double d  = pow(d2,0.5);

  double gamma     = acos( evec.Dot(dvec) / (e*d) );

  double r  =  m_SphMirrRad[m_CurrentRichDetNum];
  if( r != 0.0 ) {

    double r2 = r*r;
    double dx = d * cos(gamma);
    double dy = d * sin(gamma);

    double denom =  4.0 * e2 * d2;

    if( denom != 0.0 ) {

      double a[4]=
        {  - 4 * e2 * dy * r ,
           dy * dy * r2 + (e+dx) * (e+dx) * r2 - denom,
           2 * e * dy * (e-dx) * r,
           ( e2 - r2 ) * dy * dy
        };
      gsl_complex qsol[4];

      //     std::vector<std::complex<double> >
      //   qsol(4, std::complex<double> (0.0,100000.0));
      SolveQuartic (qsol, denom, a);
      int nrealsolnum=-1;
      Gaudi::XYZVector nvec = evec.Cross(dvec); // normal vector to reflection plane
      Gaudi::XYZVector delta[2] = { Gaudi::XYZVector(0.0,0.0,0.0),
                                    Gaudi::XYZVector(0.0,0.0,0.0) };


      for (int isol=0 ; isol< 4; isol++ ) {
        // now require real and physical solutions.
        if(GSL_IMAG (qsol[isol] ) == 0.0 && GSL_REAL(qsol[isol]) <= 1.0  ) {
          nrealsolnum++;
          if(nrealsolnum < 2) {
            double beta = asin(GSL_REAL(qsol[isol]));
            Gaudi::XYZVector aa = evec;
            aa *= (r/e);
            const Gaudi::Rotation3D rotn( Gaudi::AxisAngle(nvec, beta));
            Gaudi::XYZVector bb = rotn(aa);
            delta[ nrealsolnum] = bb;
          }


        }

      }

      if( nrealsolnum >= 0 ) {

        const Gaudi::XYZVector deltaF =
          ( (nrealsolnum == 0) ||
            (delta[0].z() > delta[1].z()))? delta[0]:delta[1];

        ReflPt = aMirrCC  + deltaF;

      }

      // now verify that the reflection pt is in the
      // same sector (half) as the detection pt.
      double proda =0.0;

      if(   m_CurrentRichDetNum == 0 ) {
        proda = ReflPt.y() * aDetPt.y();

      } else if (  m_CurrentRichDetNum == 1 ) {
        proda = ReflPt.x() * aDetPt.x();


      }

      if( proda == 0.0 ) {
        ReflPt = Gaudi::XYZPoint (0.0,0.0,0.0);

      }

    }

  }
  return ReflPt;

}




void RichG4CkvRecon::SolveQuartic(  gsl_complex z[4],
                                    double denom,double a[4])
{
  double b[4] =    {0.0,0.0,0.0,0.0  };
  //  double c[8] =  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  //  int ierr=0;
  // double resolv=0.0;
  int asol=0;


  if( denom != 0.0 ) {

    for (int i=0; i<4 ; i++ ) {
      b[i]= a[i]/denom; }

    //    drteq4_(&b[0],&b[1],&b[2],&b[3],c,&resolv,&ierr);

    asol = gsl_poly_complex_solve_quartic_eqn( b[0], b[1], b[2], b[3],
                                               &z[0], &z[1], &z[2], &z[3]);

    //    int j=0;
    //  for(int ii=0; ii< 4 ; ++ii) {
    //  z[ii]= std::complex<double> (c[j],c[j+1]);
    //  j +=2;
    // }

  }


}



double  RichG4CkvRecon::CherenkovThetaFromReflPt(const Gaudi::XYZPoint & aReflPoint ,
                                                 const Gaudi::XYZPoint & aEmisPt )
{
  m_curEmisPt = aEmisPt;

  Gaudi::XYZVector PhotDir= (aReflPoint - aEmisPt ).unit();
  //  const Gaudi::XYZVector TrackDir = m_curTkMom.unit();
  double pmag = pow( PhotDir.Mag2(), 0.5);
  const Gaudi::XYZVector TrackDir=
    m_ChTrackPostStepPosition-m_ChTrackPreStepPosition;

  double tmag = pow(TrackDir.Mag2(), 0.5);
  // double CkvAngle = Root::Math::VectorUtil::Angle(PhotDir,TrackDir);
  double aAngle = acos(PhotDir.Dot(TrackDir))/( pmag* tmag);
  return  aAngle;

}

double RichG4CkvRecon::CherenkovThetaInAerogel(const Gaudi::XYZPoint & aReflPoint,
                                               const Gaudi::XYZPoint & aEmisPt  )
{

  m_curEmisPt = aEmisPt;

  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream RichG4CkvReconlog( msgSvc,"RichG4CkvRecon");


  Gaudi::XYZVector PhotDir= (aReflPoint - aEmisPt  ).unit();
  double pmag = pow( PhotDir.Mag2(), 0.5);

  //     RichG4CkvReconlog<<MSG::INFO
  //                    <<" Agel Ckv Phot dir outside "
  //                    << PhotDir.x()<<"  "
  //                    << PhotDir.y()<<"  "
  //                    << PhotDir.z()<<endreq;



  const Gaudi::XYZVector TrackDir =
    m_ChTrackPostStepPosition-m_ChTrackPreStepPosition;
  double tmag = pow(TrackDir.Mag2(), 0.5);

  const double aCkvWithoutCorrection = acos(PhotDir.Dot(TrackDir))/( pmag* tmag);


  // for now assume that the plane where photons exit from
  // aerogel is normal to the zaxis.

  Gaudi::XYZVector  aZaxis(0.0,0.0,1.0);
  // determine the angle the photon direction makes with the
  // plane normal to the aerogel exit plane.


  const double aPhotDirAgelExit =  acos(PhotDir.Dot(aZaxis))/pmag;



  // get the refindex of the aergel and c4f10 for the
  // current energy of the photon. Use the nominal ref index for now.

  const double sinIncident =
    (m_c4f10nominalrefrativeindex/ m_agelnominalrefractiveindex)*
    (sin( aPhotDirAgelExit)) ;

  const double angleIncident = asin( sinIncident);

  const double angleCorrection= aPhotDirAgelExit-angleIncident;

  const double aCkvWithCorrection= aCkvWithoutCorrection-angleCorrection;


  //     RichG4CkvReconlog<<MSG::INFO
  //                 <<"  Agel Ckv noCorr withcorr  "
  //                 <<  aCkvWithoutCorrection<<"   "
  //                 << aCkvWithCorrection<<"      "
  //                 << " Agel Ckv Angle exit Incident "
  //                 <<  aPhotDirAgelExit<<"   "<< angleIncident<<endreq;


  return aCkvWithCorrection;

}

void  RichG4CkvRecon::SetChTrackPreStepPosition( double xprepos,
                                                 double yprepos,double zprepos)
{
  m_ChTrackPreStepPosition=
    Gaudi::XYZPoint( xprepos,yprepos,zprepos);

}

void RichG4CkvRecon::SetChTrackPostStepPosition(double xpostpos,
                                                double ypostpos,
                                                double zpostpos)
{
  m_ChTrackPostStepPosition=
    Gaudi::XYZPoint( xpostpos, ypostpos,zpostpos);

}

//=============================================================================
