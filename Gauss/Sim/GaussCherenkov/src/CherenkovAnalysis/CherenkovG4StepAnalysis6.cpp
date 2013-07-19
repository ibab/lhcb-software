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

// GiGa
#include <math.h>
#include "GaussTools/GaussTrackInformation.h"
#include "GaussRICH/RichG4GaussPathNames.h"
#include "GaussCherenkov/CkvG4GaussPathNames.h"
#include "GaussCherenkov/CkvG4AnalysisConstGauss.h"
 
// local
#include "GaussCherenkov/CherenkovG4PmtReflTag.h"
#include "CherenkovG4StepAnalysis6.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CherenkovG4StepAnalysis6
//
// 2011-04-19 : Sajan Easo
//-----------------------------------------------------------------------------
DECLARE_TOOL_FACTORY(CherenkovG4StepAnalysis6);


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CherenkovG4StepAnalysis6::CherenkovG4StepAnalysis6
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ): GiGaStepActionBase ( type , name , parent ) {

}
//=============================================================================
// Destructor
//=============================================================================
CherenkovG4StepAnalysis6::~CherenkovG4StepAnalysis6() {} 

//=============================================================================
void CherenkovG4StepAnalysis6::UserSteppingAction( const G4Step* aStep )
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
        const G4ThreeVector & PhotCurDir = aTrack->GetMomentumDirection();


        if( ( prePos.z() >= ZUpsRich1Analysis &&
            postPos.z() <= ZDnsRich1Analysis )  ||
            ( prePos.z() >= ZUpsRich2Analysis &&
              postPos.z() <= ZDnsRich2Analysis )) {
             const G4String & aPreVolName=
                              aPreStepPoint->GetPhysicalVolume()->
                              GetLogicalVolume()->GetName();
             const G4String & aPostVolName=
                              aPostStepPoint->GetPhysicalVolume()->
                              GetLogicalVolume()->GetName();
             //const G4Material* Material1 = aPreStepPoint  -> GetMaterial();
             // const G4Material* Material2 = aPostStepPoint -> GetMaterial();
             //const G4String & aMaterial1Name =  Material1 ->GetName();
             // const G4String & aMaterial2Name =  Material2 ->GetName();
             std::string::size_type iPmtSMStrPrePos0=
                      aPreVolName.find(LogVolPmtSMasterNameAnalysisListStrPrefix[0]);
             if( (aPreVolName == LogVolPmtSMasterNameAnalysis || iPmtSMStrPrePos0 != std::string::npos )  &&
               aPostVolName ==  LogVolPmtQWindowNameAnalysis ){

               if(PhotCurDir.z() > 0.0 ) {

                 RichG4PmtQWIncidentTag((* aTrack), postPos,0 ); // zero indicates that it is for QW point
                
               }
             }


             if( (aPreVolName == LogVolLPmtSMasterNameAnalysis || iPmtSMStrPrePos0 != std::string::npos )  &&
               aPostVolName ==  LogVolPmtLensNameAnalysis){

               if(PhotCurDir.z() > 0.0 ) {

                 RichG4PmtQWIncidentTag((* aTrack), postPos,1); // 1 indicates that it is for lens point.
                
               }
             }



             
       }}
    }}}
   

   
}


