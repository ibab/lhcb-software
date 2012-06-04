// $Id: $
// Include files 

#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4DynamicParticle.hh"
#include "G4Material.hh"
#include "G4OpticalPhoton.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Step.hh"
#include "GaudiKernel/DeclareFactoryEntries.h"
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
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include <math.h>
#include "GaussTools/GaussTrackInformation.h"
#include "TorchTBGaussPathNames.h"

// local
#include "TorchTBBasicStepAnalysis.h"

DECLARE_TOOL_FACTORY(TorchTBBasicStepAnalysis);

//-----------------------------------------------------------------------------
// Implementation file for class : TorchTBBasicStepAnalysis
//
// 2012-05-30 : Sajan Easo
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TorchTBBasicStepAnalysis::TorchTBBasicStepAnalysis
 (const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent  ): GiGaStepActionBase ( type , name , parent ) {


}
//=============================================================================
// Destructor
//=============================================================================
TorchTBBasicStepAnalysis::~TorchTBBasicStepAnalysis() {} 

//=============================================================================
void TorchTBBasicStepAnalysis::UserSteppingAction( const G4Step* aStep ) {
  G4StepPoint* aPreStepPoint = aStep->GetPreStepPoint();
  G4StepPoint* aPostStepPoint = aStep->GetPostStepPoint();

    const G4Track* aTrack= aStep ->GetTrack();
    const G4DynamicParticle* aParticle=aTrack->GetDynamicParticle();
    const G4double  aParticleKE= aParticle->GetKineticEnergy();
    G4cout<<" Now in TorchTBBasic Step Analysis "<<G4endl;
    G4cout<<" Particle name KineticEnergy "<<aParticle->GetDefinition()->GetParticleName()<<"   "
          << aParticleKE  <<G4endl;
    if(   aParticleKE > 40000.0 || (aParticle->GetDefinition() == G4OpticalPhoton::OpticalPhoton())  ) {

    if(aPostStepPoint->GetStepStatus() == fGeomBoundary ) {
    if(aPostStepPoint->GetPhysicalVolume() &&
       aPreStepPoint->GetPhysicalVolume()){
       G4cout<<" Now at Geom boundary "<<G4endl;
   const G4ThreeVector & prePos=aPreStepPoint->GetPosition();
     const G4ThreeVector & postPos=aPostStepPoint->GetPosition();
     const G4String & aPreVolName= aPreStepPoint->GetPhysicalVolume()->
                              GetLogicalVolume()->GetName();
     const G4String & aPostVolName=aPostStepPoint->GetPhysicalVolume()->
                              GetLogicalVolume()->GetName();

     if( prePos.z() < 12000.0) {
       
     G4cout<<" Pre Post Vol names "<<  aPreVolName<<"   "<<  aPostVolName<<G4endl;



            const G4ThreeVector & PhotCurDir = aTrack->GetMomentumDirection();
            G4cout<<" Particle PrePos PostPos CurDir "<<prePos<<"  "<<postPos<<"   "
              <<PhotCurDir<<G4endl;

       if(aParticle->GetDefinition() == G4OpticalPhoton::OpticalPhoton() ) {

             G4cout<<" Photon PrePos PostPos CurDir "<<prePos<<"  "<<postPos<<"   "
              <<PhotCurDir<<G4endl;
       
            
       }
       
     }
     
    
    }}}
}
