// $Id: RichG4CkvRecon.cpp,v 1.1 2004-02-04 13:52:59 seaso Exp $
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

// local
#include "RichG4SvcLocator.h"


// local
#include "RichG4CkvRecon.h"
#include "RichG4ReconHpd.h"
#include "RichG4ReconTransformHpd.h"
#include "RichG4ReconFlatMirr.h"
#include "RichG4AnalysisConstGauss.h"

extern "C" void drteq4_(double*,double*,double*,double*,double*,double*,int*);
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
  

    SmartDataPtr<IDetectorElement> Rich1DE(detSvc, "/dd/Structure/LHCb/Rich1");
    SmartDataPtr<IDetectorElement> Rich2DE(detSvc, "/dd/Structure/LHCb/Rich2");

   m_agelnominalrefractiveindex = 1.0339124;
   m_c4f10nominalrefrativeindex = 1.0014069;
   
 
  if( !Rich1DE ){
    RichG4CkvReconlog << MSG::ERROR 
               << "Can't retrieve /dd/Structure/LHCb/Rich1 for CkvRecon" 
               << endreq;
  }else {
  

    
    m_NumRichDet =  
       Rich1DE->userParameterAsInt("RichNumberOfDetectors");
    
    
    m_NumHpdRich[0] = 
      Rich1DE->userParameterAsInt("Rich1TotNumHpd");

    m_NumHpdRich[1] = 
      Rich1DE->userParameterAsInt("Rich2TotNumHpd");
    
    m_HpdTransforms [0].resize( m_NumHpdRich[0]);
    m_HpdTransforms [1].resize( m_NumHpdRich[1]);

    m_SphMirrCC [0] [0]=
     Rich1DE->userParameterAsDouble( "Rich1Mirror1NominalCCX");    
     m_SphMirrCC [0] [1]=     
     Rich1DE->userParameterAsDouble( "Rich1Mirror1NominalCCY");    
     m_SphMirrCC [0] [2]=     
     Rich1DE->userParameterAsDouble( "Rich1Mirror1NominalCCZ");    

     m_SphMirrRad [0] =     
     Rich1DE->userParameterAsDouble( "Rich1Mirror1NominalRadiusC");    

     m_SphMirrCC [1] [0]= m_SphMirrCC [0] [0];     
     m_SphMirrCC [1] [1]= -1.0* m_SphMirrCC [0] [1];
     m_SphMirrCC [1] [2]=   m_SphMirrCC [0] [2];

     m_HpdSiDetThickness =   Rich1DE->
           userParameterAsDouble("RichHpdSiliconDetectorZSize");

     m_HpdSiPixelXSize =  Rich1DE->
       userParameterAsDouble( "RichHpdPixelXsize");
     m_HpdSiPixelYSize = Rich1DE->
        userParameterAsDouble("RichHpdPixelYsize");
      
      m_HpdSiNumPixelX =  Rich1DE->
        userParameterAsInt("RichHpdNumPixelCol");
      m_HpdSiNumPixelY =  Rich1DE->
        userParameterAsInt("RichHpdNumPixelRow");
      
  }

  
  if( !Rich2DE ){
    RichG4CkvReconlog << MSG::ERROR 
               << "Can't retrieve /dd/Structure/LHCb/Rich2 for CkvRecon" 
               << endreq;
  } else {

    m_SphMirrCC [2] [0]=
     Rich2DE->userParameterAsDouble( "Rich2NominalCoCX" );    
     m_SphMirrCC [2] [1]=     
     Rich2DE->userParameterAsDouble( "Rich2NominalCoCY");    
     m_SphMirrCC [2] [2]=     
     Rich2DE->userParameterAsDouble( "Rich2NominalCoCZ");    
     m_SphMirrRad [1]=     
     Rich2DE->userParameterAsDouble( "Rich2SphMirrorRadius");    

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
                                       
                                       
RichG4CkvRecon::~RichG4CkvRecon(  )
{
}


void RichG4CkvRecon::SetCurrentLocalHitCoord( double xhit, 
                                              double yhit, double zhit) 
{
  m_curLocalHitCoord =  HepPoint3D(xhit,yhit,zhit);
  
}
void RichG4CkvRecon::SetCurrentEmissPt( double xEmiss, double yEmiss, 
                                         double zEmiss)
{
  m_curEmisPt= HepPoint3D(xEmiss,yEmiss, zEmiss);
  
}
void  RichG4CkvRecon::SetCurrentTkMom( double xMom, double yMom,
                                       double zMom) 
{
  m_curTkMom=  HepVector3D( xMom, yMom,zMom);
  
}
void  RichG4CkvRecon::SetCurrentRichDetNum(int aRichDetNum) 
{
  m_CurrentRichDetNum=aRichDetNum;
  
}
void  RichG4CkvRecon::SetCurrentHpdNum(int aHpdNum)
{
  m_CurrentHpdNum= aHpdNum;
  
}
int  RichG4CkvRecon::CurrentFlatMirrorType() 
{
  return    m_CurrentFlatMirrorType ;
  
}

void  RichG4CkvRecon::SetCurrentFlatMirrorType(int aType)
{
  m_CurrentFlatMirrorType = aType;
  
}

void RichG4CkvRecon::SetCurrentRichSector( int aSectorNum)
{
  m_CurrentRichSector=  aSectorNum;
  
} 
void RichG4CkvRecon::SetcurDetPoint(  HepPoint3D aDetPoint ) 
{
  m_curDetPoint = aDetPoint;
  
}
void RichG4CkvRecon::SetcurReflPt (  HepPoint3D aReflPt )
{
  m_curReflPt = aReflPt;
  
}

HepPoint3D RichG4CkvRecon::GetSiHitCoordFromPixelNum(int aPXNum,
                                                     int aPYNum ) 
{

  double zhitc=  m_HpdSiDetThickness/2.0;

  double xhit= (aPXNum - (m_HpdSiNumPixelX-1)*0.5  ) * m_HpdSiPixelXSize;
  double yhit= (aPYNum - (m_HpdSiNumPixelY-1)*0.5  ) * m_HpdSiPixelYSize;
  
  return  HepPoint3D(xhit,yhit,zhitc);
  
  
}

HepPoint3D  RichG4CkvRecon::ReconPhCoordFromLocalCoord
        (HepPoint3D aLocalHitCoord )
{

   IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream RichG4CkvReconlog( msgSvc,"RichG4CkvRecon");
  //  RichG4CkvReconlog << MSG::INFO 
  //                  <<" Now in ReconPhCoordFromLocalCoord "
  //                  <<endreq;
  

  HepPoint3D acurGlobalHitPhCath (0.0,0.0,0.0);
  
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
    
  HepPoint3D curLocalHitPhCath =  
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
    
    HepTransform3D HpdtoGlobalTransform = CurHpdTransform->HpdLocalToGlobal();

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

HepPoint3D RichG4CkvRecon::getPhotAgelExitZ( double ex, double ey, double ez, 
                            RichG4Hit* bHit ) 
{
  const HepPoint3D aPhotTrueEmitPt(ex,ey,ez);
   
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

  HepPlane3D AgelExitPlane(0,0,1,-AgelZEndAnalysis);
  double distEmisToAgelExit= abs( AgelExitPlane.distance(aPhotTrueEmitPt)) ;
  
  pDir *= distEmisToAgelExit;
  
  HepVector3D PhotonDir ( pDir.x(), pDir.y(),pDir.z());
  
  HepPoint3D aPhotExitPt = aPhotTrueEmitPt+ PhotonDir;   
  
  return aPhotExitPt;
  
}

HepPoint3D RichG4CkvRecon::ReconReflectionPointOnSPhMirror
(HepPoint3D aDetectionPoint, HepPoint3D aEmissionPoint)
{
  m_curEmisPt=aEmissionPoint;
  m_curDetPoint=aDetectionPoint;
  
  HepPoint3D aReflPoint=ReconReflectionPointOnSPhMirrorStdInput();
  m_curReflPt = aReflPoint;
  return  aReflPoint;
  
} 


HepPoint3D RichG4CkvRecon::ReconReflectionPointOnSPhMirrorStdInput()
{
  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
   MsgStream RichG4CkvReconlog( msgSvc,"RichG4CkvRecon");
    
  HepPoint3D ReflPt = HepPoint3D (0.0,0.0,0.0);
  const HepPoint3D aEmisPt = m_curEmisPt;
  const HepPoint3D aMirrCC (m_SphMirrCC[m_CurrentRichSector][0],
            m_SphMirrCC[m_CurrentRichSector][1], 
             m_SphMirrCC[m_CurrentRichSector][2]) ;
  const HepPoint3D aDetPt = m_curDetPoint ;
  
  HepVector3D evec =  aEmisPt -  aMirrCC;
  HepVector3D dvec = aDetPt  - aMirrCC ;
  double gamma     = evec.angle(dvec);
  double r  =  m_SphMirrRad[m_CurrentRichDetNum];
  if( r != 0.0 ) {
    
  double r2 = r*r;
  double e  = evec.mag() ;
  double d  = dvec.mag();
  double e2 = e*e;
  double d2 = d*d;
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
   std::vector<std::complex<double> > qsol = SolveQuartic (denom, a);
    int nrealsolnum=-1;
     HepVector3D nvec = evec.cross(dvec); // normal vector to reflection plane
     HepVector3D delta[2] = { HepVector3D(0.0,0.0,0.0),
                              HepVector3D(0.0,0.0,0.0) };     
    
    if(qsol.size() <= 4 ) {
      for (int isol=0 ; isol< (int) qsol.size(); isol++ ) {
        // now require real and physical solutions.
        if(std::imag (qsol[isol] ) == 0.0 && std::real(qsol[isol]) <= 1.0  ) {
          nrealsolnum++;
          if(nrealsolnum < 2) {
            double beta = asin(std::real(qsol[isol]));
            delta[ nrealsolnum] = evec;
            delta[ nrealsolnum].setMag(r);
            delta[ nrealsolnum].rotate(beta,nvec);
            
          }
          
          
        }
      } 
       
      if( nrealsolnum >= 0 ) {
        
        const HepVector3D deltaF = 
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
        ReflPt = HepPoint3D (0.0,0.0,0.0);
        
        
      }
      
    }
    
    
    
    
    
  }
  
  
  }
  
  
  return ReflPt;
  
}


std::vector<std::complex<double> > RichG4CkvRecon::SolveQuartic 
   ( double denom,double a[4]) 
{
  
  
  std::vector<std::complex<double> > 
    z(4, std::complex<double> (0.0,100000.0));
  

  double b[4] =    {0.0,0.0,0.0,0.0  };
  double c[8] =  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  int ierr=0;  
  double resolv=0.0;
  

  if( denom != 0.0 ) {
    
    for (int i=0; i<4 ; i++ ) {
      b[i]= a[i]/denom; }
       
    drteq4_(&b[0],&b[1],&b[2],&b[3],c,&resolv,&ierr);

   int j=0;
   for(int ii=0; ii< 4 ; ii++) {
     z[ii]= std::complex<double> (c[j],c[j+1]);     
     j +=2;
     
   }
   

   
  }     
  
  return z;
  
}


double  RichG4CkvRecon::CherenkovThetaFromReflPt(HepPoint3D aReflPoint ,
              HepPoint3D aEmisPt ) 
{
  m_curEmisPt = aEmisPt;
  
  HepPoint3D PhotDir= (aReflPoint - aEmisPt ).unit();
  //  const HepVector3D TrackDir = m_curTkMom.unit();

  const HepVector3D TrackDir= 
    m_ChTrackPostStepPosition-m_ChTrackPreStepPosition;
  

  // double CkvAngle = PhotDir.angle(TrackDir);
  
  return  PhotDir.angle(TrackDir);
  
}

double RichG4CkvRecon::CherenkovThetaInAerogel(HepPoint3D aReflPoint,
       HepPoint3D aEmisPt  )
{

  m_curEmisPt = aEmisPt;
 
  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
   MsgStream RichG4CkvReconlog( msgSvc,"RichG4CkvRecon");


  HepPoint3D PhotDir= (aReflPoint - aEmisPt  ).unit();

  //     RichG4CkvReconlog<<MSG::INFO
  //                    <<" Agel Ckv Phot dir outside "
  //                    << PhotDir.x()<<"  "
  //                    << PhotDir.y()<<"  "
  //                    << PhotDir.z()<<endreq;
     

   
  const HepVector3D TrackDir =
    m_ChTrackPostStepPosition-m_ChTrackPreStepPosition;

  double aCkvWithoutCorrection =  PhotDir.angle(TrackDir);
 
  // for now assume that the plane where photons exit from
  // aerogel is normal to the zaxis.

  HepVector3D  aZaxis(0.0,0.0,1.0);
  // determine the angle the photon direction makes with the
  // plane normal to the aerogel exit plane.

  
  double aPhotDirAgelExit =  PhotDir.angle(aZaxis);
  
  // get the refindex of the aergel and c4f10 for the
  // current energy of the photon. Use the nominal ref index for now.

     double sinIncident =  
       (m_c4f10nominalrefrativeindex/ m_agelnominalrefractiveindex)*
       (sin( aPhotDirAgelExit)) ;
     
     double angleIncident = asin( sinIncident);

     double angleCorrection= aPhotDirAgelExit-angleIncident;
     
     double aCkvWithCorrection= aCkvWithoutCorrection-angleCorrection;


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
               HepPoint3D( xprepos,yprepos,zprepos);
  
}

void RichG4CkvRecon::SetChTrackPostStepPosition(double xpostpos,
                                                double ypostpos,
                                                 double zpostpos)
{
  m_ChTrackPostStepPosition=  
              HepPoint3D( xpostpos, ypostpos,zpostpos);
  
}

//=============================================================================
