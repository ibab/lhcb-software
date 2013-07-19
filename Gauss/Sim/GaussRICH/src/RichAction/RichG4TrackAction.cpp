// $Id: RichG4TrackAction.cpp,v 1.4 2008-01-21 16:56:27 seaso Exp $
// Include files 

// from Gaudi
//#include "GaudiKernel/DeclareFactoryEntries.h" 

// CLHEP
#include "CLHEP/Geometry/Point3D.h"

// Geant4 
#include "G4Track.hh"
#include "G4TrackVector.hh"
#include "G4TrackingManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4DynamicParticle.hh"
#include "G4Electron.hh"
#include "GaussRICH/RichPhotoElectron.h"
#include "G4ThreeVector.hh"
#include "G4OpticalPhoton.hh"
#include "globals.hh"
#include <math.h>
#include "G4VProcess.hh"

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"

// local
#include "GaussRICH/RichG4TrackAction.h"
#include "GaussRICH/RichG4AnalysisConstGauss.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4TrackAction
//
// xxxx-xx-xx : X X
// 2007-01-11 : Gloria Corti, modified for Gaudi v19
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory. moved to Factories.cpp
//DECLARE_TOOL_FACTORY( RichG4TrackAction );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
// ============================================================================
RichG4TrackAction::RichG4TrackAction
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaTrackActionBase( type , name , parent ) {  }

//=============================================================================
// Destructor
//=============================================================================
RichG4TrackAction::~RichG4TrackAction(){ }

//=============================================================================
// PreUserTrackingAction (Geant4)
//=============================================================================
void RichG4TrackAction::PreUserTrackingAction  ( const G4Track* aTrack ) {


  if( 0 == aTrack || 0 == trackMgr()           )  { return ; } /// RETURN !!!


 
}


//=============================================================================
// PostUserTrackingAction (Geant4)
//=============================================================================
void RichG4TrackAction::PostUserTrackingAction ( const G4Track* aTrack ) 
{

  // Is the track valid? Is tracking manager valid?
  if( 0 == aTrack || 0 == trackMgr()           )  { return ; } /// RETURN !!!
  if ( 0 == trackMgr()->GimmeTrajectory    () )  { return ; } /// RETURN !!!
  // first test if this is an optical photon. 
  // If the optical photon has produced  a secondary track which is 
  // a  electron, store the photon as  a trajectory, assuming that 
  // the electron is a photoelectron.
  // This is to be put as an option feature in the future.  
  // The check is made on the Z origin of the photon is commented out for now.
  // This may be put back in the future.
  

   G4bool AlreadySetToStore = trackMgr()->GetStoreTrajectory() ;
   if( AlreadySetToStore ) {return ;}    //RETURN
  
    
  const G4DynamicParticle * aParticle = aTrack->GetDynamicParticle();
  //  G4cout<<"At at post usertrackingaction of  "
  //      <<aParticle->GetDefinition()->GetParticleName()<<G4endl;
  
  if(aParticle->GetDefinition() == G4OpticalPhoton::OpticalPhoton() ) {
    // The following variable is to be put in the options file.
    G4bool StoreAllRichOpticalPhotons= false;
    

    // G4double CurPhotonOrigZcoord= aTrack->GetVertexPosition().z();

    // if(( CurPhotonOrigZcoord >= ZUpsRich1Analysis && 
    //           CurPhotonOrigZcoord <= ZDnsRich1Analysis) ||
    //           ( CurPhotonOrigZcoord >= ZUpsRich2Analysis && 
    //             CurPhotonOrigZcoord <= ZDnsRich2Analysis) ) 

    if( StoreAllRichOpticalPhotons ) {
   	      // set the opticalphoton to be a trajectory.
     
      trackMgr()->SetStoreTrajectory( true );
      AlreadySetToStore=true;
      
    }else {
      
    
        const G4TrackVector* PhotSec = trackMgr()->GimmeSecondaries() ;
	      for( unsigned int isec = 0 ; isec <PhotSec->size() ; ++isec ){
              G4Track* SecTrk = (*PhotSec)[isec];
              if(( SecTrk->GetDynamicParticle()->
                 GetDefinition() == G4Electron::Electron()) || 
                 ( SecTrk->GetDynamicParticle()->
                 GetDefinition() == RichPhotoElectron::PhotoElectron()) ) {
  	      // set the opticalphoton to be a trajectory.
	      
	                trackMgr()->SetStoreTrajectory( true );
                   AlreadySetToStore=true;
                  //                  break;
                  
            }
              
              
              
        }
        
    }
    
    
        
        // }
  
    
  }
  
  

  // Now for the charged tracks, 
  // if they produce optical photons, save them as
  // trajectory too.
  if(aParticle->GetDefinition()->GetPDGCharge() != 0.0) {
    
     // The test on the Z coordinate is commented out for now.

     const G4TrackVector* SecTr = trackMgr()->GimmeSecondaries() ;
     unsigned int itrsec = 0;
     while( !(AlreadySetToStore) && (itrsec <SecTr->size()) ) {
       
          G4Track* ChildTrack = (*SecTr)[itrsec];
           if(ChildTrack->GetDynamicParticle()
                        ->GetDefinition() == G4OpticalPhoton::OpticalPhoton()){
 //   G4double PhotonOrigZcoord= ChildTrack->GetVertexPosition().z();
//  if((   PhotonOrigZcoord >= ZUpsRich1Analysis
 //    && PhotonOrigZcoord <= ZDnsRich1Analysis) ||
//    ( PhotonOrigZcoord >= ZUpsRich2Analysis
//    && PhotonOrigZcoord <= ZDnsRich2Analysis) ) {
	        
      trackMgr()->SetStoreTrajectory( true );
      AlreadySetToStore=true;
      
                  
                  //  }
       
           }
           itrsec++;           
     }
     
     
  
     // if it is a photoelectron created by the RichPhotoelectric process
     // store it to the trajectory anyway.
     if(  !(AlreadySetToStore) ) {
       
     G4String aCreatorProcessName = "NullProcess";
     const G4VProcess* aProcess = aTrack->GetCreatorProcess();
     if(aProcess) aCreatorProcessName =  aProcess->GetProcessName();
     if(aCreatorProcessName  == "RichHpdPhotoelectricProcess") {
                  trackMgr()->SetStoreTrajectory( true );
                  AlreadySetToStore=true;
                  
  
     }
     }
     
     
  }
  
  // delete trajectory by hand if not set to store.

    if ( !(AlreadySetToStore)  )
    {
      G4VTrajectory* traj = trackMgr()->GimmeTrajectory();    
      if( 0 != traj ) { delete traj ; } 
      trackMgr()->SetTrajectory( 0 ) ;
    }

  
  

  //
} 
//=============================================================================











