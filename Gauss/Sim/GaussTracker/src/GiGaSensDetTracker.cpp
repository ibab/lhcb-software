// ============================================================================
/// CLHEP 
#include "CLHEP/Geometry/Point3D.h"
/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
/// GiGa 
#include "GiGa/GiGaMACROs.h"
/// Geant4 
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4TouchableHistory.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4SDManager.hh"
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
  collectionName.insert(HCname="Hits");
};

// ============================================================================

GiGaSensDetTracker::~GiGaSensDetTracker(){};

// ============================================================================

void GiGaSensDetTracker::Initialize(G4HCofThisEvent*HCE)
{
  static int HCID;
  
  trackerCol = new TrackerHitsCollection
    (SensitiveDetectorName,collectionName[0]);
  
  HCID = G4SDManager::GetSDMpointer()
        ->GetCollectionID(SensitiveDetectorName + "/" + collectionName[0]);

  HCE->AddHitsCollection(HCID,trackerCol);

}


bool GiGaSensDetTracker::ProcessHits( G4Step* step , 
                                      G4TouchableHistory* /* history */ ) 
{
  if( 0 == step ) { return false ; } 
  
  G4Track* track=step->GetTrack();
  G4double charge = track->GetDefinition()->GetPDGCharge();
  
  if(charge!=0.0)
    {
      double edep = step->GetTotalEnergyDeposit();     
      
      if (edep!=0.0)
        {
          double timeof = track-> GetGlobalTime();
          HepPoint3D postpos  = step->GetPostStepPoint()->GetPosition();
          HepPoint3D prepos  = step->GetPreStepPoint()->GetPosition();
          int trid = track->GetTrackID();
          
          // temp
//           G4TouchableHistory* TT =  
//             (G4TouchableHistory*)(step->GetPreStepPoint()->GetTouchable());
//           G4VPhysicalVolume*  PV =   TT->GetVolume();
//           G4LogicalVolume*    LV =   PV->GetLogicalVolume();
          
//           G4TouchableHistory* postTT =  
//             (G4TouchableHistory*)(step->GetPostStepPoint()->GetTouchable());
//           G4VPhysicalVolume*  postPV =   postTT->GetVolume();
//           G4LogicalVolume*    postLV =   postPV->GetLogicalVolume();
          
//           MsgStream log( msgSvc() , name() );
          
//           log << MSG::DEBUG << "Processing TrackerHit:" << " edep="  
//               << edep << endreq;
          
//           log << MSG::DEBUG
//               << " PrePos=("  << prepos.x() << "," << prepos.y() << "," << prepos.z() 
//               << ")" 
//               << " PrePV="    << PV->GetName()  
//               << " PreLV="    << LV->GetName() << endreq;
          
//           log << MSG::DEBUG
//               << " PostPos=("
//               << postpos.x() << "," << postpos.y() << "," << postpos.z() << ")" 
//               << " PostPV="    << postPV->GetName()  
//               << " PostLV="    << postLV->GetName() << endreq;
          // end of temp
          
          ///

          TrackerHit* newHit = new TrackerHit();
          newHit->SetEdep( edep );
          newHit->SetEntryPos( prepos );
          newHit->SetExitPos( postpos );
          newHit->SetTimeOfFlight( timeof );  
          newHit->SetTrackID( trid );
          ///
          G4VUserTrackInformation* ui = track->GetUserInformation(); 
          GaussTrackInformation*    gi = 
            ( 0 == ui )  ? 0 : static_cast<GaussTrackInformation*> ( ui );
          gi->setCreatedHit(true);
          gi->setToBeStored(true);
          gi->addHit(newHit);
          
          //  newHit->Print();
          trackerCol->insert( newHit );
        }
    }
  return false;
  
};
// ============================================================================


