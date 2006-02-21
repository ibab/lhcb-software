// $Id: RichG4TransformPhDet.cpp,v 1.1 2006-02-21 17:05:27 seaso Exp $
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
 
//#include <CLHEP/Geometry/Point3D.h>
//#include <CLHEP/Geometry/Vector3D.h>
//#include <CLHEP/Geometry/Transform3D.h>
#include "Kernel/Point3DTypes.h"
#include "Kernel/Vector3DTypes.h"
#include "Kernel/Transform3DTypes.h"
 
#include "RichG4SvcLocator.h"
 



// local
#include "RichG4TransformPhDet.h"


//-----------------------------------------------------------------------------
// Implementation file for class : RichG4TransformPhDet
//
// 2006-02-17 : Sajan EASO
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4TransformPhDet::RichG4TransformPhDet(  ) {


}

void RichG4TransformPhDet::initialise() 
{
 m_Rich1SubMasterPvIndex=0;
  m_Rich1MagShPvName0 = "pvRich1MagShH0:0";
  m_Rich1MagShPvName1 = "pvRich1MagShH1:1";
  m_Rich1PhotDetSupPvIndex=1;
  m_HpdSMasterIndex=0;
  m_Rich2HpdPanelIndex0=3;
  m_Rich2HpdPanelIndex1=4;
  m_Rich2HpdPanelName0="pvRich2HPDPanel:0";
  m_Rich2HpdPanelName1="pvRich2HPDPanel:1";
 
  
}

RichG4TransformPhDet::RichG4TransformPhDet(int aRichDetNum , int aSectorNum) {

  initialise();
  
  IDataProviderSvc* detSvc = RichG4SvcLocator::RichG4detSvc();
  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream RichG4TransformPhDetlog( msgSvc,"RichG4TransformPhDet");

 if(aRichDetNum == 0 ) {

    SmartDataPtr<DetectorElement> Rich1DE(detSvc,
                                          DeRichLocation::Rich1);

    if( !Rich1DE ){

      RichG4TransformPhDetlog << MSG::ERROR
                                 << "Can't retrieve " << DeRichLocation::Rich1
                                 << " for RichG4TransformPhDet"
                                 << endreq;

    }else {



      const ILVolume* aRich1MasterLogVol=
        Rich1DE->geometry()->lvolume();
      const Gaudi::Transform3D & aRich1MasterTrans =
        Rich1DE->geometry()->matrix();
      const Gaudi::Transform3D & aRich1MasterTransInv =
        Rich1DE->geometry()->matrixInv();


      const IPVolume* apva =
        aRich1MasterLogVol->pvolume(m_Rich1SubMasterPvIndex);

      //   RichG4TransformPhDetlog<<MSG::INFO
      //  <<"Now creating Hpd transform for rich1 hpd "
      //                          <<aHpdNumber<<endreq;

      if(apva) {
        //  RichG4TransformPhDetlog<<MSG::INFO
        //         << " Rich1SubMaster pvol lvol num sub vol"
        //                          <<apva->name() <<"   "
        //                          <<apva->lvolumeName()<<"  "
        //                          <<apva->lvolume()-> noPVolumes()
        //                          <<endreq;

        const Gaudi::Transform3D & apvaTrans = apva->matrix();
        const Gaudi::Transform3D & apvaTransInv = apva->matrixInv();
        // for test print the names of sub volumes
        // RichG4TransformPhDetlog<<MSG::INFO
        //                           <<"rich1 test of hpd number "
        //                           <<aHpdNumber<<endreq;

        //  for (int it=0;it< (int) apva->lvolume()-> noPVolumes(); it++) {
        //  RichG4TransformPhDetlog<<MSG::INFO
        //                         <<"daughter pv index name "
        //                         <<it<<"  "
        //                         <<  apva->lvolume()->
        //                      pvolume(it)->name()
        //                         <<endreq;


        // }


      if( aSectorNum ==0 || aSectorNum == 1 ) {

        const IPVolume* apvb = (aSectorNum== 0) ?
          apva->lvolume()->pvolume(m_Rich1MagShPvName0):
          apva->lvolume()->pvolume(m_Rich1MagShPvName1);

        if(apvb ) {

          // RichG4TransformPhDetlog<<MSG::INFO
          //             << " Rich1Magsh pvol lvol hpdnum "
          //                        <<apvb->name() <<"   "
          //                        <<apvb->lvolumeName
          //                        <<endreq;

          const Gaudi::Transform3D & apvbTrans= apvb->matrix();
          const Gaudi::Transform3D & apvbTransInv= apvb->matrixInv();

          const IPVolume* apvc = apvb->lvolume()
            ->pvolume(m_Rich1PhotDetSupPvIndex);
          if(apvc) {

            //   RichG4TransformPhDetlog<<MSG::INFO
            //         << " Rich1PhotDetSup pvol lvol "
            //                      <<apvc->name() <<"   "
            //                      <<apvc->lvolumeName()
            //                      <<endreq;
            // now account for the fact that the index is
            // restarted int he bottom ph det sup vol.

            const Gaudi::Transform3D & apvcTrans= apvc->matrix();
            const Gaudi::Transform3D & apvcTransInv= apvc->matrixInv();



	      m_PhDetGlobalToLocal =  apvcTrans * apvbTrans *
	                                apvaTrans * aRich1MasterTrans;
	      m_PhDetLocalToGlobal = aRich1MasterTransInv*apvaTransInv*
                                  apvbTransInv* apvcTransInv;
        


          }}}}}
    
 
     }else if( aRichDetNum == 1){

       SmartDataPtr<DetectorElement> Rich2DE(detSvc,
                                          DeRichLocation::Rich2);

       if( !Rich2DE ){

         RichG4TransformPhDetlog << MSG::ERROR
                                 << "Can't retrieve " << DeRichLocation::Rich2
                                 << " for RichG4TransformPhDet"
                                 << endreq;

          }else {

      //       RichG4TransformPhDetlog<<MSG::INFO
      //                       <<"Now creating Hpd transform for rich2 "
      //                       <<endreq;

      //  RichG4TransformPhDetlog<<MSG::INFO
      //        << " Rich2Master lvol num sub vol "
      //                         <<Rich2DE->geometry()->lvolume()->name()
      //              <<"  "<<Rich2DE->geometry()->lvolume()-> noPVolumes()
      //                         <<endreq;

      // for (int it2=0;it2< (int) Rich2DE->geometry()->lvolume()
      //               -> noPVolumes(); it2++) {
      // RichG4TransformPhDetlog<<MSG::INFO
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
        Rich2DE->geometry()->matrix();
      const Gaudi::Transform3D & aRich2MasterTransInv =
        Rich2DE->geometry()->matrixInv();

      if( aSectorNum ==2 || aSectorNum == 3 ) {
        
       const IPVolume* bpva = (aSectorNum ==2 )?
         aRich2MasterLogVol->
         pvolume(m_Rich2HpdPanelIndex0):
         aRich2MasterLogVol->
         pvolume(m_Rich2HpdPanelIndex1);



      if(bpva) {
        // RichG4ReconTransformPhDetlog<<MSG::INFO
        //                  <<" hpd num hpdpanel name "
        //                  << bpva->name()<<endreq;

        const Gaudi::Transform3D & bpvaTrans = bpva->matrix();
        const Gaudi::Transform3D & bpvaTransInv = bpva->matrixInv();

	      m_PhDetGlobalToLocal = bpvaTrans * aRich2MasterTrans;

	      m_PhDetLocalToGlobal = aRich2MasterTransInv*bpvaTransInv;        

      }}}
  }

}

//=============================================================================
// Destructor
//=============================================================================
RichG4TransformPhDet::~RichG4TransformPhDet() {} 

//=============================================================================
