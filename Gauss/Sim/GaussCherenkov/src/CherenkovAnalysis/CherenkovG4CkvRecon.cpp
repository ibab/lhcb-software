// $Id: CherenkovG4CkvRecon.cpp,v 1.15 2008-05-30 13:43:23 gcorti Exp $
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
#include "GaussCherenkov/CkvG4SvcLocator.h"
#include "CherenkovG4CkvRecon.h"
//#include "RichG4ReconHpd.h"
#include "GaussCherenkov/RichG4ReconPmt.h"
//#include "RichG4ReconTransformHpd.h"
#include "RichG4ReconTransformPmt.h"
#include "GaussRICH/RichG4TransformPhDet.h"
#include "GaussCherenkov/CkvG4ReconFlatMirr.h"
#include "GaussRICH/RichG4AnalysisConstGauss.h"
#include "GaussRICH/RichG4GaussPathNames.h"
#include "GaussRICH/RichSolveQuarticEqn.h"
#include <math.h>
#include "GaussCherenkov/CkvGeometrySetupUtil.h"

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
// Implementation file for class : CherenkovG4CkvRecon
//
// 2003-09-08 : Sajan Easo
//-----------------------------------------------------------------------------
//
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CherenkovG4CkvRecon::CherenkovG4CkvRecon()
  :m_NumRichDet(2),m_NumPmtRich(std::vector<int> (2)),
   m_Rich1_PmtTransforms(std::vector<RichG4ReconTransformPmt*> (2000)),
   m_Rich2_PmtTransforms(std::vector<RichG4ReconTransformPmt*> (2000)),
   m_PhDetTransforms(std::vector<RichG4TransformPhDet*> (4)),
   m_SphMirrCC(4,std::vector<double>(3)),
   m_SphMirrR1CCX(std::vector<double> (4)),
   m_SphMirrRad( std::vector<double>(2)),
   m_RichG4ReconPmt(0),
   m_curLocalHitCoord(-10000.,-10000.,-10000.0), m_curEmisPt(0,0,0),
   m_curTkMom(0,0,0),m_curGlobalHitPhCath(0,0,0),
   m_curDetPoint(0,0,0), m_curReflPt(0,0,0),
   m_Rich2MixedModuleArrayColumnSize(std::vector<int> (3)),
   m_ModuleArrayRichCopyNumBegin(std::vector<int> (4))   {


  //  m_PmtTransforms(2,std::vector<RichG4ReconTransformPmt*>(6000)),

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



  IDataProviderSvc* detSvc = CkvG4SvcLocator::RichG4detSvc();
  IMessageSvc*  msgSvc = CkvG4SvcLocator::RichG4MsgSvc ();
  MsgStream CherenkovG4CkvReconlog( msgSvc,"CherenkovG4CkvRecon");
  //      CherenkovG4CkvReconlog << MSG::VERBOSE
  //             << "Now creating CherenkovG4CkvRecon "
  //             << endreq;


  SmartDataPtr<DetectorElement> Rich1DE(detSvc, Rich1DeStructurePathName);
  SmartDataPtr<DetectorElement> Rich2DE(detSvc, Rich2DeStructurePathName);

  m_agelnominalrefractiveindex = 1.0339124;
  m_c4f10nominalrefrativeindex = 1.0014069;


  if( !Rich1DE ){

    CherenkovG4CkvReconlog << MSG::ERROR
                           << "Can't retrieve " + Rich1DeStructurePathName+ " for CkvRecon"
                           << endreq;
  }else {

    m_NumRichDet = Rich1DE->param<int>("RichNumberOfDetectors");

    m_NumPmtRich[0] = Rich1DE->param<int>("Rich1TotNumPmt");

    m_NumPmtRich[1] = Rich1DE->param<int>("Rich2TotNumPmt");

    m_NumPmtInModule = Rich1DE->param<int>("RichTotNumPmtInModule") ;


    //    m_NumModulesInRich[0]= Rich1DE->param<int>("Rich1TotNumModules");
    // m_NumModulesInRich[1]= Rich1DE->param<int>("Rich2TotNumModules");


    //  CherenkovG4CkvReconlog<<MSG::INFO<<"Number of pmts in rich1 rich2 "
    //                  << m_NumPmtRich[0] <<"   "<<m_NumPmtRich[1]<<endreq;

    //    m_NumRichDet =
    //  Rich1DE->userParameterAsInt("RichNumberOfDetectors");


    //   m_NumHpdRich[0] =
    //  Rich1DE->userParameterAsInt("Rich1TotNumHpd");

    //   m_NumHpdRich[1] =
    //  Rich1DE->userParameterAsInt("Rich2TotNumHpd");

    //  m_PmtTransforms [0].resize( m_NumPmtRich[0]);
    // m_PmtTransforms [1].resize( m_NumPmtRich[1]);

    m_Rich1_PmtTransforms.resize(m_NumPmtRich[0]);
    m_Rich2_PmtTransforms.resize(m_NumPmtRich[1]);

     CherenkovG4CkvReconlog <<MSG::INFO<<" Size of pmt transforms "
                           << (int) m_Rich1_PmtTransforms.size()<<"  "<<(int) m_Rich2_PmtTransforms.size()<<endreq;

    //    std::vector<double> r1NominalCoC = Rich1DE->param<std::vector<double> >("Rich1NominalCoC");
    std::vector<double> r1NominalCoC = Rich1DE->param<std::vector<double> >("NominalSphMirrorCoC");
    std::vector<double> r1NominalCoCXCoord (4);
    if( Rich1DE-> exists ("NominalSphMirrorXCOC") ){

         r1NominalCoCXCoord = Rich1DE->param<std::vector<double> >("NominalSphMirrorXCOC");
         m_SphMirrCC [0] [0]= r1NominalCoCXCoord[0];
         m_SphMirrCC [1] [0]= r1NominalCoCXCoord[1];

         m_SphMirrR1CCX[0]= r1NominalCoCXCoord[0];
         m_SphMirrR1CCX[1]= r1NominalCoCXCoord[1];
         m_SphMirrR1CCX[2]= r1NominalCoCXCoord[2];
         m_SphMirrR1CCX[3]= r1NominalCoCXCoord[3];

    } else {

        m_SphMirrCC [0] [0]= r1NominalCoC[0];
        m_SphMirrCC [1] [0]= r1NominalCoC[0];

        m_SphMirrR1CCX[0]=0.0;
        m_SphMirrR1CCX[1]=0.0;
        m_SphMirrR1CCX[2]=0.0;
        m_SphMirrR1CCX[3]=0.0;

    }




       //    m_SphMirrCC [0] [0]= r1NominalCoC[0];
    m_SphMirrCC [0] [1]= r1NominalCoC[1];
    m_SphMirrCC [0] [2]= r1NominalCoC[2];



    m_SphMirrRad [0] = Rich1DE->param<double>( "Rich1Mirror1NominalRadiusC");


    CherenkovG4CkvReconlog << MSG::DEBUG
                           << "Rich1 Spherical Mirror1 top  COC and Rad "<< m_SphMirrCC [0] [0]<<"  "<< m_SphMirrCC [0] [1]
                           <<"  "<< m_SphMirrCC [0] [2]<<"  "<<m_SphMirrRad [0]<<endreq;



    //    m_SphMirrCC [0] [0]=
    //   Rich1DE->userParameterAsDouble( "Rich1Mirror1NominalCCX");
    // m_SphMirrCC [0] [1]=
    //  Rich1DE->userParameterAsDouble( "Rich1Mirror1NominalCCY");
    // m_SphMirrCC [0] [2]=
    //  Rich1DE->userParameterAsDouble( "Rich1Mirror1NominalCCZ");

    //    m_SphMirrRad [0] =
    //  Rich1DE->userParameterAsDouble( "Rich1Mirror1NominalRadiusC");


       //    m_SphMirrCC [1] [0]= m_SphMirrCC [0] [0];
    m_SphMirrCC [1] [1]= -1.0* m_SphMirrCC [0] [1];
    m_SphMirrCC [1] [2]=   m_SphMirrCC [0] [2];

    CherenkovG4CkvReconlog << MSG::DEBUG<< "Rich1 Spherical Mirror1 bottom  COC and Rad "
                           << m_SphMirrCC [1] [0]<<"  "<< m_SphMirrCC [1] [1]
                           <<"  "<< m_SphMirrCC [1] [2]<<"  "<<m_SphMirrRad [0]<<endreq;




    m_PmtAnodeThickness= Rich1DE->param<double> ("RichPmtAnodeZSize" );

    m_PmtAnodeXSize= Rich1DE->param<double> ("RichPmtAnodeXSize" );
    m_PmtAnodeYSize= Rich1DE->param<double> ("RichPmtAnodeYSize" );
    m_PmtNumPixelX=Rich1DE->param<int>("RichPmtNumPixelRow" );

    m_PmtNumPixelY=Rich1DE->param<int>("RichPmtNumPixelCol" );

    m_PmtAnodePixelXSize=Rich1DE->param<double> ("RichPmtPixelXsize" );
    m_PmtAnodePixelYSize=Rich1DE->param<double> ("RichPmtPixelYsize" );
    m_PmtAnodePixelGap=Rich1DE->param<double> ("RichPmtPixelGap"  );
    m_NumPmtInModule=Rich1DE->param<int> ("RichTotNumPmtInModule"  );
    m_MaxNumModuleRich1=Rich1DE->param<int>("Rich1TotNumModules");
    m_MaxNumModuleRich2=Rich1DE->param<int>("Rich2TotNumModules");


    m_NumModuleInRich2ModuleArrayRow= ( Rich1DE->exists("Rich2NumberOfModulesInRow"))  ?  Rich1DE->param<int> ( "Rich2NumberOfModulesInRow"  ) : 7  ;
    m_NumModuleInRich2ModuleArrayCol= ( Rich1DE->exists("Rich2NumberOfModulesInCol"))  ?  Rich1DE->param<int> ( "Rich2NumberOfModulesInCol"  ): 13  ;
    if ( Rich1DE->exists("RichPmtModuleNumBeginInPanels") ) m_ModuleArrayRichCopyNumBegin =   Rich1DE->param<std::vector<int> >( "RichPmtModuleNumBeginInPanels"  );


    m_PmtPhCathZFromPMTCenter=10.0;
    if( Rich1DE->exists("RichPmtQwToCenterZDist"))
      m_PmtPhCathZFromPMTCenter= Rich1DE->param<double>("RichPmtQwToCenterZDist");

     CkvGeometrySetupUtil * aCkvGeometrySetup=CkvGeometrySetupUtil::getCkvGeometrySetupUtilInstance() ;
     if(aCkvGeometrySetup-> Rich2_UseGrandPmt()) {
       if(Rich1DE->exists("RichGrandPmtAnodeXSize") )    {

           m_GrandPmtAnodeThickness= Rich1DE->param<double> ("RichGrandPmtAnodeZSize" );
           m_GrandPmtAnodeXSize= Rich1DE->param<double> ("RichGrandPmtAnodeXSize" );
           m_GrandPmtAnodeYSize= Rich1DE->param<double> ("RichGrandPmtAnodeYSize" );
           m_GrandPmtAnodePixelXSize=Rich1DE->param<double> ("RichGrandPmtPixelXSize" );
           m_GrandPmtAnodePixelYSize=Rich1DE->param<double> ("RichGrandPmtPixelYSize" );
           m_GrandPmtAnodePixelGap=Rich1DE->param<double> ("RichGrandPmtPixelGap"  );
           m_NumGrandPmtInModule=Rich1DE->param<int> ("RichTotNumGrandPmtInModule" );
           if(  ( aCkvGeometrySetup-> Rich2PmtArrayConfig()) == 2  ) {

             if(Rich1DE->exists("Rich2MixedNumModulesArraySetup") )    {
               m_Rich2MixedModuleArrayColumnSize = Rich1DE->param<std::vector<int> >("Rich2MixedNumModulesArraySetup");
             }


           }


       }





     }else {
       m_GrandPmtAnodeThickness=m_PmtAnodeThickness;
       m_GrandPmtAnodeXSize = m_PmtAnodeXSize;
       m_GrandPmtAnodeYSize = m_PmtAnodeYSize ;
       m_GrandPmtAnodePixelXSize = m_PmtAnodePixelXSize;
       m_GrandPmtAnodePixelYSize = m_PmtAnodePixelYSize;
       m_GrandPmtAnodePixelGap  = m_PmtAnodePixelGap;
       m_NumGrandPmtInModule = m_NumPmtInModule;

     }





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
    CherenkovG4CkvReconlog << MSG::ERROR
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

    m_SphMirrRad [1]= Rich2DE->param<double>( "SphMirrorRadius");


    m_SphMirrCC [3] [0]= -1.0* m_SphMirrCC [2] [0];
    m_SphMirrCC [3] [1]= m_SphMirrCC [2] [1];
    m_SphMirrCC [3] [2]= m_SphMirrCC [2] [2];

    CherenkovG4CkvReconlog << MSG::DEBUG<< "Rich2 Spherical Mirror1 left  COC and Rad "<< m_SphMirrCC [2] [0]
                           <<"  "<< m_SphMirrCC [2] [1]
                           <<"  "<< m_SphMirrCC [2] [2]<<"  "<<m_SphMirrRad [1]<<endreq;
    CherenkovG4CkvReconlog << MSG::DEBUG<< "Rich2 Spherical Mirror1 right  COC and Rad "<< m_SphMirrCC [3] [0]<<"  "
                           << m_SphMirrCC [3] [1]
                           <<"  "<< m_SphMirrCC [3] [2]<<"  "<<m_SphMirrRad [1]<<endreq;

  }

  //  CherenkovG4CkvReconlog << MSG::INFO<<
  //  "Si pixel x size ysize zsize num pix X Y "
  //                  <<  m_HpdSiPixelXSize <<"  "
  //                  <<  m_HpdSiPixelYSize<<"  "
  //                  << m_HpdSiDetThickness<<"  "
  //                  <<  m_HpdSiNumPixelX<<"   "
  //                  <<  m_HpdSiNumPixelY<< endreq;
  //

  if( Rich1DE && Rich2DE ) {

    for (int idet=0; idet<m_NumRichDet; idet++){

      //   CherenkovG4CkvReconlog << MSG::INFO
      //           << "Now looping through the rich det for transforms"
      //           << endreq;

      for (int ih=0; ih<m_NumPmtRich[idet] ; ih++ ) {
        //                CherenkovG4CkvReconlog << MSG::DEBUG
        //         << " Now looping through pmt for transforms  "
        //                << idet <<"  "<<ih << endreq;
       std::vector<int> aPmtVV=  GetPmtModuleNumber(ih);

       //     CherenkovG4CkvReconlog <<MSG::INFO<<"Transform creation for "<<idet <<"   "
       //                        << ih <<"   "<<aPmtVV[0]<<"   "<<aPmtVV[1]<<endreq;


       //        m_PmtTransforms[idet][ih] = new RichG4ReconTransformPmt (idet, aPmtVV[0], aPmtVV[1]);

        if(idet == 0 ) {

          m_Rich1_PmtTransforms[ih]= new RichG4ReconTransformPmt (idet, aPmtVV[0], aPmtVV[1] );

        } else if ( idet == 1 ) {
          bool getR2Transform=true;
          CkvGeometrySetupUtil * aCkvGeometrySetup=CkvGeometrySetupUtil::getCkvGeometrySetupUtilInstance() ;
          if( (aCkvGeometrySetup-> Rich2_UseGrandPmt()) &&  (aCkvGeometrySetup->  Rich2PmtArrayConfig() ==1)   ) {
            // all PMTS in Rich2 are large pmts
            if( aPmtVV[1] >=  m_NumGrandPmtInModule ) getR2Transform=false;
          }

          if(getR2Transform  ) {

            m_Rich2_PmtTransforms[ih]=  new RichG4ReconTransformPmt (idet, aPmtVV[0], aPmtVV[1]);
          }


        }


        // CherenkovG4CkvReconlog <<MSG::INFO<<"Transform for idet ih "<< idet <<"  "<< ih<<"  "
        //                             <<m_Rich1_PmtTransforms[idet][ih]  <<endreq;



        //      Gaudi::XYZPoint testkallst(0.0,0.0,0.0);
        //  CherenkovG4CkvReconlog  <<MSG::INFO<<"test Kallst "<<testkallst<<"   "
        //                         << (( m_Rich1_PmtTransforms[ih] )-> PmtLocalToGlobal())  * testkallst<<endreq;

        //             if(idet ==0 ) {

        //   Gaudi::XYZPoint testkr1st(0.0,0.0,0.0);
        //    CherenkovG4CkvReconlog  <<MSG::INFO<<"test Kr1st "<<testkr1st<<"   "
        //                            << ((m_Rich1_PmtTransforms[ih])-> PmtLocalToGlobal())  * testkr1st<<endreq;
        //    }

        //   }else if (idet == 1 ) {

        //
        //     Gaudi::XYZPoint testkr2st(0.0,0.0,0.0);
        //      CherenkovG4CkvReconlog  <<MSG::INFO<<"test Kr2st "<<testkr2st<<"   "
        //                           << ((m_Rich2_PmtTransforms[ih])-> PmtLocalToGlobal())  * testkr2st<<endreq;
        //
        //   }



      }


      for (int is=0 ;is < 2 ; is ++) {

        int isect = idet*2 + is;
        m_PhDetTransforms[isect] = new RichG4TransformPhDet(idet, isect );
      }

    }


  }



  m_RichG4ReconPmt = new RichG4ReconPmt();

  //  CherenkovG4CkvReconlog << MSG::INFO
  //             << "Num richdet numPmtIn r1 r2  "<<
  //      m_NumRichDet<<"    "<< m_NumPmtRich[0]
  //                         <<"   "<< m_NumPmtRich[1]<<endmsg;
  //



 }
CherenkovG4CkvRecon::~CherenkovG4CkvRecon(){}


std::vector<int> CherenkovG4CkvRecon::GetPmtModuleNumber(int aPmtNum) {
    std::vector<int> aPmtV(2);

    aPmtV[0] = aPmtNum/m_NumPmtInModule;
    aPmtV[1]=  aPmtNum - aPmtV[0] * m_NumPmtInModule;

return aPmtV;
}

bool CherenkovG4CkvRecon::HitIsFromGrandPmt(int aPmtNum, int aRichDetNum) {
  // Here the pmt num used is that inside each richdet rahter than the global pmtnum.
  bool aFlagGrand = false;
  if(  aRichDetNum ==1 ) {
      CkvGeometrySetupUtil * aCkvGeometrySetup=CkvGeometrySetupUtil::getCkvGeometrySetupUtilInstance() ;
      if(  aCkvGeometrySetup-> Rich2_UseGrandPmt() ) {
        if( aCkvGeometrySetup-> Rich2PmtArrayConfig() ==1 ) {
          aFlagGrand =true;
        }else {
          // Now using Mixed PMT configuration
          // Get the PMT Module Row number.
          std::vector<int> aPmtAV= GetPmtModuleNumber(aPmtNum);
          int aModuleLocalNum=aPmtAV[0];
         if (aModuleLocalNum >= ( m_MaxNumModuleRich2/2)   ) {
          aModuleLocalNum -= ( m_MaxNumModuleRich2/2)  ;
         }


          int aColNum= aModuleLocalNum/m_NumModuleInRich2ModuleArrayCol;
          int aRowNum= aModuleLocalNum - aColNum*m_NumModuleInRich2ModuleArrayCol;
          if( (aRowNum < m_Rich2MixedModuleArrayColumnSize[0] ) ||
              (aRowNum >= ( m_Rich2MixedModuleArrayColumnSize[0]+ m_Rich2MixedModuleArrayColumnSize[1] ) ) ) {
           aFlagGrand =true;
          }

          // std::cout<<" RDet PmtNum Module ModuleLocal Col Row GrandFlag "<<aRichDetNum<<"  "<<aPmtNum<<"   "<<aPmtAV[0]
          //          <<"  "<<aModuleLocalNum<<"  "<<aColNum<<"  "<<aRowNum<<"  "<<aFlagGrand<<std::endl;


        }


      }// end   if rich2use_grandpmt


  }  // end richdet==1



  return  aFlagGrand;
}

bool  CherenkovG4CkvRecon::GrandPmtFlagfromPmtNumInRichDet(int aPmtNumInRichDet, int aRichDetNum) {

     CkvGeometrySetupUtil * aCkvGeometrySetup=CkvGeometrySetupUtil::getCkvGeometrySetupUtilInstance() ;
     std::vector<int> aPmtAV= GetPmtModuleNumber(aPmtNumInRichDet);
     int GlobalModuleNum = (aRichDetNum==1) ? (aPmtAV[0]+m_MaxNumModuleRich1) : aPmtAV[0]  ;
     return aCkvGeometrySetup->ModuleWithGrandPMT(GlobalModuleNum);


}

Gaudi::XYZPoint CherenkovG4CkvRecon::GetSiHitCoordFromPixelNumRDet(int aPXNum,
                                                                   int aPYNum, int aRichDetNum ,int aPmtNumInRichDet  )
{
  //   CkvGeometrySetupUtil * aCkvGeometrySetup=CkvGeometrySetupUtil::getCkvGeometrySetupUtilInstance() ;
   //     bool CurHitIsFromGrandPMT = HitIsFromGrandPmt(aPmtNumInRichDet,aRichDetNum);

     bool CurHitIsFromGrandPMT = GrandPmtFlagfromPmtNumInRichDet(aPmtNumInRichDet,aRichDetNum);
     // std::cout<<" flaggrand  px py rdet pmtnumInRDet SiHitcoord Gr std  "<< CurHitIsFromGrandPMT <<"  "
     //         <<aPXNum<<"  "<<aPYNum<<"  "<<aRichDetNum<<"   "
     //         <<GetSiHitCoordFromGrandPixelNum(aPXNum,aPYNum)<<"   "<<GetSiHitCoordFromPixelNum(aPXNum,aPYNum)<<std::endl;


     if( CurHitIsFromGrandPMT ) {
       return GetSiHitCoordFromGrandPixelNum(aPXNum,aPYNum);

     }else {
       return   GetSiHitCoordFromPixelNum(aPXNum,aPYNum);
     }
}



Gaudi::XYZPoint CherenkovG4CkvRecon::GetSiHitCoordFromPixelNum(int aPXNum,
                                                     int aPYNum )
{

  const double zhitc=   m_PmtAnodeThickness/2.0;
  const double EffectivePixelXSize= m_PmtAnodePixelXSize+m_PmtAnodePixelGap;
  const double EffectivePixelYSize= m_PmtAnodePixelYSize+m_PmtAnodePixelGap;

  const double xhit= (aPXNum - (m_PmtNumPixelX-1)*0.5  ) * EffectivePixelXSize;
  const double yhit= (aPYNum - (m_PmtNumPixelY-1)*0.5  ) * EffectivePixelYSize;


  return  Gaudi::XYZPoint(xhit,yhit,zhitc);
}

Gaudi::XYZPoint CherenkovG4CkvRecon::GetSiHitCoordFromGrandPixelNum(int aPXNum,
                                                     int aPYNum )
{

  const double zhitc=   m_GrandPmtAnodeThickness/2.0;
  const double EffectivePixelXSize= m_GrandPmtAnodePixelXSize+m_GrandPmtAnodePixelGap;
  const double EffectivePixelYSize= m_GrandPmtAnodePixelYSize+m_GrandPmtAnodePixelGap;

  const double xhit= (aPXNum - (m_PmtNumPixelX-1)*0.5  ) * EffectivePixelXSize;
  const double yhit= (aPYNum - (m_PmtNumPixelY-1)*0.5  ) * EffectivePixelYSize;


  return  Gaudi::XYZPoint(xhit,yhit,zhitc);
}

Gaudi::XYZPoint  CherenkovG4CkvRecon::GetCoordInPhDetPanelPlane(const Gaudi::XYZPoint & aLocalHitCoord ,G4int aPmtLensFlag ){

  IMessageSvc*  msgSvc = CkvG4SvcLocator::RichG4MsgSvc ();
  MsgStream CherenkovG4CkvReconlog( msgSvc,"CherenkovG4CkvRecon");
  Gaudi::XYZPoint aPhDetCoordPoint(0.0,0.0,0.0);

  if(aLocalHitCoord.x() == -10000.0 ||
     aLocalHitCoord.y() == -10000.0 ||
     aLocalHitCoord.z() == -10000.0 ) {

    CherenkovG4CkvReconlog << MSG::ERROR
                      <<" For Pmt occupancy Pmt local Hit coord not set "
                      <<endreq;

  }else {

    //  CherenkovG4CkvReconlog << MSG::INFO<<"CoordInPhDetPanelPlane Localcoord Lensflag" <<aLocalHitCoord<<"   "
    //                       <<aPmtLensFlag<<endreq;

    Gaudi::XYZPoint curLocalHitPhCath =
      m_RichG4ReconPmt->ReconHitOnPhCathFromLocalHitCoord(aLocalHitCoord);
    Gaudi::XYZPoint curLocalHitDetPlane= curLocalHitPhCath;

    if( aPmtLensFlag > 0 ) {
      curLocalHitDetPlane = m_RichG4ReconPmt->ReconHitOnLensSurfaceFromPhCathCoord(curLocalHitPhCath);

    }
    //  CherenkovG4CkvReconlog << MSG::INFO<<"CoordInPhDetPanelPlane Phcath Detplane "<<curLocalHitPhCath<<"   "
    //                       << curLocalHitDetPlane<<endreq;

    //int mdu = m_CurrentPmtNum/16;

    // if( m_CurrentRichDetNum == 0 ) {

      // CherenkovG4CkvReconlog << MSG::INFO<<" Rich num Pmt num mNum "<<m_CurrentRichDetNum<<"  "<<m_CurrentPmtNum<<"  "
      //                    <<mdu<<endreq;

    //  }

   if( m_CurrentRichDetNum >=0 &&  m_CurrentPmtNum >=0 ) {

      //  RichG4ReconTransformPmt* CurPmtTransform =
      //  m_PmtTransforms[m_CurrentRichDetNum] [m_CurrentPmtNum];

      RichG4ReconTransformPmt* CurPmtTransformB =
        (m_CurrentRichDetNum ==0) ? m_Rich1_PmtTransforms[m_CurrentPmtNum]: m_Rich2_PmtTransforms[m_CurrentPmtNum];
      // CherenkovG4CkvReconlog << MSG::INFO<<" Get PMT transform  RDEt PMTNum "
      //                       <<m_CurrentRichDetNum<<"   "<<  m_CurrentPmtNum<<"  "<<endreq;

      if(CurPmtTransformB) {
        Gaudi::Transform3D PmttoPhDetTransform = CurPmtTransformB -> PmtLocalToPmtPhDetPanel();
        aPhDetCoordPoint  = PmttoPhDetTransform * curLocalHitDetPlane;

        //test print
        // CherenkovG4CkvReconlog << MSG::INFO<<" Ph coord pt "<<aPhDetCoordPoint <<endreq;

        //  Gaudi::Transform3D PmttoGlobalTransform  =   CurPmtTransformB -> PmtLocalToGlobal();
        //  Gaudi::XYZPoint phcathGlob =   PmttoGlobalTransform * curLocalHitDetPlane;
        //   CherenkovG4CkvReconlog << MSG::INFO<<"Phcath GlobalPt  "<<phcathGlob<<endreq;
        //end test print

      }else {

        CherenkovG4CkvReconlog << MSG::INFO<<"PMTNum No Transform Recon "<< m_CurrentPmtNum<< endreq;
        CherenkovG4CkvReconlog << MSG::INFO<<" Did not get Pmt transforms for reconstruction  PMT NUM"<< m_CurrentPmtNum<< endreq;

      }



   }
  }
  //  CherenkovG4CkvReconlog << MSG::INFO<<"  PhDetCoord  "<<  aPhDetCoordPoint<<endreq;

  return   aPhDetCoordPoint;

}



Gaudi::XYZPoint CherenkovG4CkvRecon::ReconPhCoordFromLocalCoord (const Gaudi::XYZPoint & aLocalHitCoord )
{

  IMessageSvc*  msgSvc = CkvG4SvcLocator::RichG4MsgSvc ();
  MsgStream CherenkovG4CkvReconlog( msgSvc,"CherenkovG4CkvRecon");
  //  CherenkovG4CkvReconlog << MSG::INFO
  //                  <<" Now in ReconPhCoordFromLocalCoord "<<endreq;


  Gaudi::XYZPoint acurGlobalHitPhCath (0.0,0.0,0.0);

  m_curLocalHitCoord = aLocalHitCoord ;

  //   CherenkovG4CkvReconlog << MSG::INFO
  //                  <<" Local Hit coord on SiDet  "
  //                  <<aLocalHitCoord.x()
  //              <<"   "<<aLocalHitCoord.y()
  //                  <<"   "<<aLocalHitCoord.z()
  //                  <<endreq;


  if(m_curLocalHitCoord.x() == -10000.0 ||
     m_curLocalHitCoord.y() == -10000.0 ||
     m_curLocalHitCoord.z() == -10000.0 ) {

    CherenkovG4CkvReconlog << MSG::ERROR
                      <<" Pmt local Hit coord not set "
                      <<endreq;
  }else {



    // first convert from hit on the Sidet to Hit on
    // Photocathode.
    RichG4ReconPmt* aRichG4ReconPmt = m_RichG4ReconPmt;

    Gaudi::XYZPoint curLocalHitPhCath =
      aRichG4ReconPmt->ReconHitOnPhCathFromLocalHitCoord(m_curLocalHitCoord);

    // CherenkovG4CkvReconlog <<MSG::INFO << "QWRec curLocalHitPhCath"<<curLocalHitPhCath<<endreq;

    // CherenkovG4CkvReconlog << MSG::INFO <<"Current rich det pmt  "<<m_CurrentRichDetNum<<"    "
    //                       <<m_CurrentPmtNum<<endreq;

    //CherenkovG4CkvReconlog << MSG::INFO <<"RichReconCkv  Local Hit coord on SiDet before mag "
    //                       <<m_curLocalHitCoord <<endreq;

    //  CherenkovG4CkvReconlog << MSG::INFO
    //                  <<" RichReconCkv Local Hit coord on PhCathode after mag "
    //                  <<curLocalHitPhCath.x()
    //              <<"   "<<curLocalHitPhCath.y()
    //               <<"   "<<curLocalHitPhCath.z()
    //                  <<endreq;
    //
    //            CherenkovG4CkvReconlog << MSG::DEBUG
    //                      << "Current rich det pmt Num  "
    //                      << m_CurrentRichDetNum<<"  "
    //                      <<m_CurrentPmtNum<<endreq;


    // now convert to the global coord system.
    if( m_CurrentRichDetNum >=0 &&  m_CurrentPmtNum >=0 ) {

      //  RichG4ReconTransformPmt* CurPmtTransform =
      //  m_PmtTransforms[m_CurrentRichDetNum] [m_CurrentPmtNum];

      RichG4ReconTransformPmt* CurPmtTransformA =
        (m_CurrentRichDetNum ==0) ? m_Rich1_PmtTransforms[m_CurrentPmtNum]: m_Rich2_PmtTransforms[m_CurrentPmtNum];



      if(CurPmtTransformA) {

        Gaudi::Transform3D PmttoGlobalTransform = CurPmtTransformA->PmtLocalToGlobal();

        //            Gaudi::XYZPoint testkallpmt(0.0,0.0,0.0);
        //     CherenkovG4CkvReconlog <<MSG::INFO<<"test Kallpmt   "<<testkallpmt<<"   "
        //                  << PmttoGlobalTransform *testkallpmt<<endreq;


        acurGlobalHitPhCath =
          PmttoGlobalTransform * curLocalHitPhCath;

        m_curGlobalHitPhCath = acurGlobalHitPhCath;

        //         CherenkovG4CkvReconlog << MSG::INFO  <<
        //     "QWRec Global Hit coord on PhCathode "<<m_curGlobalHitPhCath<<endreq;

        //        CherenkovG4CkvReconlog << MSG::INFO
        //               <<" Global Hit coord on PhCathode  "
        //              <<acurGlobalHitPhCath.x()
        //            <<"   "<<acurGlobalHitPhCath.y()
        //               <<"   "<<acurGlobalHitPhCath.z()
        //                <<endreq;

      }



    }

  }



  return   acurGlobalHitPhCath;

}
Gaudi::XYZPoint CherenkovG4CkvRecon::LensCoordFromPeOrigin (const Gaudi::XYZPoint &  aLocalPhCathCoord)
{
  double PhCathZinPmtSystem= aLocalPhCathCoord.z() + m_PmtPhCathZFromPMTCenter;

  const Gaudi::XYZPoint aPhC= Gaudi::XYZPoint(aLocalPhCathCoord.x(),aLocalPhCathCoord.y(),PhCathZinPmtSystem);
  return ReconPhCoordDetPlaneFromLocalCoord(aPhC,1,0, Gaudi::XYZPoint(0.0,0.0,0.0));


}

Gaudi::XYZPoint CherenkovG4CkvRecon::ReconPhCoordDetPlaneFromLocalCoord (const Gaudi::XYZPoint & aLocalHitCoord,
                                                                         int aLensFlag ,
                                                                         int aRegReconFlag,
                                                                         const Gaudi::XYZPoint & aLensSurfaceCoord  ) {

  IMessageSvc*  msgSvc = CkvG4SvcLocator::RichG4MsgSvc ();
  MsgStream CherenkovG4CkvReconlog( msgSvc,"CherenkovG4CkvRecon");
  Gaudi::XYZPoint acurGlobalHitPhCath (0.0,0.0,0.0);

  m_curLocalHitCoord = aLocalHitCoord ;

  if(m_curLocalHitCoord.x() == -10000.0 ||
     m_curLocalHitCoord.y() == -10000.0 ||
     m_curLocalHitCoord.z() == -10000.0 ) {

    CherenkovG4CkvReconlog << MSG::ERROR
                      <<" Pmt local Hit coord not set "
                      <<endreq;
  }else {



    // first convert from hit on the Sidet to Hit on
    // Photocathode.
    RichG4ReconPmt* aRichG4ReconPmt = m_RichG4ReconPmt;

    Gaudi::XYZPoint curLocalHitPhCath =
      aRichG4ReconPmt->ReconHitOnPhCathFromLocalHitCoord(m_curLocalHitCoord);
    Gaudi::XYZPoint curLocalHitDetPlane = curLocalHitPhCath;
    // CherenkovG4CkvReconlog<<MSG::INFO <<" LensFlag RegReconFlag " <<aLensFlag<< "  "<<aRegReconFlag<<endreq;

    if(  aLensFlag > 0 ) {
      if( aRegReconFlag == 0 ) {

        curLocalHitDetPlane = aRichG4ReconPmt->ReconHitOnLensSurfaceFromPhCathCoord(curLocalHitPhCath);
      }else {

        curLocalHitDetPlane =
         aRichG4ReconPmt->ReconHitOnLensSurfaceCheatedFromPhCathCoord(curLocalHitPhCath,aLensSurfaceCoord);

      }


    }

    // CherenkovG4CkvReconlog << MSG::INFO <<"Current rich det pmt  "<<m_CurrentRichDetNum<<"    "
    //                       <<m_CurrentPmtNum<<endreq;

    // CherenkovG4CkvReconlog << MSG::INFO <<"RichReconCkv  Local Hit coord on SiDet before mag "
    //                        <<m_curLocalHitCoord <<endreq;
    // CherenkovG4CkvReconlog << MSG::INFO <<"RichReconCkv Hit on PhCath Det Plane "
    //                        <<curLocalHitPhCath<<"  "<<curLocalHitDetPlane <<endreq;

    // CherenkovG4CkvReconlog << MSG::INFO
    //                  <<" RichReconCkv Local Hit coord on PhCathode after mag "
    //                  <<curLocalHitPhCath.x()
    //              <<"   "<<curLocalHitPhCath.y()
    //               <<"   "<<curLocalHitPhCath.z()
    //                  <<endreq;

    //            CherenkovG4CkvReconlog << MSG::DEBUG
    //                      << "Current rich det pmt Num  "
    //                      << m_CurrentRichDetNum<<"  "
    //                      <<m_CurrentPmtNum<<endreq;


    // now convert to the global coord system.
    // CherenkovG4CkvReconlog << MSG::INFO<<" Currentrdet pmt "<< m_CurrentRichDetNum<<"  "<< m_CurrentPmtNum<<endreq;

    if( m_CurrentRichDetNum >=0 &&  m_CurrentPmtNum >=0 ) {

      //  RichG4ReconTransformPmt* CurPmtTransform =
      //  m_PmtTransforms[m_CurrentRichDetNum] [m_CurrentPmtNum];

      RichG4ReconTransformPmt* CurPmtTransformA =
        (m_CurrentRichDetNum ==0) ? m_Rich1_PmtTransforms[m_CurrentPmtNum]: m_Rich2_PmtTransforms[m_CurrentPmtNum];



      if(CurPmtTransformA) {

        Gaudi::Transform3D PmttoGlobalTransform = CurPmtTransformA->PmtLocalToGlobal();

        //            Gaudi::XYZPoint testkallpmt(0.0,0.0,0.0);
        //     CherenkovG4CkvReconlog <<MSG::INFO<<"test Kallpmt   "<<testkallpmt<<"   "
        //                  << PmttoGlobalTransform *testkallpmt<<endreq;


        acurGlobalHitPhCath =
          PmttoGlobalTransform * curLocalHitDetPlane;

        m_curGlobalHitPhCath = acurGlobalHitPhCath;

        //         CherenkovG4CkvReconlog << MSG::INFO
        //                               <<" Global Hit coord on PhCathode  "<<m_curGlobalHitPhCath<<endreq;

                  //              <<acurGlobalHitPhCath.x()
        //            <<"   "<<acurGlobalHitPhCath.y()
        //               <<"   "<<acurGlobalHitPhCath.z()
        //                <<endreq;

      }



    }

  }



  return   acurGlobalHitPhCath;

}





Gaudi::XYZPoint CherenkovG4CkvRecon::getPhotAgelExitZ( double ex, double ey, double ez,
                                                       CkvG4Hit* bHit )
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

Gaudi::XYZPoint CherenkovG4CkvRecon::ReconReflectionPointOnSPhMirror (const Gaudi::XYZPoint & aDetectionPoint,
                                                                      const Gaudi::XYZPoint & aEmissionPoint,
                                                                      const Gaudi::XYZPoint & aQwPoint,
                                                                      G4int aRichDetNum,
                                                                      G4int aFlatMirrNum ,
                                                                      int TFlag )
{

  IMessageSvc*  msgSvc = CkvG4SvcLocator::RichG4MsgSvc ();
  MsgStream CherenkovG4CkvReconlog( msgSvc,"CherenkovG4CkvRecon");
  //  G4cout<<"  Now in  ReconReflectionPointOnSPhMirror G4cout"<<G4endl;

  //  CherenkovG4CkvReconlog<<MSG::INFO <<" Now in  ReconReflectionPointOnSPhMirror  " << endreq;

  m_curEmisPt   = aEmissionPoint;
  m_curDetPoint = aDetectionPoint;

  //  CherenkovG4CkvReconlog<<MSG::INFO <<" current richdet and flat mirror "<<aRichDetNum<<"  "<<aFlatMirrNum
  //                << endreq;


  Gaudi::XYZPoint curFlatMCoC =  m_CurReconFlatMirr-> FlatMirrorCoC(aRichDetNum,aFlatMirrNum);

  //  CherenkovG4CkvReconlog<<MSG::INFO <<" current richdet and flat mirror flatmirrorcoc "<<aRichDetNum<<"  "<<aFlatMirrNum
  //                <<"  "<<curFlatMCoC<< endreq;

  //    CherenkovG4CkvReconlog<<MSG::INFO <<" current emission pt "<< m_curEmisPt<<endreq;
  //  CherenkovG4CkvReconlog<<MSG::INFO <<" current detection pt "<<m_curDetPoint <<endreq;


  Gaudi::XYZPoint aSphReflPt = ReconReflectionPointOnSPhMirrorStdInput() ;
  //  CherenkovG4CkvReconlog<<" current Sph Mirror pt first iteration "<<aSphReflPt   <<endreq;

  // for now 5 iterations

  if( TFlag == 0 ) {

    for (int aItr=0; aItr<5 ; ++aItr) {

      Gaudi::XYZPoint aFlatMirrReflPt =  m_CurReconFlatMirr->FlatMirrorIntersection( aSphReflPt,
                                                                                     m_curDetPoint ,
                                                                                     aRichDetNum,
                                                                                     aFlatMirrNum );


      // create a plane at the flat mirr refl point


      const Gaudi::Plane3D aPlane(Gaudi::XYZVector( curFlatMCoC - aFlatMirrReflPt ).unit(), aFlatMirrReflPt);
      // find the detection pt wrt this plane

      double adist = aPlane.Distance(aQwPoint);

      Gaudi::XYZPoint  afrelPt = aQwPoint - 2.0*adist * aPlane.Normal();
      m_curDetPoint =  afrelPt;
      aSphReflPt = ReconReflectionPointOnSPhMirrorStdInput() ;
      ++aItr;

    }
  }


  return  aSphReflPt ;

}


Gaudi::XYZPoint CherenkovG4CkvRecon::ReconReflectionPointOnSPhMirrorStdInput()
{
  IMessageSvc*  msgSvc = CkvG4SvcLocator::RichG4MsgSvc ();
  MsgStream CherenkovG4CkvReconlog( msgSvc,"CherenkovG4CkvRecon");


  Gaudi::XYZPoint ReflPt = Gaudi::XYZPoint (0.0,0.0,0.0);
  const Gaudi::XYZPoint & aEmisPt = m_curEmisPt;

  const Gaudi::XYZPoint & aDetPt = m_curDetPoint ;

  // use  best coc if the rich1 sph mirror has rotated wrt Y axis by large amount.
  double aMirrCCX = m_SphMirrCC[m_CurrentRichSector][0];

  if( (m_CurrentRichSector==0) ||( m_CurrentRichSector==1 ) ){

    if(m_SphMirrR1CCX[0] != 0.0  && m_SphMirrR1CCX[1] != 0.0  ) {

      if (( m_CurrentRichSector==0) && (aDetPt.x() > 0.0) )  aMirrCCX =  m_SphMirrR1CCX [1];
      if (( m_CurrentRichSector==0) && (aDetPt.x() <= 0.0) )  aMirrCCX =  m_SphMirrR1CCX [0];

      if (( m_CurrentRichSector==1) && (aDetPt.x() > 0.0) )  aMirrCCX =  m_SphMirrR1CCX [2];
      if (( m_CurrentRichSector==1) && (aDetPt.x() <= 0.0) )  aMirrCCX =  m_SphMirrR1CCX [3];

    }

  }

  const Gaudi::XYZPoint aMirrCC (aMirrCCX,
                                 m_SphMirrCC[m_CurrentRichSector][1],
                                 m_SphMirrCC[m_CurrentRichSector][2]) ;

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




void CherenkovG4CkvRecon::SolveQuartic(  gsl_complex z[4],
                                         double denom,
                                         double a[4])
{
  double b[4] =    {0.0,0.0,0.0,0.0  };
  //  double c[8] =  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  //  int ierr=0;
  // double resolv=0.0;
  //int asol=0;


  if( denom != 0.0 ) {

    for (int i=0; i<4 ; i++ ) {
      b[i]= a[i]/denom; }

    //    drteq4_(&b[0],&b[1],&b[2],&b[3],c,&resolv,&ierr);

    // asol = gsl_poly_complex_solve_quartic_eqn( b[0], b[1], b[2], b[3],
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

double  CherenkovG4CkvRecon::CherenkovThetaFromReflPt(const Gaudi::XYZPoint & aReflPoint ,
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

double CherenkovG4CkvRecon::CherenkovThetaInAerogel(const Gaudi::XYZPoint & aReflPoint,
                                                    const Gaudi::XYZPoint & aEmisPt  )
{

  m_curEmisPt = aEmisPt;

  IMessageSvc*  msgSvc = CkvG4SvcLocator::RichG4MsgSvc ();
  MsgStream CherenkovG4CkvReconlog( msgSvc,"CherenkovG4CkvRecon");


  Gaudi::XYZVector PhotDir= (aReflPoint - aEmisPt  ).unit();
  double pmag = pow( PhotDir.Mag2(), 0.5);

  //     CherenkovG4CkvReconlog<<MSG::INFO
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

  const double aCkvWithCorrection= aCkvWithoutCorrection-angleCorrection;


  //     CherenkovG4CkvReconlog<<MSG::INFO
  //                 <<"  Agel Ckv noCorr withcorr  "
  //                 <<  aCkvWithoutCorrection<<"   "
  //                 << aCkvWithCorrection<<"      "
  //                 << " Agel Ckv Angle exit Incident "
  //                 <<  aPhotDirAgelExit<<"   "<< angleIncident<<endreq;


  return aCkvWithCorrection;

}

void  CherenkovG4CkvRecon::SetChTrackPreStepPosition( double xprepos,
                                                      double yprepos,
                                                      double zprepos)
{
  m_ChTrackPreStepPosition= Gaudi::XYZPoint( xprepos,yprepos,zprepos);

}

void CherenkovG4CkvRecon::SetChTrackPostStepPosition(double xpostpos,
                                                double ypostpos,
                                                double zpostpos)
{
  m_ChTrackPostStepPosition=Gaudi::XYZPoint( xpostpos, ypostpos,zpostpos);

}


double CherenkovG4CkvRecon::DistFromSphMirror( const Gaudi::XYZPoint & aSphRefPoint )
{

  const Gaudi::XYZPoint & aDetPt = m_curDetPoint ;

  // use  best coc if the rich1 sph mirror has rotated wrt Y axis by large amount.
  double aMirrCCX = m_SphMirrCC[m_CurrentRichSector][0];

  if( (m_CurrentRichSector==0) ||( m_CurrentRichSector==1 ) ){

    if(m_SphMirrR1CCX[0] != 0.0  && m_SphMirrR1CCX[1] != 0.0  ) {

      if (( m_CurrentRichSector==0) && (aDetPt.x() > 0.0) )  aMirrCCX =  m_SphMirrR1CCX [1];
      if (( m_CurrentRichSector==0) && (aDetPt.x() <= 0.0) )  aMirrCCX =  m_SphMirrR1CCX [0];

      if (( m_CurrentRichSector==1) && (aDetPt.x() > 0.0) )  aMirrCCX =  m_SphMirrR1CCX [2];
      if (( m_CurrentRichSector==1) && (aDetPt.x() <= 0.0) )  aMirrCCX =  m_SphMirrR1CCX [3];

    }
  }

  const Gaudi::XYZPoint aMirrCC (aMirrCCX,
                                 m_SphMirrCC[m_CurrentRichSector][1],
                                 m_SphMirrCC[m_CurrentRichSector][2]) ;

  double r  =  m_SphMirrRad[m_CurrentRichDetNum];

  Gaudi::XYZVector oMc =  aSphRefPoint - aMirrCC;

  return sqrt( oMc.Mag2() ) - r ;

}




//=============================================================================
