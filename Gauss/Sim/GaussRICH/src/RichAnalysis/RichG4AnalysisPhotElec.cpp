// $Id: RichG4AnalysisPhotElec.cpp,v 1.2 2004-02-10 14:24:06 jonesc Exp $
// Include files



// local
#include "RichG4AnalysisConstGauss.h"
#include "RichG4AnalysisPhotElec.h"
#include "RichG4Counters.h"
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

#include <math.h>
//-----------------------------------------------------------------------------
//
// 2003-06-08 : Sajan EASO
//-----------------------------------------------------------------------------
void RichG4AnalysisPhotElecA ( const G4Step& aStep,
                               G4int currentRichDetNumber,
                               G4int currentHpdNumber, 
                               G4double PhotonEnergy )
{
  // Histogram and analysis from befor the QE.


  RichG4Counters* aRichCounter =  RichG4Counters::getInstance();

  if(currentRichDetNumber == 0) {

    const G4Track* bTrack= aStep.GetTrack();
    const G4DynamicParticle* bParticle=bTrack->GetDynamicParticle();
    const G4double  bParticleKE= bParticle->GetKineticEnergy();
    if(    bParticleKE > 0.0 ) {

      G4int aRadiatorNumber = -1;

      const G4ThreeVector & bPhotProdPos =
        bTrack->GetVertexPosition();
      if( bPhotProdPos.z()  >=  C4F10ZBeginAnalysis &&
          bPhotProdPos.z() <=  C4F10ZEndAnalysis ) {

        aRadiatorNumber=1;


      }else if (bPhotProdPos.z()  >=  AgelZBeginAnalysis &&
                bPhotProdPos.z()  <=  AgelZEndAnalysis &&
                bPhotProdPos.x()  >=  AgelXBeginAnalysis &&
                bPhotProdPos.x()  <=  AgelXEndAnalysis &&
                bPhotProdPos.y()  >=  AgelYBeginAnalysis &&
                bPhotProdPos.y()  <=  AgelYEndAnalysis){

        aRadiatorNumber=0;
      }

      //             G4cout<<"RichPhotElecA RadiatorNum "<<aRadiatorNumber <<G4endl;

      if(  aRichCounter ) {

        if(    aRadiatorNumber == 1 ) {

          aRichCounter->bumpNumPhotGasRich1BeforeQE();
        }else if ( aRadiatorNumber == 0 ) {

          aRichCounter->bumpNumPhotAgelRich1BeforeQE();

        }


      }





    }

  }

}

void RichG4AnalysisPhotElecB ( const G4Step& aStep,
                               G4int currentRichDetNumber,
                               G4int currentHpdNumber, 
                               G4double PhotonEnergy )
{
  // Histogram and analysis from after the QE.


  RichG4Counters* aRichCounter =  RichG4Counters::getInstance();


  if(currentRichDetNumber == 0) {

    const G4Track* bTrack= aStep.GetTrack();
    const G4DynamicParticle* bParticle=bTrack->GetDynamicParticle();
    const G4double  bParticleKE= bParticle->GetKineticEnergy();
    if(    bParticleKE > 0.0 ) {

      G4int aRadiatorNumber = -1;

      const G4ThreeVector & bPhotProdPos = bTrack->GetVertexPosition();
      if( bPhotProdPos.z()  >=  C4F10ZBeginAnalysis &&
          bPhotProdPos.z() <=  C4F10ZEndAnalysis ) {
        aRadiatorNumber = 1;
      }else if (bPhotProdPos.z()  >=  AgelZBeginAnalysis &&
                bPhotProdPos.z()  <=  AgelZEndAnalysis &&
                bPhotProdPos.x()  >=  AgelXBeginAnalysis &&
                bPhotProdPos.x()  <=  AgelXEndAnalysis &&
                bPhotProdPos.y()  >=  AgelYBeginAnalysis &&
                bPhotProdPos.y()  <=  AgelYEndAnalysis){

        aRadiatorNumber=0;
      }

      if(  aRichCounter ) {

        if(  aRadiatorNumber == 1 ) {

          aRichCounter->bumpNumPhotGasRich1AfterQE();
        } else if (  aRadiatorNumber == 0 ) {

          aRichCounter->bumpNumPhotAgelRich1AfterQE();

        }


      }


    }

  }

}



//=============================================================================
