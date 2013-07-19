// $Id: RichG4StepAnalysis6.cpp,v 1.7 2009-09-13 13:07:18 seaso Exp $
// Include files 

#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4DynamicParticle.hh"
#include "G4Material.hh"
#include "G4OpticalPhoton.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Step.hh"
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
//#include "GaudiKernel/DeclareFactoryEntries.h"
// GiGa
#include <math.h>
#include "GaussTools/GaussTrackInformation.h"
#include "GaussRICH/RichG4StepAnalysis6.h"
#include "GaussRICH/RichG4AnalysisConstGauss.h"
#include "GaussRICH/RichG4GaussPathNames.h"
#include "GaussRICH/RichG4HpdReflectionFlag.h"
#include "GaussRICH/RichG4HpdReflTag.h"


//-----------------------------------------------------------------------------
// Implementation file for class : RichG4StepAnalysis6
//
// 2007-10-30 : Sajan EASO
//-----------------------------------------------------------------------------
//DECLARE_TOOL_FACTORY(RichG4StepAnalysis6); // moved to Factories.cpp


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4StepAnalysis6::RichG4StepAnalysis6
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaStepActionBase ( type , name , parent ) {





}
//=============================================================================
// Destructor
//=============================================================================
RichG4StepAnalysis6::~RichG4StepAnalysis6() {} 

//=============================================================================
//=============================================================================
void RichG4StepAnalysis6::UserSteppingAction( const G4Step* aStep )
{


  RichG4HpdReflectionFlag*  aRichG4HpdReflectionFlag 
    = RichG4HpdReflectionFlag::RichG4HpdReflectionFlagInstance();

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
        const G4ThreeVector & PhotCurDir = aTrack->GetMomentumDirection();


        if( ( prePos.z() >= ZUpsRich1Analysis &&
            postPos.z() <= ZDnsRich1Analysis )  ||
            ( prePos.z() >= ZUpsRich2Analysis &&
              postPos.z() <= ZDnsRich2Analysis )) {

          // additional safety to ensure that the photon step
          // is within rich1 or rich2. modified since the hpds are outside the following.
         
          // if( ( prePos.z() <= ZDnsRich1Analysis &&
          //     prePos.x() >= -1.0*  Rich1AnalysisXHalfSize &&
          //     prePos.x() <=  Rich1AnalysisXHalfSize &&
          //     prePos.y() >= -1.0* Rich1AnalysisYHalfSize &&
          //     prePos.y() <= Rich1AnalysisYHalfSize &&
          //     postPos.z() >= ZUpsRich1Analysis &&
          //     postPos.x() >=  -1.0*  Rich1AnalysisXHalfSize &&
          //     postPos.x() <=    Rich1AnalysisXHalfSize &&
          //     postPos.y() >= -1.0* Rich1AnalysisYHalfSize &&
          //     postPos.y() <=  Rich1AnalysisYHalfSize ) ||
          //    ( prePos.z() <= ZDnsRich2Analysis &&
          //      prePos.x() >= -1.0*  CF4AnalysisDnstrXHalfSize &&
          //      prePos.x() <=  CF4AnalysisDnstrXHalfSize  &&
          //     prePos.y() >= -1.0* CF4AnalysisDnstrYHalfSize &&
          //     prePos.y() <= CF4AnalysisDnstrYHalfSize &&
          //     postPos.z() >= ZUpsRich2Analysis &&
          //     postPos.x() >=  -1.0*  CF4AnalysisDnstrXHalfSize &&
          //     postPos.x() <=   CF4AnalysisDnstrXHalfSize &&
          //     postPos.y() >= -1.0* CF4AnalysisDnstrYHalfSize &&
          //     postPos.y() <=  CF4AnalysisDnstrYHalfSize ) ) {


             const G4String & aPreVolName=
                              aPreStepPoint->GetPhysicalVolume()->
                              GetLogicalVolume()->GetName();
             const G4String & aPostVolName=
                              aPostStepPoint->GetPhysicalVolume()->
                              GetLogicalVolume()->GetName();
             const G4Material* Material1 = aPreStepPoint  -> GetMaterial();
             const G4Material* Material2 = aPostStepPoint -> GetMaterial();
             const G4String & aMaterial1Name =  Material1 ->GetName();
             const G4String & aMaterial2Name =  Material2 ->GetName();

             std::string::size_type iHpdSiDetPrePos=
                      aPreVolName.find(LogVolSiDetNameAnalysisStrPrefix);
             std::string::size_type iHpdSiDetPostPos=
                      aPostVolName.find(LogVolSiDetNameAnalysisStrPrefix);
             std::string::size_type iHpdSMStrPrePos0=
                      aPreVolName.find(LogVolHpdSMasterNameAnalysisListStrPrefix[0]);
             std::string::size_type iHpdSMStrPrePos1=
                      aPreVolName.find(LogVolHpdSMasterNameAnalysisListStrPrefix[1]);
             
             if(aPreVolName  ==  LogVolSiDetNameAnalysis ||
                aPostVolName ==  LogVolSiDetNameAnalysis || 
                iHpdSiDetPrePos != std::string::npos ||
                iHpdSiDetPostPos != std::string::npos ) {
                 RichG4HpdReflectionTag ( (*aTrack),  aRichG4HpdReflectionFlag->HpdSiliconRefl()); 
             }
             if(  (aMaterial1Name == RichHpdKovarMaterialName) || 
                  (aMaterial2Name == RichHpdKovarMaterialName) ){
                 RichG4HpdReflectionTag ( (*aTrack),  aRichG4HpdReflectionFlag->HpdKovarRefl());
             }
             if(  (aMaterial1Name == RichHpdKaptonMaterialName) || 
                  (aMaterial2Name == RichHpdKaptonMaterialName) ){
                 RichG4HpdReflectionTag ( (* aTrack),  aRichG4HpdReflectionFlag->HpdKaptonRefl());
             }
             if(  (aMaterial1Name == RichHpdChromiumMaterialName) || 
                  (aMaterial2Name == RichHpdChromiumMaterialName) ){
                 RichG4HpdReflectionTag ( (* aTrack),  aRichG4HpdReflectionFlag->HpdChromiumRefl());
             }             
             // now for the Hpd Quartz Window incidence location
             
            if( (aPreVolName == LogVolHpdSMasterNameAnalysis || iHpdSMStrPrePos0 != std::string::npos 
                  || iHpdSMStrPrePos1 != std::string::npos )  &&
               aPostVolName ==  LogVolHpdQWindowNameAnalysis ){
              //test 
              // G4cout<<" Rich HpdQWIncident point photdirZ "<<postPos.x()<<"  "<<postPos.y()<<"  "<<postPos.z()
              //      <<"   "<< PhotCurDir.z()<<G4endl;
              // 
              // end test
              if(PhotCurDir.z() > 0.0 ) {

                RichG4HpdQWIncidentTag((* aTrack), postPos );
                
              }
              
              
            }
            
            
            
 

             

            
            //}
      
          
          
          
        }
      }
    }
    
    }
    
  }



}


