#include "RichPEInfoAttach.h"
#include "GaussTools/GaussTrackInformation.h"
#include "RichInfo.h"
#include "RichPhotInfo.h"
#include "RichPEInfo.h"
#include "RichG4AnalysisConstGauss.h"

G4Track* RichPEInfoAttach(const G4Track& aPhotonTk, G4Track* aPETk) 
{
  G4VUserTrackInformation* aTkInfo=aPhotonTk.GetUserInformation();
  GaussTrackInformation* CurRichPhotTrackInfo=
    (0 ==aTkInfo ) ? 0 : 
                   dynamic_cast<GaussTrackInformation*>(aTkInfo);
  
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
    if(CurRichPhotTrackInfo->richInfo()){                 
      RichInfo* aPhRichInfo= CurRichPhotTrackInfo->richInfo();
      if( aPhRichInfo !=0 && aPhRichInfo-> HasUserPhotInfo() ) {
        
        RichPhotInfo* aPhotInfo = 
          aPhRichInfo->RichPhotInformation() ;
        
        if(aPhotInfo != 0 ) {	  
          CurRichPEInfo->
            setMotherOfPhotonPDGcode(aPhotInfo-> MotherChTrackPDGcode());
          
          CurRichPEInfo-> 
            setPhotonEnergyAtCkvProd(aPhotInfo->CkvPhotonEnergyAtProd());
          CurRichPEInfo-> 
            setCherenkovThetaAtProd(aPhotInfo->CkvAngleThetaAtProd());
          CurRichPEInfo->
            setCherenkovPhiAtProd(aPhotInfo->CkvAnglePhiAtProd());
          CurRichPEInfo->
            setMotherofPhotonMomAtProd(aPhotInfo->ChTrackMomAtProd() );
          CurRichPEInfo->
            setMotherofPhotonMomVectAtProd
            (aPhotInfo->ChTrackMomentumVector() );
                    CurRichPEInfo->
                      setMotherofPhotonPDGMass(aPhotInfo->
                                       MotherChTrackPDGMass());
                    
 
                  
        } 
      } 
    }  
  }
  
  CurRichPEInfo-> setPhotonEmisPoint(aPhotonTk.GetVertexPosition());
  CurRichPEInfo->  setMotherOfPhotonId(aPhotonTk.GetParentID());
  CurRichPEInfo->  setPhotOriginRadiatorNumber(CurRadiatorNumber);
  
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
      aRichPETrackInfo->setRichInfo(aPETypeRichInfo);
      aPETk->SetUserInformation(aRichPETrackInfo);
    }    
  //
  
  //  GaussTrackInformation* aRichPETrackInfo = 
  //    new  GaussTrackInformation();
  //  aPETk->SetUserInformation(aRichPETrackInfo);
  
  //  aRichPETrackInfo->setRichInfo(aPETypeRichInfo);
  return aPETk;
}




