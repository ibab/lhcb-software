// ============================================================================
/// CLHEP
#include "CLHEP/Geometry/Point3D.h"
/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"
/// GiGa
#include "GiGa/GiGaMACROs.h"
#include "GiGa/GiGaBase.h"
/// Geant4
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4TransportationManager.hh"
#include "G4Electron.hh"
#include "G4VProcess.hh"

#include "DetDesc/DetectorElement.h"
#include "GaussTools/GaussTrackInformation.h"

/// local
#include "RichSensDet.h"
#include "RichInfo.h"
#include "RichPEInfo.h"
#include "RichPhotoElectron.h"
// ============================================================================
/// factory business
// ============================================================================
IMPLEMENT_GiGaFactory( RichSensDet );
// ============================================================================

// ============================================================================
/** standard constructor
 *  @see GiGaSensDetBase
 *  @see GiGaBase
 *  @see AlgTool
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
RichSensDet::RichSensDet
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaSensDetBase     ( type , name , parent )
  , G4VSensitiveDetector( name  )
{

  IDataProviderSvc* detSvc;
  if ( svcLoc()->service( "DetectorDataSvc" , detSvc , true ) ) {

    m_RichGeomProperty= new
      RichG4GeomProp(detSvc,msgSvc()) ;

    m_RichG4HCName= new RichG4HitCollName();
    m_NumberOfHCInRICH=m_RichG4HCName->NumberOfHCollectionInRICH();
    m_HpdHCID.reserve(m_NumberOfHCInRICH);
    m_RichHC.reserve(m_NumberOfHCInRICH);

    G4String HCName;
    for(int ihc=0; ihc<m_RichG4HCName->RichHCSize(); ++ihc ) {
      HCName=m_RichG4HCName->RichHCName(ihc);
      collectionName.push_back(HCName);
      m_HpdHCID.push_back(-1);
    }

  }

};
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
RichSensDet::~RichSensDet(){};
// ============================================================================

// ============================================================================
/** process the hit
 *  @param step     pointer to current Geant4 step
 *  @param history  pointert to touchable history
 */
// ============================================================================

bool RichSensDet::ProcessHits( G4Step* aStep ,
                               G4TouchableHistory* )
{
  if( 0 == aStep ) { return false ; }

  double CurEdep = aStep->GetTotalEnergyDeposit();

  // Create a hit only when there is
  // non-zero energy deposit.  SE June 2003.

  // cout<<"Rich SensDet CurEdep "<< CurEdep<<endl;

  //if ( CurEdep <= 0.1 ) { return false; }
  if(  CurEdep <= 0.001 ) { return false; }

  // end of Modif in June 2003 by SE.

  G4StepPoint* prePosPoint =aStep->GetPreStepPoint();
  const G4ThreeVector & CurGlobalPos  = prePosPoint->GetPosition();

  ///
  G4TouchableHistory* CurTT =
    (G4TouchableHistory*)(prePosPoint->GetTouchable());

  G4VPhysicalVolume*  CurPV =   CurTT->GetVolume();
  G4LogicalVolume*    CurLV =   CurPV->GetLogicalVolume();
  // Now get the charged track (ie. photoelectron )  which created the
  // hit.
  G4Track* aTrack = aStep->GetTrack();
  G4ThreeVector CurPEOrigin;
  // log << MSG::VERBOSE <<" Track Def  and creator proc "
  // << aTrack->GetDefinition()
  // <<"    "<<aTrack->GetCreatorProcess()-> GetProcessName() <<endreq;

  G4String aCreatorProcessName = "NullProcess";
  const G4VProcess* aProcess = aTrack->GetCreatorProcess();
  if(aProcess) aCreatorProcessName =  aProcess->GetProcessName();
  if(((aTrack->GetDefinition() == G4Electron::Electron()) ||
      (aTrack->GetDefinition() == RichPhotoElectron::PhotoElectron())) &&
     (aCreatorProcessName  == "RichHpdPhotoelectricProcess"))
  {
    CurPEOrigin = aTrack->GetVertexPosition() ;
  }

  MsgStream log( msgSvc() , name() );

  log << MSG::DEBUG << "Now in ProcessHits() for RichSensDet"
      << " Pos=("  << CurGlobalPos.x() << "," << CurGlobalPos.y()
      << "," << CurGlobalPos.z() << ")"
      << " PV="    << CurPV->GetName()
      << " LV="    << CurLV->GetName()
      << " edep in MeV ="  << CurEdep << endreq;
  log << MSG::DEBUG << " PE Origin X Y Z "<<CurPEOrigin.x()
      <<"   "<<CurPEOrigin.y()<<"   "<<CurPEOrigin.z()<<endreq;

  //  G4double CurGlobalX=CurGlobalPos.x();
  //  G4double CurGlobalY=CurGlobalPos.y();
  G4double CurGlobalZ=CurGlobalPos.z();

  int CurrentRichDetNumber =-1;

  if( CurGlobalZ <= 0.0 ) {
    log << MSG::ERROR << "Inadmissible Rich Hit Z coordinate = "
        <<  CurGlobalZ <<endreq;
  }else if ( CurGlobalZ < MaxZHitInRich1Detector() ) {
    // hit coordinate in Rich1
    CurrentRichDetNumber=0;
  }else {
    // hit coordinate in Rich2
    CurrentRichDetNumber= 1;
  }
 // the following modif done for the new G4 version. SE Nov,2005.
  
  //  CurTT -> MoveUpHistory(2);
  // G4int CurrentHpdNumber= CurTT -> GetVolume() -> GetCopyNo() ;
  //  CurTT -> MoveUpHistory(1);


  G4int CurrentHpdNumber= CurTT -> GetReplicaNumber(2);
  G4int CurrentRichDetSector = CurTT ->GetReplicaNumber(3);

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
  G4ThreeVector CurPEOriginLocal = theNavigator->
    GetGlobalToLocalTransform().
    TransformPoint(CurPEOrigin);

  log << MSG::DEBUG << "Now in ProcessHits() of RichSensDet LocalPos X Y Z "
      <<CurLocalPos.x() <<"  "<<CurLocalPos.y()<<"   "<<CurLocalPos.z()<<endreq;


  G4int CurrentPixelXNum=  PixelXNum(CurLocalPos.x());
  G4int CurrentPixelYNum=  PixelYNum(CurLocalPos.y());
  log << MSG::DEBUG <<
    "Now in ProcessHits() of RichSensDet : PixelX and Y = " <<
    CurrentPixelXNum << "   " << CurrentPixelYNum<<endreq;

  //  G4int CurOptPhotID= aTrack->GetParentID();

  G4double CurGlobalTime =  aTrack-> GetGlobalTime();



  G4int CurOptPhotMotherChTrackID=-1;
  G4int CurOptPhotMotherChTrackPDG=0;
  G4int CurRadiatorNumber=-1;
  G4ThreeVector CurEmissPt;
  G4double CurPhotEnergyAtProd=0;;
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


  if( ( (aTrack->GetDefinition() == G4Electron::Electron()) ||
        (aTrack->GetDefinition() == RichPhotoElectron::PhotoElectron()))  &&
      (aCreatorProcessName  == "RichHpdPhotoelectricProcess")) {

    G4VUserTrackInformation* aUserTrackinfo=aTrack->GetUserInformation();
    GaussTrackInformation* aRichPETrackInfo
      = (GaussTrackInformation*)aUserTrackinfo;

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
            CurOptPhotID        =  aPEInfo->OptPhotonId();

            // log << MSG::INFO << "Now in ProcessHits()  "
            //  <<" Track id of charged tk opt phot pe "
            //    << CurOptPhotMotherChTrackID <<"   "
            //    <<  CurOptPhotID<<"   "
            //    << aTrack->GetTrackID() << endreq;

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

            }


          }
        }
      }
    }
  }


  G4int CurPETrackID=aTrack->GetTrackID();
  G4int CurPETrackPDG=aTrack->GetDefinition()->GetPDGEncoding();

  RichG4Hit * newHit = new RichG4Hit();
  newHit -> SetEdep( CurEdep);
  newHit -> SetGlobalPos( CurGlobalPos );
  newHit -> SetCurHpdNum(CurrentHpdNumber);
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

  // for now the trackID from the Gausshit base class.
  // if the mother of the corresponding optical photon exists it is set
  // as the trackid. Otherwise the track creating the
  // hit is set as the track id/
  if ( CurOptPhotMotherChTrackID >=0 ) {
    newHit ->setTrackID(CurOptPhotMotherChTrackID);
  } else {
    newHit ->setTrackID(CurPETrackID);
  }

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
  }

  if ( CurrentRichCollectionSet >= 0 ) {
    int NumHitsInCurHC =m_RichHC[CurrentRichCollectionSet] ->insert( newHit );
    log << MSG::DEBUG
        << "RichSensdet: Current collection set and Hit number stored = "
        << CurrentRichCollectionSet << "  " << NumHitsInCurHC << endreq;
  }

  return true;
};
void RichSensDet::clear() {  };
void RichSensDet::DrawAll() {  };
void RichSensDet::PrintAll() {  };
void RichSensDet::Initialize(G4HCofThisEvent* ) {

  MsgStream log( msgSvc() , name() );

  log << MSG::DEBUG << "Richsensdet: Initialize. SensDetName, colName: "
      <<SensitiveDetectorName<<"  "<<collectionName[0]
      <<"  "<<collectionName[1]<<"  "
      <<collectionName[2]<<"  "<<collectionName[3]<<endreq;

  // G4String CurCollName;
  RichG4HitsCollection* CurColl;
  m_RichHC.clear();
  for(int ihhc=0; ihhc<m_RichG4HCName->RichHCSize(); ++ihhc ) {
    //    CurCollName=collectionName[ihhc];
    CurColl =
      new  RichG4HitsCollection(SensitiveDetectorName,collectionName[ihhc]);

    m_RichHC.push_back( CurColl);
  }


};
void RichSensDet::EndOfEvent(G4HCofThisEvent* HCE) {

  for(int ihid=0; ihid<m_RichG4HCName->RichHCSize(); ++ihid ) {

    if(m_HpdHCID[ihid] < 0  )
    {
      m_HpdHCID[ihid] = G4SDManager::GetSDMpointer()->
        GetCollectionID(collectionName[ihid]);
    }

    HCE->AddHitsCollection( m_HpdHCID[ihid] , m_RichHC[ihid]  );
  }

};

// ============================================================================
// The END
// ============================================================================







