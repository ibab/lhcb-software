
#include "GaussTools/GaussTrackInformation.h"
#include "RichInfo.h"
#include "RichPhotInfo.h"
#include "RichG4AnalysisConstGauss.h"
#include <math.h>
#include "G4ThreeVector.hh"
#include "G4ParticleDefinition.hh"
#include "G4DynamicParticle.hh"

G4Track* RichG4CherenkovPhotProdTag(const G4Track& aChTrack,
                                    G4Track* aCkvPhotTrack,
                                    const G4double  CkvCosTheta,
                                    const G4double CkvPhi,
                                    const G4double CkvPhotEnergy ){
  // The test on the Z coord is commented out for now.
  // it can be uncommented in the future.
  //    G4double PhotonOrigZcoord= aCkvPhotTrack->GetVertexPosition().z();
    //    if((   PhotonOrigZcoord >= ZUpsRich1Analysis 
    //       && PhotonOrigZcoord <= ZDnsRich1Analysis) ||
    //      ( PhotonOrigZcoord >= ZUpsRich2Analysis 
    //      && PhotonOrigZcoord <= ZDnsRich2Analysis) ) {
    //          RichGiGaPhotTrackInformation* aRichPhotTrackInfo 
    //                =new RichGiGaPhotTrackInformation();
    //          RichPhotInfo* aPhotInfo=aRichPhotTrackInfo->
    //          AttachedRichPhotInfo();
              
          const G4DynamicParticle* aChTrackParticle 
                        = aChTrack.GetDynamicParticle(); 

          RichPhotInfo* aPhotInfo = new  RichPhotInfo();
          aPhotInfo->
                setMotherChTrackPDGcode(aChTrackParticle->
                    GetDefinition()->GetPDGEncoding());
          aPhotInfo->setCkvPhotonEnergyAtProd(CkvPhotEnergy);
          aPhotInfo->setCkvAngleThetaAtProd(acos(CkvCosTheta));
          aPhotInfo->setCkvAnglePhiAtProd(CkvPhi);
          //    G4double CurChTrakMom = 
          //       aChTrackParticle->GetTotalMomentum();
          // aPhotInfo->setChTrackMomAtProd( CurChTrakMom);

          aPhotInfo->
           setChTrackMomAtProd(aChTrackParticle->GetTotalMomentum());
          aPhotInfo->
           setChTrackMomentumVector(aChTrackParticle->GetMomentum());


          RichInfo* aCkvPhotonTypeRichInfo = new   RichInfo(aPhotInfo);
          GaussTrackInformation* aRichPhotTrackInfo 
                =new  GaussTrackInformation;
          aRichPhotTrackInfo-> setRichInfo(aCkvPhotonTypeRichInfo);
          
          aCkvPhotTrack->SetUserInformation(aRichPhotTrackInfo);
          
          //    }
return aCkvPhotTrack; 
}
