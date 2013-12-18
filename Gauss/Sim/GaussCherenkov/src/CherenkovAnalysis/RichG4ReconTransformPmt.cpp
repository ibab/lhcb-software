// $Id: RichG4ReconTransformHpd.cpp,v 1.11 2008-06-24 15:56:35 jonrob Exp $
// Include files
#include <boost/lexical_cast.hpp>


#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/RegistryEntry.h"
#include "GaudiKernel/SmartDataPtr.h"



#include "GaudiKernel/MsgStream.h"

#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/Material.h"

#include "DetDesc/DetectorElement.h"
#include "DetDesc/TabulatedProperty.h"

// RichDet
#include "RichDet/DeRich.h"

#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Transform3DTypes.h"


// local
#include "RichG4ReconTransformPmt.h"
#include "GaussCherenkov/CkvG4SvcLocator.h"
#include "GaussCherenkov/CherenkovPmtLensUtil.h"
#include "GaussRICH/RichG4GaussPathNames.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4ReconTransformPmt
//
// 2003-09-09 : Sajan EASO
//-----------------------------------------------------------------------------
//

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4ReconTransformPmt::RichG4ReconTransformPmt(  ) {

}
RichG4ReconTransformPmt::~RichG4ReconTransformPmt(  ) {
}

void RichG4ReconTransformPmt::initialise() {

  IDataProviderSvc* detSvc = CkvG4SvcLocator::RichG4detSvc();
  //  IMessageSvc*  msgSvc = CkvG4SvcLocator::RichG4MsgSvc ();


 m_Rich1SubMasterPvIndex=0;
  m_Rich1MagShPvName0 = "pvRich1MagShH0:0";
  m_Rich1MagShPvName1 = "pvRich1MagShH1:1";
  m_Rich1PhtDetSupName0 = "pvRich1PhDetSupFrameH0:0";
  m_Rich1PhtDetSupName1 = "pvRich1PhDetSupFrameH1:1";
  
  //  m_Rich1PhotDetSupPvIndex=1;
  m_RichPmtInModuleStartIndex=0;
  m_PmtSMasterIndex=0;
  m_Rich2PmtPanelName0="pvRich2HPDPanel:0";
  m_Rich2PmtPanelName1="pvRich2HPDPanel:1";
  m_Rich2PmtN2EnclName0="pvRich2HPDN2Encl0";
  m_Rich2PmtN2EnclName1="pvRich2HPDN2Encl1";  
  // m_Rich2N2EnclIndex0=3;
  // m_Rich2N2EnclIndex1=4;
  // m_Rich2PmtPanelIndex0=0;
  // m_Rich2PmtPanelIndex1=0;
  m_Rich1PmtModuleMaxH0=84;
  m_Rich2PmtModuleMaxH0=91;

  SmartDataPtr<DetectorElement> Rich1DE(detSvc, Rich1DeStructurePathName);
  if(Rich1DE) {
    m_Rich1PmtModuleMaxH0=(Rich1DE->param<int>("Rich1TotNumModules"))/2;
    m_Rich2PmtModuleMaxH0=(Rich1DE->param<int>("Rich2TotNumModules"))/2;
  }

  m_Rich1PmtModuleMasterNamePrefClassic="pvRichPMTModuleMaster";
  m_Rich1PmtModuleMasterNamePrefLens="pvRichPMTLensModuleMaster";
  //  m_Rich1PmtModuleMaxH0=60;
  // m_Rich2PmtModuleMaxH0=91;
  m_RichNumPmtInModule=16;
  m_Rich1PmtArrayMaxH0=m_Rich1PmtModuleMaxH0*m_RichNumPmtInModule;  
  m_Rich2PmtArrayMaxH0=m_Rich2PmtModuleMaxH0*m_RichNumPmtInModule;
  
  // the following 2 not used for now.
  //  m_Rich1MagShPvIndexH0=20;
  // m_Rich1MagShPvIndexH1=21;
  m_Rich1PmtModuleMasterNameSuf.resize(2.0*m_Rich1PmtModuleMaxH0);
  for (int i=0; i<(int)m_Rich1PmtModuleMasterNameSuf.size();++i) {
    std::string ss= boost::lexical_cast<std::string> (i);
    std::string suf= "00";
    if(i>9) suf="0";
    if(i>99) suf="";
    std::string sIndex=suf+ss+":"+ss;
    m_Rich1PmtModuleMasterNameSuf[i]=sIndex;
    
  }
  

}

RichG4ReconTransformPmt::RichG4ReconTransformPmt( int aRichDetNum,
                                                  int aPmtModuleNumber,
                                                  int aPmtNumberInModule )
{

  initialise();
  IDataProviderSvc* detSvc = CkvG4SvcLocator::RichG4detSvc();
  IMessageSvc*  msgSvc = CkvG4SvcLocator::RichG4MsgSvc ();
  MsgStream RichG4ReconTransformPmtlog( msgSvc,"RichG4TransformPmt");

   //

  // RichG4ReconTransformPmtlog<<MSG::INFO
  //                <<"Now creating Pmt transform "
  //                <<  aRichDetNum
  //                              << "  "<<aPmtNumber <<endreq;


  if(aRichDetNum == 0 ) {

    SmartDataPtr<DetectorElement> Rich1DE(detSvc,
                                          DeRichLocations::Rich1);

    if( !Rich1DE ){

      RichG4ReconTransformPmtlog << MSG::ERROR
                                 << "Can't retrieve " << DeRichLocations::Rich1
                                 << " for RichG4TransformPmt"
                                 << endreq;

    }else {



      const ILVolume* aRich1MasterLogVol=
        Rich1DE->geometry()->lvolume();
      const Gaudi::Transform3D & aRich1MasterTrans =
        Rich1DE->geometry()->toLocalMatrix();
      const Gaudi::Transform3D & aRich1MasterTransInv =
        Rich1DE->geometry()->toGlobalMatrix();


      const IPVolume* apva =
        aRich1MasterLogVol->pvolume(m_Rich1SubMasterPvIndex);

      //   RichG4ReconTransformPmtlog<<MSG::INFO
      //  <<"Now creating Pmt transform for rich1 hpd "
      //                          <<aPmtNumber<<endreq;

      if(apva) {
        //  RichG4ReconTransformPmtlog<<MSG::INFO
        //         << " Rich1SubMaster pvol lvol num sub vol"
        //                          <<apva->name() <<"   "
        //                          <<apva->lvolumeName()<<"  "
        //                          <<apva->lvolume()-> noPVolumes()
        //                          <<endreq;

        const Gaudi::Transform3D & apvaTrans = apva->matrix();
        const Gaudi::Transform3D & apvaTransInv = apva->matrixInv();
        // for test print the names of sub volumes
        // RichG4ReconTransformPmtlog<<MSG::INFO
        //                           <<"rich1 test of hpd number "
        //                           <<aPmtNumber<<endreq;

        //  for (int it=0;it< (int) apva->lvolume()-> noPVolumes(); it++) {
        //  RichG4ReconTransformPmtlog<<MSG::INFO
        //                         <<"daughter pv index name "
        //                         <<it<<"  "
        //                         <<  apva->lvolume()->
        //                      pvolume(it)->name()
        //                         <<endreq;


        // }

        //   RichG4ReconTransformPmtlog<<MSG::INFO
        //                     <<"IN rich1 current hpd num = "
        //                     <<aPmtNumber<<endreq;

        const IPVolume* apvb = (aPmtModuleNumber < m_Rich1PmtModuleMaxH0) ?
          apva->lvolume()->pvolume(m_Rich1MagShPvName0):
          apva->lvolume()->pvolume(m_Rich1MagShPvName1);

        if(apvb ) {

          // RichG4ReconTransformPmtlog<<MSG::INFO
          //             << " Rich1Magsh pvol lvol hpdnum "
          //                        <<apvb->name() <<"   "
          //                        <<apvb->lvolumeName()
          //                        <<"  "<<aPmtNumber
          //                        <<endreq;

          const Gaudi::Transform3D & apvbTrans= apvb->matrix();
          const Gaudi::Transform3D & apvbTransInv= apvb->matrixInv();

          //const IPVolume* apvc = apvb->lvolume()
          //  ->pvolume(m_Rich1PhotDetSupPvIndex);


         const IPVolume* apvc= (aPmtModuleNumber < m_Rich1PmtModuleMaxH0) ?   
           apvb->lvolume()->pvolume(m_Rich1PhtDetSupName0):
           apvb->lvolume()->pvolume(m_Rich1PhtDetSupName1);
         

          if(apvc) {

            //  RichG4ReconTransformPmtlog<<MSG::INFO
            //         << " Rich1PhotDetSup pvol lvol "
            //                       <<apvc->name() <<"   "
            //                      <<apvc->lvolumeName()
            //                      <<endreq;
            // now account for the fact that the index is
            // restarted int he bottom ph det sup vol.


            const Gaudi::Transform3D & apvcTrans= apvc->matrix();
            const Gaudi::Transform3D & apvcTransInv= apvc->matrixInv();



	    //            m_PhDetSupGlobalToLocal = apvdTrans * apvcTrans * apvbTrans *
	    //       apvaTrans * aRich1MasterTrans;
	    // m_PhDetSupLocalToGlobal = aRich1MasterTransInv*apvaTransInv*
            //                          apvbTransInv* apvcTransInv*
            //                          apvdTransInv;

            // int aPmtModuleIndex =  aPmtModuleNumber;
            //  if( aPmtModuleNumber >= m_Rich1PmtModuleMaxH0 ){
            //   aPmtModuleIndex = aPmtModuleNumber-m_Rich1PmtModuleMaxH0;
            //  }



            //            const IPVolume* apvd = apvc->lvolume()
            //->pvolume( aPmtModuleIndex);
            //            RichG4ReconTransformPmtlog<<MSG::INFO<<"Module Num Index Name"
            //                          <<aPmtModuleIndex<<"  "<<aPmtModuleNumber<<"   "
            //                          <<m_Rich1PmtModuleMasterNameSuf[aPmtModuleNumber]<<endreq;

            CherenkovPmtLensUtil* aCherenkovPmtLensUtil= CherenkovPmtLensUtil::getInstance();
            bool isLensModule= aCherenkovPmtLensUtil->isPmtModuleWithLens(aPmtModuleNumber);
            std::string aPrefStr= isLensModule? m_Rich1PmtModuleMasterNamePrefLens:m_Rich1PmtModuleMasterNamePrefClassic;
            //    RichG4ReconTransformPmtlog<<MSG::INFO<<" Now look for Module "
            //                          <<aPrefStr+m_Rich1PmtModuleMasterNameSuf[aPmtModuleNumber]<<endreq;
            
            
            const IPVolume* apvd = apvc->lvolume()
              ->pvolume(aPrefStr+m_Rich1PmtModuleMasterNameSuf[aPmtModuleNumber] );
            if(apvd) {
              //      RichG4ReconTransformPmtlog<<MSG::INFO
              //         << " Rich1PmtModulemaster pvol lvol num index "
              //                    <<apvd->name() <<"   "
              //                    <<apvd->lvolumeName()
              //                              <<"  "<< aPmtModuleNumber<<endreq;
                  
                  //                                  <<"  "<< aPmtModuleIndex
                  //                <<endreq;
              //   RichG4ReconTransformPmtlog<<MSG::INFO<<" Pmt num in Module  "
              //      <<aPmtModuleNumber<<"  "<<aPmtNumberInModule<<endreq;
               

              const Gaudi::Transform3D & apvdTrans= apvd->matrix();
              const Gaudi::Transform3D & apvdTransInv= apvd->matrixInv();
              

              const IPVolume* apvf = apvd->lvolume()
              ->pvolume( m_RichPmtInModuleStartIndex+ aPmtNumberInModule);
              if(apvf) {
                
                const Gaudi::Transform3D & apvfTrans= apvf->matrix();
                const Gaudi::Transform3D & apvfTransInv= apvf->matrixInv();
              
                // RichG4ReconTransformPmtlog<<MSG::INFO<<" Pmt num in Module Vol names "<<
                //  aPmtNumberInModule << apvf->name()<<"   "<<apvf->lvolumeName()<<endreq;
                

                const IPVolume* apvg =  apvf->lvolume()
                    ->pvolume(m_PmtSMasterIndex);
                if(apvg) {
                  // RichG4ReconTransformPmtlog<<MSG::INFO
                  //      << " Rich1hpdsubmaster pvol lvol "
                  //                  <<apvf->name() <<"   "
                  //                  <<apvf->lvolumeName()
                  //                  <<endreq;

                  const Gaudi::Transform3D & apvgTrans= apvg->matrix();
                 const Gaudi::Transform3D & apvgTransInv= apvg->matrixInv();

                m_PmtGlobalToLocal =
                  apvgTrans*apvfTrans* apvdTrans * apvcTrans * apvbTrans *
                  apvaTrans * aRich1MasterTrans;
                m_PmtLocalToGlobal =
                  aRich1MasterTransInv*apvaTransInv*
                  apvbTransInv* apvcTransInv*
                  apvdTransInv* apvfTransInv*apvgTransInv;

                m_PmtPhDetPanelToLocal=apvgTrans*apvfTrans* apvdTrans;
                m_PmtLocalToPmtPhDetPanel=apvdTransInv* apvfTransInv*apvgTransInv;
                


              }

              }
            }
            

          }


        }



      }


    }
  }else if( aRichDetNum == 1){

    SmartDataPtr<DetectorElement> Rich2DE(detSvc,
                                          DeRichLocations::Rich2);

    if( !Rich2DE ){

      RichG4ReconTransformPmtlog << MSG::ERROR
                                 << "Can't retrieve " << DeRichLocations::Rich2
                                 << " for RichG4TransformPmt"
                                 << endreq;

    }else {

      //        RichG4ReconTransformPmtlog<<MSG::INFO
      //                       <<"Now creating Pmt transform for rich2 "
      //                       <<endreq;

      //      RichG4ReconTransformPmtlog<<MSG::INFO
      //        << " Rich2Master lvol num sub vol "
      //                         <<Rich2DE->geometry()->lvolume()->name()
      //              <<"  "<<Rich2DE->geometry()->lvolume()-> noPVolumes()
      //                         <<endreq;
      //
      // for (int it2=0;it2< (int) Rich2DE->geometry()->lvolume()
      //               -> noPVolumes(); it2++) {
      // RichG4ReconTransformPmtlog<<MSG::INFO
      //                         <<"rich2 daughter pv index name "
      //                         <<it2<<"  "
      //                         << Rich2DE->geometry() ->lvolume()->
      //                      pvolume(it2)->name()
      //                         <<endreq;
      //
      //
      // }

      const ILVolume* aRich2MasterLogVol=
        Rich2DE->geometry()->lvolume();
      const Gaudi::Transform3D & aRich2MasterTrans =
        Rich2DE->geometry()->toLocalMatrix();
      const Gaudi::Transform3D & aRich2MasterTransInv =
        Rich2DE->geometry()->toGlobalMatrix();

      //      Gaudi::XYZPoint testk(0.0,0.0,0.0);
      // RichG4ReconTransformPmtlog<<MSG::INFO<<"test K  "<<testk<<"   "<<aRich2MasterTrans*testk<<endreq;
      
      //        const IPVolume* bpva = (aPmtNumber< m_Rich2PmtArrayMaxH0)?
      //  aRich2MasterLogVol->
      //  pvolume(m_Rich2HpdPanelIndex0):
      //  aRich2MasterLogVol->
      //  pvolume(m_Rich2HpdPanelIndex1);

        const IPVolume* bpva = (aPmtModuleNumber< m_Rich2PmtModuleMaxH0)?
        aRich2MasterLogVol->
        pvolume(m_Rich2PmtN2EnclName0):
        aRich2MasterLogVol->
        pvolume(m_Rich2PmtN2EnclName1);


      if(bpva) {
        //       RichG4ReconTransformPmtlog<<MSG::INFO
        //                   <<" pmt num hpdpanel name "
        //                  <<aPmtModuleNumber<<"   "
        //                  << bpva->name()<<endreq;
        int aPmtModuleIndexR2=  aPmtModuleNumber;
         if(  aPmtModuleNumber >= m_Rich2PmtModuleMaxH0) {
          aPmtModuleIndexR2 = aPmtModuleNumber - m_Rich2PmtModuleMaxH0;
        }


        const Gaudi::Transform3D & bpvaTrans = bpva->matrix();
        const Gaudi::Transform3D & bpvaTransInv = bpva->matrixInv();

        const IPVolume* bpvb = (aPmtModuleNumber< m_Rich2PmtModuleMaxH0)?
          bpva->lvolume()->pvolume(m_Rich2PmtPanelName0):
          bpva->lvolume()->pvolume(m_Rich2PmtPanelName1);


        if(bpvb) {


          const Gaudi::Transform3D & bpvbTrans = bpvb->matrix();
          const Gaudi::Transform3D & bpvbTransInv = bpvb->matrixInv();

          //  RichG4ReconTransformPmtlog<<MSG::INFO<<" Now get Rich2 bpvs pmtModule  pmtModuleinSide PmtNumInModule   "
          //      << aPmtModuleNumber <<"    "<< aPmtModuleIndexR2<<"   "<<aPmtNumberInModule<< endreq;
          
          const IPVolume* bpvc =
               bpvb->lvolume()->pvolume(aPmtModuleIndexR2);


          if(bpvc) {
              const Gaudi::Transform3D & bpvcTrans = bpvc->matrix();
              const Gaudi::Transform3D & bpvcTransInv = bpvc->matrixInv();

              const IPVolume* bpvf = 
                 bpvc->lvolume()->pvolume( m_RichPmtInModuleStartIndex+aPmtNumberInModule);

              if(bpvf) {
                const Gaudi::Transform3D & bpvfTrans = bpvf->matrix();
                const Gaudi::Transform3D & bpvfTransInv = bpvf->matrixInv();

                const IPVolume* bpvg =  bpvf->lvolume()
                  ->pvolume(m_PmtSMasterIndex);
             if(bpvg ) {
              const Gaudi::Transform3D & bpvgTrans = bpvg->matrix();
              const Gaudi::Transform3D & bpvgTransInv = bpvg->matrixInv();

             
            m_PmtGlobalToLocal =
              bpvgTrans*bpvfTrans* bpvcTrans * bpvbTrans *
              bpvaTrans * aRich2MasterTrans;
            m_PmtLocalToGlobal =
              aRich2MasterTransInv*bpvaTransInv*
              bpvbTransInv *  bpvcTransInv * bpvfTransInv*bpvgTransInv;


            m_PmtPhDetPanelToLocal=bpvgTrans*bpvfTrans* bpvcTrans;
            m_PmtLocalToPmtPhDetPanel=bpvcTransInv * bpvfTransInv*bpvgTransInv;
             


            // Gaudi::XYZPoint testkall(0.0,0.0,0.0);
            // RichG4ReconTransformPmtlog<<MSG::INFO<<"test Kall  "<<testkall<<"   "<<m_PmtGlobalToLocal *testkall<<endreq;

            //           Gaudi::XYZPoint testkallInv(0.0,0.0,0.0);
            //  RichG4ReconTransformPmtlog<<MSG::INFO<<"test KallInv  "<<testkallInv<<"   "
            //              <<  m_PmtLocalToGlobal  *testkallInv<<endreq;


          }


        }

       }
          
      }
        

      }



    }



  }


  
}






//=============================================================================
