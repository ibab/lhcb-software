// ============================================================================
/// CLHEP 
#include "CLHEP/Geometry/Point3D.h"
/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
/// GiGa 
#include "GiGa/GiGaMACROs.h"
/// Geant4 
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4ios.hh"
/// local
#include "GiGaSensDetMuon.h"

// ============================================================================
IMPLEMENT_GiGaFactory( GiGaSensDetMuon );
// ============================================================================


GiGaSensDetMuon::GiGaSensDetMuon
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaSensDetBase     ( type , name , parent ) 
  , G4VSensitiveDetector( name  )
{  
  G4String HCname=name;
  std::string::size_type posdot = HCname.find(".");
  HCname.erase(0,posdot+1);

  collectionName.insert(HCname);
};

// ============================================================================

GiGaSensDetMuon::~GiGaSensDetMuon(){};

// ============================================================================

void GiGaSensDetMuon::Initialize(G4HCofThisEvent*HCE)
{
  static int HCID;

  muonCol = new MuonHitsCollection
    (SensitiveDetectorName,collectionName[0]);
  
  HCID = G4SDManager::GetSDMpointer()
    ->GetCollectionID(SensitiveDetectorName + "/" + collectionName[0]);
  
  HCE->AddHitsCollection(HCID,muonCol);
}


bool GiGaSensDetMuon::ProcessHits( G4Step* step , 
                                    G4TouchableHistory* /* history */ ) 
{
  if( 0 == step ) { return false ; } 
  
  double edep = step->GetTotalEnergyDeposit();
  double timeof = step->GetDeltaTime();
  HepPoint3D postpos  = step->GetPostStepPoint()->GetPosition();
  HepPoint3D prepos  = step->GetPreStepPoint()->GetPosition();
  int trid = step->GetTrack()->GetTrackID();
  G4double charge = step->GetTrack()->GetDefinition()->GetPDGCharge();

  if(charge!=0.0)
    {
      // temp  
      G4TouchableHistory* TT =  
      (G4TouchableHistory*)(step->GetPreStepPoint()->GetTouchable());
      G4VPhysicalVolume*  PV =   TT->GetVolume();
      G4LogicalVolume*    LV =   PV->GetLogicalVolume();

      G4TouchableHistory* postTT =  
      (G4TouchableHistory*)(step->GetPostStepPoint()->GetTouchable());
      G4VPhysicalVolume*  postPV =   postTT->GetVolume();
      G4LogicalVolume*    postLV =   postPV->GetLogicalVolume();

      MsgStream log( msgSvc() , name() );
      
      log << MSG::DEBUG << "Processing MuonHit:" << " edep="  
      << edep << endreq;

      std::string pvname=PV->GetName();
      G4TouchableHistory* MotherTT = TT;
      MotherTT ->MoveUpHistory(1);

      std::string stname=MotherTT -> GetVolume()->GetName();
      
      log << MSG::DEBUG << "Mother: " << stname <<  endreq;      

      log << MSG::DEBUG 
      << " PrePos=("  << prepos.x() << "," << prepos.y() << "," << prepos.z() 
      << ")" 
      << " PrePV="    << pvname  
      << " PreLV="    << LV->GetName() << endreq;

      log << MSG::DEBUG 
      << " PostPos=("
      << postpos.x() << "," << postpos.y() << "," << postpos.z() << ")" 
      << " PostPV="    << postPV->GetName()  
      << " PostLV="    << postLV->GetName() << endreq;
      // end of temp
  
      ///
      MuonHit* newHit = new MuonHit();
      newHit->SetEdep( edep );
      newHit->SetEntryPos( prepos );
      newHit->SetExitPos( postpos );
      newHit->SetTimeOfFlight( timeof );  
      newHit->SetTrackID( trid );
      newHit->SetChamberID(0);
      newHit->SetGapID(0);
      ///

      //  newHit->Print();
      muonCol->insert( newHit );
    }
     return false;
     
};
// ============================================================================


