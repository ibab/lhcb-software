// $Id: CkvG4ReconFlatMirr.cpp,v 1.13 2009-07-03 11:59:49 seaso Exp $
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
#include "DetDesc/SolidSphere.h"
#include "DetDesc/ISolid.h"
#include "DetDesc/SolidBoolean.h"

#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Transform3DTypes.h"
#include "GaudiKernel/Plane3DTypes.h"
                                                                                                                    

// local
#include "GaussCherenkov/CkvG4SvcLocator.h"
#include "GaussRICH/RichG4GaussPathNames.h"
#include "GaussCherenkov/CkvG4GaussPathNames.h"
#include "GaussCherenkov/CkvGeometrySetupUtil.h"


#include "GaussCherenkov/CkvG4ReconFlatMirr.h"



//-----------------------------------------------------------------------------
// Implementation file for class : CkvG4ReconFlatMirr
//
// 2003-09-11 : Sajan EASO
//-----------------------------------------------------------------------------
//

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CkvG4ReconFlatMirr::CkvG4ReconFlatMirr(  )
  :    m_RichNumFlatMirrorOrientationParam(4),
       m_RichNumFlatMirrorLocationTypes(4),
       m_RichFlatMirrorNominalOrientation(m_RichNumFlatMirrorLocationTypes,
                                          std::vector<double>(m_RichNumFlatMirrorOrientationParam)),
       m_RichNumSecMirrorCoCParam(4),
       m_Rich1NumSecMirror(16),
       m_Rich2NumSecMirror(40),
       m_RichSecMirrCoCRad( m_Rich1NumSecMirror+m_Rich2NumSecMirror,
                            std::vector<double> (m_RichNumSecMirrorCoCParam)),
       m_Rich1Mirror2CommonPath(Rich1DeStructurePathName),
       m_Rich2Mirror2CommonPath(Rich2DeStructurePathName),
       m_Rich1Mirror2IndPathString(std::vector<std::string> (m_Rich1NumSecMirror)),
       m_Rich2Mirror2IndPathString(std::vector<std::string> (m_Rich2NumSecMirror))
{

  IDataProviderSvc* detSvc = CkvG4SvcLocator::RichG4detSvc();
  IMessageSvc*  msgSvc = CkvG4SvcLocator::RichG4MsgSvc ();
  MsgStream CkvG4ReconFlatMirrlog( msgSvc,"CkvG4ReconFlatMirr");
  //  CkvG4ReconFlatMirrlog << MSG::INFO <<
  //  "Now creating CkvG4ReconFlatMirr "<<endreq;
  // Now for the orientation of the flat mirror.
  // for the array   m_RichFlatMirrorNominalOrientation
  // the first element 0->3 correspond to the flat mirrors
  // at Rich1Top, Rich1Bottom, Rich2Left, Rich2Right respectively.
  // The second element 0->3 is the three direction cosines
  // of the plane of a flatmirror and the distance of the
  // flat mirror plane from the origin so that one can
  // write the equation of the plane as ax+by+cz+d=0.
  // for the m_RichSecMirrCoCRad the first parameter is the mirror number
  // in Rich1+Rich2; The second parameter is the xyz of Coc and radius. 
  // m_RichNumSecMirrorCoCParam = 4 for the xyz+rad for each segment.
  // the mirror number 0-15 for the rich1 and 16-45 for rich2 in this array.  
  CkvGeometrySetupUtil * aCkvGeometrySetupUtil = CkvGeometrySetupUtil::getCkvGeometrySetupUtilInstance();
  m_Rich1HorizFlag  = aCkvGeometrySetupUtil->hasOptimizedHorizontalRich1();
  // bool aSuperRichFlag  = aCkvGeometrySetupUtil-> isSuperRich() ;
  

  SmartDataPtr<DetectorElement> Rich1DE(detSvc, Rich1DeStructurePathName);
  SmartDataPtr<DetectorElement> Rich2DE(detSvc, Rich2DeStructurePathName);

  std::string r1m2MPath []={"/Rich1Mirror2MasterTop/Rich1Mirror2Seg",
                            "/Rich1Mirror2MasterBot/Rich1Mirror2Seg" };
  if(m_Rich1HorizFlag ) {
    r1m2MPath[0] = "/Rich1Mirror2MasterLeft/Rich1Mirror2Seg" ;
    r1m2MPath[1] = "/Rich1Mirror2MasterRight/Rich1Mirror2Seg";    
  }
  

  std::string r2mNum [] ={"00:0","01:1","02:2","03:3","04:4","05:5",
                          "06:6","07:7","08:8","09:9","10:10","11:11",
                          "12:12","13:13","14:14","15:15"};
  for(int ii=0; ii<m_Rich1NumSecMirror; ++ii) {
    if(ii < 8 ) {

      m_Rich1Mirror2IndPathString[ii]= m_Rich1Mirror2CommonPath + r1m2MPath[0] + r2mNum[ii];

    }else {

      m_Rich1Mirror2IndPathString[ii]= m_Rich1Mirror2CommonPath + r1m2MPath[1] + r2mNum[ii];     

    }    
  }


  std::string mnum []= {"0","1","2","3","4","5","6","7","8","9","10",
                        "11","12","13","14","15","16","17","18","19","20",
                        "21","22","23","24","25","26","27","28","29","30",
                        "31","32","33","34","35","36","37","38","39"};
  
  std::string Rich2Mirror2DetElemCont0= "/Rich2Gas/Rich2SecMirrorContainer_0";
  std::string Rich2Mirror2DetElemCont1= "/Rich2Gas/Rich2SecMirrorContainer_1";
  std::string Rich2Mirror2String= "/Rich2SecMirror:" ;
  
        
  for(int ij=0; ij<m_Rich2NumSecMirror;++ij) {
    if(ij< 20 ) {
      
      m_Rich2Mirror2IndPathString[ij]= m_Rich2Mirror2CommonPath + Rich2Mirror2DetElemCont0 + Rich2Mirror2String+mnum[ij];
      
    }else {
      
      m_Rich2Mirror2IndPathString[ij]= m_Rich2Mirror2CommonPath + Rich2Mirror2DetElemCont1 + Rich2Mirror2String+mnum[ij];
      
    }
    
  }

  if(Rich1DE) {

    setRich1FlatMirrorParam(); 

  }

  if(Rich2DE){

    setRich2FlatMirrorParam();

  }
}

CkvG4ReconFlatMirr::~CkvG4ReconFlatMirr(  ) {
}



void CkvG4ReconFlatMirr::setRich1FlatMirrorParam( )
{

  IDataProviderSvc* detSvc = CkvG4SvcLocator::RichG4detSvc();
  IMessageSvc*  msgSvc = CkvG4SvcLocator::RichG4MsgSvc ();
  MsgStream CkvG4ReconFlatMirrlog( msgSvc,"CkvG4ReconFlatMirr");

  // modif in the following to be compatible with recent Detdesc. 
  // SE 16-6-2005.

  SmartDataPtr<DetectorElement> Rich1DE(detSvc, Rich1DeStructurePathName );
  // modif in the following to be compatible with the xml userparameter name.
  // SE 10-5-2005.
  if( Rich1DE) {

    //    std::vector<double> r1m2Nor = Rich1DE->param<std::vector<double> >("Rich1NominalFlatMirrorPlane");
    std::vector<double> r1m2Nor = Rich1DE->param<std::vector<double> >("NominalSecMirrorPlane");    

    double r1m2A = r1m2Nor[0];
    double r1m2B = r1m2Nor[1];
    double r1m2C = r1m2Nor[2];
    double r1m2D = r1m2Nor[3];

    // the following is for vertical Rich1   

    m_RichFlatMirrorNominalOrientation[0][0]= r1m2A;
    m_RichFlatMirrorNominalOrientation[0][1]= r1m2B;
    m_RichFlatMirrorNominalOrientation[0][2]= r1m2C;
    m_RichFlatMirrorNominalOrientation[0][3]= r1m2D;

    m_RichFlatMirrorNominalOrientation[1][0]= r1m2A;
    m_RichFlatMirrorNominalOrientation[1][1]= -1.0*r1m2B;
    m_RichFlatMirrorNominalOrientation[1][2]= r1m2C;
    m_RichFlatMirrorNominalOrientation[1][3]= r1m2D;

    // now for horizontal rich1

   if(m_Rich1HorizFlag) {

      m_RichFlatMirrorNominalOrientation[1][0]= -1.0*r1m2A;
      m_RichFlatMirrorNominalOrientation[1][1]= r1m2B;

      std::vector<double> r1m2NorLeft ;
      std::vector<double> r1m2NorRight ;
      
      
      if(  Rich1DE->exists("NominalSecMirrorPlaneLeft"  )  && Rich1DE->exists("NominalSecMirrorPlaneRight")  ){
          r1m2NorLeft = Rich1DE->param<std::vector<double> >("NominalSecMirrorPlaneLeft");
          r1m2NorRight= Rich1DE->param<std::vector<double> >("NominalSecMirrorPlaneRight"); //!!! Something wrong!
          m_RichFlatMirrorNominalOrientation[0][0]= r1m2NorLeft[0];
          m_RichFlatMirrorNominalOrientation[0][1]= r1m2NorLeft[1];
          m_RichFlatMirrorNominalOrientation[0][2]= r1m2NorLeft[2];
          m_RichFlatMirrorNominalOrientation[0][3]= r1m2NorLeft[3];

          m_RichFlatMirrorNominalOrientation[1][0]= r1m2NorRight[0];
          m_RichFlatMirrorNominalOrientation[1][1]= r1m2NorRight[1];
          m_RichFlatMirrorNominalOrientation[1][2]= r1m2NorRight[2];
          m_RichFlatMirrorNominalOrientation[1][3]= r1m2NorRight[3];
  
          // CkvG4ReconFlatMirrlog << MSG::INFO<<"Horiz Rich1 Flat mirror orientation "<<
          //  r1m2NorLeft <<"   "<<r1m2NorRight<<endreq;
          
      }     
   }

   CkvG4ReconFlatMirrlog << MSG::DEBUG
                         << "Flat Mirr param in rich1  " 
                         << r1m2A <<"  "
                         << r1m2B <<"  "
                         << r1m2C <<"  "
                         << r1m2D 
                         << endreq;
    
   // double r0c0X =  Rich1DE->param<double>("Rich1Mirror2NominalCCLHCbXR0C0");
   // double r0c0Y =  Rich1DE->param<double>("Rich1Mirror2NominalCCLHCbYR0C0");
   // double r0c0Z =  Rich1DE->param<double>("Rich1Mirror2NominalCCLHCbZR0C0");
    
   // double r2c2X =  Rich1DE->param<double>("Rich1Mirror2NominalCCLHCbXR2C2");
   //  double r2c2Y =  Rich1DE->param<double>("Rich1Mirror2NominalCCLHCbYR2C2");
   //  double r2c2Z =  Rich1DE->param<double>("Rich1Mirror2NominalCCLHCbZR2C2");
   
   //    double r2delX =   Rich1DE->param<double>("Rich1Mirror2CoCNominalDeltaX");
   // double r2delY =   Rich1DE->param<double>("Rich1Mirror2CoCNominalDeltaY");
   // double r2delZ =   Rich1DE->param<double>("Rich1Mirror2CoCNominalDeltaZ");
   //    double r2rad=     Rich1DE->param<double>("Rich1Mirror2NominalRadiusC");
   //   CkvG4ReconFlatMirrlog << MSG::INFO<<"Rich1 Mirror2 nominal radius "<< r2rad<<endreq;
   
    
   for(int im=0; im< m_Rich1NumSecMirror; ++im) { 
      //      std::string apath = m_Rich1Mirror2IndPathString[ii];
      //     SmartDataPtr<DetectorElement> Rich1M2(detSvc, apath);
     DetectorElement* Rich1M2=getMirrorDetElem (0, im );
      
     if( !Rich1M2 ) {

       CkvG4ReconFlatMirrlog << MSG::ERROR<<       
       "Rich1 mirror2 detelem does not exist . Mirror num "<<im << endreq;
       
     }else {
       
     
       Gaudi::XYZPoint zero(0.0,0.0,0.0);
       Gaudi::XYZPoint mcoc = Rich1M2 ->geometry() ->toGlobal(zero);
       const SolidSphere* aSphereSolid = getCurMirrorSolid (0,im );
          
       double r2rad = aSphereSolid->insideRadius();
       // CkvG4ReconFlatMirrlog << MSG::INFO<<"Rich1 Mirror2 nominal radius "<< r2rad<<endreq;
      
       m_RichSecMirrCoCRad[im] [0] = mcoc.x();
       m_RichSecMirrCoCRad[im] [1] = mcoc.y();
       m_RichSecMirrCoCRad[im] [2] = mcoc.z();
       m_RichSecMirrCoCRad[im] [3] =  r2rad;    
       
       CkvG4ReconFlatMirrlog << MSG::DEBUG<< "Rich1 Mirror2 num CoCxyz rad "<< im 
                             <<"  "<< mcoc.x()<<"  "
                             << mcoc.y()<<"  "<< mcoc.z()<<"  "<<r2rad <<endreq;      
       
     }  
    }    
  }
}



void CkvG4ReconFlatMirr::setRich2FlatMirrorParam( )
{
  IDataProviderSvc* detSvc = CkvG4SvcLocator::RichG4detSvc();
  IMessageSvc*  msgSvc = CkvG4SvcLocator::RichG4MsgSvc ();
  MsgStream CkvG4ReconFlatMirrlog( msgSvc,"CkvG4ReconFlatMirr");


  SmartDataPtr<DetectorElement> Rich2DE(detSvc, Rich2DeStructurePathName );

  if(Rich2DE) {

    std::vector<double> r2SecNormPlane =  Rich2DE->param<std::vector<double> >("NominalSecMirrorPlane");
    double r2m2A = r2SecNormPlane[0];
    double r2m2B = r2SecNormPlane[1];
    double r2m2C = r2SecNormPlane[2];
    double r2m2D = r2SecNormPlane[3];


    m_RichFlatMirrorNominalOrientation[2][0]= r2m2A;
    m_RichFlatMirrorNominalOrientation[2][1]= r2m2B;
    m_RichFlatMirrorNominalOrientation[2][2]= r2m2C;
    m_RichFlatMirrorNominalOrientation[2][3]= r2m2D;

    m_RichFlatMirrorNominalOrientation[3][0]= -1.0*r2m2A;
    m_RichFlatMirrorNominalOrientation[3][1]= r2m2B;
    m_RichFlatMirrorNominalOrientation[3][2]= r2m2C;
    m_RichFlatMirrorNominalOrientation[3][3]= r2m2D;

    // CkvG4ReconFlatMirrlog << MSG::INFO
    //                       << "Flat Mirr nominal param in rich2  "
    //                       << r2m2A<<"  "<<r2m2B
    //                       <<"   "<<r2m2C
    //                       <<"   "<<r2m2D<< endreq;
    //
    
        
    for(int im=0; im< m_Rich2NumSecMirror; ++im) {
      DetectorElement* Rich2M2=getMirrorDetElem (1, im );

      //      std::string apath = aa+mnum[im];
      // SmartDataPtr<DetectorElement> Rich2M2(detSvc, apath);
      if( !Rich2M2 ) {
        CkvG4ReconFlatMirrlog << MSG::ERROR<<       
          "Rich2 mirror2 detelem does not exist . Mirror num "<<im<< endreq;
        
      }else {
        
        Gaudi::XYZPoint zero(0.0,0.0,0.0);
        Gaudi::XYZPoint mcoc = Rich2M2 ->geometry() ->toGlobal(zero);
        
        const SolidSphere* aSphereSolid = getCurMirrorSolid (1,im );     
        double r2rad = aSphereSolid->insideRadius();
        
        m_RichSecMirrCoCRad[im+m_Rich1NumSecMirror] [0] = mcoc.x();
        m_RichSecMirrCoCRad[im+m_Rich1NumSecMirror] [1] = mcoc.y();
        m_RichSecMirrCoCRad[im+m_Rich1NumSecMirror] [2] = mcoc.z();
        m_RichSecMirrCoCRad[im+m_Rich1NumSecMirror] [3] =  r2rad;    
        
        //  CkvG4ReconFlatMirrlog << MSG::INFO<< "Rich2 Mirror2 num CoCxyz rad "<<
        //                        im<<"  "<< mcoc.x()<<"  "
        //                       << mcoc.y()<<"  "<< mcoc.z()<<"   "<<r2rad <<endreq; 
        //
      }    
      
    }    
  }
  
}

Gaudi::XYZPoint  CkvG4ReconFlatMirr::FlatMirrorReflect(const Gaudi::XYZPoint & HitCoordQw ,
                                                       int FlatMirrorType)
{

  IMessageSvc*  msgSvc = CkvG4SvcLocator::RichG4MsgSvc ();
  MsgStream CkvG4ReconFlatMirrlog( msgSvc,"RichG4ReconFlatMirr");
  //CkvG4ReconFlatMirrlog<<MSG::INFO<<" Now in Flat mirror reflect "<<  FlatMirrorType<<"  aHitCoordQW  "<< HitCoordQw<< endreq;
  // CkvG4ReconFlatMirrlog<<MSG::INFO<<"Flat mirr reflect orientation "
  //      << m_RichFlatMirrorNominalOrientation[FlatMirrorType] [3] <<endreq;
  

  Gaudi::Plane3D aMPlane(m_RichFlatMirrorNominalOrientation[FlatMirrorType][0],
                         m_RichFlatMirrorNominalOrientation[FlatMirrorType][1],
                         m_RichFlatMirrorNominalOrientation[FlatMirrorType][2],
                         m_RichFlatMirrorNominalOrientation[FlatMirrorType][3]);
  double adist =  aMPlane.Distance(HitCoordQw);
  Gaudi::XYZPoint virtDetPoint =
    HitCoordQw - 2.0*adist * (aMPlane.Normal() );
  
  // CkvG4ReconFlatMirrlog<<MSG::INFO<<" Now in Flat mirror reflect "<< FlatMirrorType<<"  "<< HitCoordQw 
  //                     <<"  "<<virtDetPoint<<endmsg;
  
  return virtDetPoint;
  
}

double CkvG4ReconFlatMirr::DistFromFlatMirror(const Gaudi::XYZPoint & HitCoordQw ,
                                                       int FlatMirrorType )
{
  
  Gaudi::Plane3D aMPlane(m_RichFlatMirrorNominalOrientation[FlatMirrorType][0],
                         m_RichFlatMirrorNominalOrientation[FlatMirrorType][1],
                         m_RichFlatMirrorNominalOrientation[FlatMirrorType][2],
                         m_RichFlatMirrorNominalOrientation[FlatMirrorType][3]);
  double adist =  aMPlane.Distance(HitCoordQw);
    
  return adist;
}

double CkvG4ReconFlatMirr::AngleOnFlatMirror(const Gaudi::XYZPoint& HitCoordQw,
                                             const Gaudi::XYZPoint& PtOnFM,
                                             int FlatMirrorType)
{
  
  Gaudi::Plane3D aMPlane(m_RichFlatMirrorNominalOrientation[FlatMirrorType][0],
                         m_RichFlatMirrorNominalOrientation[FlatMirrorType][1],
                         m_RichFlatMirrorNominalOrientation[FlatMirrorType][2],
                         m_RichFlatMirrorNominalOrientation[FlatMirrorType][3]);

  Gaudi::XYZVector v= (HitCoordQw-PtOnFM).unit();

  return acos(v.Dot(aMPlane.Normal()));
  
}

Gaudi::XYZPoint CkvG4ReconFlatMirr::ConvertToLocal(const Gaudi::XYZPoint & aGlobalPoint, 
                                                   int aRichDetNum, 
                                                   int aMirrorNum ) 
{
  
  DetectorElement* aMDet = getMirrorDetElem(aRichDetNum,aMirrorNum);
  
  Gaudi::XYZPoint aLocp = aMDet->geometry()->toLocal( aGlobalPoint);
  
  return aLocp;
  
}

Gaudi::XYZPoint CkvG4ReconFlatMirr::ConvertToGlobal(const Gaudi::XYZPoint & aLocalPoint,
                                                    int aRichDetNum, 
                                                    int aMirrorNum )   
{
  
  DetectorElement* aMDet = getMirrorDetElem(aRichDetNum,aMirrorNum);
  Gaudi::XYZPoint aGlop = aMDet->geometry()->toGlobal( aLocalPoint);
  return aGlop;
  
}

Gaudi::XYZPoint CkvG4ReconFlatMirr::FlatMirrorIntersection( const Gaudi::XYZPoint & aGlobalPoint1,
                                                            const Gaudi::XYZPoint & aGlobalPoint2, 
                                                            int aRichDetNum,
                                                            int aFlatMirrorNum ) 
{


  IMessageSvc*  msgSvc = CkvG4SvcLocator::RichG4MsgSvc ();
  MsgStream CkvG4ReconFlatMirrlog( msgSvc,"CkvG4ReconFlatMirr");
 
  const Gaudi::XYZPoint & aLocalPoint1 =  ConvertToLocal( aGlobalPoint1,aRichDetNum,aFlatMirrorNum);
  const Gaudi::XYZPoint & aLocalPoint2 =  ConvertToLocal( aGlobalPoint2,aRichDetNum,aFlatMirrorNum);
  
  const Gaudi::XYZVector aVect = (aLocalPoint2-aLocalPoint1).unit();
  ISolid::Ticks sphTicks;


  const SolidSphere* aSphereSolid= getCurMirrorSolid (aRichDetNum,aFlatMirrorNum);
  const unsigned int sphTicksSize = aSphereSolid->intersectionTicks(aLocalPoint1, aVect, sphTicks);

  
  const Gaudi::XYZPoint & aLocalIntersectionPoint = (sphTicksSize > 0 ) ? aLocalPoint1+ sphTicks[0] * aVect: aLocalPoint1;
  
  const Gaudi::XYZPoint & aGlobalIntersectionPoint = ConvertToGlobal(aLocalIntersectionPoint,
                                                                     aRichDetNum,aFlatMirrorNum );
  return aGlobalIntersectionPoint;
  
}

const SolidSphere* CkvG4ReconFlatMirr::getCurMirrorSolid (int aRichDetNum, int aFlatMirrorNum ) 
{

  IMessageSvc*  msgSvc = CkvG4SvcLocator::RichG4MsgSvc ();
  MsgStream CkvG4ReconFlatMirrlog( msgSvc,"CkvG4ReconFlatMirr");

  const SolidSphere* sphereSolid = 0;
  DetectorElement* aMDet = getMirrorDetElem(aRichDetNum,aFlatMirrorNum);
  if(!aMDet ) {
    CkvG4ReconFlatMirrlog << MSG::ERROR<< "Rich mirror detelem does not exist . richdet Mirror num "
                          << aRichDetNum
                          << aFlatMirrorNum 
                          << endreq;
      
  }else {

    const ISolid* aSolid = aMDet ->geometry() ->lvolume()->solid();
    const std::string type = aSolid->typeName();
    if( type == "SolidSphere" ){
      sphereSolid = dynamic_cast<const SolidSphere*>(aSolid);
    }else {
      // assume that the sphere segment boolean is always made with sphere as the
      // staring vol.
      const SolidBoolean* compSolid = dynamic_cast<const SolidBoolean*>(aSolid);
      sphereSolid =  dynamic_cast<const SolidSphere*>(compSolid->first());   
    }
  }
  
  return  sphereSolid ;
     
}

DetectorElement* CkvG4ReconFlatMirr::getMirrorDetElem (int aRichDetNum, int aMirrorNum ) 
{
  IDataProviderSvc* detSvc = CkvG4SvcLocator::RichG4detSvc();
  IMessageSvc*  msgSvc = CkvG4SvcLocator::RichG4MsgSvc ();
  MsgStream CkvG4ReconFlatMirrlog( msgSvc,"CkvG4ReconFlatMirr");

  std::string apath = "NULL";  
  
  if( aRichDetNum == 0 ) {
    if( aMirrorNum < (int) m_Rich1Mirror2IndPathString.size() ) {
    
      apath = m_Rich1Mirror2IndPathString[aMirrorNum];
    }else {
      
      CkvG4ReconFlatMirrlog << MSG::ERROR<<" Unknown sec mirror num in Rich1  "
                            << aMirrorNum 
                            <<endreq;
      
    }
    
    
    
  } else if (aRichDetNum == 1 ) {
    
    if( aMirrorNum < (int) m_Rich2Mirror2IndPathString.size() ) {


      apath =  m_Rich2Mirror2IndPathString[aMirrorNum];
      
      // G4cout<<" Current rich2 flat mirror Path detelem "<< apath<<G4endl;
      
    }else {
     
      CkvG4ReconFlatMirrlog << MSG::ERROR<<" Unknown sec mirror num in Rich2  "
       << aMirrorNum<<endreq;
    }
    
  } else {
    
      CkvG4ReconFlatMirrlog << MSG::ERROR<<" Unknown Rich det num   "
       << aRichDetNum <<endreq;
    
  }

  SmartDataPtr<DetectorElement> RichM2(detSvc, apath);
  
  return RichM2;
       
  
}


Gaudi::XYZPoint CkvG4ReconFlatMirr::FlatMirrorCoC( int aRichDetNum, 
                                                   int aMirrorNum ) {

  Gaudi::XYZPoint aPoint(0,0,0);

  if( aRichDetNum == 0 ) {
    aPoint = Gaudi::XYZPoint (m_RichSecMirrCoCRad[aMirrorNum] [0],
                              m_RichSecMirrCoCRad[aMirrorNum] [1],
                              m_RichSecMirrCoCRad[aMirrorNum] [2]);

  }else if ( aRichDetNum == 1 ) {

    aPoint = Gaudi::XYZPoint (m_RichSecMirrCoCRad[aMirrorNum+m_Rich1NumSecMirror] [0],
                              m_RichSecMirrCoCRad[aMirrorNum+m_Rich1NumSecMirror] [1],
                              m_RichSecMirrCoCRad[aMirrorNum+m_Rich1NumSecMirror] [2]);

  }

  return aPoint;
}
//=============================================================================
