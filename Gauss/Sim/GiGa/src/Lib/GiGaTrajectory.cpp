// $Id: GiGaTrajectory.cpp,v 1.11 2003-03-11 09:34:55 ibelyaev Exp $ 
// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.10  2002/12/16 16:23:15  ibelyaev
//  update for newer version of Geant4 (release 5.0)
//
// Revision 1.9  2002/12/07 14:27:51  ibelyaev
//  see $GIGAROOT/cmt/requirements file
//
// ============================================================================
#define  GIGA_GIGATRAJECTORY_CPP 1 
// ============================================================================
/// from GiGa
#include "GiGa/GiGaTrajectory.h"
#include "GiGa/GiGaUtil.h"
#include "GiGa/GiGaException.h"
/// from Geant4 
#include "G4Polyline.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4Allocator.hh"
#include "G4VisAttributes.hh"
#include "G4VVisManager.hh"
///
#include "G4OpticalPhoton.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4SteppingManager.hh"
#include "G4ParticleDefinition.hh"

// ============================================================================
/** @file 
 *  iplementation of class GiGaTrajectory
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    22/02/2001 
 */
// ============================================================================

// ============================================================================
namespace GiGaTrajectoryLocal
{
  // ==========================================================================
  /** @var s_Allocator 
   *  G4 allocator for GiGaTrakectory objects  
   */
  // ==========================================================================
  G4Allocator<GiGaTrajectory> s_Allocator;
  // ==========================================================================
  /** @var s_Counter 
   *  static instance counter
   */
  // ==========================================================================
#ifdef GIGA_DEBUG
  static GiGaUtil::InstanceCounter<GiGaTrajectory> s_Counter ;
#endif 
  // ==========================================================================
};
// ============================================================================

// ============================================================================
/// constructor 
// ============================================================================
GiGaTrajectory::GiGaTrajectory (   )
  : G4VTrajectory                     (   ) 
  , std::vector<GiGaTrajectoryPoint*> (   ) 
  , m_trackID                         ( 0 ) 
  , m_parentID                        ( 0 )
  , m_partDef                         ( 0 ) 
  , m_4vect                           (   )
{
#ifdef GIGA_DEBUG
  GiGaTrajectoryLocal::s_Counter.increment () ;
#endif 
};
// ============================================================================

// ============================================================================
/** constructor 
 *  @param aTrack pointer to the track
 */
// ============================================================================
GiGaTrajectory::GiGaTrajectory   ( const G4Track* aTrack )
  : G4VTrajectory(                                              ) 
  , std::vector<GiGaTrajectoryPoint*>   (                       )
  , m_trackID    ( aTrack->GetTrackID        ()                 ) 
  , m_parentID   ( aTrack->GetParentID       ()                 )
  , m_partDef    ( aTrack->GetDefinition     ()                 ) 
  , m_4vect      ( aTrack->GetDynamicParticle()->Get4Momentum() )
{
  ///
  GiGaTrajectoryPoint* firstPoint = 
    new GiGaTrajectoryPoint( aTrack->GetPosition() , aTrack->GetGlobalTime() );
  push_back( firstPoint );
  ///
#ifdef GIGA_DEBUG
  GiGaTrajectoryLocal::s_Counter.increment () ;
#endif
};
// ============================================================================

// ============================================================================
/** constructor 
 *  @param right object to be copied 
 */
// ============================================================================
GiGaTrajectory::GiGaTrajectory ( const GiGaTrajectory & right )
  : G4VTrajectory                     (                              )
  , std::vector<GiGaTrajectoryPoint*> (                              )
  , m_trackID                         ( right.trackID       ()       )
  , m_parentID                        ( right.parentID      ()       )
  , m_partDef                         ( right.partDef       ()       )
  , m_4vect                           ( right.fourMomentum  ()       )
{
  clear();
  for( const_iterator it = right.begin() ; right.end() != it ; ++it ) 
    {  push_back( (*it)->clone() ) ; }
#ifdef GIGA_DEBUG
  GiGaTrajectoryLocal::s_Counter.increment () ;
#endif
};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GiGaTrajectory::~GiGaTrajectory()
{
  /// delete all points 
  for( iterator it = begin() ; it != end() ; ++it )
    { if( 0 != *it ) { delete *it ; *it = 0 ; } }  
  /// clear container 
  clear();
  ///
#ifdef GIGA_DEBUG
  GiGaTrajectoryLocal::s_Counter.decrement () ;
#endif
};
// ============================================================================

// ============================================================================
/** clone (virtual constructor) method 
 *  @return clone of current trajectory
 */
// ============================================================================
GiGaTrajectory* GiGaTrajectory::clone() const 
{ return new GiGaTrajectory( *this ); }
// ============================================================================

// ============================================================================
/// overloaded new  operator 
// ============================================================================
void* GiGaTrajectory::operator new(size_t)
{ return (void*) GiGaTrajectoryLocal::s_Allocator.MallocSingle(); };
// ============================================================================

// ============================================================================
/// overloaded delete operator 
// ============================================================================
void  GiGaTrajectory::operator delete(void* traj )
{ GiGaTrajectoryLocal::s_Allocator.FreeSingle( (GiGaTrajectory*) traj ); };
// ============================================================================

///
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
        { if( 0 != *it ) { pPolyline.push_back( (*it)->GetPosition() ) ;}}
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
///
void GiGaTrajectory::AppendStep      ( const G4Step*  step )       
{
  ///
  bool append = false;
  /// 
  if     ( empty()                                       )  
    { append = true ; } 
  // if some information is not available
  // follow ordinary routine and just add the step
  // else if( 0 == step->GetTrack()                      || 
  //          0 == stepMgr()                             || 
  //         step != stepMgr()->GetStep()               || 
  //         step->GetTrack() != stepMgr()->GetTrack    () ) 
  //  { append = true ; }   
  /// if  it is the last step, the step must be appended 
  else if ( fAlive != step->GetTrack()->GetTrackStatus() ) 
    { append = true ; }
  //   if  there are some secondaries, the step must be appended  
  //   else if ( 0 != stepMgr()->GetSecondary()            && 
  //  	    0 != stepMgr()->GetSecondary()->size   () ) { append = true ; }
  // for optical photons also the reflection/refraction step must be appended  
  else if ( step->GetPostStepPoint()->GetStepStatus() == 
            fGeomBoundary &&
            step->GetTrack()->GetDefinition        () == 
            G4OpticalPhoton::OpticalPhoton         ()    ) { append = true ; }
  ///
  if( append && 
      ( empty()                                                              || 
        step->GetPostStepPoint()->GetGlobalTime () != back()->GetTime     () ||
        step->GetPostStepPoint()->GetPosition() != back()->GetPosition () ) ) 
    {
      GiGaTrajectoryPoint* p = 
        new GiGaTrajectoryPoint( step->GetPostStepPoint()->GetPosition   () ,
                                 step->GetPostStepPoint()->GetGlobalTime () ) ; 
      push_back( p );
      ///
    };
  ///
};
///
void GiGaTrajectory::ShowTrajectory  () const {};
///
void GiGaTrajectory::MergeTrajectory ( G4VTrajectory* st )       
{
  if( 0 == st                    ) { return ; } 
  /// do not copy "almost empty" trajectories
  if( 1 >= st->GetPointEntries() ) { return ; } 
  GiGaTrajectory* gt = dynamic_cast<GiGaTrajectory*> ( st );
  if( 0 == gt || this == gt      ) { return ; } 
  /// copy points 
  /// skip the first point from copy
  iterator it = gt->empty() ? gt->end() : gt->begin() + 1 ; 
  std::copy( it , gt->end() , std::back_inserter( *this ) );
  /// remove copied points   
  gt->erase( it , gt->end() );
  ///
};
//

/// get particle name 
G4String      GiGaTrajectory::GetParticleName () const 
{
  if( 0 == partDef() ) 
    { throw GiGaException(" GiGaTrajectory: G4ParticleDefinition is NULL"); }
  return partDef()->GetParticleName();
};


/// get particle charge 
G4double      GiGaTrajectory::GetCharge       () const 
{
  if( 0 == partDef() ) 
    { throw GiGaException(" GiGaTrajectory: G4ParticleDefinition is NULL"); }
  return partDef()->GetPDGCharge();
};

/// get particle encoding
G4int         GiGaTrajectory::GetPDGEncoding  () const 
{
  if( 0 == partDef() ) 
    { throw GiGaException(" GiGaTrajectory: G4ParticleDefinition is NULL"); }
  return partDef()->GetPDGEncoding();
};

G4ThreeVector GiGaTrajectory::GetInitialMomentum () const 
{ return momentum(); };

// ============================================================================
// The END 
// ============================================================================









