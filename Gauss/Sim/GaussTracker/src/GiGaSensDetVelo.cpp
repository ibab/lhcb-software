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
#include "GiGaSensDetVelo.h"

// ============================================================================
IMPLEMENT_GiGaFactory( GiGaSensDetVelo );
// ============================================================================


GiGaSensDetVelo::GiGaSensDetVelo
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

GiGaSensDetVelo::~GiGaSensDetVelo(){};

// ============================================================================

void GiGaSensDetVelo::Initialize(G4HCofThisEvent*HCE)
{
  int HCID;
  
  veloCol= new VeloHitsCollection
    (SensitiveDetectorName,collectionName[0]);
  
  HCID=G4SDManager::GetSDMpointer()
    ->GetCollectionID(SensitiveDetectorName + "/" + collectionName[0]);      
  
  HCE->AddHitsCollection(HCID,veloCol);
}


bool GiGaSensDetVelo::ProcessHits( G4Step* step , 
                                    G4TouchableHistory* /* history */ ) 
{
  if( 0 == step ) { return false ; }
 
  G4Track* track=step->GetTrack();
  G4double charge = track->GetDefinition()->GetPDGCharge();

  if(charge!=0.0)
    {
      double edep = step->GetTotalEnergyDeposit();

      if(edep!=0.0 && step->GetStepLength()!=0.0)
        {
          double timeof = track-> GetGlobalTime();
          HepPoint3D postpos  = step->GetPostStepPoint()->GetPosition();
          HepPoint3D prepos  = step->GetPreStepPoint()->GetPosition();
          int trid = step->GetTrack()->GetTrackID();
          
          G4TouchableHistory* TT =  
            (G4TouchableHistory*)(step->GetPreStepPoint()->GetTouchable());
          G4VPhysicalVolume*  PV = TT->GetVolume();
          TT -> MoveUpHistory(1);
          G4VPhysicalVolume* MPV = TT->GetVolume();
 
          ///
          VeloHit* newHit = new VeloHit();
          newHit->SetEdep( edep );
          newHit->SetEntryPos( prepos );
          newHit->SetExitPos( postpos );
          newHit->SetTimeOfFlight( timeof );  
          newHit->SetTrackID( trid );
          newHit->SetSensor(PV->GetCopyNo() + MPV->GetCopyNo());
 
          ///
          G4VUserTrackInformation* ui = track->GetUserInformation(); 
          GaussTrackInformation*    gi = (GaussTrackInformation*) ui;
          gi->setCreatedHit(true);
          gi->setToBeStored(true);
          gi->addHit(newHit);

          veloCol->insert( newHit );
        }    
    }
  return false;
};
// ============================================================================


