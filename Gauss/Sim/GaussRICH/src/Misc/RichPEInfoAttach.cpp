#include "RichPEInfoAttach.h"
#include "GaussTools/GaussTrackInformation.h"
#include "RichInfo.h"
#include "RichPhotInfo.h"
#include "RichPEInfo.h"
#include "RichG4AnalysisConstGauss.h"

G4Track* RichPEInfoAttach(const G4Track& aPhotonTk, G4Track* aPETk)
{
  G4VUserTrackInformation* aTkInfo=aPhotonTk.GetUserInformation();
  GaussTrackInformation* CurRichPhotTrackInfo=(GaussTrackInformation*)aTkInfo;

  // the test on photon origin is commented out for now. It
  // can be revived in the future.

  G4double CurPhotonOrigZcoord= aPhotonTk.GetVertexPosition().z();
  //    if(( CurPhotonOrigZcoord >= ZUpsRich1Analysis &&
  //     CurPhotonOrigZcoord <= ZDnsRich1Analysis) ||
  //     ( CurPhotonOrigZcoord >= ZUpsRich2Analysis &&
  //     CurPhotonOrigZcoord <= ZDnsRich2Analysis) ) {

  G4int CurRadiatorNumber=-1;
  if(CurPhotonOrigZcoord >=AgelZBeginAnalysis &&
     CurPhotonOrigZcoord <= AgelZEndAnalysis) {
    CurRadiatorNumber=0;
  }else if(CurPhotonOrigZcoord >= C4F10ZBeginAnalysis &&
           CurPhotonOrigZcoord <= C4F10ZEndAnalysis) {
    CurRadiatorNumber=1;
  }else if(CurPhotonOrigZcoord >= CF4ZBeginAnalysis &&
           CurPhotonOrigZcoord <= CF4ZEndAnalysis) {
    CurRadiatorNumber=2;
  }

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

          }


        }
      }
    }
  }

  CurRichPEInfo-> setPhotonEmisPoint(aPhotonTk.GetVertexPosition());
  CurRichPEInfo->  setMotherOfPhotonId(aPhotonTk.GetParentID());
  CurRichPEInfo->  setPhotOriginRadiatorNumber(CurRadiatorNumber);
  CurRichPEInfo-> setOptPhotonId(aPhotonTk.GetTrackID());

  // G4cout<<" RichPEInfo attach: Trackid Chtk photon  "
  //      <<aPhotonTk.GetParentID()<<"   "<<aPhotonTk.GetTrackID()
  //      <<G4endl;

  RichInfo* aPETypeRichInfo = new  RichInfo( CurRichPEInfo);

  // check if GaussTrackInformation already exists and if not
  // attach it to the track
  G4VUserTrackInformation* uinf = aPETk->GetUserInformation();
  GaussTrackInformation* aRichPETrackInfo;

  if(uinf)
  {
    aRichPETrackInfo = (GaussTrackInformation*) uinf;
  }
  else
  {
    aRichPETrackInfo = new GaussTrackInformation();
    aRichPETrackInfo->setDetInfo(aPETypeRichInfo);
    aPETk->SetUserInformation(aRichPETrackInfo);
  }
  //

  //  GaussTrackInformation* aRichPETrackInfo =
  //    new  GaussTrackInformation();
  //  aPETk->SetUserInformation(aRichPETrackInfo);

  //  aRichPETrackInfo->setRichInfo(aPETypeRichInfo);
  return aPETk;
}




