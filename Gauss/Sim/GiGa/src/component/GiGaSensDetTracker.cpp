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
#include "G4HCofThisEvent.hh"
#include "G4ios.hh"
/// local
#include "GiGaSensDetTracker.h"

// ============================================================================
IMPLEMENT_GiGaFactory( GiGaSensDetTracker );
// ============================================================================


GiGaSensDetTracker::GiGaSensDetTracker
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaSensDetBase     ( type , name , parent ) 
  , G4VSensitiveDetector( name  )
{
  G4String HCname;
  collectionName.insert(HCname="trackerCollection");
};

// ============================================================================

GiGaSensDetTracker::~GiGaSensDetTracker(){};

// ============================================================================

void GiGaSensDetTracker::Initialize(G4HCofThisEvent*HCE)
{
  static int HCID = -1;
  trackerCollection = new TrackerHitsCollection
                      (SensitiveDetectorName,collectionName[0]); 
  if(HCID<0)
  { HCID = GetCollectionID(0); }
  HCE->AddHitsCollection(HCID,trackerCollection);
}


bool GiGaSensDetTracker::ProcessHits( G4Step* step , 
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
      
      log << MSG::INFO << "Processing TrackerHit:" << " edep="  
      << edep << endreq;
      
      log << MSG::INFO 
      << " PrePos=("  << prepos.x() << "," << prepos.y() << "," << prepos.z() 
      << ")" 
      << " PrePV="    << PV->GetName()  
      << " PreLV="    << LV->GetName() << endreq;

      log << MSG::INFO 
      << " PostPos=("
      << postpos.x() << "," << postpos.y() << "," << postpos.z() << ")" 
      << " PostPV="    << postPV->GetName()  
      << " PostLV="    << postLV->GetName() << endreq;
      // end of temp
  
      ///
      TrackerHit* newHit = new TrackerHit();
      newHit->SetEdep( edep );
      newHit->SetEntryPos( prepos );
      newHit->SetExitPos( postpos );
      newHit->SetTimeOfFlight( timeof );  
      newHit->SetTrackID( trid );
      ///

      //  newHit->Print();
      trackerCollection->insert( newHit );
    }
     return false;
     
};
// ============================================================================


