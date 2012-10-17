// $Id: RichSensDet.cpp,v 1.24 2009-07-17 13:46:13 jonrob Exp $
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
#include "RichSensDet.h"
#include "RichInfo.h"
#include "RichPEInfo.h"
#include "RichPhotoElectron.h"

using namespace std;

//-----------------------------------------------------------------------------
// Implementation file for class : RichSensDet
//
// 2007-01-11 : Gloria Corti, adapt to Gaudi v19 (compatible also with v18)
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RichSensDet );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichSensDet::RichSensDet( const std::string& type   ,
                          const std::string& name   ,
                          const IInterface*  parent )
  : G4VSensitiveDetector ( name  ),  
    GiGaSensDetBase      ( type , name , parent ),
    m_RichAviodDuplicateHitsActivate(false),
    m_RichFlagDuplicateHitsActivate(false),
    m_TotNumHpdsInRich(500),         
    m_RichHpdAlreadyHit(std::vector<bool>(m_TotNumHpdsInRich))
{
  // the above dummey value 500 is set to the correct value below.

  declareProperty("RichAviodDuplicateHitsActivate", m_RichAviodDuplicateHitsActivate);
  declareProperty("RichFlagDuplicateHitsactivate", m_RichFlagDuplicateHitsActivate);
  
}

//=============================================================================
// Initialize, method of GiGaSensDetBase
//=============================================================================
StatusCode RichSensDet::initialize() {
  StatusCode sc = GiGaSensDetBase::initialize();
  if (sc.isFailure()) return sc;

  
  IDataProviderSvc* detSvc;
  if ( svcLoc()->service( "DetectorDataSvc" , detSvc , true ) ) {

    m_RichGeomProperty= new RichG4GeomProp(detSvc,msgSvc()) ;

    m_RichG4HCName= new RichG4HitCollName();
    m_NumberOfHCInRICH=m_RichG4HCName->NumberOfHCollectionInRICH();
    m_HpdHCID.reserve(m_NumberOfHCInRICH);
    m_RichHC.reserve(m_NumberOfHCInRICH);

    G4String HCName;


    for(int ihc=0; ihc<m_RichG4HCName->RichHCSize(); ++ihc ) {
      HCName=(m_RichG4HCName->RichHCName(ihc));
      collectionName.push_back(HCName);
      m_HpdHCID.push_back(-1);
      
    }
  
    G4int m_TotNumHpdsInRich =   ( m_RichGeomProperty-> NumberOfHPDsInRich1()) +
                          ( m_RichGeomProperty-> NumberOfHPDsInRich2());
    
    m_RichHpdAlreadyHit.resize(m_TotNumHpdsInRich);
    ResetHpdMapInCurrentEvent();
    
    
  } else {
    return StatusCode::FAILURE;
  }

  return sc;
}

//=============================================================================
// Finalize, method of GiGaSensDetBase
//=============================================================================
StatusCode RichSensDet::finalize() {

  return GiGaSensDetBase::finalize();

}

//=============================================================================
// Destructor
//=============================================================================
RichSensDet::~RichSensDet(){}

//=============================================================================
// process the hit (G4VSensitiveDetector method
//  step     pointer to current Geant4 step
//  history  pointert to touchable history
//=============================================================================
bool RichSensDet::ProcessHits( G4Step* aStep ,
                               G4TouchableHistory* )
{
  if( 0 == aStep ) { return false ; }

  double CurEdep = aStep->GetTotalEnergyDeposit();

  // Create a hit only when there is
  // non-zero energy deposit.  SE June 2003.

  // G4cout<<"Rich SensDet CurEdep "<< CurEdep<<G4endl;

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
       ( aCreatorProcessName  == "RichHpdSiEnergyLossProcess")) )
  {
    CurPEOrigin = aTrack->GetVertexPosition() ;
  }

  MsgStream log( msgSvc() , name() );

  log << MSG::VERBOSE << "Now in ProcessHits() for RichSensDet"
      << " Pos=("  << CurGlobalPos.x() << "," << CurGlobalPos.y()
      << "," << CurGlobalPos.z() << ")"
      << " PV="    << CurPV->GetName()
      << " LV="    << CurLV->GetName()
      << " edep in MeV ="  << CurEdep << endreq;
  log << MSG::VERBOSE << " PE Origin X Y Z "<<CurPEOrigin.x()
      <<"   "<<CurPEOrigin.y()<<"   "<<CurPEOrigin.z()<<endreq;

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
  // The following is a dummy initial value , valid 
  // for the case where there is no Si chip shift from hpd axis.
  // It is modfied with the correct value few lines below, with the value stored from
  // when the pe was created. SE 6-4-2010
  G4ThreeVector CurPEOriginLocal = theNavigator->
    GetGlobalToLocalTransform().
    TransformPoint(CurPEOrigin);

  log << MSG::VERBOSE << "Now in ProcessHits() of RichSensDet LocalPos X Y Z "
      <<CurLocalPos.x() <<"  "<<CurLocalPos.y()<<"   "<<CurLocalPos.z()<<endreq;


  G4int CurrentPixelXNum=  PixelXNum(CurLocalPos.x());
  G4int CurrentPixelYNum=  PixelYNum(CurLocalPos.y());
  log << MSG::VERBOSE <<
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
  G4int CurHpdQW2PhCathReflFlag=0;
  G4int CurElectronBackScatFlag=0;
  G4int CurPhotoElectricFlag=0;
  G4int CurHpdReflectionFlag=0;
  G4ThreeVector CurHpdQwPhotIncidentPosition;
  G4int CurPhotonSourceProcInfo=0;
  


  G4VUserTrackInformation* aUserTrackinfo=aTrack->GetUserInformation();
  GaussTrackInformation* aRichPETrackInfo
    = (GaussTrackInformation*)aUserTrackinfo;

  if( ( (aTrack->GetDefinition() == G4Electron::Electron()) ||
        (aTrack->GetDefinition() == RichPhotoElectron::PhotoElectron()))  &&
      (( aCreatorProcessName  == "RichHpdPhotoelectricProcess")  ||
       ( aCreatorProcessName  == "RichHpdSiEnergyLossProcess") ) ) {
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
            
   
            log << MSG::VERBOSE << "Now in ProcessHits()  "
                <<" Track id of charged tk opt phot pe "
                << CurOptPhotMotherChTrackID <<"   "
                <<  CurOptPhotID<<"   "
                << aTrack->GetTrackID() << endreq;
            if( CurElectronBackScatFlag > 0) {
              log << MSG::VERBOSE << "Now in RichSensDet ProcessHits() backscattered eln  "
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
  newHit -> setRichHpdQW2PhCathReflFlag(CurHpdQW2PhCathReflFlag);
  newHit ->  setElectronBackScatterFlag(CurElectronBackScatFlag );
  newHit ->  setPhotoElectricProductionFlag(CurPhotoElectricFlag);
  newHit->   setRichHpdPhotonReflectionFlag(CurHpdReflectionFlag);
  newHit->   setHpdQuartzWindowExtSurfPhotIncidentPosition (CurHpdQwPhotIncidentPosition);
  newHit ->  setPhotonSourceProcessInfo( CurPhotonSourceProcInfo );
  
  
  // for now the trackID from the Gausshit base class.
  // if the mother of the corresponding optical photon exists it is set
  // as the trackid. Otherwise the track creating the
  // hit is set as the track id. It also checks if the particle is produced
  // by the photoelectric process or the backscatteting inside hpd; in both cases the
  // mother of the corresponding optical photon used as trackid, if it exists.
  // if( CurElectronBackScatFlag > 0 ) {
  //  G4cout<<" RichsensDet Backsct eln Mothertrackid currentTrackid "<<CurOptPhotMotherChTrackID<<"  "<<CurPETrackID<<G4endl;
  //}

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
  }

  if ( CurrentRichCollectionSet >= 0 ) {
    bool EnableThisHitStore=true;
    G4bool FlagThisHitAsDuplicate=false;
    log << MSG::VERBOSE<<" Avoid duplicate  Hits  "<< m_RichAviodDuplicateHitsActivate
           <<"   "<<m_RichFlagDuplicateHitsActivate<< endreq;
    G4int CurPixelNumInHpd = m_RichGeomProperty ->GetPixelNumInHpd( CurrentPixelXNum, CurrentPixelYNum);
    
    if(m_RichAviodDuplicateHitsActivate || m_RichFlagDuplicateHitsActivate  ) { 
      if(  m_RichHpdAlreadyHit[CurrentHpdNumber] ) {
        pair<multimap<G4int, G4int>::iterator, multimap<G4int,G4int>::iterator> ppp= 
                                    m_RichHpdToPixelNumMap.equal_range(CurrentHpdNumber);
        multimap<G4int,G4int>::iterator ipm= ppp.first;
        while( (ipm !=ppp.second ) &&   ( EnableThisHitStore ) ) {
          if((*ipm).second == CurPixelNumInHpd) {
               FlagThisHitAsDuplicate=true;
               if(m_RichAviodDuplicateHitsActivate )EnableThisHitStore=false;
          }
          ++ipm;
        }
      }// end test hpd already hit
      
    
    

    //test print for debug
      if(FlagThisHitAsDuplicate ) {
        
        //log << MSG::VERBOSE<<"Current Rich Hpd hit is duplicate HpdNum Pixel PixelX PixelY "<<CurrentHpdNumber<<"  "
        //   << CurPixelNumInHpd  <<"   "<<  CurrentPixelXNum <<"   "<<CurrentPixelYNum<<endreq;
      //  G4int nHitInCurColl = (m_RichHC[CurrentRichCollectionSet])->entries();
      //  int iha=0;
      //  bool foundDup=false;
      //  while( (iha<nHitInCurColl) && !(foundDup) ) {
      //    RichG4Hit* aHitEx =( * m_RichHC[CurrentRichCollectionSet]) [iha];
      //    G4int aPixelXNumEx = aHitEx-> GetCurPixelXNum();
      //    G4int aPixelYNumEx = aHitEx-> GetCurPixelYNum();
      //    G4int anHpdNumEx =    aHitEx-> GetCurHpdNum();
      //    G4int aRichDetNumEx = aHitEx->  GetCurRichDetNum();
      //    if( (aRichDetNumEx == CurrentRichDetNumber) && (anHpdNumEx == CurrentHpdNumber) && 
      //        ( aPixelXNumEx == CurrentPixelXNum) && ( aPixelYNumEx == CurrentPixelYNum)) {
      //      foundDup=true;
      //      log << MSG::INFO<<" Duplicate Hit number Det hpd pixelXY "<< iha << "  "<<aRichDetNumEx <<"   "
      //          <<  anHpdNumEx<<"   "<< aPixelXNumEx<<"    "<<aPixelYNumEx<<endreq;
      //    }  
      //    iha++;
      //  }
      //  if( !(foundDup) ) {
      //    log << MSG::INFO<<"DuplicateHpd NotFound Please check "<<endreq;
      //    
      //  }
      //  
      }
      
      // end of test print
      
    }// end test activation of avoiding or flagging hits as duplicate .
    
    
    

    int NumHitsInCurHC=0;

    newHit -> setCurrentHitAsDuplicate( FlagThisHitAsDuplicate);
    
    if(EnableThisHitStore) {
      NumHitsInCurHC =m_RichHC[CurrentRichCollectionSet] ->insert( newHit );
    
    if(m_RichAviodDuplicateHitsActivate || m_RichFlagDuplicateHitsActivate ) { 
      m_RichHpdAlreadyHit[CurrentHpdNumber]=true;
      m_RichHpdToPixelNumMap.insert(pair<G4int,G4int>(CurrentHpdNumber, CurPixelNumInHpd));
    }
    
    }
    
    

    log << MSG::VERBOSE
        << "RichSensdet: Current collection set and Hit number stored  ActivationOfRepatedHits  = "
        << CurrentRichCollectionSet << "  " << NumHitsInCurHC << "    "<<EnableThisHitStore<< endreq;
    
    
  }// end test on collection set existance
  

  return true;
}



//=============================================================================
// clear (G4VSensitiveDetector method)
//=============================================================================
void RichSensDet::clear() {  }

//=============================================================================
// DrawAll (G4VSensitiveDetector method)
//=============================================================================
void RichSensDet::DrawAll() {  }

//=============================================================================
// PrintAll (G4VSensitiveDetector method)
//=============================================================================
void RichSensDet::PrintAll() {  }

//=============================================================================
//  (G4VSensitiveDetector method)
//=============================================================================
void RichSensDet::Initialize(G4HCofThisEvent*  HCE) {

  MsgStream log( msgSvc() , name() );

  log << MSG::VERBOSE << "Richsensdet: Initialize. SensDetName, colName: "
      <<SensitiveDetectorName<<"  "<<collectionName[0]
      <<"  "<<collectionName[1]<<"  "
      <<collectionName[2]<<"  "<<collectionName[3]<<endreq;

  // G4String CurCollName;
  RichG4HitsCollection* CurColl;
  m_RichHC.clear();

  for(int ihhc=0; ihhc<m_RichG4HCName->RichHCSize(); ++ihhc ) {

    CurColl =
      new  RichG4HitsCollection(SensitiveDetectorName,collectionName[ihhc]);

    m_RichHC.push_back(CurColl);

    if(m_HpdHCID[ihhc] < 0  ){
      m_HpdHCID[ihhc] = G4SDManager::GetSDMpointer()->
        GetCollectionID(collectionName[ihhc]);
    }

    HCE->AddHitsCollection( m_HpdHCID[ihhc] , m_RichHC[ihhc]  );
  }

  // log << MSG::INFO<<" resetting SensDet Counters in current Event "<<endreq;
  
   ResetHpdMapInCurrentEvent();
  
}
void RichSensDet::ResetHpdMapInCurrentEvent() {
    m_RichHpdAlreadyHit.assign(m_TotNumHpdsInRich,false);
    m_RichHpdToPixelNumMap.clear();

  
}


//=============================================================================
// EndOfEvent (G4VSensitiveDetector method) commented out and moved all the contents to initialize
//=============================================================================
//void RichSensDet::EndOfEvent(G4HCofThisEvent* HCE) {

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






