// ============================================================================
/// CLHEP 
#include "CLHEP/Geometry/Point3D.h"
/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
/// GiGa 
#include "GiGa/GiGaMACROs.h"

#include "TrackerHit.h"
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
/// factory business 
// ============================================================================
IMPLEMENT_GiGaFactory( GiGaSensDetTracker );
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

// ============================================================================
/// destructor 
// ============================================================================
GiGaSensDetTracker::~GiGaSensDetTracker(){};
// ============================================================================

// ============================================================================
/** process the hit
 *  @param step     pointer to current Geant4 step 
 *  @param history  pointert to touchable history 
 */
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
  
  double     edep = step->GetTotalEnergyDeposit();
  HepPoint3D pos  = step->GetPostStepPoint()->GetPosition(); 
  
  /// 
  G4TouchableHistory* TT =  
    (G4TouchableHistory*)(step->GetPreStepPoint()->GetTouchable());
  G4VPhysicalVolume*  PV =   TT->GetVolume();
  G4LogicalVolume*    LV =   PV->GetLogicalVolume();
  
  MsgStream log( msgSvc() , name() );
  log << MSG::INFO << "My TrackerDet: " 
      << " Pos=("  << pos.x() << "," << pos.y() << "," << pos.z() << ")" 
      << " PV="    << PV->GetName() 
      << " LV="    << LV->GetName() 
      << " edep="  << edep << endreq;

  TrackerHit* newHit = new TrackerHit();
  newHit->SetEdep( edep );
  newHit->SetPos( step->GetPreStepPoint()->GetPosition() );
  newHit->Print();
  trackerCollection->insert( newHit );

  return false;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

