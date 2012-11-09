#include "RichG4CherenkovAnalysis.h"
#include "GaussRICH/RichG4SvcLocator.h"
#include "GaussRICH/RichG4MatRadIdentifier.h"
#include "GaussRICH/RichG4RadiatorMaterialIdValues.h"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4DynamicParticle.hh"
#include "G4Material.hh"
#include "G4Electron.hh"
#include "G4OpticalPhoton.hh"
#include "G4PionMinus.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
/// GaudiKernel
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IValidity.h"
#include "GaudiKernel/Time.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/INTuple.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/Bootstrap.h"


// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include <math.h>

void RichG4CherenkovAnalysis1(const G4Step& aStep, G4double CosThCkv,
                              G4double PhotProdCkvKE , 
                              const G4ThreeVector & PhotProdPosition,
                              G4double RefInd, G4double BetaInvChPart ) {

  G4double BetaInvCut=1.01;
  G4double energycut=30.0;
  G4double energyAgelcut=3.0;

  G4double chtken=  aStep.GetTrack()->GetTotalEnergy();

  const G4StepPoint* aPreStepPoint = aStep.GetPreStepPoint();
  // G4StepPoint* aPostStepPoint = aStep.GetPostStepPoint();
  const G4ThreeVector & prePos = aPreStepPoint->GetPosition();

  // const G4ThreeVector postPos=aPostStepPoint->GetPosition();
  //Check to see if we are in RICH1.
  if(prePos.z() >= ZUpsRich1Analysis || prePos.z() <= ZDnsRich1Analysis ){

    G4String prePosMaterialName=aPreStepPoint->GetPhysicalVolume()->
      GetLogicalVolume()->GetMaterial()->GetName();
    G4String pPreVolName=
      aPreStepPoint->GetPhysicalVolume()->GetLogicalVolume()->GetName();
    // G4String pPostVolName=
    // aPostStepPoint->GetPhysicalVolume()->GetLogicalVolume()->GetName();
    IHistogramSvc* CurrentHistoSvc=RichG4SvcLocator::RichG4HistoSvc();
    G4double ChrAngl=acos(CosThCkv);
    G4String pPreVolNameAgel =std::string(pPreVolName,0,33);
    G4String pPreVolNameC4F10=std::string(pPreVolName,0,35);
    G4String pPreVolNameCF4=std::string(pPreVolName,0,29);
    G4String pPreVolMatNameAgel=std::string( prePosMaterialName,0,35);
    G4String pPreVolMatNameC4F10=std::string( prePosMaterialName,0,33);
    G4String pPreVolMatNameRich1Gas=std::string( prePosMaterialName,0,41);
    G4String pPreVolMatNameCF4=std::string( prePosMaterialName,0,31);
    G4String pPreVolMatNameRich2Gas=std::string( prePosMaterialName,0,41);

    SmartDataPtr<IHistogram1D>hCkvZEmissionPtRich1(CurrentHistoSvc,"RICHG4HISTOSET1/25");
    if(hCkvZEmissionPtRich1) hCkvZEmissionPtRich1->
                               fill(PhotProdPosition.z(),1.0);

    G4double Photwavelen=PhotMomCnv/(PhotProdCkvKE*1000000);

    if(pPreVolNameAgel == LogVolAgelNameAnalysis  &&
       pPreVolMatNameAgel == AgelMaterialName &&
       prePos.z()>= AgelZBeginAnalysis &&  prePos.z() <= AgelZEndAnalysis ) {
      SmartDataPtr<IHistogram1D>hCkvAgelRich1(CurrentHistoSvc,"RICHG4HISTOSET1/10");
      if(BetaInvChPart < BetaInvCut ) {
        if(( chtken/GeV)  >  energyAgelcut) {

          if(hCkvAgelRich1) hCkvAgelRich1->fill( ChrAngl ,1.0);
        }

      }

      //          SmartDataPtr<IHistogram2D>hRefIndAgelRich1(CurrentHistoSvc,
      //                                    "RICHG4HISTOSET1/12");
      //  if(hRefIndAgelRich1)hRefIndAgelRich1->fill(Photwavelen,(RefInd-1.0),1.0);

    }

    if(pPreVolNameC4F10 == LogVolC4F10NameAnalysis &&
       ( ( pPreVolMatNameC4F10 == C4F10MaterialName)||( pPreVolMatNameC4F10 == C3F8MaterialName) ||
         ( pPreVolMatNameRich1Gas == R1RadGasMaterialName)   ) &&
       prePos.z() >= C4F10ZBeginAnalysis &&
       prePos.z() <=  C4F10ZEndAnalysis ) {
      SmartDataPtr<IHistogram1D>hCkvC4F10Rich1(CurrentHistoSvc,"RICHG4HISTOSET1/20");
      if(BetaInvChPart < BetaInvCut ) {
        if(( chtken/GeV)  >  energycut) {

          if(hCkvC4F10Rich1)hCkvC4F10Rich1->fill( ChrAngl,1.0);
        }

      }

      //   SmartDataPtr<IHistogram2D>hRefIndC4F10Rich1(CurrentHistoSvc,
      //                              "RICHG4HISTOSET1/22");
      // if(hRefIndC4F10Rich1)hRefIndC4F10Rich1->fill(Photwavelen,(RefInd-1.0),1.0);

    }
    if(pPreVolNameCF4 == LogVolCF4NameAnalysis &&
       ( (pPreVolMatNameCF4 == CF4MaterialName) || (pPreVolMatNameRich2Gas == R2RadGasMaterialName) ) &&
       (prePos.z() >= CF4ZBeginAnalysis) &&  (prePos.z() <=  CF4ZEndAnalysis) ) {
      SmartDataPtr<IHistogram1D>hCkvCF4Rich2(CurrentHistoSvc,"RICHG4HISTOSET1/70");
      if(BetaInvChPart < BetaInvCut ) {
        if(( chtken/GeV)  >  energycut) {

          if(hCkvCF4Rich2)hCkvCF4Rich2->fill( ChrAngl,1.0);
        }

      }

      SmartDataPtr<IHistogram2D>hRefIndCF4Rich2(CurrentHistoSvc,
                                                "RICHG4HISTOSET1/72");
      if(hRefIndCF4Rich2)hRefIndCF4Rich2->fill(Photwavelen,
                                               (RefInd-1.0),1.0);

    }
  }
}
void RichG4CherenkovAnalysis2(const G4Step& cStep) {

  G4StepPoint* cPreStepPoint = cStep.GetPreStepPoint();
  G4StepPoint* cPostStepPoint = cStep.GetPostStepPoint();
  const G4ThreeVector & cprePos = cPreStepPoint->GetPosition();
  // const G4ThreeVector & cpostPos = cPostStepPoint->GetPosition();

  if(cprePos.z()>ZUpsRich1Analysis || cprePos.z() <ZDnsRich1Analysis ){
    G4Track* cTrack = cStep.GetTrack();
    const G4DynamicParticle* cParticle=cTrack->GetDynamicParticle();
    const G4double  cParticleKE= cParticle->GetKineticEnergy();
    // Check if this is an optical photon.
    if(cParticle->GetDefinition() == G4OpticalPhoton::OpticalPhoton()) {
      //Check to see if we are a Geometrical boundary.
      if(cPostStepPoint->GetStepStatus() == fGeomBoundary) {
        G4String cPreVolName=cPreStepPoint->GetPhysicalVolume()
          ->GetLogicalVolume()->GetName();
        G4String cPostVolName=cPostStepPoint->GetPhysicalVolume()
          ->GetLogicalVolume()->GetName();

        //Avoid photons which are already dead.
        if(cParticleKE > 0.0 ){
          G4double CurPhotWaveLen= PhotMomCnv/((cParticleKE/MeV)*MeVtoeV);
          const G4double PhotOriginZ= cTrack->GetVertexPosition().z();

          //          const G4ThreeVector & PhotOriginDir=
          //          cTrack->GetVertexMomentumDirection();

          const G4ThreeVector & PhotCurDir=
            cTrack->GetMomentumDirection();

          G4String cPostLogVolName1=std::string(cPostVolName,0,33);

          IHistogramSvc* CurrentHistoSvc=RichG4SvcLocator::RichG4HistoSvc();

          //now for the case just before incident on the Mirror1 of Rich1.
          if(cPreVolName == LogVolC4F10NameAnalysis &&
             cPostLogVolName1 == LogVolRich1Mirror1NameAnalysis ){
            // fill a histogram for photon wavelength before mirror1.
            // the reflection already happened at this point.
            if(PhotOriginZ > C4F10ZBeginAnalysis &&
               PhotOriginZ < C4F10ZEndAnalysis ) {
              if(PhotCurDir.z() > 0.0 ) {

                SmartDataPtr<IHistogram1D>hWaveLenBeforeRich1Mirror1
                  (CurrentHistoSvc,
                   "RICHG4HISTOSET1/40");
                if(hWaveLenBeforeRich1Mirror1)
                  hWaveLenBeforeRich1Mirror1->fill(CurPhotWaveLen,1.0);

              }

            }

          }

          // Now for the case just before incident on Quartz Window.

          G4String cPreLogVolName1=std::string(cPreVolName,0,31);

          if(cPreLogVolName1 == LogVolRich1MagShNameAnalysis &&
             cPostVolName == LogColRich1GasQWNameAnalysis ){
            if(PhotOriginZ > C4F10ZBeginAnalysis &&
               PhotOriginZ < C4F10ZEndAnalysis ) {
              if(PhotCurDir.z() > 0.0 ) {
                SmartDataPtr<IHistogram1D>hWaveLenBeforeRich1GasQW
                  (CurrentHistoSvc,"RICHG4HISTOSET1/50");
                if(hWaveLenBeforeRich1GasQW)
                  hWaveLenBeforeRich1GasQW->fill(CurPhotWaveLen,1.0);

              }
            }

          }


        }
      }
    }

  }
}
void RichG4CherenkovProdFeaturesHisto(const G4Track& aChTrack) {


   RichG4RadiatorMaterialIdValues* aRMIdValues= 
            RichG4RadiatorMaterialIdValues::RichG4RadiatorMaterialIdValuesInstance();

  //  INTupleSvc* CurrentNtupleSvc=RichG4SvcLocator::RichG4NtupleSvc();

  //    NTuple::Item <long> m_nPartQw ;
    
  //  int m_nPartMax=100000;
  //  NTuple::Array <float> m_ChTkProdX, m_ChTkProdY, m_ChTkProdZ ;

       
    IHistogramSvc* CurrentHistoSvc=RichG4SvcLocator::RichG4HistoSvc();


 
  //   NTuplePtr nt ( CurrentNtupleSvc , "/NTUPLES/FILE1/1001" ) ;
  //     if(! nt ) {
        
	 //        NTupleFilePtr file ( CurrentNtupleSvc , "/NTUPLES/FILERICH" ) ;
	 //if(file) {
  //	 m_nPartQw=0;
  //     	 nt = CurrentNtupleSvc ->book (  "/NTUPLES/FILE1/1001",
  //                                       CLID_ColumnWiseTuple ,"HpdQwNt");
  //	 StatusCode sc = nt->addItem("Npartqw", m_nPartQw, 0, m_nPartMax);
  //       sc = nt->addIndexedItem("chProdX", m_nPartQw,  m_ChTkProdX);
  //       sc = nt->addIndexedItem("chProdY", m_nPartQw,  m_ChTkProdY);
  //       sc = nt->addIndexedItem("chProdZ", m_nPartQw,  m_ChTkProdZ);


	 // }else {
	 //  G4cout <<"Not ntuple file pointer "<<G4endl;
	 //  }

  //    } else {
 
  //	 if( m_nPartQw <m_nPartMax ) { 

    SmartDataPtr<IHistogram2D> hHpdQwOrigXZR1 (CurrentHistoSvc,"RICHG4HISTOSET1/1001");
    SmartDataPtr<IHistogram2D> hHpdQwOrigXZR2 (CurrentHistoSvc,"RICHG4HISTOSET1/1002");
    SmartDataPtr<IHistogram2D> hHpdQwOrigYZR1 (CurrentHistoSvc,"RICHG4HISTOSET1/1010");
    SmartDataPtr<IHistogram2D> hHpdQwOrigYZR2 (CurrentHistoSvc,"RICHG4HISTOSET1/1011");
       
   RichG4MatRadIdentifier* aRichG4MatRadIdentifier =
                          RichG4MatRadIdentifier::RichG4MatRadIdentifierInstance();
   const G4DynamicParticle* aChTrackParticle
    = aChTrack.GetDynamicParticle();
 
   G4ThreeVector aCurPos = aChTrack.GetStep()->GetPreStepPoint()->GetPosition();

  
   G4int aRadiatorNum = aRichG4MatRadIdentifier->getRadiatorNumForG4MatIndex(aChTrack.GetMaterial()->GetIndex());
   if( ( aRadiatorNum == (aRMIdValues -> RichHpdQuartzWindowCkvRadiatorNum() ) )  || 
       ( aRadiatorNum == (aRMIdValues -> Rich1GasQWindowCkvRadiatorNum ()) )  ||
       ( aRadiatorNum == (aRMIdValues -> Rich2GasQWindowCkvRadiatorNum() ) ) ) {
    const G4ThreeVector& aChTrackProdPos = aChTrack.GetVertexPosition();
    //  const G4ThreeVector& aMomAtProd = aChTrack.GetVertexMomentumDirection();
    G4String aParticleName = aChTrackParticle->GetDefinition()->GetParticleName();
    // G4double aParticleEnergy = aChTrack.GetTotalEnergy() ;
    
     if( aCurPos.z() < ZDnsRich1Analysis ) {
       if(hHpdQwOrigXZR1) hHpdQwOrigXZR1->fill(aChTrackProdPos.z(),aChTrackProdPos.x(),1.0);
       if(hHpdQwOrigYZR1) hHpdQwOrigYZR1->fill(aChTrackProdPos.z(),aChTrackProdPos.y(),1.0);
       // we are in rich1
       // G4cout<<"rich1 cur track pos xyz "<<aCurPos.x()<<"  "<<aCurPos.y()<<"  "<<aCurPos.z()<<G4endl;

       //    m_ChTkProdX[ m_nPartQw ]= aCurPos.x();
       // m_ChTkProdY[ m_nPartQw ]= aCurPos.y();
       // m_ChTkProdY[ m_nPartQw ]= aCurPos.z();

       // we are in rich1
       //       G4cout<<"rich1 cur track pos xyz "<<aCurPos.x()<<"  "<<aCurPos.y()<<"  "<<aCurPos.z()<<G4endl;
       
       //   G4cout<<"rich1 RadNum ProdPos En XYZ PXYZ  "<<aRadiatorNum<<"  "<<aParticleName<<"  "<< aParticleEnergy<<"  "
       //      <<aChTrackProdPos.x()<<"  "<<aChTrackProdPos.y()
       //      <<"  "<<aChTrackProdPos.z()<< "  "<<aMomAtProd.x()<<"  "<<aMomAtProd.y()
       //      <<"  "<< aMomAtProd.z()<< G4endl;
       

     }else {

       if(hHpdQwOrigXZR2) hHpdQwOrigXZR2->fill(aChTrackProdPos.z(),aChTrackProdPos.x(),1.0);
       if(hHpdQwOrigYZR2) hHpdQwOrigYZR2->fill(aChTrackProdPos.z(),aChTrackProdPos.y(),1.0);

       // we are in rich2
       //       G4cout<<"rich2 aChTrackProdPos XYZ  "<<aRadiatorNum<<"  "<<aParticleName<<"  "<<aParticleEnergy<<"  "
       //     << aChTrackProdPos.x()<<"  "<<aChTrackProdPos.y()
       //     <<"  "<<aChTrackProdPos.z()<<aMomAtProd.x()<<"  "<<aMomAtProd.y()
       //     <<"  "<< aMomAtProd.z()<< G4endl;
     }
   }
   //	 m_nPartQw++;
   //	 }
}





