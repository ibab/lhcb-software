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
#include "G4TransportationManager.hh"
#include "G4Navigator.hh"

#include "GaudiKernel/AlgFactory.h"
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
#include "GaudiKernel/IToolSvc.h"


// GiGa
#include <math.h>
#include "GaussTools/GaussTrackInformation.h"
#include "GaussRICH/RichG4GaussPathNames.h"
#include "GaussCherenkov/CkvG4GaussPathNames.h"
#include "GaussCherenkov/CkvG4AnalysisConstGauss.h"

#include "GaussRICH/RichG4SvcLocator.h"

#include "AIDA/IHistogram1D.h"
#include "globals.hh"


// local
#include "CherenkovStepAnalysis7.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CherenkovStepAnalysis7
//
// 2012-06-10 : Sajan Easo
//-----------------------------------------------------------------------------
DECLARE_TOOL_FACTORY(CherenkovStepAnalysis7);

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CherenkovStepAnalysis7::CherenkovStepAnalysis7(const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaStepActionBase ( type , name , parent ) {


}
//=============================================================================
// Destructor
//=============================================================================
CherenkovStepAnalysis7::~CherenkovStepAnalysis7() {} 

//=============================================================================
void CherenkovStepAnalysis7::UserSteppingAction( const G4Step* aStep )
{

  G4StepPoint* aPreStepPoint = aStep->GetPreStepPoint();
  G4StepPoint* aPostStepPoint = aStep->GetPostStepPoint();
   if(aPostStepPoint->GetStepStatus() == fGeomBoundary ) {
    if(aPostStepPoint->GetPhysicalVolume() &&
       aPreStepPoint->GetPhysicalVolume()){

    const G4Track* aTrack= aStep ->GetTrack();
    const G4DynamicParticle* aParticle=aTrack->GetDynamicParticle();
    if(aParticle->GetDefinition() == G4OpticalPhoton::OpticalPhoton() ) {
      const G4double  aParticleKE= aParticle->GetKineticEnergy();
      if(   aParticleKE > 0.0 ) {
        const G4ThreeVector & prePos=aPreStepPoint->GetPosition();
        const G4ThreeVector & postPos=aPostStepPoint->GetPosition();
        const G4ThreeVector & PhotCurDir = aTrack->GetMomentumDirection().unit();

        if( ( prePos.z() >= ZUpsRich1Analysis &&
            postPos.z() <= ZDnsRich1Analysis ) ) {
             
             const G4String & aPreVolName=
                              aPreStepPoint->GetPhysicalVolume()->
                              GetLogicalVolume()->GetName();
             const G4String & aPostVolName=
                              aPostStepPoint->GetPhysicalVolume()->
                              GetLogicalVolume()->GetName();
             // G4cout<<"UserStepSeven Pre Post vol "<<aPreVolName<<"  "
             //      << aPostVolName<<G4endl;
             

             if( (aPreVolName == LogVolRich1MagShH0NameAnalysis  )  && 
                   aPostVolName == LogVolRich1PhDetSupFrameH0NameAnalysis ) {
                    G4Navigator* theNavigator =
                       G4TransportationManager::GetTransportationManager()->
                                                    GetNavigatorForTracking();                     
                  G4bool valid;
                  G4ThreeVector theLocalNormal = theNavigator->GetLocalExitNormal(&valid);
                  
                  G4ThreeVector theGlobalNormal = theNavigator->GetLocalToGlobalTransform().
                                               TransformAxis(theLocalNormal);
                  G4double PdotN=  PhotCurDir * theGlobalNormal;

                  IHistogramSvc* CurrentHistoSvc = RichG4SvcLocator::RichG4HistoSvc();
                  SmartDataPtr<IHistogram1D> m_hHistoPhotDirInPhDetFrame(CurrentHistoSvc,"RICHG4HISTOSET5/601");
                  if(!m_hHistoPhotDirInPhDetFrame) {
                    m_hHistoPhotDirInPhDetFrame=CurrentHistoSvc->book("RICHG4HISTOSET5/601", "PhdetPhotonDir",
                                                                      300,-1.0,1.0);
                    
                  }

                  // G4cout<<" PhDet dir PdotN "<<  PhotCurDir <<"  "<<  theGlobalNormal <<"  "<<PdotN<<G4endl;
                  
                  if(m_hHistoPhotDirInPhDetFrame) m_hHistoPhotDirInPhDetFrame->fill(acos(PdotN));
                  
                  
             }
             
             
             
        }
        
      }
      
    }
    }
   }
}

