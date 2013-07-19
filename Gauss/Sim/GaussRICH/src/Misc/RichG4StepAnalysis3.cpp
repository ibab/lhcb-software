// $Id: RichG4StepAnalysis3.cpp,v 1.15 2009-09-13 13:07:18 seaso Exp $
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

//
/// GaudiKernel
//#include "GaudiKernel/DeclareFactoryEntries.h"
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
#include "GaussRICH/RichG4StepAnalysis3.h"
#include "GaussRICH/RichG4AnalysisConstGauss.h"
#include "GaussRICH/RichG4GaussPathNames.h"
#include "GaussRICH/RichG4Counters.h"
#include "GaussRICH/RichInfo.h"
#include "GaussRICH/RichPEInfo.h"
#include "GaussRICH/RichPhotInfo.h"
#include "GaussRICH/RichPhotoElectron.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4StepAnalysis3
//
// 2003-06-07 : Sajan EASO
//-----------------------------------------------------------------------------
// moved to Factores.cpp
//DECLARE_TOOL_FACTORY(RichG4StepAnalysis3);

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4StepAnalysis3::RichG4StepAnalysis3
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaStepActionBase ( type , name , parent ) {

  m_RichG4AgelPlanePosTolerence= 2.0*mm;

  m_RichG4AgelPhotonDirTolerence=0.005*rad;


};

RichG4StepAnalysis3::~RichG4StepAnalysis3()
{
}
G4bool RichG4StepAnalysis3::RichG4AgelPhotRad(G4int aRadNum) 
{
  G4bool aflAg=false;
  
 RichG4RadiatorMaterialIdValues* aRMIdValues= 
            RichG4RadiatorMaterialIdValues::RichG4RadiatorMaterialIdValuesInstance();
 if( aRMIdValues -> IsRich1AerogelAnyTileRad( aRadNum )) {
   aflAg=true;
 }
 
 
 return aflAg;  
}

G4bool RichG4StepAnalysis3::RichG4Rich1GasPhotRad(G4int aRadiatorNumber) 
{
  G4bool aflR1=false;
 RichG4RadiatorMaterialIdValues* aRMIdValues= 
            RichG4RadiatorMaterialIdValues::RichG4RadiatorMaterialIdValuesInstance();
  
 if( aRadiatorNumber == (aRMIdValues->Rich1GaseousCkvRadiatorNum())) {
   aflR1=true;
   
 }
 return aflR1;
 
}


void RichG4StepAnalysis3::UserSteppingAction( const G4Step* aStep )
{

  RichG4Counters* aRichCounter =  RichG4Counters::getInstance();

  G4StepPoint* aPreStepPoint = aStep->GetPreStepPoint();
  G4StepPoint* aPostStepPoint = aStep->GetPostStepPoint();

  const G4Track* aTrack= aStep ->GetTrack();

  const G4DynamicParticle* aParticle=aTrack->GetDynamicParticle();
  const G4double  aParticleKE= aParticle->GetKineticEnergy();

  // Check if this is an optical photon.
  if(aParticle->GetDefinition() == G4OpticalPhoton::OpticalPhoton() ) {

    if(   aParticleKE > 0.0 ) {

      //          G4cout<<"Now in Step analysis 3 "<<G4endl;
      G4int aRadiatorNum = -5;
 
     G4VUserTrackInformation* aUserTrackInfo=aTrack->GetUserInformation();
     GaussTrackInformation* aRichPhotTrackInfo
        = (GaussTrackInformation*)aUserTrackInfo;
                                                                                                                                    
   if( aRichPhotTrackInfo) {
     if( aRichPhotTrackInfo->detInfo() ){
  
       RichInfo* aRichTypeInfo =
        ( RichInfo*) (aRichPhotTrackInfo->detInfo());
       if( aRichTypeInfo && aRichTypeInfo->HasUserPhotInfo() ){
         RichPhotInfo* aRichPhotInfo =
                    aRichTypeInfo-> RichPhotInformation();
         if( aRichPhotInfo ) {
            
           aRadiatorNum = aRichPhotInfo->PhotProdRadiatorNum() ;
 
	 }}}}

      const G4ThreeVector & prePos = aPreStepPoint->GetPosition();


      if(prePos.z() >= ZUpsRich1Analysis &&
         prePos.z() <= ZDnsRich1Analysis ){

        //          G4cout<<"Now in Step analysis 3 inside Rich1 "<<G4endl;

	//        const G4ThreeVector & aPhotProdPos = aTrack->  GetVertexPosition();
        const G4ThreeVector & postPos=aPostStepPoint->GetPosition();
        //           G4cout<<"Now in Step analysis 3 postPosZ  "
        //      << postPos<< G4endl;

        G4String postPosMaterialName="NoMaterial";
        G4String aPostVolName="NoVolName ";


        if(aPostStepPoint->GetPhysicalVolume()) {
          if(  aPostStepPoint->GetPhysicalVolume()-> GetLogicalVolume()) {

            postPosMaterialName=aPostStepPoint->GetPhysicalVolume()->
              GetLogicalVolume()->GetMaterial()->GetName();
          }
        }


        const G4ThreeVector & PhotCurDir = aTrack->GetMomentumDirection();

        const G4String & aPreVolName=
          aPreStepPoint->GetPhysicalVolume()->GetLogicalVolume()->GetName();

        // const G4String & prePosMaterialName=aPreStepPoint->GetPhysicalVolume()->
        //  GetLogicalVolume()->GetMaterial()->GetName();
	// test of rad length.
        //  G4double RadLenPre= aPreStepPoint->GetPhysicalVolume()->
        //    GetLogicalVolume()->GetMaterial()-> GetRadlen();
        //  G4double RadLenPost= aPostStepPoint->GetPhysicalVolume()->
        //    GetLogicalVolume()->GetMaterial()-> GetRadlen();
	//   G4double NucLenPre = aPreStepPoint->GetPhysicalVolume()->
        //    GetLogicalVolume()->GetMaterial()->GetNuclearInterLength ();
        //  G4double NucLenPost = aPostStepPoint->GetPhysicalVolume()->
        //    GetLogicalVolume()->GetMaterial()->GetNuclearInterLength ();
        //         G4cout<<"pre and Post step material name radlen pre post nuclen prepost"
        //     <<  prePosMaterialName <<"   "
	//		       <<postPosMaterialName<<"   "
	//	       <<RadLenPre<<"   "<<RadLenPost<<"   "
	//              <<  NucLenPre<<"   "<<NucLenPost << G4endl;
		 
        //        RichG4RadiatorMaterialIdValues* aRMIdValues= 
        //    RichG4RadiatorMaterialIdValues::RichG4RadiatorMaterialIdValuesInstance();



        // Now for the photon production point in c4f10.

        if( RichG4AgelPhotRad(aRadiatorNum) || RichG4Rich1GasPhotRad(aRadiatorNum) ) {
          

          G4int aStepNum = aTrack -> GetCurrentStepNumber() ;

          if(aStepNum == 1 ) {

            if(  RichG4Rich1GasPhotRad (aRadiatorNum) ) {

              aRichCounter->bumpNumPhotProdRich1Gas();

            }else if ( RichG4AgelPhotRad(aRadiatorNum) ) {
              aRichCounter->bumpNumPhotProdRich1Agel();
            }
          }
          if(aPostStepPoint->GetStepStatus() == fGeomBoundary) {

            if(aPostStepPoint->GetPhysicalVolume()) {
              if(  aPostStepPoint->GetPhysicalVolume()->GetLogicalVolume()){

                aPostVolName=
                  aPostStepPoint->GetPhysicalVolume()
                  ->GetLogicalVolume()->GetName();
              }
            }


            //             G4cout<<" RichStepAnalysis Prevol Postvol  RadiatorNum"<<aPreVolName<<"   "
            //                   <<aPostVolName<<"   "<<aRadiatorNum<<G4endl;

            // Now for the Aerogel photons entering the C4F10 volume.

            G4String aPreVolNameA =std::string(aPreVolName,0,33);

            if(RichG4AgelPhotRad( aRadiatorNum )  ) {

              if(aPreVolNameA ==  LogVolAgelNameAnalysis &&
                 aPostVolName ==  LogVolC4F10NameAnalysis) {
                if(PhotCurDir.z() > 0.0 ) {
                  if(prePos.z() <=
                     (AgelZEndAnalysis+m_RichG4AgelPlanePosTolerence) &&
                     postPos.z() >=
                     (AgelZEndAnalysis-m_RichG4AgelPlanePosTolerence) ){

                    aRichCounter->bumpNumPhotAgelAtAgelDownstrZ();
                    //                    cout<<"Current numcross at AgeldownstrZ=  "
                    //    <<aRichCounter->   NumPhotAgelAtAgelDownstrZ()<<endl;
                  }
                }
              }
            }
            // Now for photon hitting the mirror.

            G4String  aPostVolNameM =std::string(aPostVolName,0,33);

            if(aPreVolName == LogVolC4F10NameAnalysis &&
               aPostVolNameM == LogVolRich1Mirror1NameAnalysis ){
              // the reflection already happened at this point.

              //                   if(PhotCurDir.z() > 0.0 ) {

              if( aRadiatorNum == 1 ) {

                aRichCounter->  bumpNumPhotGasOnRich1Mirror1();

              }else if (  RichG4AgelPhotRad (aRadiatorNum ) ) {

                aRichCounter->  bumpNumPhotAgelOnRich1Mirror1();

              }

            }

            // Now for photons hitting mirror2
            // The following 'if' has to parts to keep backward compatibility
            std::string::size_type iR1M2StrPrePos=
                      aPreVolName.find(LogVolRich1Mirror2MasterNameAnalysis);
            std::string::size_type iR1M2SegStrPostPos=
                      aPostVolName.find(LogVolRich1Mirror2SegNameAnalysis);
            
            if( ( (aPreVolName == LogVolC4F10NameAnalysis) &&
               (aPostVolName == LogVolRich1Mirror2NameAnalysis)) || 
               ( ( iR1M2StrPrePos != std::string::npos)  && (iR1M2SegStrPostPos !=  std::string::npos )   ) ){
              // the reflection already happened at this point.

              //                   if(PhotCurDir.z() < 0.0 ) {

              if( RichG4Rich1GasPhotRad ( aRadiatorNum)  ) {
                aRichCounter->  bumpNumPhotGasOnRich1Mirror2();
              } else if (RichG4AgelPhotRad( aRadiatorNum)    ) {

                aRichCounter->  bumpNumPhotAgelOnRich1Mirror2();

              }
            }
            // now for photon hitting the Gas QW

            G4String aPreLogVolName1=std::string(aPreVolName,0,31);
            if(aPreLogVolName1 == LogVolRich1MagShNameAnalysis &&
               aPostVolName == LogColRich1GasQWNameAnalysis ){

              if(PhotCurDir.z() > 0.0 ) {

                if( RichG4Rich1GasPhotRad ( aRadiatorNum)   ) {
                  aRichCounter->  bumpNumPhotGasOnRich1GasQW();
                } else if ( RichG4AgelPhotRad( aRadiatorNum)   ) {

                  aRichCounter->  bumpNumPhotAgelOnRich1GasQW();

                }
              }
            }

            // now for photon hitting the HpdQW
            
             std::string::size_type iHpdSMStrPos0=
                      aPreVolName.find(LogVolHpdSMasterNameAnalysisListStrPrefix[0]);
             std::string::size_type iHpdSMStrPos1=
                      aPreVolName.find(LogVolHpdSMasterNameAnalysisListStrPrefix[1]);
            

            if( ((aPreVolName == LogVolHpdSMasterNameAnalysis) || 
                 (iHpdSMStrPos0 !=  std::string::npos  ) || iHpdSMStrPos1 !=  std::string::npos    ) &&
               aPostVolName ==  LogVolHpdQWindowNameAnalysis ){

              if(PhotCurDir.z() > 0.0 ) {

                if( RichG4Rich1GasPhotRad ( aRadiatorNum)  ) {
                  aRichCounter->  bumpNumPhotGasOnRich1HpdQW();
                } else if ( RichG4AgelPhotRad( aRadiatorNum)  ) {

                  aRichCounter->  bumpNumPhotAgelOnRich1HpdQW();

                }
              }
            }
          }
        }
      }
    }

    //          G4cout<<"Now in Step analysis 3 A "<<G4endl;

    //  }else if( aParticle->GetDefinition() == G4Electron::Electron() ) {
  }else if( ( aParticle->GetDefinition() == RichPhotoElectron::PhotoElectron()) || 
            ( aParticle->GetDefinition() == G4Electron::Electron())) {

    const G4ThreeVector & eprePos=aPreStepPoint->GetPosition();
    if(eprePos.z() >= ZUpsRich1Analysis &&
       eprePos.z() <= ZDnsRich1Analysis ){

      if(aPostStepPoint->GetStepStatus() == fGeomBoundary) {

        const G4String & aelnPreVolName=
          aPreStepPoint->GetPhysicalVolume()->GetLogicalVolume()->GetName();
         std::string::size_type jHpdSMStrPos0=
                      aelnPreVolName.find(LogVolHpdSMasterNameAnalysisListStrPrefix[0]);
         std::string::size_type jHpdSMStrPos1=
                      aelnPreVolName.find(LogVolHpdSMasterNameAnalysisListStrPrefix[1]);

        if( (aelnPreVolName == LogVolHpdSMasterNameAnalysis) ||
            (jHpdSMStrPos0 !=  std::string::npos)  ||  (jHpdSMStrPos1 !=  std::string::npos) ) {

          const G4String & aelnPostVolName=
            aPostStepPoint->GetPhysicalVolume()
            ->GetLogicalVolume()->GetName();

          G4String aCreatorProcessName = "NullProcess";
          const G4VProcess* aProcess = aTrack->GetCreatorProcess();
          if(aProcess) aCreatorProcessName =  aProcess->GetProcessName();
          if(aCreatorProcessName  == "RichHpdPhotoelectricProcess") {


            if(aParticleKE > 0.0 ) {
              if(  ( (aelnPreVolName == LogVolHpdSMasterNameAnalysis) ||
                     (jHpdSMStrPos0 !=  std::string::npos) || (jHpdSMStrPos1 !=  std::string::npos)    ) &&
                   ( (aelnPostVolName == LogVolSiDetNameAnalysis) ||
                     (aelnPostVolName.find(LogVolSiDetNameAnalysisStrPrefix) 
                      != std::string::npos ))){


                G4int   aPeRadiatorNumber =  -1;
                // the following variable not used for now.
                //G4int   aMotherChTkId = -1;


                G4VUserTrackInformation* aUserTrackinfo=
                  aTrack->GetUserInformation();
                GaussTrackInformation* aRichPETrackInfo
                  =(GaussTrackInformation*)aUserTrackinfo;

                if(aRichPETrackInfo) {
                  if( aRichPETrackInfo->detInfo() ) {

                    RichInfo* aRichPETypeInfo=
                      (RichInfo*)(aRichPETrackInfo->detInfo());
                    if(aRichPETypeInfo &&
                       aRichPETypeInfo->HasUserPEInfo()) {

                      RichPEInfo* aPEInfo=
                        aRichPETypeInfo->RichPEInformation();

                      if( aPEInfo) {

                        aPeRadiatorNumber =
                          aPEInfo->PhotOriginRadiatorNumber();
                        //  aMotherChTkId
                        //  = aPEInfo-> MotherOfPhotonId();

                      }
                    }
                  }
                }
                if( RichG4Rich1GasPhotRad(aPeRadiatorNumber)   ) {
                  aRichCounter-> bumpNumPhotGasRich1SiDet();


                }else if(RichG4AgelPhotRad(aPeRadiatorNumber)  ) {

                  aRichCounter-> bumpNumPhotAgelRich1SiDet();

                }
              }
            }
          }
        }
      }
    }
  }
}
//=============================================================================
