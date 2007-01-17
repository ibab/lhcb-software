// $Id: RichG4StepAnalysis5.cpp,v 1.6 2007-01-17 17:49:14 ranjard Exp $
// Include files 

#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4DynamicParticle.hh"
#include "G4Material.hh"
#include "G4OpticalPhoton.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Step.hh"
// Gaudi
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
#include "GaudiKernel/DeclareFactoryEntries.h"
// GiGa
#include <math.h>
#include "GaussTools/GaussTrackInformation.h"


// local
#include "RichG4StepAnalysis5.h"
#include "RichG4AnalysisConstGauss.h"
#include "RichG4GaussPathNames.h"
#include "RichG4MirrorReflPointTag.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4StepAnalysis5
//
// 2004-05-27 : Sajan EASO
//-----------------------------------------------------------------------------
DECLARE_TOOL_FACTORY(RichG4StepAnalysis5);

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4StepAnalysis5::RichG4StepAnalysis5
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaStepActionBase ( type , name , parent ) {

}
//=============================================================================
// Destructor
//=============================================================================
RichG4StepAnalysis5::~RichG4StepAnalysis5() {}; 

//=============================================================================
void RichG4StepAnalysis5::UserSteppingAction( const G4Step* aStep ) 
{
  //    G4cout<<"Now in Rich Step Analysis 5 " <<G4endl;
  G4StepPoint* aPreStepPoint = aStep->GetPreStepPoint();
  G4StepPoint* aPostStepPoint = aStep->GetPostStepPoint();
  if(aPostStepPoint->GetStepStatus() == fGeomBoundary ) {
    const G4Track* aTrack= aStep ->GetTrack();
    const G4DynamicParticle* aParticle=aTrack->GetDynamicParticle();
    if(aParticle->GetDefinition() == G4OpticalPhoton::OpticalPhoton() ) {
      const G4double  aParticleKE= aParticle->GetKineticEnergy();
      if(   aParticleKE > 0.0 ) {
        const G4ThreeVector & prePos=aPreStepPoint->GetPosition();
        const G4ThreeVector & postPos=aPostStepPoint->GetPosition();

        if( ( prePos.z() >= ZUpsRich1Analysis &&
            postPos.z() <= ZDnsRich1Analysis )  ||
            ( prePos.z() >= ZUpsRich2Analysis &&
	      postPos.z() <= ZDnsRich2Analysis )) {
	  // additional safety to ensure that the photon step
          // is within rich1 or rich2.
          if( ( prePos.z() <= ZDnsRich1Analysis &&
               prePos.x() >= -1.0*  Rich1AnalysisXHalfSize &&
               prePos.x() <=  Rich1AnalysisXHalfSize &&
               prePos.y() >= -1.0* Rich1AnalysisYHalfSize &&
               prePos.y() <= Rich1AnalysisYHalfSize && 
               postPos.z() >= ZUpsRich1Analysis &&
               postPos.x() >=  -1.0*  Rich1AnalysisXHalfSize &&
               postPos.x() <=    Rich1AnalysisXHalfSize &&
               postPos.y() >= -1.0* Rich1AnalysisYHalfSize &&
               postPos.y() <=  Rich1AnalysisYHalfSize ) ||
	      ( prePos.z() <= ZDnsRich2Analysis &&
                prePos.x() >= -1.0*  CF4AnalysisDnstrXHalfSize &&
                prePos.x() <=  CF4AnalysisDnstrXHalfSize  &&
               prePos.y() >= -1.0* CF4AnalysisDnstrYHalfSize &&
               prePos.y() <= CF4AnalysisDnstrYHalfSize && 
               postPos.z() >= ZUpsRich2Analysis &&
               postPos.x() >=  -1.0*  CF4AnalysisDnstrXHalfSize &&
               postPos.x() <=   CF4AnalysisDnstrXHalfSize &&
               postPos.y() >= -1.0* CF4AnalysisDnstrYHalfSize &&
               postPos.y() <=  CF4AnalysisDnstrYHalfSize ) ) {


        const G4String & aPreVolName=
                              aPreStepPoint->GetPhysicalVolume()->
                              GetLogicalVolume()->GetName();
        const G4String & aPostVolName=
                              aPostStepPoint->GetPhysicalVolume()->
                              GetLogicalVolume()->GetName();
       
        int aCurrentCopyNum=  aPostStepPoint->GetPhysicalVolume()->GetCopyNo();        

        int CurrentRichDetnum=0;
        
        
        // now for rich1.        
        if( prePos.z() >= ZUpsRich1Analysis &&
            postPos.z() <= ZDnsRich1Analysis  )
        { 

          // now for mirror1 in rich1.
            G4String  aPostVolNameM =std::string(aPostVolName,0,52);
            if(aPreVolName == LogVolC4F10NameAnalysis &&
               aPostVolNameM == LogVolRich1Mirror1NameAnalysis ){
              // the reflection already happened at mirror1 at this point.
	      RichG4MirrorReflPointTag(aTrack,  postPos, 0,0, aCurrentCopyNum);
                             
            }
            // now for mirror2 in rich1.
            if(aPreVolName == LogVolC4F10NameAnalysis &&
               aPostVolName == LogVolRich1Mirror2NameAnalysis ){
              // reflection happened in mirror2 .             
	      RichG4MirrorReflPointTag(aTrack,  postPos, 0,1, aCurrentCopyNum);
               
            }
            
            // now for rich2.    
        } else if ( prePos.z() >= ZUpsRich2Analysis &&
            postPos.z() <= ZDnsRich2Analysis  )
            // now for mirror1 in rich2.    
         {
           //           G4cout<<"step analysis5  rich2 post vol name "<< aPostVolName<<G4endl;
           // G4cout<<"step analysis5  rich2 pre vol name "<< aPreVolName<<G4endl;

           CurrentRichDetnum=1;
           G4String  aPostVolNameM2 =std::string(aPostVolName,0,53);
           if(aPreVolName ==  LogVolCF4NameAnalysis &&
               aPostVolNameM2 == LogVolRich2Mirror1NameAnalysis ){
              // the reflection happened at mirror1 at this point.
       	      RichG4MirrorReflPointTag(aTrack,  postPos, 1,0, aCurrentCopyNum);
            // G4cout<<"G4stepanalysis5 : Now at rich2 mirror1  "<< aPreVolName<<"   "
            //         <<aPostVolName<< G4endl;
              
           }
           
            // now for Mirror2 in rich2
           G4String  aPreVolNameSC =std::string(aPreVolName,0,57);
           G4String  aPostVolNameMS =std::string(aPostVolName,0,56);
            if(aPreVolNameSC == LogVolRich2Mirror2ContainerAnalysis &&
               aPostVolNameMS == LogVolRich2Mirror2NameAnalysis ){
              // the reflection  happened at mirror2 at this point.

     	      RichG4MirrorReflPointTag(aTrack,  postPos, 1,1, aCurrentCopyNum);
            //  G4cout<<"G4stepanalysis5 : Now at rich2 mirror2  "<< aPreVolName<<"   "
            //       <<aPostVolName<< G4endl;
            
            }
            
	 }
	}              
	}
      }
    }
  
  
  }

}
