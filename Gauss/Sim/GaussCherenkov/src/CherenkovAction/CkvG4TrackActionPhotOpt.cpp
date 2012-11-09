// $Id: CkvG4TrackActionPhotOpt.cpp,v 1.7 2009-07-03 11:59:49 seaso Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/IMessageSvc.h"

// CLHEP
#include "CLHEP/Geometry/Point3D.h"

// Geant4 
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

// GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"

// GiGa 
#include "GiGa/GiGaTrajectory.h"

// LHCb
#include "DetDesc/DetectorElement.h"

// local
#include "GaussRICH/RichG4AnalysisConstGauss.h"
#include "GaussRICH/RichG4GaussPathNames.h"
#include "GaussCherenkov/CkvG4GaussPathNames.h"
#include "CkvG4TrackActionPhotOpt.h"
#include "GaussRICH/RichG4SvcLocator.h"
#include "RichPmtProperties.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CkvG4TrackActionPhotOpt
//
// 2003-04-29 : Sajan EASO
// 2007-01-11 : Gloria Corti, modified for Gaudi v19
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CkvG4TrackActionPhotOpt );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CkvG4TrackActionPhotOpt::CkvG4TrackActionPhotOpt
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaTrackActionBase( type , name , parent ), m_MaxPhdQuantumEffFromDB(0),
     m_MaxRich1Mirror1Reflect(0), m_MaxRich1Mirror2Reflect(0),
     m_MaxRich2Mirror1Reflect(0),  m_MaxRich2Mirror2Reflect(0),
      m_ZDownstreamOfRich1(0), m_Rich1TotPhotonSuppressFactor(0), 
    m_Rich2TotPhotonSuppressFactor(0),
    m_RichPhdMaxQEOverRideDB(false),
    m_RichPhdMaxQEValueWhenOverRideDB(0.60)  
{ 
  declareProperty("RichPhdMaxQEOverRideDB",m_RichPhdMaxQEOverRideDB);
  declareProperty("RichPhdMaxQEValueWhenOverRideDB",m_RichPhdMaxQEValueWhenOverRideDB);  

}

//=============================================================================
// Destructor
//=============================================================================
CkvG4TrackActionPhotOpt::~CkvG4TrackActionPhotOpt(){ }

//=============================================================================
// initialize
//=============================================================================
StatusCode CkvG4TrackActionPhotOpt::initialize() 
{

 // initialize the base 
  StatusCode status = GiGaTrackActionBase::initialize() ; 

 if( status.isFailure() ) 
    { return Error("Could not intialize base class GiGaTrackActionBase!", 
                   status ) ; } 
  IMessageSvc*  msgSvc = RichG4SvcLocator::RichG4MsgSvc ();
  MsgStream log( msgSvc , "CkvG4TrackActionPhotOpt" );


 //the following may be extracted from gigabase in the future.
  IDataProviderSvc* detSvc = RichG4SvcLocator:: RichG4detSvc ();
 

  SmartDataPtr<DetectorElement> Rich1DE(detSvc,Rich1DeStructurePathName  );
     if( !Rich1DE ){
       return Error( 
      "Can't retrieve  " +Rich1DeStructurePathName  +" in CkvG4TrackActionPhotOpt" );
     } 
     else {
       
       m_MaxPhdQuantumEffFromDB=Rich1DE->param<double>("RichPmtMaxQE");
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



       //       m_MaxHpdQuantumEff=Rich1DE->userParameterAsDouble("RichPmtMaxQE");
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
       
       m_Rich1TotPhotonSuppressFactor=  m_MaxPhdQuantumEffFromDB;
       if(m_RichPhdMaxQEOverRideDB) {
         m_Rich1TotPhotonSuppressFactor= m_RichPhdMaxQEValueWhenOverRideDB;
         RichPmtProperties* aRichPmtProperties = RichPmtProperties::getRichPmtPropertiesInstance();
         aRichPmtProperties->setPmtActivateOverRideMaxQEFromDB(m_RichPhdMaxQEOverRideDB);
         aRichPmtProperties->setPmtDBOverRideMaxQEValue(m_RichPhdMaxQEValueWhenOverRideDB);
         aRichPmtProperties-> setPmtMaximumQuantumEfficiency();
       }
       
       log <<MSG::INFO<<" Rich PhotoDet (PMT)  Max QE from DB and UsedInThisRun are  "
                <<m_MaxPhdQuantumEffFromDB<<"    "<< m_Rich1TotPhotonSuppressFactor<<endreq;
       

       //       m_Rich1TotPhotonSuppressFactor=   
       //  (m_MaxPhdQuantumEff * m_MaxRich1Mirror1Reflect
       //   * m_MaxRich1Mirror2Reflect);
       
       m_Rich2TotPhotonSuppressFactor=   m_MaxPhdQuantumEffFromDB;
       if(m_RichPhdMaxQEOverRideDB) {
        m_Rich2TotPhotonSuppressFactor= m_RichPhdMaxQEValueWhenOverRideDB;  
       }
       
       

     }
     
     return status;
     

}

//=============================================================================
// PreUserTrackingAction (Geant4)
//=============================================================================
void CkvG4TrackActionPhotOpt::PreUserTrackingAction  
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

//=============================================================================
// PostUserTrackingAction (Geant4)
//=============================================================================
//void CkvG4TrackActionPhotOpt::PostUserTrackingAction 
//   ( const G4Track* aTrack ) 
//{ } 

//=============================================================================

