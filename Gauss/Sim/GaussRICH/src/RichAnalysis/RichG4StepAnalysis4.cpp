// $Id: RichG4StepAnalysis4.cpp,v 1.3 2004-06-03 12:44:00 seaso Exp $
// Include files


#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4DynamicParticle.hh"
#include "G4Material.hh"
#include "G4OpticalPhoton.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Step.hh"
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
// GiGa
#include "GiGa/GiGaMACROs.h"
#include <math.h>
#include "GaussTools/GaussTrackInformation.h"

// local
#include "RichG4StepAnalysis4.h"
#include "RichG4AnalysisConstGauss.h"
#include "RichG4AgelExitTag.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4StepAnalysis4
//
// 2003-10-29 : Sajan EASO
//-----------------------------------------------------------------------------
IMPLEMENT_GiGaFactory(RichG4StepAnalysis4);

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4StepAnalysis4::RichG4StepAnalysis4
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaStepActionBase ( type , name , parent ) {


};
RichG4StepAnalysis4::~RichG4StepAnalysis4(  ) { }
void RichG4StepAnalysis4::UserSteppingAction( const G4Step* aStep )
{

  //     G4cout<<"Now in Rich Step Analysis 4 " <<G4endl;

  G4StepPoint* aPreStepPoint = aStep->GetPreStepPoint();
  G4StepPoint* aPostStepPoint = aStep->GetPostStepPoint();
  if(aPostStepPoint->GetStepStatus() == fGeomBoundary) {
    const G4Track* aTrack= aStep ->GetTrack();
    const G4DynamicParticle* aParticle=aTrack->GetDynamicParticle();
    const G4double  aParticleKE= aParticle->GetKineticEnergy();
    const G4ThreeVector & prePos=aPreStepPoint->GetPosition();

    if(aParticle->GetDefinition() == G4OpticalPhoton::OpticalPhoton() ) {

      if(   aParticleKE > 0.0 ) {
        if( prePos.z() >= ZUpsRich1Analysis &&
            prePos.z() <= ZDnsRich1Analysis  )
        {

          if (prePos.z()  >=  AgelZBeginAnalysis &&
              prePos.z()  <=  AgelZEndAnalysis &&
              prePos.x()  >=  AgelXBeginAnalysis &&
              prePos.x()  <=  AgelXEndAnalysis &&
              prePos.y()  >=  AgelYBeginAnalysis &&
              prePos.y()  <=  AgelYEndAnalysis){


            G4String aPreVolName= aPreStepPoint->GetPhysicalVolume()
              ->GetLogicalVolume()->GetName();
            G4String aPostVolName= aPostStepPoint->GetPhysicalVolume()
              ->GetLogicalVolume()->GetName();
            G4String  aPreVolNameA =std::string(aPreVolName,0,33);
            if(aPreVolNameA ==  LogVolAgelNameAnalysis &&
               aPostVolName ==  LogVolC4F10NameAnalysis) {
              const G4ThreeVector & aPhotProdPos = aTrack->  GetVertexPosition();

              if (aPhotProdPos.z()  >=  AgelZBeginAnalysis &&
                  aPhotProdPos.z()  <=  AgelZEndAnalysis &&
                  aPhotProdPos.x()  >=  AgelXBeginAnalysis &&
                  aPhotProdPos.x()  <=  AgelXEndAnalysis &&
                  aPhotProdPos.y()  >=  AgelYBeginAnalysis &&
                  aPhotProdPos.y()  <=  AgelYEndAnalysis){

                // now store the exit point from aerogel into c4f10
                // in the photon user track info.

                const G4ThreeVector & aPhotAgelExitPos =
                  aPostStepPoint->GetPosition();
                //               G4cout<<"Now in Rich Step Analysis 4 at Agel exit "
                //      <<aPhotAgelExitPos.x()<<"  "
                //      <<aPhotAgelExitPos.y()<<"   "
                //      << aPhotAgelExitPos.z() <<G4endl;

                RichG4AgelExitTag(aTrack,aPhotAgelExitPos);

              }


            }
          }

        }


      }
    }
  }
  //      G4cout<<"Now end of Rich Step Analysis 4 " <<G4endl;

}

//=============================================================================
