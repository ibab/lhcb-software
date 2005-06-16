// $Id: RichG4TrackActionPhotOpt.cpp,v 1.2 2005-06-16 11:40:18 seaso Exp $
// Include files 

/// CLHEP
#include "CLHEP/Geometry/Point3D.h"
/// Geant4 
#include "G4Track.hh"
#include "G4TrackVector.hh"
#include "G4TrackingManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4DynamicParticle.hh"
#include "G4ThreeVector.hh"
#include "G4OpticalPhoton.hh"
#include "globals.hh"
#include <math.h>
#include "G4VProcess.hh"
/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"
/// GiGa 
#include "GiGa/GiGaMACROs.h"
#include "GiGa/GiGaTrajectory.h"
#include "DetDesc/DetectorElement.h"

/// local
#include "RichG4AnalysisConstGauss.h"
#include "RichG4TrackActionPhotOpt.h"
#include "RichG4SvcLocator.h"
//-----------------------------------------------------------------------------
// Implementation file for class : RichG4TrackActionPhotOpt
//
// 2003-04-29 : Sajan EASO
//-----------------------------------------------------------------------------

/// factory business 
// ============================================================================
IMPLEMENT_GiGaFactory( RichG4TrackActionPhotOpt );
// ============================================================================

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
// ============================================================================
RichG4TrackActionPhotOpt::RichG4TrackActionPhotOpt
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaTrackActionBase( type , name , parent ), m_MaxHpdQuantumEff(0),
     m_MaxRich1Mirror1Reflect(0), m_MaxRich1Mirror2Reflect(0),
     m_MaxRich2Mirror1Reflect(0),  m_MaxRich2Mirror2Reflect(0),
      m_ZDownstreamOfRich1(0), m_Rich1TotPhotonSuppressFactor(0), 
      m_Rich2TotPhotonSuppressFactor(0)  {; }
// ============================================================================
RichG4TrackActionPhotOpt::~RichG4TrackActionPhotOpt(){; }

StatusCode RichG4TrackActionPhotOpt::initialize() 
{

 // initialize the base 
  StatusCode status = GiGaTrackActionBase::initialize() ; 
 if( status.isFailure() ) 
    { return Error("Could not intialize base class GiGaTrackActionBase!", 
                   status ) ; } 

 //the following may be extracted from gigabase in the future.
  IDataProviderSvc* detSvc = RichG4SvcLocator:: RichG4detSvc ();
 

  SmartDataPtr<DetectorElement> Rich1DE(detSvc, 
                                 "/dd/Structure/LHCb/Rich1");
     if( !Rich1DE ){
       return Error( 
      "Can't retrieve /dd/Structure/LHCb/Rich1 in RichG4TrackActionPhotOpt" );
     } 
     else {

       m_MaxHpdQuantumEff=Rich1DE->param<double>("RichHpdMaxQE");
       m_MaxRich1Mirror1Reflect=
                 Rich1DE->param<double>("Rich1Mirror1MaxReflect");
       m_MaxRich1Mirror2Reflect=
                 Rich1DE->param<double>("Rich1Mirror2MaxReflect");
       m_MaxRich2Mirror1Reflect=
                 Rich1DE->param<double>("Rich2Mirror1MaxReflect");
       m_MaxRich2Mirror2Reflect=
                 Rich1DE->param<double>("Rich2Mirror2MaxReflect");

       m_ZDownstreamOfRich1 = 
         Rich1DE->param<double>("RichZDownstreamOfRich1");


       //       m_MaxHpdQuantumEff=Rich1DE->userParameterAsDouble("RichHpdMaxQE");
       //  m_MaxRich1Mirror1Reflect=
       //          Rich1DE->userParameterAsDouble("Rich1Mirror1MaxReflect");
       //  m_MaxRich1Mirror2Reflect=
       //          Rich1DE->userParameterAsDouble("Rich1Mirror2MaxReflect");
       //  m_MaxRich2Mirror1Reflect=
       //          Rich1DE->userParameterAsDouble("Rich2Mirror1MaxReflect");
       // m_MaxRich2Mirror2Reflect=
       //          Rich1DE->userParameterAsDouble("Rich2Mirror2MaxReflect");

       //     m_ZDownstreamOfRich1 = 
       //  Rich1DE->userParameterAsDouble("RichZDownstreamOfRich1");
       
       m_Rich1TotPhotonSuppressFactor=  m_MaxHpdQuantumEff;
       

       //       m_Rich1TotPhotonSuppressFactor=   
       //  (m_MaxHpdQuantumEff * m_MaxRich1Mirror1Reflect
       //   * m_MaxRich1Mirror2Reflect);
       
       m_Rich2TotPhotonSuppressFactor=   m_MaxHpdQuantumEff;
       

     }
     
     return status;
     

}



void RichG4TrackActionPhotOpt::PreUserTrackingAction  
       ( const G4Track* aTrack ) {
   if( 0 == aTrack || 0 == trackMgr()) { return ; } /// RETURN !!!
 
  const G4DynamicParticle * aParticle = aTrack->GetDynamicParticle();
  if(aParticle->GetDefinition() == G4OpticalPhoton::OpticalPhoton() ) {
    const G4double ZPhotOrigin=  aTrack-> GetVertexPosition().z();
    G4double PhotonSupFact=  m_Rich1TotPhotonSuppressFactor;
    if(ZPhotOrigin >  m_ZDownstreamOfRich1){
      PhotonSupFact=  m_Rich2TotPhotonSuppressFactor;
    }
    
   
   G4double aRandomNum = G4UniformRand();
   if(aRandomNum >  PhotonSupFact ) {
     // Kill the photon       
       trackMgr()->GetTrack() ->SetTrackStatus(fStopAndKill);
     
   }
   
              
      
  }
  
}

void RichG4TrackActionPhotOpt::PostUserTrackingAction 
   ( const G4Track* aTrack ) 
{;} 

//=============================================================================
// ============================================================================
// The END 
// ============================================================================
