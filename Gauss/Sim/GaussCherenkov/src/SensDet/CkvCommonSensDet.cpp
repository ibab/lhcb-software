// Include files 



// local

#include "GaussRICH/RichPhotoElectron.h"
#include "GaussRICH/RichInfo.h"
#include "GaussRICH/RichPEInfo.h"


#include "GaussCherenkov/CkvCommonSensDet.h"
#include "GaussCherenkov/CkvG4SvcLocator.h"
#include "GaussCherenkov/CkvGeometrySetupUtil.h"
#include "GaussCherenkov/CherenkovPmtLensUtil.h"
// Geant4
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4TransportationManager.hh"
#include "G4Electron.hh"
#include "G4VProcess.hh"
// LHCb and Gauss
#include "DetDesc/DetectorElement.h"
#include "GaussTools/GaussTrackInformation.h"
using namespace std;

//-----------------------------------------------------------------------------
// Implementation file for class : CkvCommonSensDet
//
// 2014-08-19 : Sajan Easo
//-----------------------------------------------------------------------------
CkvCommonSensDet* CkvCommonSensDet::CkvCommonSensDetInstance=0;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CkvCommonSensDet::CkvCommonSensDet(  ) {

  m_GeomPropInitFlag=true;
  m_TotNumPmtsInRich=5000;
    
}
//=============================================================================
// Destructor
//=============================================================================
CkvCommonSensDet::~CkvCommonSensDet() {
  if(m_RichGeomProperty) delete m_RichGeomProperty;

} 

//=============================================================================
CkvCommonSensDet* CkvCommonSensDet::getCkvCommonSensDetInstance() 
{
  if(CkvCommonSensDetInstance == 0 ) 
  {
    CkvCommonSensDetInstance = new CkvCommonSensDet();
    
  }
  return CkvCommonSensDetInstance;
  
}
//=============================================================================

void  CkvCommonSensDet::InitGeomProp() 
{
  if (m_GeomPropInitFlag) 
  { 
   IDataProviderSvc* detSvc = CkvG4SvcLocator::RichG4detSvc();
   IMessageSvc*  msgSvc = CkvG4SvcLocator::RichG4MsgSvc ();
 
   m_RichGeomProperty= new  CkvG4GeomProp(detSvc,msgSvc) ;
   CkvGeometrySetupUtil * aCkvGeometrySetup= CkvGeometrySetupUtil::getCkvGeometrySetupUtilInstance();
   m_Rich2UseGrandPmt= aCkvGeometrySetup ->Rich2_UseGrandPmt();
    
   m_TotNumPmtsInRich = ( m_RichGeomProperty-> NumberOfPMTsInRich1()) + ( m_RichGeomProperty-> NumberOfPMTsInRich2());
   m_RichPmtAlreadyHit.resize(m_TotNumPmtsInRich);
   ResetPmtMapInCurrentEvent();
   m_GeomPropInitFlag=false;


  }
  
  
}
//=============================================================================




void CkvCommonSensDet::ResetPmtMapInCurrentEvent()
{
    m_RichPmtAlreadyHit.assign(m_TotNumPmtsInRich,false);
    m_RichPmtToPixelNumMap.clear();

}
//=============================================================================

StatusCode  CkvCommonSensDet::ProcessRichPmtHits( G4Step* aStep , G4TouchableHistory* ) { 

  if( 0 == aStep ) { return StatusCode::FAILURE ; }

  double CurEdep = aStep->GetTotalEnergyDeposit();

  //   G4cout<<"Rich SensDet CurEdep "<< CurEdep<<G4endl;
  if(  CurEdep <= 0.001 ) { return StatusCode::FAILURE ; }
  G4StepPoint* prePosPoint =aStep->GetPreStepPoint();
  const G4ThreeVector & CurGlobalPos  = prePosPoint->GetPosition();

  ///
  G4TouchableHistory* CurTT =
    (G4TouchableHistory*)(prePosPoint->GetTouchable());
  //  G4VPhysicalVolume*  CurPV =   CurTT->GetVolume();
  // G4LogicalVolume*    CurLV =   CurPV->GetLogicalVolume();
  //G4cout<<" Ckv SensDet  PV LV "<<   CurPV->GetName()<<"   "<<CurLV ->GetName()<<G4endl;
  // Now get the charged track (ie. photoelectron or backscattered electron or the mip)
  // which created the  hit.
  G4Track* aTrack = aStep->GetTrack();
   if( aTrack->GetDefinition()->GetPDGCharge() == 0.0 ) {
    return  StatusCode::FAILURE ;
  }
  G4ThreeVector CurPEOrigin;

  G4String aCreatorProcessName = "NullProcess";
  const G4VProcess* aProcess = aTrack->GetCreatorProcess();

  if(aProcess) aCreatorProcessName =  aProcess->GetProcessName();
  if(((aTrack->GetDefinition() == G4Electron::Electron()) ||
      (aTrack->GetDefinition() == RichPhotoElectron::PhotoElectron())) &&
     ( (aCreatorProcessName  == "RichHpdPhotoelectricProcess" ) ||
       ( aCreatorProcessName  == "RichHpdSiEnergyLossProcess")  || 
       (aCreatorProcessName  == "RichPmtPhotoelectricProcess" ) ||
       ( aCreatorProcessName  == "RichPmtSiEnergyLossProcess")  ) )
  {
    CurPEOrigin = aTrack->GetVertexPosition() ;
  }
     MsgStream CkvCommonSensDetlog(CkvG4SvcLocator::RichG4MsgSvc(), "CkvCommonSensDet" );

     G4double CurGlobalZ=CurGlobalPos.z();
     int CurrentRichDetNumber =-1;
     if( CurGlobalZ <= 0.0 ) {
         CkvCommonSensDetlog << MSG::ERROR << "Inadmissible Rich Hit Z coordinate = "
             <<  CurGlobalZ <<endreq;
     }else {

        if ( CurGlobalZ < MaxZHitInRich1Detector() ) {
           // hit coordinate in Rich1
           CurrentRichDetNumber=0;
        }else {
           // hit coordinate in Rich2
           CurrentRichDetNumber= 1;
        }
    
     }
     
   G4int CurrentPmtNumber= CurTT -> GetReplicaNumber(2);
   G4int CurrentPmtModuleNumber=  CurTT -> GetReplicaNumber(3);
   bool CurModuleWithLensFlag = false;
   CherenkovPmtLensUtil * aCherenkovPmtLensUtil =  CherenkovPmtLensUtil::getInstance();
   CurModuleWithLensFlag= aCherenkovPmtLensUtil->isPmtModuleWithLens(CurrentPmtModuleNumber);

   G4int CurrentRichDetSector = CurTT ->GetReplicaNumber(4);
   if(CurrentRichDetSector < 0 || CurrentRichDetSector > 1 ) {
     CkvCommonSensDetlog << MSG::ERROR <<"Inadmisible Rich Det Sector=  "<<CurrentRichDetSector
        <<"  Current RichDetNum =   "<<CurrentRichDetNumber<<endreq;
   }
   G4Navigator* theNavigator =
    G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
    G4ThreeVector CurLocalPos = theNavigator->
    GetGlobalToLocalTransform().TransformPoint(CurGlobalPos);
    G4ThreeVector CurPEOriginLocal = theNavigator->GetGlobalToLocalTransform().TransformPoint(CurPEOrigin);
  G4int CurrentPixelXNum= -1;
  G4int CurrentPixelYNum= -1;
  G4bool PXG = false;
  G4bool PYG =false;

  CkvGeometrySetupUtil * aCkvGeometrySetup= CkvGeometrySetupUtil::getCkvGeometrySetupUtilInstance();
  
  if( (CurrentRichDetNumber == 1) && m_Rich2UseGrandPmt && ( aCkvGeometrySetup-> ModuleWithGrandPMT(CurrentPmtModuleNumber) )) {
  
    CurrentPixelXNum =  GrandPixelXNum( CurLocalPos.x());
    CurrentPixelYNum  = GrandPixelYNum( CurLocalPos.y());
    PXG = m_RichGeomProperty->GrandPixelGapFinderX (CurLocalPos.x());
    PYG = m_RichGeomProperty->GrandPixelGapFinderY (CurLocalPos.y());

    
  }else {
    CurrentPixelXNum = PixelXNum(CurLocalPos.x());
    CurrentPixelYNum = PixelYNum(CurLocalPos.y());
    PXG = m_RichGeomProperty->PixelGapFinderX(CurLocalPos.x());
    PYG = m_RichGeomProperty->PixelGapFinderY(CurLocalPos.y());    
  }
  G4int CurrentHitInPixelGap  = ( PXG || PYG ) ? 1 : 0;
  G4double CurGlobalTime =  aTrack-> GetGlobalTime();

  G4int CurOptPhotMotherChTrackID=-1;
  G4int CurOptPhotMotherChTrackPDG=0;
  G4int CurRadiatorNumber=-1;
  G4ThreeVector CurEmissPt;
  G4double CurPhotEnergyAtProd=0;
  G4double CurThetaCkvAtProd=0;
  G4double CurPhiCkvAtProd=0;
  G4double CurChTrackTotMom=0;
  G4ThreeVector CurChTrackMomVect;
  G4double CurChTrackPDGMass=0;
  G4ThreeVector CurChTrackCkvPreStepPos;
  G4ThreeVector CurChTrackCkvPostStepPos;
  G4int CurPhotRayleighScatFlag=0;
  G4ThreeVector CurPhotAgelExitPos;
  G4ThreeVector CurMirror1PhotonReflPosition;
  G4ThreeVector CurMirror2PhotonReflPosition;
  G4int CurMirror1PhotonDetectorCopyNum=-1;
  G4int CurMirror2PhotonDetectorCopyNum=-1;
  G4int aRichVerboseFlag=0;
  G4int CurOptPhotID =0;
  G4int CurHpdQW2PhCathReflFlag=0;
  G4int CurElectronBackScatFlag=0;
  G4int CurPhotoElectricFlag=0;
  G4int CurHpdReflectionFlag=0;
  G4ThreeVector CurHpdQwPhotIncidentPosition;
  G4int CurpdLens=0;
  G4ThreeVector CurLensIncidentPosition;
  G4int CurPhotonSourceProcInfo=0;
  G4ThreeVector CurQWExtSurfaceCoordLocal;
  G4ThreeVector CurLensExtSurfaceCoordLocal;
  


  //  if( m_SuperRichFlag) CurpdLens=1;


  G4VUserTrackInformation* aUserTrackinfo=aTrack->GetUserInformation();
  GaussTrackInformation* aRichPETrackInfo
    = (GaussTrackInformation*)aUserTrackinfo;

  if( ( (aTrack->GetDefinition() == G4Electron::Electron()) ||
        (aTrack->GetDefinition() == RichPhotoElectron::PhotoElectron()))  &&
      (( aCreatorProcessName  == "RichPmtPhotoelectricProcess")  ||
       ( aCreatorProcessName  == "RichPmtSiEnergyLossProcess") ) ) {
    if(aRichPETrackInfo)
    {
      if(aRichPETrackInfo->detInfo())
      {
        RichInfo* aRichPETypeInfo = (RichInfo*)(aRichPETrackInfo->detInfo());
        if(aRichPETypeInfo && aRichPETypeInfo->HasUserPEInfo())
        {
          RichPEInfo* aPEInfo=aRichPETypeInfo->RichPEInformation();
          if( aPEInfo)
          {
            CurOptPhotMotherChTrackID = aPEInfo-> MotherOfPhotonId();
            CurOptPhotMotherChTrackPDG =
              aPEInfo-> MotherOfPhotonPDGcode();
            CurRadiatorNumber   =   aPEInfo->PhotOriginRadiatorNumber();
            CurEmissPt          =   aPEInfo->PhotonEmisPoint();
            CurPhotEnergyAtProd =   aPEInfo->PhotonEnergyAtCkvProd();
            CurThetaCkvAtProd   =   aPEInfo->CherenkovThetaAtProd();
            CurPhiCkvAtProd     =   aPEInfo->CherenkovPhiAtProd();
            CurChTrackTotMom    =   aPEInfo-> MotherofPhotonMomAtProd();
            CurOptPhotID        =   aPEInfo->OptPhotonId();
            CurHpdQW2PhCathReflFlag= aPEInfo->QW2PCreflFlagSave() ;
            CurElectronBackScatFlag = aPEInfo->BackscatteredPeFlag();
            CurPhotoElectricFlag = aPEInfo->PhotoElectricFlag();
            CurHpdReflectionFlag=aPEInfo->HpdPhotonReflectionFlag();
            CurPEOriginLocal= aPEInfo->HpdPeLocalOriginPosition();
            CurPhotonSourceProcInfo=aPEInfo->PhotonSourceInformation();
   
            //CkvCommonSensDetlog << MSG::VRBOSE << "Now in ProcessHits()  "
            //    <<" Track id of charged tk opt phot pe "
            //    << CurOptPhotMotherChTrackID <<"   "
            //    <<  CurOptPhotID<<"   "
            //    << aTrack->GetTrackID() << endreq;

            if( CurElectronBackScatFlag > 0) {
              CkvCommonSensDetlog << MSG::DEBUG << "Now in CkvSensDet ProcessHits() backscattered eln  "
                  << CurElectronBackScatFlag << endreq;
            }
            //CkvCommonSensDetlog<<MSG::INFO<<" Now in processHits Photon source info "<<CurPhotonSourceProcInfo
            //   <<endreq;
            

            if(  aPEInfo->  VerbosePeTagFlag() ) {
              aRichVerboseFlag =1;
              CurChTrackMomVect    =
                aPEInfo->  MotherofPhotonMomVectAtProd();
              CurChTrackPDGMass    = aPEInfo-> MotherofPhotonPDGMass();
              CurChTrackCkvPreStepPos=
                aPEInfo->MotherofPhotonCkvPreStep();
              CurChTrackCkvPostStepPos=
                aPEInfo->MotherofPhotonCkvPostStep();
              CurPhotRayleighScatFlag=
                aPEInfo->PhotonRayleighScatteringFlag();
              CurPhotAgelExitPos=
                aPEInfo->  PhotonAerogelExitPos();
              CurMirror1PhotonReflPosition= aPEInfo->Mirror1PhotReflPosition();
              CurMirror2PhotonReflPosition= aPEInfo->Mirror2PhotReflPosition();
              CurMirror1PhotonDetectorCopyNum=aPEInfo->Mirror1PhotDetCopyNum();
              CurMirror2PhotonDetectorCopyNum=aPEInfo->Mirror2PhotDetCopyNum();
              CurHpdQwPhotIncidentPosition=aPEInfo->HpdQWExtPhotIncidentPosition();
              CurLensIncidentPosition=aPEInfo->PmtLensPhotIncidentPosition();
              if(CurModuleWithLensFlag && ( CurLensIncidentPosition.z() > 500.0) ) CurpdLens=1;
              
              CurQWExtSurfaceCoordLocal =  
                 theNavigator->GetGlobalToLocalTransform().TransformPoint(CurHpdQwPhotIncidentPosition );
              CurLensExtSurfaceCoordLocal = 
                 theNavigator->GetGlobalToLocalTransform().TransformPoint(CurLensIncidentPosition);
              // if(CurpdLens >0  ) {
              //  log<<MSG::VERBOSE<<" SensDet Lens global local coord  "<< CurLensIncidentPosition <<"   "
              //     <<CurLensExtSurfaceCoordLocal<<endmsg;                
              //  log<<MSG::VERBOSE<<" SensDet QW global local coord  "<<CurHpdQwPhotIncidentPosition  <<"   "
              //    <<CurQWExtSurfaceCoordLocal <<endmsg;                
              //  
              // }
               
              

              //if(CurrentRichDetNumber == 1 ) {
                //  log << MSG::INFO<<" Mirr1 xyz  Mirr2 xyz "<<CurMirror1PhotonReflPosition<<"  "
                //    << CurMirror2PhotonReflPosition<<endreq;
                // log << MSG::INFO<<" Mirr1 xyz  Mirr2 xyz "<<CurMirror1PhotonDetectorCopyNum<<"  "
                //    << CurMirror2PhotonDetectorCopyNum <<endreq;
              // }
  

            }



          }
        }
      }
    }
  }
  


  G4int CurPETrackID=aTrack->GetTrackID();
  G4int CurPETrackPDG=aTrack->GetDefinition()->GetPDGEncoding();

  m_newHit = new CkvG4Hit();

  m_newHit -> SetEdep( CurEdep);
  m_newHit -> SetGlobalPos( CurGlobalPos );
  m_newHit -> SetCurSectorNum(CurrentRichDetSector);
  m_newHit -> SetCurRichDetNum(CurrentRichDetNumber);
  m_newHit -> SetLocalPos( CurLocalPos );
  m_newHit -> SetGlobalPEOriginPos( CurPEOrigin) ;
  m_newHit -> SetLocalPEOriginPos( CurPEOriginLocal) ;
  m_newHit -> SetCurPixelXNum(CurrentPixelXNum);
  m_newHit -> SetCurPixelYNum(CurrentPixelYNum);
  m_newHit -> SetOptPhotID(CurOptPhotID);
  m_newHit -> SetChTrackID(CurOptPhotMotherChTrackID);
  m_newHit -> SetChTrackPDG(CurOptPhotMotherChTrackPDG);
  m_newHit -> SetRadiatorNumber( CurRadiatorNumber);
  m_newHit -> SetPhotEmisPt( CurEmissPt);
  m_newHit -> SetPhotEnergyAtProd(CurPhotEnergyAtProd);
  m_newHit -> SetThetaCkvAtProd(CurThetaCkvAtProd);
  m_newHit -> SetPhiCkvAtProd(CurPhiCkvAtProd);
  m_newHit -> SetChTrackTotMom(CurChTrackTotMom);
  m_newHit -> SetChTrackMomVect(CurChTrackMomVect);
  m_newHit -> SetPETrackID(CurPETrackID);
  m_newHit -> SetPETrackPDG(CurPETrackPDG);
  m_newHit -> SetRichHitGlobalTime(CurGlobalTime);
  m_newHit -> SetRichChTrackMass ( CurChTrackPDGMass);
  m_newHit -> setChTrackCkvPreStepPos(CurChTrackCkvPreStepPos);
  m_newHit -> setChTrackCkvPostStepPos(CurChTrackCkvPostStepPos);
  m_newHit -> setOptPhotRayleighFlag(CurPhotRayleighScatFlag);
  m_newHit -> setOptPhotAgelExitPos(  CurPhotAgelExitPos);
  m_newHit -> setMirror1PhotonReflPosition(CurMirror1PhotonReflPosition);
  m_newHit -> setMirror2PhotonReflPosition(CurMirror2PhotonReflPosition);
  m_newHit -> setMirror1PhotonDetectorCopyNum(CurMirror1PhotonDetectorCopyNum);
  m_newHit -> setMirror2PhotonDetectorCopyNum(CurMirror2PhotonDetectorCopyNum);
  m_newHit -> setRichVerboseHitInfo(aRichVerboseFlag);
  m_newHit -> setRichHpdQW2PhCathReflFlag(CurHpdQW2PhCathReflFlag);
  m_newHit ->  setElectronBackScatterFlag(CurElectronBackScatFlag );
  m_newHit ->  setPhotoElectricProductionFlag(CurPhotoElectricFlag);
  m_newHit->   setRichHpdPhotonReflectionFlag(CurHpdReflectionFlag);
  m_newHit->   setHpdQuartzWindowExtSurfPhotIncidentPosition (CurHpdQwPhotIncidentPosition);
  m_newHit->   setHpdQuartzWindowExtSurfPhotIncidentLocalPosition ( CurQWExtSurfaceCoordLocal );  
  m_newHit ->  setPhotonSourceProcessInfo( CurPhotonSourceProcInfo );
  m_newHit ->  setpdWithLens(CurpdLens );
  m_newHit -> setPmtLensPhotIncidentPosition(CurLensIncidentPosition );
  m_newHit -> setPmtLensPhotIncidentLocalPosition ( CurLensExtSurfaceCoordLocal ); 

   m_newHit->SetCurModuleNum(CurrentPmtModuleNumber );
   m_newHit->SetCurHitInPixelGap(CurrentHitInPixelGap);
   m_newHit->SetCurPmtNum (CurrentPmtNumber);
   // m_newHit-> setSuperRichHit( curSuperRichDet   );
   // m_newHit-> setOptHorizontalRich1Hit(curHorizRich1Det );

  if(aRichPETrackInfo) aRichPETrackInfo->setCreatedHit(true);
  m_newHit ->setTrackID(CurOptPhotMotherChTrackID); // this covers all the normal cases
                                                  // like Charged track -> Cherenkov photon -> photoelectron-> hit
  if( CurOptPhotMotherChTrackID <= 0) {           // this is for mip particle and the possible backscatted electrons from them.
    m_newHit ->setTrackID(CurPETrackID);
    if(aRichPETrackInfo) aRichPETrackInfo ->setToBeStored (true);
  }
   
    bool EnableThisHitStore=true;
    G4bool FlagThisHitAsDuplicate=false;
    G4int CurPixelNumInPmt = m_RichGeomProperty ->GetPixelNumInPmt( CurrentPixelXNum, CurrentPixelYNum);
    if(m_RichPmtAviodDuplicateHitsActivate || m_RichPmtFlagDuplicateHitsActivate  ) { 
      if(  m_RichPmtAlreadyHit[CurrentPmtNumber] ) {
        pair<multimap<G4int, G4int>::iterator, multimap<G4int,G4int>::iterator> ppp= 
                                    m_RichPmtToPixelNumMap.equal_range(CurrentPmtNumber);
        multimap<G4int,G4int>::iterator ipm= ppp.first;
        while( (ipm !=ppp.second ) &&   ( EnableThisHitStore ) ) {
          if((*ipm).second == CurPixelNumInPmt) {
               FlagThisHitAsDuplicate=true;
               if(m_RichPmtAviodDuplicateHitsActivate )EnableThisHitStore=false;
          }
          ++ipm;
        }
        
      }// end test pmt already hit
      
    }
    
    //test print 
    if( FlagThisHitAsDuplicate ) {
      CkvCommonSensDetlog << MSG::VERBOSE<<" Rich PMT Duplicate hit "<<CurrentPmtNumber<<"  "<<CurPixelNumInPmt<<endreq;
    }
    

    // end test print
    m_newHit -> setCurrentHitAsDuplicate( FlagThisHitAsDuplicate);
    //int NumHitsInCurHC =0; // Unused variable
    m_newHit -> setFlagHitAsStore(EnableThisHitStore);
    if(EnableThisHitStore) {
      if(m_RichPmtAviodDuplicateHitsActivate || m_RichPmtFlagDuplicateHitsActivate ) { 
         m_RichPmtAlreadyHit[CurrentPmtNumber]=true;
         m_RichPmtToPixelNumMap.insert(pair<G4int,G4int>(CurrentPmtNumber, CurPixelNumInPmt));
      }
      
      //CkvCommonSensDetlog << MSG::VERBOSE
      //  << "CkvSensdet: Current collection set AuxSet and Hit number stored = "
      //  << CurrentRichCollectionSet << "  " 
      // <<NumHitsInCurHC << "   " <<endmsg;
    
 
    }
    
    


    return  StatusCode::SUCCESS ;
    

}


