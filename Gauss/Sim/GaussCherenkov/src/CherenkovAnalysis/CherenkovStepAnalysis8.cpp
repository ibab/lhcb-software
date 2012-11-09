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
#include "GaussRICH/RichG4RadiatorMaterialIdValues.h"
#include "G4Navigator.hh"
#include "G4TransportationManager.hh"
#include "G4ProcessVector.hh"
#include "G4ProcessManager.hh"
#include "G4GeometryTolerance.hh"

/// GaudiKernel
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
// local
#include "CherenkovStepAnalysis8.h"

#include "GaussRICH/RichG4GaussPathNames.h"
#include "GaussCherenkov/CkvG4GaussPathNames.h"
#include "GaussCherenkov/CkvG4AnalysisConstGauss.h"


#include "GaussCherenkov/CkvG4SvcLocator.h"


// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "globals.hh"




//-----------------------------------------------------------------------------
// Implementation file for class : CherenkovStepAnalysis8
//
// 2012-10-08 : Sajan Easo
//----------------------------------------------------------------------------
DECLARE_TOOL_FACTORY(CherenkovStepAnalysis8);
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CherenkovStepAnalysis8::CherenkovStepAnalysis8( 
 const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent 
 ) : GiGaStepActionBase ( type , name , parent ) {
  
};

//=============================================================================
// Destructor
//=============================================================================
CherenkovStepAnalysis8::~CherenkovStepAnalysis8() {} 

//=============================================================================
void CherenkovStepAnalysis8::UserSteppingAction( const G4Step* aStep ) 
{
  G4StepPoint* aPreStepPoint = aStep->GetPreStepPoint();
  G4StepPoint* aPostStepPoint = aStep->GetPostStepPoint();
  IHistogramSvc* CurrentHistoSvc = CkvG4SvcLocator::RichG4HistoSvc();
  //  G4cout<<"Now start step action Cherenkov Step analysis 8 "<<G4endl;


  const G4Track* aTrack= aStep ->GetTrack();

  const G4DynamicParticle* aParticle=aTrack->GetDynamicParticle();
  const G4double  aParticleKE= aParticle->GetKineticEnergy();

  // Check if this is an optical photon.
  if(aParticle->GetDefinition() == G4OpticalPhoton::OpticalPhoton() ) {

    if(   aParticleKE > 0.0 ) {

      //                 G4cout<<"Now in Cherenkov Step analysis 8 "<<G4endl;

      const G4ThreeVector & prePos = aPreStepPoint->GetPosition();


      if(prePos.z() >= ZUpsRich1Analysis &&
         prePos.z() <= ZDnsRich1Analysis ){

        const G4ThreeVector & postPos=aPostStepPoint->GetPosition();

        G4double postPosYPoint=  postPos.y();

                
        G4String postPosMaterialName="NoMaterial";
        G4String aPostVolName="NoVolName ";
        const G4String & aPreVolName=
          aPreStepPoint->GetPhysicalVolume()->GetLogicalVolume()->GetName();


        if(aPostStepPoint->GetPhysicalVolume()) {
          if(  aPostStepPoint->GetPhysicalVolume()-> GetLogicalVolume()) {

            postPosMaterialName=aPostStepPoint->GetPhysicalVolume()->
              GetLogicalVolume()->GetMaterial()->GetName();
            aPostVolName=aPostStepPoint->GetPhysicalVolume()->GetLogicalVolume()->GetName();
          }
        }
        
        //const G4ThreeVector & PhotCurDir = aTrack->GetMomentumDirection();




        if(aPostStepPoint->GetStepStatus() == fGeomBoundary) {


         G4Navigator* theNavigator =
                     G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

         G4ThreeVector LocalPhotonPos  = theNavigator->GetGlobalToLocalTransform().TransformPoint(postPos);

         G4double LocalPhotonPosX=  LocalPhotonPos.x();
         G4double LocalPhotonPosY= (postPosYPoint > 0.0) ?  LocalPhotonPos.y(): ( -1.0*(LocalPhotonPos.y()) );

          //now for Rich1Mirror1
            G4String  aPostVolNameM =std::string(aPostVolName,0,33);

            // G4cout<<" prevol postvol name "<<aPreVolName<<"   "<<aPostVolName<<G4endl;

             std::string::size_type iR1M1StrPostPos=
                      aPostVolName.find(LogVolC4F10NameAnalysis);
             std::string::size_type iR1M1StrPrePos= 
               aPreVolName.find(LogVolRich1Mirror1MasterNameAnalysis);
             
            if( ( iR1M1StrPrePos != std::string::npos)  && (iR1M1StrPostPos !=  std::string::npos )){

              // the reflection already happened at this point.
              
              SmartDataPtr<IHistogram2D>  m_CkvHisto_XY_Rich1Mirror1( CurrentHistoSvc,"RICHG4HISTOSET8/501");

              // std::cout<<" Stepaction8 r1 mirror1 x y "<<  postPos.x() <<"   "<<  postPos.y()<<std::endl;
              
              if( m_CkvHisto_XY_Rich1Mirror1) m_CkvHisto_XY_Rich1Mirror1->fill(prePos.x(),prePos.y());              
            }

            //Now for Rich1Mirror2           
            std::string::size_type iR1M2StrPrePos=
                      aPreVolName.find(LogVolC4F10NameAnalysis);
            std::string::size_type iR1M2SegStrPostPos=
                      aPostVolName.find(LogVolRich1Mirror2MasterNameAnalysis);
            
            //            if( ( (aPreVolName == LogVolC4F10NameAnalysis) &&
            //   (aPostVolName == LogVolRich1Mirror2MasterNameAnalysis)) || 


            

            if( ( iR1M2StrPrePos != std::string::npos)  && (iR1M2SegStrPostPos !=  std::string::npos )){


              
              SmartDataPtr<IHistogram2D> m_CkvHisto_XY_Rich1Mirror2(CurrentHistoSvc,"RICHG4HISTOSET8/503");
              if( m_CkvHisto_XY_Rich1Mirror2)  m_CkvHisto_XY_Rich1Mirror2->fill(postPos.x(),postPos.y()); 
                
              if(postPosYPoint > 0.0) {

                SmartDataPtr<IHistogram2D> m_CkvHisto_XY_Rich1Mirror2LocalTop(CurrentHistoSvc,"RICHG4HISTOSET8/504");
                if( m_CkvHisto_XY_Rich1Mirror2LocalTop)  m_CkvHisto_XY_Rich1Mirror2LocalTop->
                                   fill(LocalPhotonPosX,LocalPhotonPosY);
              }else {
                SmartDataPtr<IHistogram2D> m_CkvHisto_XY_Rich1Mirror2LocalBot(CurrentHistoSvc,"RICHG4HISTOSET8/505");
                if( m_CkvHisto_XY_Rich1Mirror2LocalBot)  m_CkvHisto_XY_Rich1Mirror2LocalBot->
                                     fill(LocalPhotonPosX,LocalPhotonPosY);
              }
        
            }
            
            // now for RICH QW

            //G4String aPreLogVolName1=std::string(aPreVolName,0,31);
            std::string::size_type iR1MagShPre= aPreVolName.find(Rich1MagShPathLogSeg);
            std::string::size_type iR1QWPost = aPostVolName.find( Rich1QWPathLogSeg);


            
            if((  iR1MagShPre != std::string::npos )&& (iR1QWPost  != std::string::npos   ) ){

              SmartDataPtr<IHistogram2D>  m_CkvHisto_XY_Rich1QW(CurrentHistoSvc, "RICHG4HISTOSET8/516");

              if( m_CkvHisto_XY_Rich1QW) m_CkvHisto_XY_Rich1QW->fill(postPos.x(),postPos.y());

              if(postPosYPoint > 0.0) {
                
                SmartDataPtr<IHistogram2D>  m_CkvHisto_XY_Rich1QWLocalTop(CurrentHistoSvc, "RICHG4HISTOSET8/517");
                if(m_CkvHisto_XY_Rich1QWLocalTop)  m_CkvHisto_XY_Rich1QWLocalTop->fill(LocalPhotonPosX,LocalPhotonPosY); 
              
              }else {
  
                SmartDataPtr<IHistogram2D>  m_CkvHisto_XY_Rich1QWLocalBot(CurrentHistoSvc, "RICHG4HISTOSET8/518");              
                if(m_CkvHisto_XY_Rich1QWLocalBot)  m_CkvHisto_XY_Rich1QWLocalBot->fill(LocalPhotonPosX,LocalPhotonPosY); 
                
              }
              
              
            }
            
            
            // now for RICH phDet sup frame
            std::string::size_type  iR1PhDetSupFramePathPost=  aPostVolName.find(Rich1PhDetSupFramePathSeg);
            


              if(( iR1MagShPre != std::string::npos ) && (iR1PhDetSupFramePathPost != std::string::npos) ){

                SmartDataPtr<IHistogram2D> m_CkvHisto_XY_Rich1PhSup (CurrentHistoSvc, "RICHG4HISTOSET8/506");

                
                if( m_CkvHisto_XY_Rich1PhSup)m_CkvHisto_XY_Rich1PhSup->fill(postPos.x(),postPos.y()); 

                if(postPosYPoint > 0.0) {

                 SmartDataPtr<IHistogram2D> m_CkvHisto_XY_Rich1PhSupLocalTop (CurrentHistoSvc, "RICHG4HISTOSET8/507");
                 if(m_CkvHisto_XY_Rich1PhSupLocalTop) m_CkvHisto_XY_Rich1PhSupLocalTop->fill(LocalPhotonPosX,LocalPhotonPosY); 
                }else {
                  

                 SmartDataPtr<IHistogram2D> m_CkvHisto_XY_Rich1PhSupLocalBot (CurrentHistoSvc, "RICHG4HISTOSET8/508");
                 if(m_CkvHisto_XY_Rich1PhSupLocalBot) m_CkvHisto_XY_Rich1PhSupLocalBot->fill(LocalPhotonPosX,LocalPhotonPosY); 
                  
                }
                
                

            }
            
            // now for RICH PMT Module array
  
          
              std::string::size_type  iR1PhDetSupFramePathPre=  aPreVolName.find(Rich1PhDetSupFramePathSeg);
              std::string::size_type  iR1Rich1PmtModulePathPost= aPostVolName.find(Rich1PmtModulePathLogSeg);
            if( (iR1PhDetSupFramePathPre != std::string::npos ) && (iR1Rich1PmtModulePathPost != std::string::npos) ){

             
                if(postPosYPoint > 0.0) {

                  SmartDataPtr<IHistogram2D>  m_CkvHisto_XY_Rich1ModulePlaneTop( CurrentHistoSvc, "RICHG4HISTOSET8/510");
              
                  if( m_CkvHisto_XY_Rich1ModulePlaneTop)    m_CkvHisto_XY_Rich1ModulePlaneTop->fill(postPos.x(),postPos.y()); 
                }else {
                  
                  SmartDataPtr<IHistogram2D>  m_CkvHisto_XY_Rich1ModulePlaneBot( CurrentHistoSvc, "RICHG4HISTOSET8/511");
              
                  if( m_CkvHisto_XY_Rich1ModulePlaneBot)    m_CkvHisto_XY_Rich1ModulePlaneBot->fill(postPos.x(),postPos.y()); 
                  
                }
                
                
            }
            
            

            

        }// end checking on geom boundary
          


 

        
      }
      
    }// end test on PartileKE
  }//end test on opticalphoton
  

  
}


