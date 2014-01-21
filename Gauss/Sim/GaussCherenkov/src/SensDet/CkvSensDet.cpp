//$Id: CkvSensDet.cpp,v 1.24 2009-07-17 13:46:13 jonrob Exp $
// Include files

// from CLHEP
#include "CLHEP/Geometry/Point3D.h"

// Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"

// GiGa
#include "GiGa/GiGaBase.h"

// Geant4
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
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

// local
#include "GaussCherenkov/CkvSensDet.h"
#include "GaussRICH/RichInfo.h"
#include "GaussRICH/RichPEInfo.h"
#include "GaussRICH/RichPhotoElectron.h"
#include "GaussCherenkov/CkvG4Hit.h"
#include "GaussCherenkov/CherenkovPmtLensUtil.h"
#include "GaussCherenkov/CkvGeometrySetupUtil.h"
using namespace std;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CkvSensDet );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CkvSensDet::CkvSensDet( const std::string& type   ,
                          const std::string& name   ,
                          const IInterface*  parent )
  : G4VSensitiveDetector ( name  ),  
    GiGaSensDetBase      ( type , name , parent ),
    m_RichPmtAviodDuplicateHitsActivate(false),
    m_RichPmtFlagDuplicateHitsActivate(true),
    m_TotNumPmtsInRich(5000),
    m_RichPmtAlreadyHit(std::vector<bool>(m_TotNumPmtsInRich))
{
 // the above dummey value 5000 is set to the correct value below.

  declareProperty("RichPmtAviodDuplicateHitsActivate", m_RichPmtAviodDuplicateHitsActivate);
  declareProperty("RichPmtFlagDuplicateHitsactivate", m_RichPmtFlagDuplicateHitsActivate);



}

//=============================================================================
// Destructor
//=============================================================================
CkvSensDet::~CkvSensDet(){  
  

}
//=============================================================================
// initialize
//=============================================================================
StatusCode CkvSensDet::initialize() 
{
  StatusCode sc = GiGaSensDetBase::initialize();
  if (sc.isFailure()) return sc;

  IDataProviderSvc* detSvc;
  if ( svcLoc()->service( "DetectorDataSvc" , detSvc , true ) ) {

    m_RichGeomProperty= new
      CkvG4GeomProp(detSvc,msgSvc()) ;

    CkvGeometrySetupUtil * aCkvGeometrySetup= CkvGeometrySetupUtil::getCkvGeometrySetupUtilInstance();
   m_SuperRichFlag = aCkvGeometrySetup ->isSuperRich();
   m_OptHorizRichFlag =  aCkvGeometrySetup ->hasOptimizedHorizontalRich1();
   m_Rich2UseGrandPmt= aCkvGeometrySetup ->Rich2_UseGrandPmt();
   //G4cout<<"CkvSensDet :  m_Rich2Use Grand pmt "<<m_Rich2UseGrandPmt<<G4endl;
   
   
    
    m_RichG4HCName= new RichG4HitCollName();
    if(m_SuperRichFlag) m_RichG4HCName->setCollConfigWithSuperRich();
    m_NumberOfHCInRICH=m_RichG4HCName->NumberOfHCollectionInRICH();
    // G4cout<<" Number of HC in SuperRich superrichglag  "<< m_NumberOfHCInRICH<< "  "<<
    //  m_SuperRichFlag<<  G4endl;
    
    m_PhdHCID.reserve(m_NumberOfHCInRICH);
    m_RichHC.reserve(m_NumberOfHCInRICH);

    G4String HCName;


    for(int ihc=0; ihc<m_RichG4HCName->RichHCSize(); ++ihc ) {
      HCName=(m_RichG4HCName->RichHCName(ihc));
      collectionName.push_back(HCName);
      m_PhdHCID.push_back(-1);

    }
    G4int m_TotNumPmtsInRich =(!m_SuperRichFlag ) ?  (( m_RichGeomProperty-> NumberOfPMTsInRich1() ) +
                                                      ( m_RichGeomProperty-> NumberOfPMTsInRich2())) 
                                                      : m_RichGeomProperty->NumberOfPmtsInSuperRich() ;

    m_RichPmtAlreadyHit.resize(m_TotNumPmtsInRich);
    ResetPmtMapInCurrentEvent();

    
  }
  

  
  return sc;
}
//=============================================================================
// finalize
//=============================================================================
StatusCode CkvSensDet::finalize() 
{
  std::cout<<" Now in CkvSensDet finalize "<<std::endl;
  
  return GiGaSensDetBase::finalize();  
  
}


//=============================================================================
// process the hit (G4VSensitiveDetector method
//  step     pointer to current Geant4 step
//  history  pointert to touchable history
//=============================================================================
bool CkvSensDet::ProcessHits( G4Step* aStep ,
                               G4TouchableHistory* )
{
  if( 0 == aStep ) { return false ; }

  double CurEdep = aStep->GetTotalEnergyDeposit();

  // Create a hit only when there is
  // non-zero energy deposit.  SE June 2003.

  //   G4cout<<"Rich SensDet CurEdep "<< CurEdep<<G4endl;

  //if ( CurEdep <= 0.1 ) { return false; }
  if(  CurEdep <= 0.001 ) { return false; }

  // end of Modif in June 2003 by SE.

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
  // Check that tracks are charged - GC & SE April 2006
  if( aTrack->GetDefinition()->GetPDGCharge() == 0.0 ) {
    return false;
  }

  G4ThreeVector CurPEOrigin;
  // log << MSG::VERBOSE <<" Track Def  and creator proc "
  // << aTrack->GetDefinition()
  // <<"    "<<aTrack->GetCreatorProcess()-> GetProcessName() <<endreq;

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

  MsgStream log( msgSvc() , name() );

  // log << MSG::INFO << "Now in ProcessHits() for CkvSensDet"
  //    << " Pos=("  << CurGlobalPos.x() << "," << CurGlobalPos.y()
  //    << "," << CurGlobalPos.z() << ")"
  //    << " PV="    << CurPV->GetName()
  //    << " LV="    << CurLV->GetName()
  //    << " edep in MeV ="  << CurEdep << endreq;
  // log << MSG::INFO << " PE Origin X Y Z "<<CurPEOrigin.x()
  //     <<"   "<<CurPEOrigin.y()<<"   "<<CurPEOrigin.z()<<endreq;

  G4double CurGlobalZ=CurGlobalPos.z();


  int CurrentRichDetNumber =-1;


  if( CurGlobalZ <= 0.0 ) {
    log << MSG::ERROR << "Inadmissible Rich Hit Z coordinate = "
        <<  CurGlobalZ <<endreq;
  }else {
    
    if(m_SuperRichFlag) {
      
           CurrentRichDetNumber=2;
    
    }else {
      
    

     if ( CurGlobalZ < MaxZHitInRich1Detector() ) {
           // hit coordinate in Rich1
         CurrentRichDetNumber=0;
      }else {
      // hit coordinate in Rich2
       CurrentRichDetNumber= 1;
     }
    }
    
    
  }
  


  
  G4int CurrentPmtNumber= CurTT -> GetReplicaNumber(2);
  G4int CurrentPmtModuleNumber=  CurTT -> GetReplicaNumber(3);

   bool CurModuleWithLensFlag = false;
   if( m_SuperRichFlag  ) { 
      CurModuleWithLensFlag=true; 
   }else {
    CherenkovPmtLensUtil * aCherenkovPmtLensUtil =  CherenkovPmtLensUtil::getInstance();   
    CurModuleWithLensFlag= aCherenkovPmtLensUtil->isPmtModuleWithLens(CurrentPmtModuleNumber);
   }
   
   // log<< MSG::VERBOSE<<" SensDet PmtNumber Module RichDet Number  LensFlag "<< CurrentPmtNumber <<"   "
   //   << CurrentPmtModuleNumber<<"  "<<CurrentRichDetNumber<<"  "<<CurModuleWithLensFlag<< endreq;

   G4int curSuperRichDet =0;
   
   if( m_SuperRichFlag) curSuperRichDet =1;

   G4int curHorizRich1Det =0;
   if(m_OptHorizRichFlag && (CurrentRichDetNumber ==0)) curHorizRich1Det=1;
   
   
  
  G4int CurrentRichDetSector = CurTT ->GetReplicaNumber(4);

  if(CurrentRichDetSector < 0 || CurrentRichDetSector > 1 ) {
    log << MSG::ERROR <<"Inadmisible Rich Det Sector=  "<<CurrentRichDetSector
        <<"  Current RichDetNum =   "<<CurrentRichDetNumber<<endreq;
  }

  G4Navigator* theNavigator =
    G4TransportationManager::GetTransportationManager()->
    GetNavigatorForTracking();
  G4ThreeVector CurLocalPos = theNavigator->
    GetGlobalToLocalTransform().
    TransformPoint(CurGlobalPos);




  // The following is a dummy initial value , valid 
  // for the case where there is no Si chip shift from hpd axis.
  // It is modfied with the correct value few lines below, with the value stored from
  // when the pe was created. SE 6-4-2010
  G4ThreeVector CurPEOriginLocal = theNavigator->
    GetGlobalToLocalTransform().
    TransformPoint(CurPEOrigin);

  

  //test print
  //if(CurrentRichDetNumber == 0 ) {
  //  
  //  log << MSG::DEBUG << "Now in ProcessHits() of CkvSensDet LocalPos X Y Z "
  //    <<CurLocalPos.x() <<"  "<<CurLocalPos.y()<<"   "<<CurLocalPos.z()<<endreq;

  //         log << MSG::DEBUG << "Now in ProcessHits() of CkvSensDet Global Pos X Y Z "
  //   <<CurGlobalPos.x() <<"  "<<CurGlobalPos.y()<<"   "<<CurGlobalPos.z()<<endreq;

  //         log << MSG::DEBUG<<" Pe origin localxyz global xyz "<< CurPEOriginLocal.x()<<"   "
  //     <<CurPEOriginLocal.y()<<"   "<< CurPEOriginLocal.z()<<"   "
  //    <<CurPEOrigin.x()<<"   "<<CurPEOrigin.y()<<"   "<<CurPEOrigin.z()<<"   "<<endreq;
    
  // }
  
  // end test print
  
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
  
  
  


  //log << MSG::VERBOSE <<
  //  "Now in ProcessHits() of CkvSensDet : RichDetNum  localpos PixelX and Y Rich2Grandpmt  = " <<CurrentRichDetNumber 
  //    <<"   "<<CurLocalPos<< "  "<<
  //   CurrentPixelXNum << "   " << CurrentPixelYNum<<"   "<< m_Rich2UseGrandPmt<< endmsg;

  //  G4int CurOptPhotID= aTrack->GetParentID();   
     
     

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
  






  
  //  if(CurModuleWithLensFlag )CurpdLens=1;
  if( m_SuperRichFlag) CurpdLens=1;


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
   
            //log << MSG::VRBOSE << "Now in ProcessHits()  "
            //    <<" Track id of charged tk opt phot pe "
            //    << CurOptPhotMotherChTrackID <<"   "
            //    <<  CurOptPhotID<<"   "
            //    << aTrack->GetTrackID() << endreq;

            if( CurElectronBackScatFlag > 0) {
              log << MSG::DEBUG << "Now in CkvSensDet ProcessHits() backscattered eln  "
                  << CurElectronBackScatFlag << endreq;
            }
            //log<<MSG::INFO<<" Now in processHits Photon source info "<<CurPhotonSourceProcInfo
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

  CkvG4Hit * newHit = new CkvG4Hit();
  newHit -> SetEdep( CurEdep);
  newHit -> SetGlobalPos( CurGlobalPos );
  newHit -> SetCurSectorNum(CurrentRichDetSector);
  newHit -> SetCurRichDetNum(CurrentRichDetNumber);
  newHit -> SetLocalPos( CurLocalPos );
  newHit -> SetGlobalPEOriginPos( CurPEOrigin) ;
  newHit -> SetLocalPEOriginPos( CurPEOriginLocal) ;
  newHit -> SetCurPixelXNum(CurrentPixelXNum);
  newHit -> SetCurPixelYNum(CurrentPixelYNum);
  newHit -> SetOptPhotID(CurOptPhotID);
  newHit -> SetChTrackID(CurOptPhotMotherChTrackID);
  newHit -> SetChTrackPDG(CurOptPhotMotherChTrackPDG);
  newHit -> SetRadiatorNumber( CurRadiatorNumber);
  newHit -> SetPhotEmisPt( CurEmissPt);
  newHit -> SetPhotEnergyAtProd(CurPhotEnergyAtProd);
  newHit -> SetThetaCkvAtProd(CurThetaCkvAtProd);
  newHit -> SetPhiCkvAtProd(CurPhiCkvAtProd);
  newHit -> SetChTrackTotMom(CurChTrackTotMom);
  newHit -> SetChTrackMomVect(CurChTrackMomVect);
  newHit -> SetPETrackID(CurPETrackID);
  newHit -> SetPETrackPDG(CurPETrackPDG);
  newHit -> SetRichHitGlobalTime(CurGlobalTime);
  newHit -> SetRichChTrackMass ( CurChTrackPDGMass);
  newHit -> setChTrackCkvPreStepPos(CurChTrackCkvPreStepPos);
  newHit -> setChTrackCkvPostStepPos(CurChTrackCkvPostStepPos);
  newHit -> setOptPhotRayleighFlag(CurPhotRayleighScatFlag);
  newHit -> setOptPhotAgelExitPos(  CurPhotAgelExitPos);
  newHit -> setMirror1PhotonReflPosition(CurMirror1PhotonReflPosition);
  newHit -> setMirror2PhotonReflPosition(CurMirror2PhotonReflPosition);
  newHit -> setMirror1PhotonDetectorCopyNum(CurMirror1PhotonDetectorCopyNum);
  newHit -> setMirror2PhotonDetectorCopyNum(CurMirror2PhotonDetectorCopyNum);
  newHit -> setRichVerboseHitInfo(aRichVerboseFlag);
  newHit -> setRichHpdQW2PhCathReflFlag(CurHpdQW2PhCathReflFlag);
  newHit ->  setElectronBackScatterFlag(CurElectronBackScatFlag );
  newHit ->  setPhotoElectricProductionFlag(CurPhotoElectricFlag);
  newHit->   setRichHpdPhotonReflectionFlag(CurHpdReflectionFlag);
  newHit->   setHpdQuartzWindowExtSurfPhotIncidentPosition (CurHpdQwPhotIncidentPosition);
  newHit->   setHpdQuartzWindowExtSurfPhotIncidentLocalPosition ( CurQWExtSurfaceCoordLocal );  
  newHit ->  setPhotonSourceProcessInfo( CurPhotonSourceProcInfo );
  newHit ->  setpdWithLens(CurpdLens );
  newHit -> setPmtLensPhotIncidentPosition(CurLensIncidentPosition );
  newHit -> setPmtLensPhotIncidentLocalPosition ( CurLensExtSurfaceCoordLocal ); 

   newHit->SetCurModuleNum(CurrentPmtModuleNumber );
   newHit->SetCurHitInPixelGap(CurrentHitInPixelGap);
   newHit->SetCurPmtNum (CurrentPmtNumber);
   newHit-> setSuperRichHit( curSuperRichDet   );
   newHit-> setOptHorizontalRich1Hit(curHorizRich1Det );
   

  // for now the trackID from the Gausshit base class.
  // if the mother of the corresponding optical photon exists it is set
  // as the trackid. Otherwise the track creating the
  // hit is set as the track id. It also checks if the particle is produced
  // by the photoelectric process or the backscatteting inside hpd; in both cases the
  // mother of the corresponding optical photon used as trackid, if it exists.
  // if( CurElectronBackScatFlag > 0 ) {
  //  G4cout<<" RichsensDet Backsct eln Mothertrackid currentTrackid "<<CurOptPhotMotherChTrackID<<"  "<<CurPETrackID<<G4endl;
  //}
  // G4cout<<" RichsensDet superrich flgag pdlensflag "<<newHit->  SuperRichHit() <<"  "<<newHit->pdWithLens()<<G4endl;
   
  if(aRichPETrackInfo) aRichPETrackInfo->setCreatedHit(true);
  newHit ->setTrackID(CurOptPhotMotherChTrackID); // this covers all the normal cases
                                                  // like Charged track -> Cherenkov photon -> photoelectron-> hit
  //  photoelectron-> backscattered electron->hit.

  //     if( CurOptPhotMotherChTrackID == 0) { // this line fixed as below on Oct-24-2008 by SE to avoid mc hit without mc tracks.
  if( CurOptPhotMotherChTrackID <= 0) {           // this is for mip particle and the possible backscatted electrons from them.
    newHit ->setTrackID(CurPETrackID);
    if(aRichPETrackInfo) aRichPETrackInfo ->setToBeStored (true);
  }

  //    if( CurElectronBackScatFlag > 0 ) {
  // newHit ->setTrackID(CurPETrackID); //  photoelectron-> backscattered electron->hit.
  // if(aRichPETrackInfo) aRichPETrackInfo ->setToBeStored (true);
  // }





  //  if ( CurOptPhotMotherChTrackID >=0 ) {
  // Charged track -> Cherenkov photon -> photoelectron-> hit
  //  newHit ->setTrackID(CurOptPhotMotherChTrackID);
  // } else {
  // Charged track -> hit
  //   newHit ->setTrackID(CurPETrackID);
  //   if(aRichPETrackInfo) aRichPETrackInfo ->setToBeStored (true);
  //  }

  

  int CurrentRichCollectionSet=-1;
  
  if ( CurrentRichDetNumber == 0 ) {
    if ( CurrentRichDetSector == 0 ) {
      CurrentRichCollectionSet=0;

    } else {
      CurrentRichCollectionSet=1;
    }
  } else if ( CurrentRichDetNumber == 1 ) {
    if ( CurrentRichDetSector == 0 ) {
      CurrentRichCollectionSet=2;
    } else {
      CurrentRichCollectionSet=3;
    }
  }else if (CurrentRichDetNumber == 2 ) { 
  
    if ( CurrentRichDetSector == 0 ) {
      CurrentRichCollectionSet=0;

    } else {
      CurrentRichCollectionSet=1;
    }
    
  }
  
  
  

  if ( CurrentRichCollectionSet >= 0 ) {
    bool EnableThisHitStore=true;
    G4bool FlagThisHitAsDuplicate=false;
    //  log << MSG::VERBOSE<<" Avoid duplicate  Hits  "<< m_RichPmtAviodDuplicateHitsActivate
    //       <<"   "<<m_RichPmtFlagDuplicateHitsActivate<< endreq;
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
      log << MSG::VERBOSE<<" Rich PMT Duplicate hit "<<CurrentPmtNumber<<"  "<<CurPixelNumInPmt<<endreq;
    }
    

    // end test print


    newHit -> setCurrentHitAsDuplicate( FlagThisHitAsDuplicate);
    //int NumHitsInCurHC =0; // Unused variable
    
    if(EnableThisHitStore) {
      
      //NumHitsInCurHC =m_RichHC[CurrentRichCollectionSet] ->insert( newHit ); // Unused variable

      if(m_RichPmtAviodDuplicateHitsActivate || m_RichPmtFlagDuplicateHitsActivate ) { 
         m_RichPmtAlreadyHit[CurrentPmtNumber]=true;
         m_RichPmtToPixelNumMap.insert(pair<G4int,G4int>(CurrentPmtNumber, CurPixelNumInPmt));
      }
      
      
    }
    
     
    // log << MSG::VERBOSE
    //   << "CkvSensdet: Current collection set AuxSet and Hit number stored = "
    //   << CurrentRichCollectionSet << "  " 
    //   <<NumHitsInCurHC << "   " <<endmsg;
        
  }
  
    
  

  return true;
}

  

//=============================================================================
// clear (G4VSensitiveDetector method)
//=============================================================================
void CkvSensDet::clear() {  }

//=============================================================================
// DrawAll (G4VSensitiveDetector method)
//=============================================================================
void CkvSensDet::DrawAll() {  }

//=============================================================================
// PrintAll (G4VSensitiveDetector method)
//=============================================================================
void CkvSensDet::PrintAll() {  }

//=============================================================================
//  (G4VSensitiveDetector method)
//=============================================================================
void CkvSensDet::Initialize(G4HCofThisEvent*  HCE) {

  MsgStream log( msgSvc() , name() );

  //  log << MSG::DEBUG << "Cherenkovsensdet: Initialize. SensDetName, colName: "
  //    <<SensitiveDetectorName<<"  "<<collectionName[0]
  //    <<"  "<<collectionName[1]<<"  "
  //    <<collectionName[2]<<"  "<<collectionName[3]<<endreq;

  // G4String CurCollName;
  CkvG4HitsCollection* CurColl;
  m_RichHC.clear();

  for(int ihhc=0; ihhc<m_RichG4HCName->RichHCSize(); ++ihhc ) {

    CurColl =
      new  CkvG4HitsCollection(SensitiveDetectorName,collectionName[ihhc]);

    m_RichHC.push_back(CurColl);

    if(m_PhdHCID[ihhc] < 0  ){
      m_PhdHCID[ihhc] = G4SDManager::GetSDMpointer()->
        GetCollectionID(collectionName[ihhc]);
    }

    HCE->AddHitsCollection( m_PhdHCID[ihhc] , m_RichHC[ihhc]  );
  }

    ResetPmtMapInCurrentEvent();

}


//=============================================================================
// EndOfEvent (G4VSensitiveDetector method) commented out and moved all the contents to initialize
//=============================================================================
//void CkvSensDet::EndOfEvent(G4HCofThisEvent* HCE) {

//  for(int ihid=0; ihid<m_RichG4HCName->RichHCSize(); ++ihid ) {
//  G4cout<<" richsensdet collectionname endevent "<<ihid<<"  "<<collectionName[ihid]<<"  "<< ihid
//   <<"  "<< m_HpdHCID[ihid] << G4endl;
//
//
//  if(m_HpdHCID[ihid] < 0  )
//  {
//    m_HpdHCID[ihid] = G4SDManager::GetSDMpointer()->
//      GetCollectionID(collectionName[ihid]);
//  }
//  G4cout<<" richsensdet collectionname "<<ihid<<"  "<<collectionName[ihid]<<"   "<< m_HpdHCID[ihid]<< G4endl;
//
//
//    HCE->AddHitsCollection( m_HpdHCID[ihid] , m_RichHC[ihid]  );
// }

//}

//=============================================================================
void CkvSensDet::ResetPmtMapInCurrentEvent() {
    m_RichPmtAlreadyHit.assign(m_TotNumPmtsInRich,false);
    m_RichPmtToPixelNumMap.clear();


}










