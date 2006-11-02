// $Id: RichG4TrackActionAerogelPhoton.cpp,v 1.1 2006-11-02 10:26:34 seaso Exp $
// Include files 
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
#include "GaussTools/GaussTrackInformation.h"
#include "RichInfo.h"
#include "RichPhotInfo.h"


/// local
#include "RichG4AnalysisConstGauss.h"
#include "RichG4GaussPathNames.h"
#include "RichG4SvcLocator.h"
#include "RichG4TrackActionAerogelPhoton.h"
#include "RichG4MatRadIdentifier.h"
#include "RichG4RadiatorMaterialIdValues.h"


//-----------------------------------------------------------------------------
// Implementation file for class : RichG4TrackActionAerogelPhoton
//
// 2006-11-01 : Sajan EASO
//-----------------------------------------------------------------------------
//
/// factory business
// ============================================================================
IMPLEMENT_GiGaFactory( RichG4TrackActionAerogelPhoton );
// ============================================================================

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4TrackActionAerogelPhoton::RichG4TrackActionAerogelPhoton
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaTrackActionBase( type , name , parent ),
    m_ActivateAerogelFilter(true),
    m_EnergyCutoffInAerogel(4.5*eV),
    m_EnergyCutoffNoFilterInAerogel(6.5*eV),
    m_EnergyCutoffWithFilterInAerogel(4.5*eV)
{
  declareProperty("ActivateAerogelFilter", m_ActivateAerogelFilter);
  declareProperty("MaxEnergyCutoffNoFilterInAerogel", m_EnergyCutoffNoFilterInAerogel);
  declareProperty("MaxEnergyCutoffWithFilterInAerogel",m_EnergyCutoffWithFilterInAerogel);

}
//=============================================================================
// Destructor
//=============================================================================
RichG4TrackActionAerogelPhoton::~RichG4TrackActionAerogelPhoton() {} 

//=============================================================================
StatusCode RichG4TrackActionAerogelPhoton::initialize() 
{
 // initialize the base
  StatusCode status = GiGaTrackActionBase::initialize() ;
  if( status.isFailure() )
    { return Error("Could not intialize base class GiGaTrackActionBase!",
                   status ) ; 
    }
 
    if( m_ActivateAerogelFilter ) {
       m_EnergyCutoffInAerogel = m_EnergyCutoffWithFilterInAerogel;
    }else {
      m_EnergyCutoffInAerogel =  m_EnergyCutoffNoFilterInAerogel;    
    }
  
 return status; 
}

void  RichG4TrackActionAerogelPhoton::PreUserTrackingAction
       ( const G4Track* aTrack ) {
  if( 0 == aTrack || 0 == trackMgr()) { return ; } /// RETURN !!!
  const G4DynamicParticle * aParticle = aTrack->GetDynamicParticle();
  if(aParticle->GetDefinition() == G4OpticalPhoton::OpticalPhoton() ) {

        RichG4MatRadIdentifier* aRichG4MatRadIdentifier =
                          RichG4MatRadIdentifier::RichG4MatRadIdentifierInstance();

        G4VUserTrackInformation* aUserTrackInfo=aTrack->GetUserInformation();
        GaussTrackInformation* aRichPhotTrackInfo
                                      = (GaussTrackInformation*)aUserTrackInfo;
        if( aRichPhotTrackInfo) {
             if( aRichPhotTrackInfo->detInfo() ){
              RichInfo* aRichTypeInfo =( RichInfo*) (aRichPhotTrackInfo->detInfo());

              if( aRichTypeInfo && aRichTypeInfo->HasUserPhotInfo() ){
               RichPhotInfo* aRichPhotInfo = aRichTypeInfo-> RichPhotInformation();

              if( aRichPhotInfo ) {
                  G4int CurPhotProdRadiator = aRichPhotInfo-> PhotProdRadiatorNum();
                  if( CurPhotProdRadiator >= Rich1AgelTile0CkvRadiatorNum &&
                      CurPhotProdRadiator <= Rich1AgelTile15CkvRadiatorNum) {
                      G4double CurPhotonMomentum =  aRichPhotInfo-> CkvPhotonEnergyAtProd();
                      if( CurPhotonMomentum > m_EnergyCutoffInAerogel ) {
                        // This photon is above the cutoff energy in Aerogel.
                        // Hence kill this photon
                        //                        G4cout<<" Now killing photon in aerogel with momentum and cutoff "
                        //      <<CurPhotonMomentum<<"  "<<m_EnergyCutoffInAerogel<<G4endl;
                        
                          trackMgr()->GetTrack() ->SetTrackStatus(fStopAndKill);
                      }
                  }}}}}}
}
void RichG4TrackActionAerogelPhoton::PostUserTrackingAction
   ( const G4Track* aTrack )
{;}

//=============================================================================
// ============================================================================
// The END
// ============================================================================


