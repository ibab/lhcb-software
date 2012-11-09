// $Id: RichG4StepAnalysis5.cpp,v 1.11 2009-09-13 13:07:18 seaso Exp $
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
// GiGa
#include <math.h>
#include "GaussTools/GaussTrackInformation.h"


// local
#include "RichG4StepAnalysis5.h"
#include "GaussRICH/RichG4AnalysisConstGauss.h"
#include "GaussRICH/RichG4GaussPathNames.h"
#include "GaussRICH/RichG4MirrorReflPointTag.h"

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
        int aCurrentCopyNumPreVol=  aPreStepPoint->GetPhysicalVolume()->GetCopyNo();        

        int CurrentRichDetnum=0;
        
        
        // now for rich1.        
        if( prePos.z() >= ZUpsRich1Analysis &&
            postPos.z() <= ZDnsRich1Analysis  )
        { 

          // now for mirror1 in rich1. following change made in Jan2008 for the new CMA mirrors of rich1.
	  //            G4String  aPostVolNameM =std::string(aPostVolName,0,52);
            G4String  aPreVolNameM =std::string(aPreVolName,0,66);
            G4String  aPostVolNameM =std::string(aPostVolName,0,73);
            if(aPreVolNameM == LogVolRich1Mirror1QuadrantNameAnalysis &&
               aPostVolNameM == LogVolRich1Mirror1NameAnalysis ){
              // the reflection already happened at mirror1 at this point.
	      RichG4MirrorReflPointTag(aTrack,  postPos, 0,0, aCurrentCopyNumPreVol);
                             
            }
            // now for mirror2 in rich1.
            std::string::size_type iR1M2StrPrePos=
                      aPreVolName.find(LogVolRich1Mirror2MasterNameAnalysis);
            std::string::size_type iR1M2SegStrPostPos=
                      aPostVolName.find(LogVolRich1Mirror2SegNameAnalysis);

	    // the following 'if' has two parts to keep backward compatibility with older xml db.

            if( ( (aPreVolName == LogVolC4F10NameAnalysis) &&
                  (aPostVolName == LogVolRich1Mirror2NameAnalysis) ) || 
                ( ( iR1M2StrPrePos != std::string::npos)  && (iR1M2SegStrPostPos !=  std::string::npos ))   ) {
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
           G4String  aPreVolNameM2 =std::string(aPreVolName,0,57);
           G4String  aPostVolNameM2 =std::string(aPostVolName,0,53);
           std::string::size_type aPreVolNameSubStrSize=0;
           std::string::size_type aPostVolNameSubStrSize=0;
           
           aPreVolNameSubStrSize  = aPreVolName.find(LogVolRich2Mirror1ContainerNameAnalysis);
	         aPostVolNameSubStrSize = aPostVolName.find(LogVolRich2Mirror1NameAnalysisSubString);

           //           G4cout<<"G4stepanalysis5 : Now at rich2 mirror1  "<< aPreVolName<<"   "
           //        <<aPostVolName<<"  "<< aPreVolNameSubStrSize<<"  "<< aPostVolNameSubStrSize<< G4endl;
           // G4cout<<"G4stepanalysis5 : Now at rich2 mirror1 test   aPreVolNameM2 aPostVolNameM2 lognameA lognameB "
           //      << aPreVolNameM2 <<"   "<< aPostVolNameM2 <<"  "<<  LogVolRich2Mirror1ContainerNameAnalysis 
           //      <<"   "<< LogVolRich2Mirror1NameAnalysis <<G4endl;
           

            if( ( (aPreVolNameM2 ==  LogVolRich2Mirror1ContainerNameAnalysis) ||      
                  (aPreVolNameSubStrSize < std::string::npos)) &&
                ( (aPostVolNameM2 == LogVolRich2Mirror1NameAnalysis) ||  
                (aPostVolNameSubStrSize < std::string::npos) )){

              // if( ( aPreVolNameSubStrSize < std::string::npos ) &&  
              // (aPostVolNameSubStrSize < std::string::npos )){
             // the reflection happened at mirror1 at this point.
       	      RichG4MirrorReflPointTag(aTrack,  postPos, 1,0, aCurrentCopyNum);

              //  G4cout<<"G4stepanalysis5 : Now at rich2 mirror1  "<< aPreVolName<<"   "
              //     <<aPostVolName<<"  "<< aPreVolNameSubStrSize<<"  "<< aPostVolNameSubStrSize<< G4endl;
                
            }
            
           
           
            // now for Mirror2 in rich2
           G4String  aPreVolNameSC =std::string(aPreVolName,0,57);
           G4String  aPostVolNameMS =std::string(aPostVolName,0,56);
           std::string::size_type aPreVolNameSCSize=0;
           std::string::size_type aPostVolNameSCSize=0;
           
           aPreVolNameSCSize  =  aPreVolName.find(LogVolRich2Mirror2ContainerAnalysis);
           aPostVolNameSCSize =  aPostVolName.find(LogVolRich2Mirror2NameAnalysis);


             if( ( (aPreVolNameSC == LogVolRich2Mirror2ContainerAnalysis) ||
                   ( aPreVolNameSCSize <  std::string::npos))  &&
                  (( aPostVolNameMS == LogVolRich2Mirror2NameAnalysis) || 
                   ( aPostVolNameSCSize < std::string::npos )) ){

               //if(  ( aPreVolNameSCSize <  std::string::npos)  && 
               // ( aPostVolNameSCSize < std::string::npos ) ){
              // the reflection  happened at mirror2 at this point.

     	      RichG4MirrorReflPointTag(aTrack,  postPos, 1,1, aCurrentCopyNum);
            //   G4cout<<"G4stepanalysis5 : Now at rich2 mirror2  "<< aPreVolName<<"   "
            //        <<aPostVolName<<"  "<<aPreVolNameSCSize<<"   "<< aPostVolNameSCSize<< G4endl;
            
            }
            
	 }
	}              
	}
      }
    }
  
  
  }

}
