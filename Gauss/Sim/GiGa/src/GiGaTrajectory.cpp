
#include "GiGa/GiGaTrajectory.h"

#include "G4Track.hh"
#include "G4Step.hh"

#include "G4Polyline.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4VVisManager.hh"

////////////////////////////////////////////////////////////////////////////////////
G4Allocator<GiGaTrajectory> s_GiGaTrajectoryAllocator;
////////////////////////////////////////////////////////////////////////////////////
GiGaTrajectory::GiGaTrajectory ()
  : G4VTrajectory(   ) 
  , m_points     (   ) 
  , m_trackID    ( 0 ) 
  , m_parentID   ( 0 )
  , m_partDef    ( 0 ) 
  , m_4vect      (   )
{};
//////////////////////////////////////////////////////////////////////////////////// 
GiGaTrajectory::GiGaTrajectory ( const G4Track* aTrack  )
  : G4VTrajectory(   ) 
  , m_trackID    ( aTrack->GetTrackID        ()                 ) 
  , m_parentID   ( aTrack->GetParentID       ()                 )
  , m_partDef    ( aTrack->GetDefinition     ()                 ) 
  , m_4vect      ( aTrack->GetDynamicParticle()->Get4Momentum() )
{
  ///
  GiGaTrajectoryPoint* firstPoint = 
    new GiGaTrajectoryPoint( aTrack->GetPosition() , aTrack->GetGlobalTime() );
  m_points.push_back( firstPoint );
  ///
};
//////////////////////////////////////////////////////////////////////////////////// 
GiGaTrajectory::GiGaTrajectory( const GiGaTrajectory& right )
  : G4VTrajectory(                      )
  , m_trackID    ( right.trackID      () )
  , m_parentID   ( right.parentID     () )
  , m_partDef    ( right.partDef      () )
  , m_4vect      ( right.fourMomentum () )
{
  /// delete all points and clear container 
  for( IT i1 = m_points.begin() ; i1 != m_points.end() ; ++i1 )
    { if( 0 != *i1 ) { delete *i1 ; *i1 = 0 ; } }
  m_points.clear();
  /// copy points from right 
  for( CI i2 = right.begin() ; i2 != right.end() ; ++i2 )
    { if( 0 != *i2 ) { m_points.push_back( new GiGaTrajectoryPoint( **i2 ) ) ; } }
};
//////////////////////////////////////////////////////////////////////////////////// 
GiGaTrajectory::~GiGaTrajectory()
{
  /// delete all points 
  for( IT it = m_points.begin() ; it != m_points.end() ; ++it )
    { if( 0 != *it ) { delete *it ; *it = 0 ; } }  
  /// clear container 
  m_points.clear();
  ///
};
//////////////////////////////////////////////////////////////////////////////////// 
void GiGaTrajectory::ShowTrajectory  () const
{
};
//////////////////////////////////////////////////////////////////////////////////// 
void GiGaTrajectory::DrawTrajectory  (G4int i_mode=0 ) const 
{
  ///
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  G4ThreeVector pos;
  ///
  if(i_mode>=0)
    {
      G4Polyline pPolyline;
      for( CI it = begin() ; end() != it ; ++it ) 
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
      for( CI it = begin() ; end() != it ; ++it ) 
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
//////////////////////////////////////////////////////////////////////////////////// 
void GiGaTrajectory::AppendStep      ( const G4Step*  aStep )       
{
  ///
  GiGaTrajectoryPoint* p = 
    new GiGaTrajectoryPoint( aStep->GetPostStepPoint()->GetPosition   () ,
			     aStep->GetPostStepPoint()->GetGlobalTime () ) ; 
  ///
  m_points.push_back( p );
  ///
};
//////////////////////////////////////////////////////////////////////////////////// 
void GiGaTrajectory::MergeTrajectory ( G4VTrajectory* st )       
{
  if( 0 == st ) { return ; } 
  GiGaTrajectory* gt = dynamic_cast<GiGaTrajectory*> ( st );
  if( 0 == gt ) { return ; } 
  ///
  for( IT it = ( gt->GetPointEntries() > 1 ) ? gt->begin() + 1 : gt->end() ; gt->end() != it ; ++it )
    {
      if( 0 == *it ) continue; 
      m_points.push_back( new GiGaTrajectoryPoint( **it ) ); 
      delete *it ;  *it = 0;
    } 
  ///
  gt->m_points.clear();
  ///
};
/////////////////////////////////////////////////////////////////////////////////// 
















