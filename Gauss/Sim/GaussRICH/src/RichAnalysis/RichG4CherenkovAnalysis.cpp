#include "RichG4CherenkovAnalysis.h"
#include "RichG4SvcLocator.h"
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
#include "GaudiKernel/ITime.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/Bootstrap.h"
// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include <math.h>

void RichG4CherenkovAnalysis1(const G4Step& aStep, G4double CosThCkv,
                              G4double SinThCkv, G4double PhotProdCkvKE , 
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
    G4String pPreVolMatNameCF4=std::string( prePosMaterialName,0,31);

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
       pPreVolMatNameC4F10 == C4F10MaterialName &&
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
       pPreVolMatNameCF4 == CF4MaterialName &&
       prePos.z() >= CF4ZBeginAnalysis &&  prePos.z() <=  CF4ZEndAnalysis ) {
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
  const G4ThreeVector & cpostPos = cPostStepPoint->GetPosition();

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

          const G4ThreeVector & PhotOriginDir=
            cTrack->GetVertexMomentumDirection();
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





