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
  int HCID;
  
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
      
      if (edep!=0.0 && step->GetStepLength()!=0.0)
        {
          double timeof = track-> GetGlobalTime();
          HepPoint3D postpos  = step->GetPostStepPoint()->GetPosition();
          HepPoint3D prepos  = step->GetPreStepPoint()->GetPosition();

//           HepPoint3D postdir = step->GetPostStepPoint()->GetMomentumDirection();
//           HepPoint3D predir = step->GetPreStepPoint()->GetMomentumDirection();
          
//           std::cout << "predir " << predir << " postdir " << postdir
//                     << " calculated dir " << (postpos-prepos).unit() 
//                     << std::endl;
          
          int trid = track->GetTrackID();          
          ///
          TrackerHit* newHit = new TrackerHit();
          newHit->SetEdep( edep );
          newHit->SetEntryPos( prepos );
          newHit->SetExitPos( postpos );
          newHit->SetTimeOfFlight( timeof );  
          newHit->SetTrackID( trid );
          ///
          G4VUserTrackInformation* ui = track->GetUserInformation(); 
          GaussTrackInformation* gi = (GaussTrackInformation*) ui;
          gi->setCreatedHit(true);
          gi->setToBeStored(true);
          gi->addHit(newHit);
          
          trackerCol->insert( newHit );
        }
    }
  return false;
  
};
// ============================================================================


