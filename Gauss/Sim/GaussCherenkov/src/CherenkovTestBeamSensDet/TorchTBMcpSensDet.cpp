// $Id: $
// Include files 
// from CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "G4TouchableHistory.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4TransportationManager.hh"
#include "G4Electron.hh"
#include "G4VProcess.hh"

// Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"

// GiGa
#include "GiGa/GiGaBase.h"
#include "CkvSensDet.h"
#include "GaussRICH/RichInfo.h"
#include "GaussRICH/RichPEInfo.h"
#include "GaussRICH/RichPhotoElectron.h"
#include "GaussTools/GaussTrackInformation.h"




// local
#include "TorchTBMcpSensDet.h"
DECLARE_TOOL_FACTORY( TorchTBMcpSensDet );

//-----------------------------------------------------------------------------
// Implementation file for class : TorchTBMcpSensDet
//
// 2012-05-25 : Sajan Easo
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TorchTBMcpSensDet::TorchTBMcpSensDet(const std::string& type   ,
                          const std::string& name   ,
                                     const IInterface*  parent  )
:G4VSensitiveDetector ( name  ),
  GiGaSensDetBase      ( type , name , parent )  {


}
//=============================================================================
// Destructor
//=============================================================================
TorchTBMcpSensDet::~TorchTBMcpSensDet() {} 

//=============================================================================
StatusCode TorchTBMcpSensDet::initialize() 
{
  StatusCode sc = GiGaSensDetBase::initialize();
  if (sc.isFailure()) return sc;

  IDataProviderSvc* detSvc;
  if ( svcLoc()->service( "DetectorDataSvc" , detSvc , true ) ) {
    m_TorchTBHitCollName= new TorchTBHitCollName( );

    G4String HCName= m_TorchTBHitCollName ->TorchTBHCName();
    G4cout<<" Torch TB HC name "<<  HCName <<G4endl;
    
    m_torchHCID= -1;
    collectionName.push_back(HCName);
  }
  
  return sc;
}
StatusCode TorchTBMcpSensDet::finalize() 
{
  return GiGaSensDetBase::finalize();  
  
}
//=============================================================================
// process the hit (G4VSensitiveDetector method
//  step     pointer to current Geant4 step
//  history  pointert to touchable history
//=============================================================================
bool TorchTBMcpSensDet::ProcessHits( G4Step* aStep ,
                               G4TouchableHistory* )
{
  if( 0 == aStep ) { return false ; }

  double CurEdep = aStep->GetTotalEnergyDeposit();
  // Create a hit only when there is
  // non-zero energy deposit.  SE June 2003.
  // G4cout<<"Rich SensDet CurEdep "<< CurEdep<<G4endl;
  //G4cout<<" Now in SensDet Process hits for torch Tb Mcpstart  "<< CurEdep << G4endl;

  if(  CurEdep <= 0.001 ) { return false; }

  // G4cout<<" Now in SensDet Process hits for torch Tb Mcp "<<G4endl;
  
  G4StepPoint* prePosPoint =aStep->GetPreStepPoint();
  const G4ThreeVector & CurGlobalPos  = prePosPoint->GetPosition();
  ///
  G4TouchableHistory* CurTT =
    (G4TouchableHistory*)(prePosPoint->GetTouchable());
  G4VPhysicalVolume*  CurPV =   CurTT->GetVolume();
  G4LogicalVolume*    CurLV =   CurPV->GetLogicalVolume();
  G4Track* aTrack = aStep->GetTrack();
  if( aTrack->GetDefinition()->GetPDGCharge() == 0.0 ) {
    return false;
  }
  G4ThreeVector CurPEOrigin;
  G4String aCreatorProcessName = "NullProcess";
  const G4VProcess* aProcess = aTrack->GetCreatorProcess();
  if(aProcess) aCreatorProcessName =  aProcess->GetProcessName();
  if(((aTrack->GetDefinition() == G4Electron::Electron()) ||
      (aTrack->GetDefinition() == RichPhotoElectron::PhotoElectron())) &&
     ( (aCreatorProcessName  == "TorchTBMcpPhotoElectricProcess" ) ||
       ( aCreatorProcessName  ==  "TorchTBMcpEnergyLossProcess") ) )
  {
    CurPEOrigin = aTrack->GetVertexPosition() ;
  }

  MsgStream log( msgSvc() , name() );

  log << MSG::DEBUG << "Now in ProcessHits() for CkvSensDet"
      << " Pos=("  << CurGlobalPos.x() << "," << CurGlobalPos.y()
      << "," << CurGlobalPos.z() << ")"
      << " PV="    << CurPV->GetName()
      << " LV="    << CurLV->GetName()
      << " edep in MeV ="  << CurEdep << endreq;
  log << MSG::DEBUG << " PE Origin X Y Z "<<CurPEOrigin.x()
      <<"   "<<CurPEOrigin.y()<<"   "<<CurPEOrigin.z()<<endreq;

  G4double CurGlobalZ=CurGlobalPos.z();
  if( CurGlobalZ <= 0.0 ) {
    log << MSG::ERROR << "Inadmissible MCP  Hit Z coordinate = "
        <<  CurGlobalZ <<endreq;
  }
  
  G4int CurrentMcpNumber= CurTT -> GetReplicaNumber(2);
  log << MSG::DEBUG <<" Mcp SensDet current mcp number "<<CurrentMcpNumber<<endreq;
  

  G4Navigator* theNavigator =
    G4TransportationManager::GetTransportationManager()->
    GetNavigatorForTracking();
  G4ThreeVector CurLocalPos = theNavigator->
    GetGlobalToLocalTransform().
    TransformPoint(CurGlobalPos);
  G4ThreeVector CurPEOriginLocal = theNavigator->
    GetGlobalToLocalTransform().
    TransformPoint(CurPEOrigin);
  G4double CurGlobalTime =  aTrack-> GetGlobalTime();

  G4int CurOptPhotMotherChTrackID=-1;
  G4int CurOptPhotMotherChTrackPDG=0;
  G4int CurRadiatorNumber=-1;
  G4ThreeVector CurEmissPt;
  G4double CurPhotEnergyAtProd=0;
  G4double CurThetaCkvAtProd=0;
  G4double CurPhiCkvAtProd=0;
  G4double CurChTrackTotMom=0;
  G4int CurOptPhotID =0;
  G4ThreeVector CurChTrackMomVect;
  G4double CurChTrackPDGMass=0;
  G4ThreeVector CurChTrackCkvPreStepPos;
  G4ThreeVector CurChTrackCkvPostStepPos;


  G4VUserTrackInformation* aUserTrackinfo=aTrack->GetUserInformation();
  GaussTrackInformation* aRichPETrackInfo
    = (GaussTrackInformation*)aUserTrackinfo;
  if( ( (aTrack->GetDefinition() == G4Electron::Electron()) ||
        (aTrack->GetDefinition() == RichPhotoElectron::PhotoElectron()))  &&
      (( aCreatorProcessName  == "TorchTBMcpPhotoElectricProcess")  ||
       ( aCreatorProcessName  == "TorchTBMcpEnergyLossProcess") ) ) {
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
            CurChTrackMomVect    =
                aPEInfo->  MotherofPhotonMomVectAtProd();
            CurChTrackPDGMass    = aPEInfo-> MotherofPhotonPDGMass();
            CurChTrackCkvPreStepPos=
                aPEInfo->MotherofPhotonCkvPreStep();
            CurChTrackCkvPostStepPos=
              aPEInfo->MotherofPhotonCkvPostStep();
          }
        }
      }
    }
  }
  G4int CurPETrackID=aTrack->GetTrackID();
  G4int CurPETrackPDG=aTrack->GetDefinition()->GetPDGEncoding();

  log<<MSG::DEBUG<<" Now create a new hit class instance :"<<endreq;
  
  CkvG4Hit * newHit = new CkvG4Hit();
  newHit -> SetEdep( CurEdep);
  newHit -> SetGlobalPos( CurGlobalPos );
  newHit -> SetLocalPos( CurLocalPos );
  newHit -> SetGlobalPEOriginPos( CurPEOrigin) ;
  newHit -> SetLocalPEOriginPos( CurPEOriginLocal) ;
  //  newHit -> SetCurPixelXNum(CurrentPixelXNum);
  // newHit -> SetCurPixelYNum(CurrentPixelYNum);
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
  newHit->SetCurPmtNum (CurrentMcpNumber);
  newHit ->setTrackID(CurOptPhotMotherChTrackID); // this covers all the normal cases
  if( CurOptPhotMotherChTrackID <= 0) {           // this is for mip particle
    newHit ->setTrackID(CurPETrackID);
    if(aRichPETrackInfo) aRichPETrackInfo ->setToBeStored (true);
  }

  int NumHitsInCurHC =m_TorchHC ->insert( newHit );
  log << MSG::DEBUG << "NumHit in curent Hit coll for Torch TB Mcp "<<NumHitsInCurHC<<endreq;
  
  return true;
  
}
void TorchTBMcpSensDet::clear() {  }

void TorchTBMcpSensDet::Initialize(G4HCofThisEvent*  HCE )
{
  MsgStream log( msgSvc() , name() );
  log << MSG::INFO << "TorchTbMcpSensDet Init Sensdet name CollName "
      << SensitiveDetectorName <<"   "<< collectionName[0]<<  endreq;
  
  
  m_TorchHC =new  CkvG4HitsCollection(SensitiveDetectorName,collectionName[0]);
  if( m_torchHCID < 0 ) {
    m_torchHCID = G4SDManager::GetSDMpointer()->
        GetCollectionID(collectionName[0]);
    
  }
  
    HCE->AddHitsCollection( m_torchHCID , m_TorchHC  );
    
}

                            
                            
      

