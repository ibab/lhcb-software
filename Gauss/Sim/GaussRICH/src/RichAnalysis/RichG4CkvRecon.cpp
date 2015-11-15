// $Id: RichG4CkvRecon.cpp,v 1.15 2008-05-30 13:43:23 gcorti Exp $
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
#include "GaudiKernel/Plane3DTypes.h"
#include "Math/VectorUtil.h"

// local
#include "GaussRICH/RichG4SvcLocator.h"
#include "RichG4CkvRecon.h"
#include "RichG4ReconHpd.h"
#include "RichG4ReconTransformHpd.h"
#include "GaussRICH/RichG4TransformPhDet.h"
#include "GaussRICH/RichG4ReconFlatMirr.h"
#include "GaussRICH/RichG4AnalysisConstGauss.h"
#include "GaussRICH/RichG4GaussPathNames.h"
#include "GaussRICH/RichSolveQuarticEqn.h"
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
   m_PhDetTransforms(std::vector<RichG4TransformPhDet*> (4)),
   m_SphMirrCC(4,std::vector<double>(3)),
   m_SphMirrRad( std::vector<double>(2)),
   m_RichG4ReconHpd(0),
   m_curLocalHitCoord(-10000.,-10000.,-10000.0), m_curEmisPt(0,0,0),
   m_curTkMom(0,0,0),m_curGlobalHitPhCath(0,0,0),
   m_curDetPoint(0,0,0), m_curReflPt(0,0,0),
   m_AgelNominalTileRefIndex(std::vector<double>(16)),
   m_AgelNominalSubTileRefIndex(16,std::vector<double>(50))
  {

  // In the above initialization the 2 and 300 are just
  // dummy parameters, the
  // 2 is the number of richdet and 300 is the number of hpds
  // in each richdet. For the m_SphMirrCCRad , the  4 is for
  // the number of rich sectors
  // in a richdet. ie. top/bottom in rich1 and left/right in rich2
  // correspond to the 0,1,2,3 respectively. For  m_SphMirrCCRad,
  // the 2 is for the
  // the x,y, z coord of the center of curvature of each mirror.
  // 4 is the number of sectors corresponding to rich1 top,bootom
  // rich2 left right.



  IDataProviderSvc* detSvc = RichG4SvcLocator::RichG4detSvc();
  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream RichG4CkvReconlog( msgSvc,"RichG4CkvRecon");
  //     RichG4CkvReconlog << MSG::VERBOSE
  //             << "Now creating RichG4CkvRecon "
  //             << endreq;


  SmartDataPtr<DetectorElement> Rich1DE(detSvc, Rich1DeStructurePathName);
  SmartDataPtr<DetectorElement> Rich2DE(detSvc, Rich2DeStructurePathName);

  m_agelnominalrefractiveindex = 1.0339124;
  m_c4f10nominalrefrativeindex = 1.0014069;
  m_AgelNominalTileRefIndex[0]=1.0375;
  m_AgelNominalTileRefIndex[1]=1.0375;
  m_AgelNominalTileRefIndex[2]=1.0373;
  m_AgelNominalTileRefIndex[3]=1.0359;
  m_AgelNominalTileRefIndex[4]=1.0369;
  m_AgelNominalTileRefIndex[5]=1.0376;
  m_AgelNominalTileRefIndex[6]=1.0358;
  m_AgelNominalTileRefIndex[7]=1.0367;
  m_AgelNominalTileRefIndex[8]=1.0387;
  m_AgelNominalTileRefIndex[9]=1.0377;
  m_AgelNominalTileRefIndex[10]=1.0363;
  m_AgelNominalTileRefIndex[11]=1.0385;
  m_AgelNominalTileRefIndex[12]=1.0347;
  m_AgelNominalTileRefIndex[13]=1.0382;
  m_AgelNominalTileRefIndex[14]=1.0342;
  m_AgelNominalTileRefIndex[15]=1.0376;
  double asum=0.0;

  for (int i=0; i<16; ++i ) {
    asum+= m_AgelNominalTileRefIndex[i];

    for (int j=0; j<50; ++j) {
      m_AgelNominalSubTileRefIndex[i] [j] =  m_AgelNominalTileRefIndex[i];

    }

  }
   m_AgelMeanNominalTileRefIndex=asum/16.0;




  if( !Rich1DE ){
    RichG4CkvReconlog << MSG::ERROR
                      << "Can't retrieve " + Rich1DeStructurePathName+ " for CkvRecon"
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

    //    std::vector<double> r1NominalCoC = Rich1DE->param<std::vector<double> >("Rich1NominalCoC");
    std::vector<double> r1NominalCoC = Rich1DE->param<std::vector<double> >("NominalSphMirrorCoC");


    m_SphMirrCC [0] [0]= r1NominalCoC[0];
    m_SphMirrCC [0] [1]= r1NominalCoC[1];
    m_SphMirrCC [0] [2]= r1NominalCoC[2];
    m_SphMirrRad [0] =
      Rich1DE->param<double>( "Rich1Mirror1NominalRadiusC");

    RichG4CkvReconlog << MSG::DEBUG<< "Rich1 Spherical Mirror1 top  COC and Rad "<< m_SphMirrCC [0] [0]<<"  "<< m_SphMirrCC [0] [1]
		      <<"  "<< m_SphMirrCC [0] [2]<<"  "<<m_SphMirrRad [0]<<endreq;



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

    RichG4CkvReconlog << MSG::DEBUG<< "Rich1 Spherical Mirror1 bottom  COC and Rad "
         << m_SphMirrCC [1] [0]<<"  "<< m_SphMirrCC [1] [1]
		      <<"  "<< m_SphMirrCC [1] [2]<<"  "<<m_SphMirrRad [0]<<endreq;


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
                      << "Can't retrieve "+  Rich2DeStructurePathName +" for CkvRecon"
                      << endreq;
  } else {

    //    std::vector<double> r2NominalCoC = Rich2DE->param<std::vector<double> >("Rich2NominalCoC");
    std::vector<double> r2NominalCoC = Rich2DE->param<std::vector<double> >("NominalSphMirrorCoC");

    m_SphMirrCC [2] [0]= r2NominalCoC[0];
    m_SphMirrCC [2] [1]= r2NominalCoC[1];
    m_SphMirrCC [2] [2]= r2NominalCoC[2];
    //    m_SphMirrRad [1]=
    //  Rich2DE->param<double>( "Rich2SphMirrorRadius");

    m_SphMirrRad [1]=
      Rich2DE->param<double>( "SphMirrorRadius");


    m_SphMirrCC [3] [0]= -1.0* m_SphMirrCC [2] [0];
    m_SphMirrCC [3] [1]= m_SphMirrCC [2] [1];
    m_SphMirrCC [3] [2]= m_SphMirrCC [2] [2];

    RichG4CkvReconlog << MSG::DEBUG<< "Rich2 Spherical Mirror1 left  COC and Rad "<< m_SphMirrCC [2] [0]
          <<"  "<< m_SphMirrCC [2] [1]
		      <<"  "<< m_SphMirrCC [2] [2]<<"  "<<m_SphMirrRad [1]<<endreq;
    RichG4CkvReconlog << MSG::DEBUG<< "Rich2 Spherical Mirror1 right  COC and Rad "<< m_SphMirrCC [3] [0]<<"  "
                      << m_SphMirrCC [3] [1]
		                  <<"  "<< m_SphMirrCC [3] [2]<<"  "<<m_SphMirrRad [1]<<endreq;

  }

  //  RichG4CkvReconlog << MSG::INFO<<
  //  "Si pixel x size ysize zsize num pix X Y "
  //                  <<  m_HpdSiPixelXSize <<"  "
  //                  <<  m_HpdSiPixelYSize<<"  "
  //                  << m_HpdSiDetThickness<<"  "
  //                  <<  m_HpdSiNumPixelX<<"   "
  //                  <<  m_HpdSiNumPixelY<< endreq;
  //

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

      for (int is=0 ;is < 2 ; is ++) {

        int isect = idet*2 + is;
        m_PhDetTransforms[isect] = new RichG4TransformPhDet(idet, isect );
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

Gaudi::XYZPoint RichG4CkvRecon::ApplyQwRefrCorrAndGetGlobalPos(const Gaudi::XYZPoint & aPhCathPoint ){


  Gaudi::XYZPoint acurGlobalHitPhCath (0.0,0.0,0.0);

  Gaudi::XYZPoint aLocalPointWithCorr= m_RichG4ReconHpd->GetLocalPointWithQwCorr(aPhCathPoint);


  RichG4ReconTransformHpd* CurHpdTransform =
        m_HpdTransforms[m_CurrentRichDetNum] [m_CurrentHpdNum];
  if(CurHpdTransform) {
         Gaudi::Transform3D HpdtoGlobalTransform = CurHpdTransform->HpdLocalToGlobal();

          acurGlobalHitPhCath = HpdtoGlobalTransform *  aLocalPointWithCorr ;

  }

  return acurGlobalHitPhCath;

}
Gaudi::XYZPoint
RichG4CkvRecon::ReconPhDetPlaneCoordFromLocalCoord (const Gaudi::XYZPoint & aLocalHitCoord, bool applyQwRefCorr )
{
  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream RichG4CkvReconlog( msgSvc,"RichG4CkvRecon");
  Gaudi::XYZPoint acurPhDetHitPhCath (0.0,0.0,0.0);

  //  RichG4CkvReconlog << MSG::INFO
  //                  <<" Local Hit coord on SiDet  "
  //                  <<aLocalHitCoord.x()
  //              <<"   "<<aLocalHitCoord.y()
  //                  <<"   "<<aLocalHitCoord.z()
  //                  <<endreq;

  if(aLocalHitCoord.x() == -10000.0 ||
     aLocalHitCoord.y() == -10000.0 ||
     aLocalHitCoord.z() == -10000.0 ) {

    RichG4CkvReconlog << MSG::ERROR
                      <<" Hpd local Hit coord not set "
                      <<endreq;
  }else {



    // first convert from hit on the Sidet to Hit on
    // Photocathode.


    Gaudi::XYZPoint curLocalHitPhCath =
      m_RichG4ReconHpd->ReconHitOnPhCathFromLocalHitCoord(aLocalHitCoord, applyQwRefCorr );

    // now convert to the Ph Det Panel  coord system.
    if( m_CurrentRichDetNum >=0 &&  m_CurrentHpdNum >=0 ) {

      RichG4ReconTransformHpd* CurHpdTransform =
        m_HpdTransforms[m_CurrentRichDetNum] [m_CurrentHpdNum];
      if(CurHpdTransform) {

        Gaudi::Transform3D HpdtoPhDetTransform = CurHpdTransform->HpdLocalToPhDet();

        acurPhDetHitPhCath =
          HpdtoPhDetTransform * curLocalHitPhCath;



      }
    }
  }


  return   acurPhDetHitPhCath;

}


Gaudi::XYZPoint
RichG4CkvRecon::ReconPhCoordFromLocalCoord (const Gaudi::XYZPoint & aLocalHitCoord, bool applyQwRefCorr )
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
      aRichG4ReconHpd->ReconHitOnPhCathFromLocalHitCoord(m_curLocalHitCoord, applyQwRefCorr );


    //    RichG4CkvReconlog << MSG::INFO
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

        //   RichG4CkvReconlog << MSG::INFO
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

  CLHEP::Hep3Vector TrackStepDir(
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

  CLHEP::Hep3Vector pDir( px,py,pz);
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

  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream RichG4CkvReconlog( msgSvc,"RichG4CkvRecon");
  //  G4cout<<"  Now in  ReconReflectionPointOnSPhMirror G4cout"<<G4endl;

  //  RichG4CkvReconlog<<MSG::INFO <<" Now in  ReconReflectionPointOnSPhMirror  " << endreq;

  m_curEmisPt=aEmissionPoint;
  m_curDetPoint=aDetectionPoint;

  //  RichG4CkvReconlog<<MSG::INFO <<" current richdet and flat mirror "<<aRichDetNum<<"  "<<aFlatMirrNum
  //                << endreq;


  Gaudi::XYZPoint curFlatMCoC =  m_CurReconFlatMirr-> FlatMirrorCoC(aRichDetNum,aFlatMirrNum);

  //  RichG4CkvReconlog<<MSG::INFO <<" current richdet and flat mirror flatmirrorcoc "<<aRichDetNum<<"  "<<aFlatMirrNum
  //                <<"  "<<curFlatMCoC<< endreq;

  //    RichG4CkvReconlog<<MSG::INFO <<" current emission pt "<< m_curEmisPt<<endreq;
  //  RichG4CkvReconlog<<MSG::INFO <<" current detection pt "<<m_curDetPoint <<endreq;


  Gaudi::XYZPoint aSphReflPt = ReconReflectionPointOnSPhMirrorStdInput() ;
  //  RichG4CkvReconlog<<" current Sph Mirror pt first iteration "<<aSphReflPt   <<endreq;

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

    if( (e*d) != 0.0 ) {

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
  //  int asol=0;


  if( denom != 0.0 ) {

    for (int i=0; i<4 ; i++ ) {
      b[i]= a[i]/denom; }

    //    drteq4_(&b[0],&b[1],&b[2],&b[3],c,&resolv,&ierr);

    //    asol = gsl_poly_complex_solve_quartic_eqn( b[0], b[1], b[2], b[3],
    //				       &z[0], &z[1], &z[2], &z[3]);
     gsl_poly_complex_solve_quartic_eqn( b[0], b[1], b[2], b[3],
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
  // G4cout<<"Phot dir xyz "<<PhotDir.x()<<" "<<PhotDir.y()<<"  "
  //	<<PhotDir.z()<<G4endl;
  //  const Gaudi::XYZVector TrackDir = m_curTkMom.unit();
  double pmag = pow( PhotDir.Mag2(), 0.5);
  const Gaudi::XYZVector TrackDir=
    (m_ChTrackPostStepPosition-m_ChTrackPreStepPosition).unit();
  //  G4cout<<"Track dir xyz "<<TrackDir.x()<<" "<<TrackDir.y()<<"  "
  //	<<TrackDir.z()<<G4endl;

  double tmag = pow(TrackDir.Mag2(), 0.5);
  // double CkvAngle = Root::Math::VectorUtil::Angle(PhotDir,TrackDir);
  //  double aAngle = acos(PhotDir.Dot(TrackDir))/( pmag* tmag);
  //  double aDotProd=  PhotDir.x()*TrackDir.x()+PhotDir.y()*TrackDir.y()+
  //  PhotDir.z()*TrackDir.z();
  double aAngle = acos(PhotDir.Dot(TrackDir)/( pmag* tmag));
  //  G4cout<<"Reconstructed Ckv Angle= "<<aAngle<<G4endl;

  return  aAngle;

}

double RichG4CkvRecon::CherenkovThetaInAerogel(const Gaudi::XYZPoint & aReflPoint,
                                               const Gaudi::XYZPoint & aEmisPt ,
                                               const int tilenum, const int subtilenum )
{
  double aTileRefIndexCorrectionFact=0.0;

  if(subtilenum < 0 ) {
    aTileRefIndexCorrectionFact = acos(1.0/m_AgelMeanNominalTileRefIndex)-
      acos(1.0/(m_AgelNominalTileRefIndex[tilenum]));

  }else {
    aTileRefIndexCorrectionFact=acos(1.0/m_AgelMeanNominalTileRefIndex)-
      acos(1.0/(m_AgelNominalSubTileRefIndex[tilenum] [subtilenum]));

  }


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

  const double aCkvWithoutCorrection = acos(PhotDir.Dot(TrackDir)/( pmag* tmag));


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

  const double aCkvWithCorrection=
    (aCkvWithoutCorrection-angleCorrection) + aTileRefIndexCorrectionFact;

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
