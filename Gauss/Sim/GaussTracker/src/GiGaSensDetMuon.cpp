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
  m_chamLimit = 0;
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

  std::string tempName = muonCol->GetName();
  if( tempName.find("R1") != std::string::npos ) {
    m_chamLimit = 12;
  }
  if( tempName.find("R2") != std::string::npos ) {
    m_chamLimit = 24;
  }
  if( tempName.find("R3") != std::string::npos ) {
    m_chamLimit = 48;
  }
  if( tempName.find("R4") != std::string::npos ) {
    m_chamLimit = 192;
  }  
}


bool GiGaSensDetMuon::ProcessHits( G4Step* step , 
                                    G4TouchableHistory* /* history */ ) 
{
  if( 0 == step ) { return false ; }
 
  G4Track* track=step->GetTrack();
  G4double charge = track->GetDefinition()->GetPDGCharge();

  if(charge!=0.0)
    {
      double edep = step->GetTotalEnergyDeposit();

      if(edep!=0 && step->GetStepLength()!=0.0)
        {
          double timeof = step->GetTrack()-> GetGlobalTime();
          HepPoint3D postpos  = step->GetPostStepPoint()->GetPosition();
          HepPoint3D prepos  = step->GetPreStepPoint()->GetPosition();
          int trid = step->GetTrack()->GetTrackID();
                   
          //   
          G4TouchableHistory* TT =  
            (G4TouchableHistory*)(step->GetPreStepPoint()->GetTouchable());
          G4VPhysicalVolume*  PV =   TT->GetVolume();
          
          TT -> MoveUpHistory(1);
          G4int CurrentGapNumber = TT -> GetVolume() -> GetCopyNo() ;

          TT -> MoveUpHistory(1);
          G4int CurrentChamNumber= TT -> GetVolume() -> GetCopyNo() ;

          G4int GapNumber=CurrentGapNumber - 999;

          G4int ChamNumber=0;
          if(CurrentChamNumber<1012) {
            ChamNumber=CurrentChamNumber - 999;
          }
          if(CurrentChamNumber>1011 && CurrentChamNumber<1036) {
            ChamNumber=CurrentChamNumber - 1011;
          }
          if(CurrentChamNumber>1035 && CurrentChamNumber<1084) {
            ChamNumber=CurrentChamNumber - 1035;
          }
          if(CurrentChamNumber>1083) {
            ChamNumber=CurrentChamNumber - 1083;
          }

          // Check if chamber number is allowed for this collection 
          // (i.e. region)
          


          if( ChamNumber > m_chamLimit ) {
            MsgStream msg( msgSvc() , name() );
            std::string colname = TT->GetVolume()->GetName();
            Error( "Chamber out of limits in "+colname);
            msg << MSG::WARNING << colname
                << " :hit out of limit. Entry - " << prepos 
                << " Exit - " << postpos << " dE/dx = " << edep
                << " Tof = " << timeof << " Chamber = " << ChamNumber
                << " Gap = " << GapNumber << endmsg;
          }
          else {
            
          
            //MsgStream log( msgSvc() , name() );
            
            //log << MSG::INFO << "************************" << endreq;
            //log << MSG::INFO << "Cham =" << CurrentChamNumber << " , " <<
            //  ChamNumber << "   Gap =" << CurrentGapNumber << " , " <<
            //  GapNumber << endreq;
          
            
            //           G4TouchableHistory* postTT =  
            //   (G4TouchableHistory*)(step->GetPostStepPoint()
            //          ->GetTouchable());
            //           G4VPhysicalVolume*  postPV =   postTT->GetVolume();
            //      G4LogicalVolume*    postLV =   postPV->GetLogicalVolume();
          
            // std::string pvname=PV->GetName();
            // G4TouchableHistory* MotherTT = TT;
            // MotherTT ->MoveUpHistory(1);

            //           std::string stname=MotherTT -> GetVolume()->GetName();
          
            // log << MSG::INFO << "Mother: " << stname <<  endreq;      
          
            // log << MSG::INFO 
            //     << " PrePos=("  << prepos.x() << "," 
            //     << prepos.y() << "," << prepos.z() 
            //     << ")" 
            //     << " PrePV="    << pvname  
            //     << " PreLV="    << LV->GetName() << endreq;
          
            // log << MSG::INFO 
            //     << " PostPos=("
            //     << postpos.x() << "," << postpos.y() << "," 
            // << postpos.z() << ")" 
            //     << " PostPV="    << postPV->GetName()  
            //    << " PostLV="    << postLV->GetName() << endreq;
            // end of temp
  
            ///
            MuonHit* newHit = new MuonHit();
            newHit->SetEdep( edep );
            newHit->SetEntryPos( prepos );
            newHit->SetExitPos( postpos );
            newHit->SetTimeOfFlight( timeof );  
            newHit->SetTrackID( trid );
            newHit->SetChamberID( ChamNumber );
            newHit->SetGapID( GapNumber );
            ///
            G4VUserTrackInformation* ui = track->GetUserInformation(); 
            GaussTrackInformation*    gi = (GaussTrackInformation*)  ui;
            gi->setCreatedHit(true);
            gi->setToBeStored(true);
            gi->addHit(newHit);
          
            muonCol->insert( newHit );
          }
          
        }
    }
  return false;
};
// ============================================================================


