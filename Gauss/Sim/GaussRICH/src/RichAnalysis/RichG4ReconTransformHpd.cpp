// $Id: RichG4ReconTransformHpd.cpp,v 1.11 2008-06-24 15:56:35 jonrob Exp $
// Include files
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/RegistryEntry.h"


#include "GaudiKernel/MsgStream.h"

#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/Material.h"

#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/TabulatedProperty.h"

// RichDet
#include "RichDet/DeRich.h"

#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Transform3DTypes.h"


// local
#include "RichG4ReconTransformHpd.h"
#include "GaussRICH/RichG4SvcLocator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4ReconTransformHpd
//
// 2003-09-09 : Sajan EASO
//-----------------------------------------------------------------------------
//

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4ReconTransformHpd::RichG4ReconTransformHpd(  ) {

}
RichG4ReconTransformHpd::~RichG4ReconTransformHpd(  ) {
}

void RichG4ReconTransformHpd::initialise() {

 m_Rich1SubMasterPvIndex=0;
  m_Rich1MagShPvName0 = "pvRich1MagShH0:0";
  m_Rich1MagShPvName1 = "pvRich1MagShH1:1";
  m_Rich1PhotDetSupPvIndex=1;
  m_HpdSMasterIndex=0;
  m_Rich2HpdPanelName0="pvRich2HPDPanel:0";
  m_Rich2HpdPanelName1="pvRich2HPDPanel:1";
  m_Rich2HpdN2EnclName0="pvRich2HPDN2Encl0";
  m_Rich2HpdN2EnclName1="pvRich2HPDN2Encl1";  
  // m_Rich2N2EnclIndex0=3;
  // m_Rich2N2EnclIndex1=4;
  // m_Rich2HpdPanelIndex0=0;
  // m_Rich2HpdPanelIndex1=0;
  m_Rich1HpdArrayMaxH0=98;
  m_Rich2HpdArrayMaxH0=144;

  // the following 2 not used for now.
  //  m_Rich1MagShPvIndexH0=20;
  // m_Rich1MagShPvIndexH1=21;


}

RichG4ReconTransformHpd::RichG4ReconTransformHpd( int aRichDetNum,
                                                  int aHpdNumber )
{

  initialise();
  IDataProviderSvc* detSvc = RichG4SvcLocator::RichG4detSvc();
  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream RichG4ReconTransformHpdlog( msgSvc,"RichG4TransformHpd");

   //

  // RichG4ReconTransformHpdlog<<MSG::INFO
  //                <<"Now creating Hpd transform "
  //                <<  aRichDetNum
  //                              << "  "<<aHpdNumber <<endreq;


  if(aRichDetNum == 0 ) {

    SmartDataPtr<DetectorElement> Rich1DE(detSvc,
                                          DeRichLocations::Rich1);

    if( !Rich1DE ){

      RichG4ReconTransformHpdlog << MSG::ERROR
                                 << "Can't retrieve " << DeRichLocations::Rich1
                                 << " for RichG4TransformHpd"
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

      //   RichG4ReconTransformHpdlog<<MSG::INFO
      //  <<"Now creating Hpd transform for rich1 hpd "
      //                          <<aHpdNumber<<endreq;

      if(apva) {
        //  RichG4ReconTransformHpdlog<<MSG::INFO
        //         << " Rich1SubMaster pvol lvol num sub vol"
        //                          <<apva->name() <<"   "
        //                          <<apva->lvolumeName()<<"  "
        //                          <<apva->lvolume()-> noPVolumes()
        //                          <<endreq;

        const Gaudi::Transform3D & apvaTrans = apva->matrix();
        const Gaudi::Transform3D & apvaTransInv = apva->matrixInv();
        // for test print the names of sub volumes
        // RichG4ReconTransformHpdlog<<MSG::INFO
        //                           <<"rich1 test of hpd number "
        //                           <<aHpdNumber<<endreq;

        //  for (int it=0;it< (int) apva->lvolume()-> noPVolumes(); it++) {
        //  RichG4ReconTransformHpdlog<<MSG::INFO
        //                         <<"daughter pv index name "
        //                         <<it<<"  "
        //                         <<  apva->lvolume()->
        //                      pvolume(it)->name()
        //                         <<endreq;


        // }

        //   RichG4ReconTransformHpdlog<<MSG::INFO
        //                     <<"IN rich1 current hpd num = "
        //                     <<aHpdNumber<<endreq;

        const IPVolume* apvb = (aHpdNumber < m_Rich1HpdArrayMaxH0) ?
          apva->lvolume()->pvolume(m_Rich1MagShPvName0):
          apva->lvolume()->pvolume(m_Rich1MagShPvName1);

        if(apvb ) {

          // RichG4ReconTransformHpdlog<<MSG::INFO
          //             << " Rich1Magsh pvol lvol hpdnum "
          //                        <<apvb->name() <<"   "
          //                        <<apvb->lvolumeName()
          //                        <<"  "<<aHpdNumber
          //                        <<endreq;

          const Gaudi::Transform3D & apvbTrans= apvb->matrix();
          const Gaudi::Transform3D & apvbTransInv= apvb->matrixInv();

          const IPVolume* apvc = apvb->lvolume()
            ->pvolume(m_Rich1PhotDetSupPvIndex);
          if(apvc) {

            //   RichG4ReconTransformHpdlog<<MSG::INFO
            //         << " Rich1PhotDetSup pvol lvol "
            //                      <<apvc->name() <<"   "
            //                      <<apvc->lvolumeName()
            //                      <<endreq;
            // now account for the fact that the index is
            // restarted int he bottom ph det sup vol.

            int aHpdIndex =  aHpdNumber;
            if( aHpdNumber >= m_Rich1HpdArrayMaxH0 ){
              aHpdIndex = aHpdNumber-m_Rich1HpdArrayMaxH0;
            }

            const Gaudi::Transform3D & apvcTrans= apvc->matrix();
            const Gaudi::Transform3D & apvcTransInv= apvc->matrixInv();



	    //            m_PhDetSupGlobalToLocal = apvdTrans * apvcTrans * apvbTrans *
	    //       apvaTrans * aRich1MasterTrans;
	    // m_PhDetSupLocalToGlobal = aRich1MasterTransInv*apvaTransInv*
            //                          apvbTransInv* apvcTransInv*
            //                          apvdTransInv;


            const IPVolume* apvd = apvc->lvolume()
              ->pvolume( aHpdIndex);
            if(apvd) {
              // RichG4ReconTransformHpdlog<<MSG::INFO
              //         << " Rich1hpdmaster pvol lvol num index "
              //                    <<apvd->name() <<"   "
              //                    <<apvd->lvolumeName()
              //                    <<"  "<< aHpdNumber
              //                    <<"  "<< aHpdIndex
              //                    <<endreq;

              const Gaudi::Transform3D & apvdTrans= apvd->matrix();
              const Gaudi::Transform3D & apvdTransInv= apvd->matrixInv();
              const IPVolume* apvf =  apvd->lvolume()
                ->pvolume(m_HpdSMasterIndex);
              if(apvf) {
                // RichG4ReconTransformHpdlog<<MSG::INFO
                //      << " Rich1hpdsubmaster pvol lvol "
                //                  <<apvf->name() <<"   "
                //                  <<apvf->lvolumeName()
                //                  <<endreq;

                const Gaudi::Transform3D & apvfTrans= apvf->matrix();
                const Gaudi::Transform3D & apvfTransInv= apvf->matrixInv();

                m_HpdGlobalToLocal =
                  apvfTrans* apvdTrans * apvcTrans * apvbTrans *
                  apvaTrans * aRich1MasterTrans;
                m_HpdLocalToGlobal =
                  aRich1MasterTransInv*apvaTransInv*
                  apvbTransInv* apvcTransInv*
                  apvdTransInv* apvfTransInv;

                m_HpdPhDetToLocal = apvfTrans* apvdTrans;
                m_HpdLocalToPhDet = apvdTransInv* apvfTransInv;

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

      RichG4ReconTransformHpdlog << MSG::ERROR
                                 << "Can't retrieve " << DeRichLocations::Rich2
                                 << " for RichG4TransformHpd"
                                 << endreq;

    }else {

      //       RichG4ReconTransformHpdlog<<MSG::INFO
      //                       <<"Now creating Hpd transform for rich2 "
      //                       <<endreq;

      //  RichG4ReconTransformHpdlog<<MSG::INFO
      //        << " Rich2Master lvol num sub vol "
      //                         <<Rich2DE->geometry()->lvolume()->name()
      //              <<"  "<<Rich2DE->geometry()->lvolume()-> noPVolumes()
      //                         <<endreq;

      // for (int it2=0;it2< (int) Rich2DE->geometry()->lvolume()
      //               -> noPVolumes(); it2++) {
      // RichG4ReconTransformHpdlog<<MSG::INFO
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

      //        const IPVolume* bpva = (aHpdNumber< m_Rich2HpdArrayMaxH0)?
      //  aRich2MasterLogVol->
      //  pvolume(m_Rich2HpdPanelIndex0):
      //  aRich2MasterLogVol->
      //  pvolume(m_Rich2HpdPanelIndex1);

        const IPVolume* bpva = (aHpdNumber< m_Rich2HpdArrayMaxH0)?
        aRich2MasterLogVol->
        pvolume(m_Rich2HpdN2EnclName0):
        aRich2MasterLogVol->
        pvolume(m_Rich2HpdN2EnclName1);


      if(bpva) {
        // RichG4ReconTransformHpdlog<<MSG::INFO
        //                  <<" hpd num hpdpanel name "
        //                  <<aHpdNumber<<"   "
        //                  << bpva->name()<<endreq;
        int aHpdIndexR2=  aHpdNumber;
        if(  aHpdNumber >= m_Rich2HpdArrayMaxH0) {
          aHpdIndexR2 = aHpdNumber - m_Rich2HpdArrayMaxH0;

        }


        const Gaudi::Transform3D & bpvaTrans = bpva->matrix();
        const Gaudi::Transform3D & bpvaTransInv = bpva->matrixInv();


        const IPVolume* bpvb = (aHpdNumber< m_Rich2HpdArrayMaxH0)?
          bpva->lvolume()->pvolume(m_Rich2HpdPanelName0):
          bpva->lvolume()->pvolume(m_Rich2HpdPanelName1);
        if(bpvb) {

          const Gaudi::Transform3D & bpvbTrans = bpvb->matrix();
          const Gaudi::Transform3D & bpvbTransInv = bpvb->matrixInv();


          const IPVolume* bpvc =
               bpvb->lvolume()->pvolume(aHpdIndexR2);
          if(bpvc) {
              const Gaudi::Transform3D & bpvcTrans = bpvc->matrix();
              const Gaudi::Transform3D & bpvcTransInv = bpvc->matrixInv();
              const IPVolume* bpvf =  bpvc->lvolume()
                  ->pvolume(m_HpdSMasterIndex);
             if(bpvf ) {
              const Gaudi::Transform3D & bpvfTrans = bpvf->matrix();
              const Gaudi::Transform3D & bpvfTransInv = bpvf->matrixInv();

            m_HpdGlobalToLocal =
              bpvfTrans* bpvcTrans * bpvbTrans *
              bpvaTrans * aRich2MasterTrans;
            m_HpdLocalToGlobal =
              aRich2MasterTransInv*bpvaTransInv*
              bpvbTransInv *  bpvcTransInv * bpvfTransInv;


              m_HpdPhDetToLocal = bpvfTrans* bpvcTrans;
              m_HpdLocalToPhDet = bpvcTransInv* bpvfTransInv;


          }


        }
          
        
      }
        

      }



    }



  }


  
}






//=============================================================================
