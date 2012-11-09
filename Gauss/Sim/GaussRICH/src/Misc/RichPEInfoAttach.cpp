#include "GaussRICH/RichPEInfoAttach.h"
#include "GaussTools/GaussTrackInformation.h"
#include "GaussRICH/RichInfo.h"
#include "GaussRICH/RichPhotInfo.h"
#include "GaussRICH/RichPEInfo.h"
#include "GaussRICH/RichG4AnalysisConstGauss.h"
#include "GaussRICH/RichG4GaussPathNames.h"

int RichPhotTkRadiatorNumber ( const G4Track& aPhotonTrk ){
  int aRadiator=1;
  
 G4VUserTrackInformation* aTkInfo=aPhotonTrk.GetUserInformation();
 GaussTrackInformation* CurRichPhotTrackInfo=(GaussTrackInformation*)aTkInfo;
  
  if(CurRichPhotTrackInfo != 0 ){
    if(CurRichPhotTrackInfo->detInfo()){
      RichInfo* aPhRichInfo= (RichInfo*)(CurRichPhotTrackInfo->detInfo());
      if( aPhRichInfo !=0 && aPhRichInfo-> HasUserPhotInfo() ) {
        RichPhotInfo* aPhotInfo =
          aPhRichInfo->RichPhotInformation() ;
        if(aPhotInfo != 0 ) {

          aRadiator = aPhotInfo->PhotProdRadiatorNum();
          


        }        
        
      }
      
    }
  }

  return  aRadiator;
  
}


G4Track* RichPEInfoAttach(const G4Track& aPhotonTk, G4Track* aPETk,const G4ThreeVector & aLocalElectronOrigin)
{
  G4VUserTrackInformation* aTkInfo=aPhotonTk.GetUserInformation();
  GaussTrackInformation* CurRichPhotTrackInfo=(GaussTrackInformation*)aTkInfo;

  // the test on photon origin is commented out for now. It
  // can be revived in the future.

  // G4double CurPhotonOrigZcoord= aPhotonTk.GetVertexPosition().z();
  //    if(( CurPhotonOrigZcoord >= ZUpsRich1Analysis &&
  //     CurPhotonOrigZcoord <= ZDnsRich1Analysis) ||
  //     ( CurPhotonOrigZcoord >= ZUpsRich2Analysis &&
  //     CurPhotonOrigZcoord <= ZDnsRich2Analysis) ) {

  RichPEInfo* CurRichPEInfo = new   RichPEInfo();

  if(CurRichPhotTrackInfo != 0 ){
    if(CurRichPhotTrackInfo->detInfo()){
      RichInfo* aPhRichInfo= (RichInfo*)(CurRichPhotTrackInfo->detInfo());
      if( aPhRichInfo !=0 && aPhRichInfo-> HasUserPhotInfo() ) {

        RichPhotInfo* aPhotInfo =
          aPhRichInfo->RichPhotInformation() ;

        if(aPhotInfo != 0 ) {
          CurRichPEInfo->
            setMotherOfPhotonPDGcode(aPhotInfo-> MotherChTrackPDGcode());

          CurRichPEInfo->
            setVerbosePeTagFlag( aPhotInfo->  VerbosePhotTagFlag());

          CurRichPEInfo->
            setPhotonEnergyAtCkvProd(aPhotInfo->CkvPhotonEnergyAtProd());
          CurRichPEInfo->
            setCherenkovThetaAtProd(aPhotInfo->CkvAngleThetaAtProd());
          CurRichPEInfo->
            setCherenkovPhiAtProd(aPhotInfo->CkvAnglePhiAtProd());
          CurRichPEInfo->
            setMotherofPhotonMomAtProd(aPhotInfo->ChTrackMomAtProd() );
          CurRichPEInfo->  setPhotOriginRadiatorNumber(aPhotInfo->PhotProdRadiatorNum());
            CurRichPEInfo->setHpdPhotonReflectionFlag(aPhotInfo->PhotonHpdReflectionFlag());
            CurRichPEInfo->setQW2PCreflFlagSave(aPhotInfo->QW2PCreflFlagSave());
            CurRichPEInfo->setPhotonSourceInformation( aPhotInfo->PhotonSourceInfo() );
            //G4cout<<" PE INFo attach photon source "<< aPhotInfo->PhotonSourceInfo() <<G4endl;
            

          // fill the following only for verbose tag mode.
          if( aPhotInfo->  VerbosePhotTagFlag() ) {

            CurRichPEInfo->
              setMotherofPhotonMomVectAtProd
              (aPhotInfo->ChTrackMomentumVector() );
            CurRichPEInfo->setMotherofPhotonPDGMass(aPhotInfo->
                                                    MotherChTrackPDGMass());
            CurRichPEInfo->setMotherofPhotonCkvPreStep(aPhotInfo->
                                                       ChTrackCkvPrestep());
            CurRichPEInfo->setMotherofPhotonCkvPostStep(aPhotInfo->
                                                        ChTrackCkvPoststep());

            CurRichPEInfo->setPhotonRayleighScatteringFlag(aPhotInfo->
                                                           PhotonRayleighScatFlag() );


            CurRichPEInfo->setPhotonAerogelExitPos ( aPhotInfo->
                                                     AerogelExitPosition() );
            
      	    CurRichPEInfo->setMirror1PhotReflPosition(aPhotInfo->Mirror1ReflPosition() );
	          CurRichPEInfo->setMirror2PhotReflPosition(aPhotInfo->Mirror2ReflPosition() );
	          CurRichPEInfo->setMirror1PhotDetCopyNum(aPhotInfo->Mirror1DetCopyNum());
	          CurRichPEInfo->setMirror2PhotDetCopyNum(aPhotInfo->Mirror2DetCopyNum());
            CurRichPEInfo->setHpdQWExtPhotIncidentPosition(aPhotInfo->HpdQWPhotIncidentPosition() );


          }


        }
      }
    }
  }

  CurRichPEInfo ->setHpdPeLocalOriginPosition(aLocalElectronOrigin);
  CurRichPEInfo-> setPhotonEmisPoint(aPhotonTk.GetVertexPosition());
  CurRichPEInfo->  setMotherOfPhotonId(aPhotonTk.GetParentID());
  CurRichPEInfo-> setOptPhotonId(aPhotonTk.GetTrackID());
  CurRichPEInfo-> setPhotoElectricFlag(1);
  
  // G4cout<<" RichPEInfo attach: Trackid Chtk photon  "
  //      <<aPhotonTk.GetParentID()<<"   "<<aPhotonTk.GetTrackID()
  //      <<G4endl;

  RichInfo* aPETypeRichInfo = new  RichInfo( CurRichPEInfo);

  // check if GaussTrackInformation already exists and if not
  // attach it to the track
  G4VUserTrackInformation* uinf = aPETk->GetUserInformation();
  GaussTrackInformation* aRichPETrackInfo;

  if(uinf){
    aRichPETrackInfo = (GaussTrackInformation*) uinf;
  }else{
    aRichPETrackInfo = new GaussTrackInformation();
  }  
    aRichPETrackInfo->setDetInfo(aPETypeRichInfo);
    aPETk->SetUserInformation(aRichPETrackInfo);
  
  //

  //  GaussTrackInformation* aRichPETrackInfo =
  //    new  GaussTrackInformation();
  //  aPETk->SetUserInformation(aRichPETrackInfo);

  //  aRichPETrackInfo->setRichInfo(aPETypeRichInfo);
  return aPETk;
}

G4Track* RichPEBackScatAttach(const G4Track& aElectronTk, G4Track* aPEBackScatTk) 
{
    G4VUserTrackInformation* aTkInfo=aElectronTk.GetUserInformation();
    GaussTrackInformation*  aRichPETrackInfo =(GaussTrackInformation*)aTkInfo;

    RichPEInfo* CurRichPEBackScatInfo = new RichPEInfo();

    // if( ( (aElectronTk->GetDefinition() == G4Electron::Electron()) ||
    //    (aTrack->GetDefinition() == RichPhotoElectron::PhotoElectron()))  &&
    //  (aCreatorProcessName  == "RichHpdPhotoelectricProcess")) {


    if(aRichPETrackInfo){
      if(aRichPETrackInfo->detInfo()){
        RichInfo* aRichPETypeInfo = (RichInfo*)(aRichPETrackInfo->detInfo());
        if(aRichPETypeInfo && aRichPETypeInfo->HasUserPEInfo())
        {
          RichPEInfo* aPEInfo=aRichPETypeInfo->RichPEInformation();
          if( aPEInfo)
          {
           

          CurRichPEBackScatInfo->
            setMotherOfPhotonPDGcode(aPEInfo-> MotherOfPhotonPDGcode());

          CurRichPEBackScatInfo->
            setVerbosePeTagFlag(  aPEInfo->  VerbosePeTagFlag());

          CurRichPEBackScatInfo->
            setPhotonEnergyAtCkvProd( aPEInfo->PhotonEnergyAtCkvProd());
          CurRichPEBackScatInfo->
            setCherenkovThetaAtProd(  aPEInfo->CherenkovThetaAtProd());
          CurRichPEBackScatInfo->
            setCherenkovPhiAtProd(  aPEInfo->CherenkovPhiAtProd());
          CurRichPEBackScatInfo->
            setMotherofPhotonMomAtProd( aPEInfo-> MotherofPhotonMomAtProd() );
          CurRichPEBackScatInfo->  
                     setPhotOriginRadiatorNumber( aPEInfo->  PhotOriginRadiatorNumber());

          // fill the following only for verbose tag mode.
          //if( aPEInfo->  VerbosePeTagFlag() ) {

            CurRichPEBackScatInfo->
              setMotherofPhotonMomVectAtProd
              ( aPEInfo->  MotherofPhotonMomVectAtProd() );
            CurRichPEBackScatInfo->setMotherofPhotonPDGMass( aPEInfo-> MotherofPhotonPDGMass());
            CurRichPEBackScatInfo->setMotherofPhotonCkvPreStep( aPEInfo->MotherofPhotonCkvPreStep());
            CurRichPEBackScatInfo->setMotherofPhotonCkvPostStep( aPEInfo->MotherofPhotonCkvPostStep());

            CurRichPEBackScatInfo->setPhotonRayleighScatteringFlag( aPEInfo->PhotonRayleighScatteringFlag());


            CurRichPEBackScatInfo->setPhotonAerogelExitPos ( aPEInfo-> PhotonAerogelExitPos());
            
      	    CurRichPEBackScatInfo->setMirror1PhotReflPosition( aPEInfo->Mirror1PhotReflPosition() );
	    CurRichPEBackScatInfo->setMirror2PhotReflPosition(aPEInfo->Mirror2PhotReflPosition());
	    CurRichPEBackScatInfo->setMirror1PhotDetCopyNum( aPEInfo->Mirror1PhotDetCopyNum());
	    CurRichPEBackScatInfo->setMirror2PhotDetCopyNum( aPEInfo->Mirror2PhotDetCopyNum());
      CurRichPEBackScatInfo->setHpdPhotonReflectionFlag(aPEInfo->HpdPhotonReflectionFlag());
      //          }
          

           CurRichPEBackScatInfo->  setPhotonEmisPoint( aPEInfo->PhotonEmisPoint());
           CurRichPEBackScatInfo->  setMotherOfPhotonId( aPEInfo->MotherOfPhotonId());
           CurRichPEBackScatInfo->  setOptPhotonId( aPEInfo->OptPhotonId());
            
            
          }
        }
      }
    }
    

    CurRichPEBackScatInfo-> bumpBackscatteredFlag();
    G4int aphElectricFlag = CurRichPEBackScatInfo-> PhotoElectricFlag();
    if(aphElectricFlag > 0) {  CurRichPEBackScatInfo-> setPhotoElectricFlag(2);}
    
      

    RichInfo* aPEBackScatTypeRichInfo = new  RichInfo(CurRichPEBackScatInfo );


  // check if GaussTrackInformation already exists and if not
  // attach it to the track
  G4VUserTrackInformation* uinf = aPEBackScatTk->GetUserInformation();
  GaussTrackInformation* aRichPEBackScatTrackInfo;

  if(uinf){
    aRichPEBackScatTrackInfo = (GaussTrackInformation*) uinf;
  }else{
    aRichPEBackScatTrackInfo = new GaussTrackInformation();
  }  
    aRichPEBackScatTrackInfo->setDetInfo(aPEBackScatTypeRichInfo );
    aPEBackScatTk->SetUserInformation(aRichPEBackScatTrackInfo);
  
    return aPEBackScatTk;
    
}





