/// from GiGa
#include "GiGa/GiGaTrajectory.h"
/// from Geant4 
#include "G4Polyline.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4VVisManager.hh"
///
#include "G4OpticalPhoton.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4SteppingManager.hh"

//////////////////////////////////////////////////////////////////////////////////////////////
G4Allocator<GiGaTrajectory> s_GiGaTrajectoryAllocator;
//////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////
GiGaTrajectory::GiGaTrajectory (   )
  : G4VTrajectory                     (   ) 
  , std::vector<GiGaTrajectoryPoint*> (   ) 
  , m_trackID                         ( 0 ) 
  , m_parentID                        ( 0 )
  , m_partDef                         ( 0 ) 
  , m_4vect                           (   )
  , m_stepMgr                         ( 0 )
{};
///////////////////////////////////////////////////////////////////////////////////////////
GiGaTrajectory::GiGaTrajectory   ( const G4Track* aTrack )
  : G4VTrajectory(                                              ) 
  , std::vector<GiGaTrajectoryPoint*>   (                       )
  , m_trackID    ( aTrack->GetTrackID        ()                 ) 
  , m_parentID   ( aTrack->GetParentID       ()                 )
  , m_partDef    ( aTrack->GetDefinition     ()                 ) 
  , m_4vect      ( aTrack->GetDynamicParticle()->Get4Momentum() )
  , m_stepMgr    ( 0 )
{
  ///
  GiGaTrajectoryPoint* firstPoint = 
    new GiGaTrajectoryPoint( aTrack->GetPosition() , aTrack->GetGlobalTime() );
  push_back( firstPoint );
  ///
};
///////////////////////////////////////////////////////////////////////////////////////////
GiGaTrajectory::GiGaTrajectory ( const GiGaTrajectory & right )
  : G4VTrajectory                     (                              )
  , std::vector<GiGaTrajectoryPoint*> (                              )
  , m_trackID                         ( right.trackID       ()       )
  , m_parentID                        ( right.parentID      ()       )
  , m_partDef                         ( right.partDef       ()       )
  , m_4vect                           ( right.fourMomentum  ()       )
  , m_stepMgr                         ( right.stepMgr       ()       )
{
  // does not work on NT
  // copy(through clone) points from right 
  //    std::transform( right.begin() , right.end() , 
  //                    std::back_inserter( *this ) ,
  //                    std::mem_fun(&GiGaTrajectoryPoint::clone) );
  clear();
  for( GiGaTrajectory::const_iterator it = right.begin() ; right.end() != it ; ++it )
    {  push_back( (*it)->clone() ) ; }
};
///////////////////////////////////////////////////////////////////////////////////////////
GiGaTrajectory::~GiGaTrajectory()
{
  /// delete all points 
  for( iterator it = begin() ; it != end() ; ++it )
    { if( 0 != *it ) { delete *it ; *it = 0 ; } }  
  /// clear container 
  clear();
  ///
  m_stepMgr = 0 ;
  ///
};
//////////////////////////////////////////////////////////////////////////////////////////////
void GiGaTrajectory::DrawTrajectory  ( G4int i_mode ) const 
{
  ///
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  G4ThreeVector pos;
  ///
  if(i_mode>=0)
    {
      G4Polyline pPolyline;
      for( const_iterator it = begin() ; end() != it ; ++it ) 
        { if( 0 != *it ) { pPolyline.append( (*it)->GetPosition() ) ; } } 
      G4Colour colour;
      if     ( 0 != partDef() && partDef()->GetPDGCharge() < 0.) 
        { colour = G4Colour(1.,0.,0.); } 
      else if( 0 != partDef() && partDef()->GetPDGCharge() > 0.) 
        { colour = G4Colour(0.,0.,1.); } 
      else 
        { colour = G4Colour(0.,1.,0.); } 
      G4VisAttributes attribs(colour);
      pPolyline.SetVisAttributes(attribs);
      if(pVVisManager) pVVisManager->Draw(pPolyline);
    }
  ///
  if(i_mode!=0)
    {
      for( const_iterator it = begin() ; end() != it ; ++it ) 
        {
          if( 0 == *it ) { continue; } 
          G4Circle circle ( (*it)->GetPosition()  ); 
          circle.SetScreenSize( 0.001*i_mode      ); 
          circle.SetFillStyle ( G4Circle::filled  );
          G4Colour colSpot    ( 0. , 0. , 0.      );
          G4VisAttributes attSpot( colSpot );
          circle.SetVisAttributes( attSpot );
          if(pVVisManager) { pVVisManager->Draw( circle ); } 
        } 
    }
};
//////////////////////////////////////////////////////////////////////////////////////////////
void GiGaTrajectory::AppendStep      ( const G4Step*  step )       
{
  ///
  bool append = false;
  /// 
  if     ( empty()                                       )  { append = true ; } 
  /// if some information is not available, follow ordinary routine and just add the step
  else if( 0 == step->GetTrack()                      || 
	   0 == stepMgr()                             || 
	   step != stepMgr()->GetStep()               || 
	   step->GetTrack() != stepMgr()->GetTrack    () )  { append = true ; }   
  /// if  it is the last step, the step must be appended 
  else if ( fAlive != step->GetTrack()->GetTrackStatus() )  { append = true ; }
  /// if  there are some secondaries, the step must be appended  
  // else if ( 0 != stepMgr()->GetSecondary()            && 
  //	    0 != stepMgr()->GetSecondary()->entries   () )  { append = true ; }
  /// for optical photons also the reflection/refraction step must be appended  
  else if ( step->GetPostStepPoint()->GetStepStatus() == 
	    fGeomBoundary &&
	    step->GetTrack()->GetDefinition        () == 
	    G4OpticalPhoton::OpticalPhoton         ()    )  { append = true ; }
  ///
  if( append && 
      ( empty()                                                              || 
	step->GetPostStepPoint()->GetGlobalTime () != back()->GetTime     () ||
	step->GetPostStepPoint()->GetPosition   () != back()->GetPosition () ) ) 
    {
      GiGaTrajectoryPoint* p = 
	new GiGaTrajectoryPoint( step->GetPostStepPoint()->GetPosition   () ,
				 step->GetPostStepPoint()->GetGlobalTime () ) ; 
      push_back( p );
      ///
    };
  ///
};
//////////////////////////////////////////////////////////////////////////////////// 
void GiGaTrajectory::ShowTrajectory  () const {};
//////////////////////////////////////////////////////////////////////////////////// 
void GiGaTrajectory::MergeTrajectory ( G4VTrajectory* st )       
{
  if( 0 == st                    ) { return ; } 
  if( 1 >= st->GetPointEntries() ) { return ; }  /// do not copy "almost empty" trajectories
  GiGaTrajectory* gt = dynamic_cast<GiGaTrajectory*> ( st );
  if( 0 == gt || this == gt      ) { return ; } 
  /// copy points 
  iterator it = gt->empty() ? gt->end() : gt->begin() + 1 ;  /// skip the first point from copy
  std::copy( it , gt->end() , std::back_inserter( *this ) );
  /// remove copied points   
  gt->erase( it , gt->end() );
  ///
};
/////////////////////////////////////////////////////////////////////////////////// 











