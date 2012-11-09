// $Id: RichG4ReconHpd.cpp,v 1.12 2009-07-03 11:59:49 seaso Exp $
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
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

// local
#include "GaussRICH/RichG4SvcLocator.h"

// local
#include "RichG4ReconHpd.h"

// RichDet
#include "RichDet/DeRich.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4ReconHpd
//
// 2003-09-09 : Sajan EASO
//-----------------------------------------------------------------------------
//
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4ReconHpd::RichG4ReconHpd():
  m_NumHpdInternalDimensions(10),
  m_HpdInternalDimensions(std::vector<double>(m_NumHpdInternalDimensions)),
  m_NumHpdCrossFocusParameters(2),
  m_HpdCrossFocusParameters
  (std::vector<double>( m_NumHpdCrossFocusParameters)),
  m_NumParamHpdQwRefractCorr(4),
  m_HpdQwRefractCorr (std::vector<double>(m_NumParamHpdQwRefractCorr))
{
  IDataProviderSvc* detSvc = RichG4SvcLocator::RichG4detSvc();
  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();

  MsgStream RichG4HpdReconlog( msgSvc,"RichG4HpdRecon");

    SmartDataPtr<DetectorElement> Rich1DE(detSvc, DeRichLocations::Rich1);

  if( !Rich1DE ){
    RichG4HpdReconlog << MSG::ERROR
                      << "Can't retrieve " << DeRichLocations::Rich1 << " for HpdRecon"
                      << endreq;
  }else {

    m_HpdSiDetXSize=
      Rich1DE->param<double>("RichHpdSiliconDetectorXSize");

    //  m_HpdSiDetXSize=
    //  Rich1DE->userParameterAsDouble("RichHpdSiliconDetectorXSize");

    m_HpdSiDetYSize=
      Rich1DE->param<double>("RichHpdSiliconDetectorYSize");

    // m_HpdSiDetYSize=
    //  Rich1DE->userParameterAsDouble("RichHpdSiliconDetectorYSize");

    m_HpdSiDetZSize=
      Rich1DE->param<double>("RichHpdSiliconDetectorZSize");

    //    m_HpdSiDetZSize=
    //  Rich1DE->userParameterAsDouble("RichHpdSiliconDetectorZSize");

    m_HpdSiDetZLocation=
      Rich1DE->param<double>("RichHpdSiliconDetectorLocalZlocation");

    //    m_HpdSiDetZLocation=
    //  Rich1DE->userParameterAsDouble("RichHpdSiliconDetectorLocalZlocation");

    m_HpdPhCathodeToSiDetMaxDist=
      Rich1DE->param<double>("RichHpdQWToSiMaxDist");

    //  m_HpdPhCathodeToSiDetMaxDist=
    //  Rich1DE->userParameterAsDouble("RichHpdQWToSiMaxDist");

    //    m_HpdPhCathodeRad=
    //  Rich1DE->userParameterAsDouble("RichHpdQWInnerSphericalRadius");
    m_HpdPhCathodeRad=
      Rich1DE->param<double>("RichHpdPhCathInnerSphericalRadius");

    //   m_HpdPhCathodeRad=
    //  Rich1DE->userParameterAsDouble("RichHpdPhCathInnerSphericalRadius");


    m_HpdPixelXSize=
      Rich1DE->param<double>( "RichHpdPixelXsize");

    //   m_HpdPixelXSize=
    //  Rich1DE->userParameterAsDouble( "RichHpdPixelXsize");

    m_HpdPixelYSize=
      Rich1DE->param<double>("RichHpdPixelYsize");

    //    m_HpdPixelYSize=
    //  Rich1DE->userParameterAsDouble("RichHpdPixelYsize");

    m_HpdPixelXNumCol=
      Rich1DE->param<int>( "RichHpdNumPixelCol");

    //    m_HpdPixelXNumCol=
    //  Rich1DE->userParameterAsInt( "RichHpdNumPixelCol");

    m_HpdPixelXNumRow=
      Rich1DE->param<int>("RichHpdNumPixelRow" );

    m_HpdQwRefractCorr =  Rich1DE->param<std::vector<double> >("RefractHPDQrtzWin");
    
    // test print 
    // for(int ii=0; ii< m_NumParamHpdQwRefractCorr; ++ii ) {
    //  RichG4HpdReconlog<<MSG::INFO<<" Hpd Qw refraction corr param "<<
    //    ii<<"   "<< m_HpdQwRefractCorr[ii]<<endreq;
    // }

    m_HpdQwOuterRadius =  Rich1DE->param<double> ("RichHpdQWOuterSphericalRadius");
    
    

    //   m_HpdPixelXNumRow=
    //  Rich1DE->userParameterAsInt("RichHpdNumPixelRow" );

    //double PhCathodeCoCZ=
    //  Rich1DE->param<double>("RichHpdPhCathodeCoCZ");

    //    double PhCathodeCoCZ=
    //  Rich1DE->userParameterAsDouble("RichHpdPhCathodeCoCZ");

    //        RichG4HpdReconlog<<MSG::INFO
    //  << "PhCathodetoSiDist SiDetZ Phcathoderad phcathZ "
    //                 <<m_HpdPhCathodeToSiDetMaxDist
    //                 <<"  "<<m_HpdSiDetZLocation
    //                 <<"  "<< m_HpdPhCathodeRad<< "  "
    //                     <<PhCathiodeCoCZ<< endreq;

    // test print
    //   double Rich1Mirr2NominalCCLHCbXR0C0 =
    //  Rich1DE->param<double>("Rich1Mirror2NominalCCLHCbXR0C0");
    // double Rich1Mirr2NominalCCLHCbYR0C0 =
    //  Rich1DE->param<double>("Rich1Mirror2NominalCCLHCbYR0C0");
    // double Rich1Mirr2NominalCCLHCbZR0C0 =
    //  Rich1DE->param<double>("Rich1Mirror2NominalCCLHCbZR0C0");

    // double Rich1Mirr2NominalCCLHCbXR0C2 =
    //  Rich1DE->param<double>("Rich1Mirror2NominalCCLHCbXR0C2");
    // double Rich1Mirr2NominalCCLHCbYR0C2 =
    //  Rich1DE->param<double>("Rich1Mirror2NominalCCLHCbYR0C2");
    // double Rich1Mirr2NominalCCLHCbZR0C2 =
    //  Rich1DE->param<double>("Rich1Mirror2NominalCCLHCbZR0C2");

    // double Rich1Mirr2NominalCCLHCbXR2C0 =
    //  Rich1DE->param<double>("Rich1Mirror2NominalCCLHCbXR2C0");
    // double Rich1Mirr2NominalCCLHCbYR2C0 =
    //  Rich1DE->param<double>("Rich1Mirror2NominalCCLHCbYR2C0");
    //  double Rich1Mirr2NominalCCLHCbZR2C0 =
    //  Rich1DE->param<double>("Rich1Mirror2NominalCCLHCbZR2C0");

    // double Rich1Mirr2NominalCCLHCbXR2C2 =
    //  Rich1DE->param<double>("Rich1Mirror2NominalCCLHCbXR2C2");
    // double Rich1Mirr2NominalCCLHCbYR2C2 =
    //  Rich1DE->param<double>("Rich1Mirror2NominalCCLHCbYR2C2");
    // double Rich1Mirr2NominalCCLHCbZR2C2 =
    //  Rich1DE->param<double>("Rich1Mirror2NominalCCLHCbZR2C2");

    // double  Rich1Mirror2CoCNominalDeltaX =
    //  Rich1DE->param<double>("Rich1Mirror2CoCNominalDeltaX");
    // double  Rich1Mirror2CoCNominalDeltaY =
    //  Rich1DE->param<double>("Rich1Mirror2CoCNominalDeltaY");
    // double  Rich1Mirror2CoCNominalDeltaZ =
    //  Rich1DE->param<double>("Rich1Mirror2CoCNominalDeltaZ");



    //   double Rich1Mirr2NominalRadiusC =
    //   Rich1DE->param<double>("Rich1Mirror2NominalRadiusC");
    //   RichG4HpdReconlog<<MSG::INFO
    //                  <<" Rich1Mirror2r0c0 XYZ radius "
    //                 <<Rich1Mirr2NominalCCLHCbXR0C0<<"    "
    //                 << Rich1Mirr2NominalCCLHCbYR0C0<<"   "
    //                 <<Rich1Mirr2NominalCCLHCbZR0C0<<"    "
    //                 <<Rich1Mirr2NominalRadiusC<<endreq;
    //  RichG4HpdReconlog<<MSG::INFO
    //                  <<" Rich1Mirror2r0c2 XYZ radius "
    //                 <<Rich1Mirr2NominalCCLHCbXR0C2<<"    "
    //                 << Rich1Mirr2NominalCCLHCbYR0C2<<"   "
    //                 <<Rich1Mirr2NominalCCLHCbZR0C2<<"    "
    //                 <<Rich1Mirr2NominalRadiusC<<endreq;
    //  RichG4HpdReconlog<<MSG::INFO
    //                 <<" Rich1Mirror2r2c0 XYZ radius "
    //                 <<Rich1Mirr2NominalCCLHCbXR2C0<<"    "
    //                 << Rich1Mirr2NominalCCLHCbYR2C0<<"   "
    //                 <<Rich1Mirr2NominalCCLHCbZR2C0<<"    "
    //                 <<Rich1Mirr2NominalRadiusC<<endreq;
    //  RichG4HpdReconlog<<MSG::INFO
    //                 <<" Rich1Mirror2r2c2 XYZ radius "
    //                 <<Rich1Mirr2NominalCCLHCbXR2C2<<"    "
    //                 << Rich1Mirr2NominalCCLHCbYR2C2<<"   "
    //                 <<Rich1Mirr2NominalCCLHCbZR2C2<<"    "
    //                 <<Rich1Mirr2NominalRadiusC<<endreq;
    //
    //  RichG4HpdReconlog<<MSG::INFO
    //                 <<" Rich1Mirror2 delta xyz "
    //                 <<Rich1Mirror2CoCNominalDeltaX<<"    "
    //                 <<Rich1Mirror2CoCNominalDeltaY<<"    "
    //                 <<Rich1Mirror2CoCNominalDeltaZ<<endreq;
    //
     // end of test print
  }


   SmartDataPtr<TabulatedProperty>tabDemag(detSvc,
     "/dd/Materials/RichMaterialTabProperties/HpdDemagnification");
  std::vector<double>HpdDemagFac;
  if(!tabDemag) {
   RichG4HpdReconlog << MSG::ERROR <<"RichG4CkvRecon: Can't retrieve  "
     <<" /dd/Materials/RichMaterialTabProperties/HpdDemagnification "
               <<endreq;

  }else{

    TabulatedProperty::Table tableDem = tabDemag->table();
    TabulatedProperty::Table::iterator itd;

    for (itd = tableDem.begin(); itd != tableDem.end(); itd++) {

      HpdDemagFac.push_back(itd->second);
    }
  }

  if( HpdDemagFac.size() >=1  ) {
    // Now for the cross-focussing effect.
    if(HpdDemagFac[0] != 0.0 )HpdDemagFac[0] = -1.0*HpdDemagFac[0];
    m_HpdCrossFocusParameters[0]=HpdDemagFac[0];
  }

    if( HpdDemagFac.size() >=2 ) {

      m_HpdCrossFocusParameters[1]=HpdDemagFac[1];


    }



};

RichG4ReconHpd::~RichG4ReconHpd()
{
};

Gaudi::XYZPoint RichG4ReconHpd::GetLocalPointWithQwCorr(const Gaudi::XYZPoint & aPhCathPoint  ){
     IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
     MsgStream RichG4HpdReconlog( msgSvc,"RichG4HpdRecon");
     double xPh=aPhCathPoint.x();
     double yPh=aPhCathPoint.y();
     // double zPh=aPhCathPoint.z();// not used
     double rphSq = (xPh*xPh) + (yPh* yPh);
     double rPh = pow(rphSq, 0.5);
     double rPhCorr = GetCorrectionForQwRefraction(rPh);
     double rPhNew = rPh+rPhCorr;
     double rPhNewSq = rPhNew* rPhNew;
     double xPhNew = xPh;
     double yPhNew = yPh;
     if(rPh != 0.0 ) {
        xPhNew =  xPh * rPhNew/rPh;
        yPhNew = yPh * rPhNew/rPh;
     }

     double phCQw=m_HpdQwOuterRadius*m_HpdQwOuterRadius;

     //     double phCPh= m_HpdPhCathodeRad*m_HpdPhCathodeRad;

      

    double adiffQw =  phCQw - rPhNewSq;
    double aZNew= (adiffQw > 0.0)  ? pow(adiffQw,0.5): 0.0;

    //double adiffPh=phCPh-rphSq;
    // double aZOld = (adiffPh > 0.0) ? pow(adiffPh,0.5): 0.0;
    //double delZ = aZNew-aZOld;
    // double zPhNew= zPh+delZ;
    // RichG4HpdReconlog<<MSG::INFO<<"PHcath  Z ph calc aZOld aZNew delZ zPh zPhNew  "<<aZOld<<"   "
    //                 <<aZNew<<"  "<<delZ<<" "<<zPh<<"  "<< zPhNew<<endreq;
    

    double zphNewInHpd =ConvertHpdSiliconZToHpdSystem(m_HpdPhCathodeToSiDetMaxDist-m_HpdPhCathodeRad+ aZNew);


    
   //    Gaudi::XYZPoint aNewC= Gaudi::XYZPoint(xPhNew,yPhNew,zPhNewInHpd);

    //  RichG4HpdReconlog<<MSG::INFO<<"PHcath correction before after "<<   aPhCathPoint 
    //                 <<"   "<< xPhNew <<"  "<<yPhNew<<"  "<<zphNewInHpd<<"   "<<aZNew<< endreq;
    //  RichG4HpdReconlog<<MSG::INFO<<"PHcath correction Z "<< m_HpdPhCathodeToSiDetMaxDist<<"   "
    //                 <<  m_HpdPhCathodeRad <<"  "<<m_HpdQwOuterRadius<<"   "<< m_HpdSiDetZLocation<<endreq;
    
    return Gaudi::XYZPoint(xPhNew,   yPhNew, zphNewInHpd);
}
                      
Gaudi::XYZPoint RichG4ReconHpd::ConvertLocalHitToHpdSystem(const Gaudi::XYZPoint & aLocalHit)
{
  double zloc = aLocalHit.z();
  double ZInHpdSystem = zloc - m_HpdSiDetZLocation;
  return Gaudi::XYZPoint(aLocalHit.x(),aLocalHit.y(), ZInHpdSystem);
}
double RichG4ReconHpd::GetCorrectionForQwRefraction( double rCathode  ) {


  return ( (m_HpdQwRefractCorr [3]*rCathode*rCathode*rCathode) +
           (  m_HpdQwRefractCorr [2]*rCathode*rCathode )+
           ( m_HpdQwRefractCorr [1]*rCathode ) +
           (  m_HpdQwRefractCorr [0]) ); 
  
}

Gaudi::XYZPoint
RichG4ReconHpd::ReconHitOnPhCathFromLocalHitCoord ( const Gaudi::XYZPoint & aLocalHitCoord, bool applyQwRefCorr )
{

  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();

  MsgStream RichG4HpdReconlog( msgSvc,"RichG4HpdRecon");

  double xsi = aLocalHitCoord.x();

  double ysi = aLocalHitCoord.y();

  // double zsi = aLocalHitCoord.z();
  double rsi =0.0;
  double xph =0.0;
  double yph =0.0;
  double rph =0.0;
  double rphtest =0.0;
  double xphLin =0.0;
  double yphLin =0.0;
  double rphLin = 0.0;
  double zph =0.0;
  double rphsq=0.0;
  double phCSq=0.0;
  double delZ=0.0;
  double zPhInHpd=0.0;

  double inverseR = 0.0;

  double inverseRtest = 0.0;

  double rphsqUncorr=0.0;
  double rphCorrection  = 0.0;
  double rCathodeMaxRadius= 0.0;
  double rphUncorr  =0.0;
  

  // RichG4HpdReconlog<<MSG::INFO<<" Current demag fact in recon "
  // <<   m_HpdCrossFocusParameters[0] <<"  "
  //     << m_HpdCrossFocusParameters[1] <<endreq;

  if(m_HpdCrossFocusParameters[0] !=0.0 ) {
     rsi = pow( (xsi*xsi + ysi*ysi), 0.5);
    xphLin= xsi/m_HpdCrossFocusParameters[0];
    yphLin= ysi/m_HpdCrossFocusParameters[0];
    rphLin = rsi/m_HpdCrossFocusParameters[0];

    //    double c= -1.0*rsi;
    //  double b = -1.0* m_HpdCrossFocusParameters[0];
    // double a = m_HpdCrossFocusParameters[1];
    // for test try linear law.
    //   if( a !=0.0) {
    //   rph = (-b + pow((b*b - 4*a*c),0.5))/(2*a);
    //  }else {
    //  rph = rphLin;

    //   }
      //rphtest = (-b + pow((b*b - 4*a*c),0.5))/(2*a);
    //  rphtest = (-b - pow((b*b - 4*a*c),0.5))/(2*a);


    //   xph = -1.0*(rph/rsi)*xsi;
    // yph = -1.0*(rph/rsi)*ysi;
    // do a solution in 1/R
    if(rsi == 0.0 ) {
      rph = 0.0;
      xph=0.0;
      yph=0.0;
    }else {
      double a = -1.0/(2.0*rsi);
      double b = m_HpdCrossFocusParameters[0];
      double c = -1.0*m_HpdCrossFocusParameters[1];
      if(c !=0.0 ) {

      inverseR = a* ( (-1.0*b)  + pow((b*b+ 4.0*rsi*c), 0.5) );
      inverseRtest = a* ( (-1.0*b)  - pow((b*b+ 4.0*rsi*c), 0.5) );

      }else {

        inverseR =  m_HpdCrossFocusParameters[0]/rsi;
        inverseRtest =  inverseR;

      }


      if( inverseR == 0.0 ) {
        rph=0.0;
        xph=0.0;
        yph=0.0;
      }else {
       rph= 1.0/inverseR;
        xph = (rph/rsi)*xsi;
        yph = (rph/rsi)*ysi;
      }

    if( inverseRtest == 0.0 ) {
      rphtest=0.0;
    }else {
      rphtest = 1.0/inverseRtest;
    }
    }


  }

  rphsqUncorr = (xph*xph) + (yph*yph);

  rCathodeMaxRadius=m_HpdPhCathodeRad;
  rphsq = rphsqUncorr;
  rphUncorr=rph;
  
    
  
  //phCSq = m_HpdPhCathodeRad*m_HpdPhCathodeRad;

  if(applyQwRefCorr) {
    rphCorrection  = GetCorrectionForQwRefraction(rph);
    rCathodeMaxRadius = m_HpdQwOuterRadius;
    rph = rphUncorr + rphCorrection ;
    rphsq= rph*rph;
    //  RichG4HpdReconlog<<MSG::INFO<<"  Correction applied to radius "<< rphUncorr<<"   "
    //                 <<rphCorrection<<"   "<< rph<<endreq;
    
    
    if(rphUncorr != 0.0 ) {
      
      xph = xph*rph/rphUncorr;
      yph = yph*rph/rphUncorr;
      
    }
    
  }  


  phCSq = rCathodeMaxRadius* rCathodeMaxRadius;

  if( ( phCSq -  rphsq) > 0.0  ) {

    delZ = pow( ( phCSq -  rphsq ),0.5);

  } else {
    //  delZ= m_HpdPhCathodeRad;
    delZ= rCathodeMaxRadius;

  }

    zph= m_HpdPhCathodeToSiDetMaxDist-m_HpdPhCathodeRad+ delZ;
  // zph= m_HpdPhCathodeToSiDetMaxDist-rCathodeMaxRadius+ delZ;


  //   RichG4HpdReconlog<<MSG::INFO<<" ReconHitOnPhCathFromLocalHitCoord rsi rph rphLin rphtest"
  //                 << rsi<<"  "<< rph<<"   "<<rphLin<<"  "<< rphtest<<endreq;
               // rphtest<<endreq;
  //  RichG4HpdReconlog<<MSG::INFO
  //         <<" ReconHitOnPhCathFromLocalHitCoord "
  //         <<"  xsi ysi xph yph xphlin yphlin zph phcathrad delZ "
  //       <<xsi<< " " <<ysi<<" "<<xph<<"  "<<yph<<"  "<< xphLin<<"  "<<  yphLin<<"  "
  //        <<zph<<"  "<<m_HpdPhCathodeRad<<"  "
  //    <<delZ<<endreq;
  zPhInHpd =   ConvertHpdSiliconZToHpdSystem(zph) ;

  // RichG4HpdReconlog<<MSG::INFO<<" Local cord for recon input output " <<aLocalHitCoord<<"    "
  //                 <<xph<<"   "<<yph<<"   "<<zPhInHpd<<"   "<<zph<< endreq;
  // RichG4HpdReconlog<<MSG::INFO<<"Z Coord in recon "<<   delZ <<"  "
  //                 <<rCathodeMaxRadius<<"   "<<m_HpdPhCathodeToSiDetMaxDist<<endreq;
  
    return Gaudi::XYZPoint(xph,yph, zPhInHpd);
};



//=============================================================================
