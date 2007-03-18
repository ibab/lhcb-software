// $Id: GiGaSensDetTracker.cpp,v 1.12 2007-03-18 18:39:07 gcorti Exp $
// Include files 

// from CLHEP
#include "CLHEP/Geometry/Point3D.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/MsgStream.h"

// from Geant4 
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4TouchableHistory.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4ios.hh"

// local
#include "GiGaSensDetTracker.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GiGaSensDetTracker
//
// 2006-07-14 : Gloria CORTI (clean up)
//-----------------------------------------------------------------------------

// Declaration of the Factory
DECLARE_TOOL_FACTORY( GiGaSensDetTracker );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GiGaSensDetTracker::GiGaSensDetTracker( const std::string& type,
                                        const std::string& name,
                                        const IInterface*  parent ) 
  : GiGaSensDetBase     ( type , name , parent ) 
  , G4VSensitiveDetector( name  )
{  

  collectionName.insert( "Hits" );

}

//=============================================================================
// Destructor
//=============================================================================
GiGaSensDetTracker::~GiGaSensDetTracker(){}


//=============================================================================
// Initialize method from G4 (Called at the begin of each G4event)
// see G4VSensitiveDetector 
// ============================================================================
void GiGaSensDetTracker::Initialize(G4HCofThisEvent*HCE) {
  
  m_trackerCol = new TrackerHitsCollection( SensitiveDetectorName,
                                            collectionName[0] );
  
  int HCID = G4SDManager::GetSDMpointer()
    ->GetCollectionID( SensitiveDetectorName + "/" + collectionName[0] );

  HCE->AddHitsCollection(HCID, m_trackerCol);

  Print (" Initialize(): CollectionName='" + m_trackerCol->GetName   () +
         "' for SensDet='"                 + m_trackerCol->GetSDname () + 
         "'" , StatusCode::SUCCESS , MSG::VERBOSE                       ) ;
  
}


//=============================================================================
// process the hit (G4 method)
//=============================================================================
bool GiGaSensDetTracker::ProcessHits( G4Step* step , 
                                      G4TouchableHistory* /* history */ ) 
{
  if( 0 == step ) { return false; } 
  
  G4Track* track=step->GetTrack();
  G4double charge = track->GetDefinition()->GetPDGCharge();
  
  if( charge == 0.0 ) { return false; }
  
  double edep = step->GetTotalEnergyDeposit();

  if( (edep != 0.0 ) && (step->GetStepLength() != 0.0) ) {

    Print("Filling a hit", StatusCode::SUCCESS, MSG::VERBOSE);

    G4ThreeVector prepos  = step->GetPreStepPoint()->GetPosition();
    double timeof = step->GetPreStepPoint()->GetGlobalTime();
    G4ThreeVector premom = step->GetPreStepPoint()->GetMomentum();
    
    TrackerHit* newHit = new TrackerHit();
    newHit->SetEdep( edep );
    newHit->SetEntryPos( prepos );
    newHit->SetTimeOfFlight( timeof );
    newHit->SetMomentum( premom );

    // Store exit point
    G4ThreeVector postpos  = step->GetPostStepPoint()->GetPosition();
    newHit->SetExitPos( postpos );
    
    // Set id to track 
    int trid = track->GetTrackID();
    newHit->SetTrackID( trid );

    G4VUserTrackInformation* ui = track->GetUserInformation(); 
    GaussTrackInformation* gi = (GaussTrackInformation*) ui;
    gi->setCreatedHit(true);
    gi->setToBeStored(true);
    gi->addHit(newHit);

    // add hit to collection
    m_trackerCol->insert( newHit );    
    return true;
    
  }

  return false;
  
}

//=============================================================================


