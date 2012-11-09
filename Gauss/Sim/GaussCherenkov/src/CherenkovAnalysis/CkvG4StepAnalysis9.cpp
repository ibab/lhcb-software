// $Id: $
// Include files 

#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4DynamicParticle.hh"
#include "G4Material.hh"
#include "G4Electron.hh"
#include "G4OpticalPhoton.hh"
#include "G4PionMinus.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Step.hh"
#include "G4VProcess.hh"

/// GaudiKernel
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
// GiGa
#include <math.h>
#include "GaussTools/GaussTrackInformation.h"


// local
#include "CkvG4StepAnalysis9.h"
#include "GaussRICH/RichG4GaussPathNames.h"
#include "GaussCherenkov/CkvG4GaussPathNames.h"
#include "GaussRICH/RichG4AnalysisConstGauss.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CkvG4StepAnalysis9
//
// 2011-03-23 : Sajan Easo
//-----------------------------------------------------------------------------
DECLARE_TOOL_FACTORY(CkvG4StepAnalysis9);

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CkvG4StepAnalysis9::CkvG4StepAnalysis9(const std::string& type   ,
                   const std::string& name   ,const IInterface*  parent   ) 
:GiGaStepActionBase ( type , name , parent ) {

}
//=============================================================================
// Destructor
//=============================================================================
CkvG4StepAnalysis9::~CkvG4StepAnalysis9() {} 

//=============================================================================

void CkvG4StepAnalysis9::UserSteppingAction( const G4Step* aStep ) {
  
  G4StepPoint* aPreStepPoint = aStep->GetPreStepPoint();
  G4StepPoint* aPostStepPoint = aStep->GetPostStepPoint();

  const G4Track* aTrack= aStep ->GetTrack();

  const G4DynamicParticle* aParticle=aTrack->GetDynamicParticle();
  const G4double  aParticleKE= aParticle->GetKineticEnergy();

  // Check if this is an optical photon.
  if(aParticle->GetDefinition() == G4OpticalPhoton::OpticalPhoton() ) {
  
    if(   aParticleKE > 0.0 ) {

      const G4ThreeVector & prePos = aPreStepPoint->GetPosition();

      const G4ThreeVector & postPos = aPostStepPoint->GetPosition();

      if( ( prePos.z() >= ZUpsRich1Analysis && 
            prePos.z() <= ZDnsRich1Analysis ) || 
          ( prePos.z() <= ZDnsRich2Analysis && 
            prePos.z() >=  ZUpsRich2Analysis  ))
         {

          
        const G4String & aPreVolName=
                              aPreStepPoint->GetPhysicalVolume()->
                              GetLogicalVolume()->GetName();
         const G4String & aPostVolName=
                              aPostStepPoint->GetPhysicalVolume()->
                              GetLogicalVolume()->GetName();
 
        int aCurrentCopyNum=  aPostStepPoint->GetPhysicalVolume()->GetCopyNo();        
        int aCurrentCopyNumPreVol=  aPreStepPoint->GetPhysicalVolume()->GetCopyNo();        

        G4cout<<" CkvStepAnalysis9  PreVol PreVolCopy PostVol PostVolCopy "<<
          aPreVolName<<"  "<< aCurrentCopyNumPreVol<<"   "<<aPostVolName<<"   "<<
          aCurrentCopyNum<<G4endl;
        G4cout<<"CkvStepAna9 XYZ PrePos  PostPos  "<< prePos.x()<<"   "<<prePos.y()<<"  "<< prePos.z()
              <<"        "<<postPos.x()<<"  "<< postPos.y()<<"   "<<postPos.z()<<G4endl;
        
 
         }    }  }
  
    

  
}


