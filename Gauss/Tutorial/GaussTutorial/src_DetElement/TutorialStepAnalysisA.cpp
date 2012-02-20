// $Id: TutorialStepAnalysisA.cpp,v 1.7 2009-09-13 13:07:18 seaso Exp $
// Include files


#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4DynamicParticle.hh"
#include "G4Material.hh"
#include "G4TransportationManager.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"

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
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/DeclareFactoryEntries.h"


#include <math.h>


// local
#include "TutorialStepAnalysisA.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TutorialStepAnalysisA
//
// 2003-10-29 : Sajan EASO
//-----------------------------------------------------------------------------
DECLARE_TOOL_FACTORY(TutorialStepAnalysisA);

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TutorialStepAnalysisA::TutorialStepAnalysisA
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaStepActionBase ( type , name , parent ) {


  m_tutorialMasterLogVolName="/dd/Geometry/AfterMagnetRegion/TutorialMaster/lvTutorialMaster";
  
  m_tutorialNumPixelContainer=3;
  m_tutorialPixelContainerLogVolName="/dd/Geometry/AfterMagnetRegion/TutorialMaster/lvTexPixContainer" ;
  m_tutorialPixelLogVolName ="/dd/Geometry/AfterMagnetRegion/TutorialMaster/lvTexSiPixel";
  m_tutorialEnergyMin=1000.0*MeV;
  
  
};
TutorialStepAnalysisA::~TutorialStepAnalysisA(  ) { }
void TutorialStepAnalysisA::UserSteppingAction( const G4Step* aStep )
{


 
  //    G4cout<<"Now begin Tutorial Step AnalysisA " <<G4endl;

  G4StepPoint* aPreStepPoint = aStep->GetPreStepPoint();
  G4StepPoint* aPostStepPoint = aStep->GetPostStepPoint();
  if(aPostStepPoint->GetStepStatus() == fGeomBoundary) {
    const G4Track* aTrack= aStep ->GetTrack();

    const G4DynamicParticle* aParticle=aTrack->GetDynamicParticle();

    const G4double  aParticleKE= aParticle->GetKineticEnergy();
    std::string  aParticleName= (std::string) aParticle->
                                GetDefinition()->GetParticleName();
    

    std::string aPreLogVolName= (std::string) aPreStepPoint->GetPhysicalVolume()
                          ->GetLogicalVolume()->GetName();
    std::string aPostLogVolName= (std::string) aPostStepPoint->GetPhysicalVolume()
                         ->GetLogicalVolume()->GetName();

    const G4ThreeVector & prePos=aPreStepPoint->GetPosition();
    const G4ThreeVector & postPos=aPostStepPoint->GetPosition();
    if(   aParticleKE > m_tutorialEnergyMin ) {

          if(  aPreLogVolName == m_tutorialMasterLogVolName && 
               aPostLogVolName == m_tutorialPixelContainerLogVolName ) {

            G4cout<<"Master-Container PostPos XYZ Energy Name "
                  <<postPos<<"   "<< aParticleKE<< "   "<<aParticleName<< G4endl;

      
           }// end check on Master-PixelContainer boundary
    

    if(  aPreLogVolName == m_tutorialPixelContainerLogVolName && 
         aPostLogVolName == m_tutorialPixelLogVolName ) {


       // const G4ThreeVector & aTrackProdPos = aTrack->  GetVertexPosition();

       //    if(aParticle->GetDefinition() == G4OpticalPhoton::OpticalPhoton() ) {

       
       G4TouchableHistory* CurTT =
          (G4TouchableHistory*)(aPreStepPoint->GetTouchable());
       int CurrentPixContainerNumber= CurTT->GetReplicaNumber(0);
       G4cout<<"Container-Pixel ContainerNum Energy Name "<<CurrentPixContainerNumber<<"   "
             << aParticleKE<< "   "<<aParticleName<<G4endl;

       G4cout<<"Container-Pixel Global Pre Post XYZ "<< prePos<<"   "<<postPos<<G4endl;
       

       G4Navigator* theNavigator =
        G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
       G4ThreeVector LocalpostPos = theNavigator->
                                    GetGlobalToLocalTransform().TransformPoint(postPos);

       G4cout<<"Container-Pixel Local Post XYZ "<<LocalpostPos<<G4endl;
       


    } // end check PixelContainer-Pixel boundary

    } // end check particleKE
    
  

    
  }// end check GeomBoundary
  
  
  //      G4cout<<"Now end of Tutorial Step AnalysisA " <<G4endl;
  
}


//=============================================================================
