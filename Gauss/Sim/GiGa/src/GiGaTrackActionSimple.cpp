/// CLEHP 
#include "CLHEP/Geometry/Point3D.h"
/// Geant4 
#include "G4Track.hh"
#include "G4TrackingManager.hh"
/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
/// GiGa 
#include "GiGa/GiGaTrackActionFactory.h"
#include "GiGa/GiGaTrajectory.h"
#include "GiGa/GiGaTrackInformation.h"
/// local
#include "GiGaTrackActionSimple.h"
///

///////////////////////////////////////////////////////////////////////////////////////////////////
static const GiGaTrackActionFactory<GiGaTrackActionSimple>          s_GiGaTrackActionSimpleFactory;
const       IGiGaTrackActionFactory& GiGaTrackActionSimpleFactory = s_GiGaTrackActionSimpleFactory;
///////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////
GiGaTrackActionSimple::GiGaTrackActionSimple( const std::string& Name , ISvcLocator* Loc ) 
  : GiGaTrackActionBase( Name , Loc ) 
{};
/////////////////////////////////////////////////////////////////////////////////////////////////
GiGaTrackActionSimple::~GiGaTrackActionSimple(){};
/////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaTrackActionSimple::initialize () { return GiGaTrackActionBase::initialize() ; } ;
/////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaTrackActionSimple::finalize   () { return GiGaTrackActionBase::initialize() ; } ;
/////////////////////////////////////////////////////////////////////////////////////////////////
void GiGaTrackActionSimple::PreUserTrackingAction  ( const G4Track* track ) 
{
  /// Is the track valid? Is tracking manager valid? Does trajectory already exist?
  if( 0 == track || 0 == trackMgr() || 0 != trackMgr()->GimmeTrajectory()  ) { return ; } 
  ///
  { 
    /// attach GiGaTrackInformation to the track, if the no such information exists 
    if( 0 == track->GetUserInformation() )
      { trackMgr()->SetUserTrackInformation( new GiGaTrackInformation() ); }
  }
  ///
  {
    /// create GiGaTrajectory and inform Tracking Manager  
    trackMgr()->SetTrajectory( new GiGaTrajectory( track ) ) ;
  }
  ///
};
/////////////////////////////////////////////////////////////////////////////////////////////////
void GiGaTrackActionSimple::PostUserTrackingAction ( const G4Track* track ) 
{
  /// Is the track valid? Is tracking manager valid?
  if( 0 == track || 0 == trackMgr()    )   { return ; }
  ////
  /// store trajectory?
  ///  (1) nothing to  be stored or already marked as stored  
  if( 0 == trackMgr()->GimmeTrajectory() || trackMgr()->GetStoreTrajectory() ) { return; } 
  ///  (2) store   all primary particles  
  if( 0 == track->GetParentID() )     { trackMgr()->SetStoreTrajectory( true ) ; return ; } 
  ///  (3) store  tracks, marked through GiGaTrackInformation class
  {
    G4VUserTrackInformation* ui = track->GetUserInformation(); 
    GiGaTrackInformation*    gi = 
      ( 0 == ui )  ? 0 : dynamic_cast<GiGaTrackInformation*> ( ui );
    if( 0 != gi && gi->toBeStored() ) { trackMgr()->SetStoreTrajectory( true ) ; return ; } 
  }  
  ///  
};
/////////////////////////////////////////////////////////////////////////////////////////////////

 
