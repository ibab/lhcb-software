// $Id: GiGaTrajectory.cpp,v 1.22 2008-10-20 10:20:40 robbep Exp $
// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.21  2004/04/07 15:23:09  gcorti
// transfer signal info
//
// Revision 1.20  2004/03/17 08:54:59  ranjard
// v16r1 - fix printout
//
// Revision 1.19  2004/02/23 08:12:54  ibelyaev
//  urgent fix!
//
// Revision 1.18  2004/02/22 19:01:51  ibelyaev
//  add new data fields to GiGaTrajectoryPoint
//
// Revision 1.17  2004/02/22 13:38:19  ibelyaev
//  relocate DumpG4*.h files
//
// Revision 1.16  2004/02/20 18:13:35  ibelyaev
//  major update in GiGaBase and GiGaTrajectory
//
// ============================================================================
#define  GIGA_GIGATRAJECTORY_CPP 1
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/System.h"
// ============================================================================
/// from GiGa
// ============================================================================
#include "GiGa/GiGaTrajectory.h"
#include "GiGa/GiGaUtil.h"
#include "GiGa/GiGaException.h"
#include "GiGa/DumpG4Track.h"
#include "GiGa/DumpG4Step.h"
// ============================================================================
/// from Geant4
// ============================================================================
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

using CLHEP::HepLorentzVector;

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
}
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
  , m_creator                         ( 0 )
  , m_hasOscillated                   ( false )
  , m_isSignal                        ( false )
  , m_signalBarcode                   ( -1 )
  , m_pHepMCEvent                     ( 0 )
  , m_motherMCParticle                ( 0 )
{
#ifdef GIGA_DEBUG
  GiGaTrajectoryLocal::s_Counter.increment () ;
#endif
}
// ============================================================================

// ============================================================================
/** constructor
 *  @param aTrack pointer to the track
 */
// ============================================================================
GiGaTrajectory::GiGaTrajectory   ( const G4Track* aTrack )
  : G4VTrajectory(                                              )
    , std::vector<GiGaTrajectoryPoint*>   (                       )
    , m_trackID       ( aTrack->GetTrackID        ()                 )
    , m_parentID      ( aTrack->GetParentID       ()                 )
    , m_partDef       ( aTrack->GetDefinition     ()                 )
    , m_4vect         ( aTrack->GetDynamicParticle()->Get4Momentum() )
    , m_creator       ( aTrack->GetCreatorProcess ()                 )
    , m_hasOscillated ( false                                        )
    , m_isSignal      ( false                                        )
    , m_signalBarcode ( -1                                           )
    , m_pHepMCEvent   ( 0                                            )
    , m_motherMCParticle( 0 )
{
  ///
  double time = aTrack->GetGlobalTime() ;
  if( !finite( time ) )
  {
    std::cout << " GiGaTrajectory  GetGlobalTime ERROR "
              << std::string( 65 , '*' )     << std::endl << std::flush ;
    GiGaUtil::DumpG4Track( std::cout , aTrack ) ;
    std::cout << " GiGaTrajectory  GeGlobalTime ERROR Time is not finite"
              << " action : time = 1000 ns " << std::endl << std::flush ;

    time = 1000 * ns ;
    // throw GiGaException ( "GiGaTrajectory(): Time is not finite ") ; }
  }
  ///
  GiGaTrajectoryPoint* firstPoint =
    new GiGaTrajectoryPoint ( aTrack -> GetPosition() ,
                              time                    ,
                              HepLorentzVector()      , //empty!
                              m_creator               ) ;
  push_back( firstPoint );
  ///
#ifdef GIGA_DEBUG
  GiGaTrajectoryLocal::s_Counter.increment () ;
#endif
}
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
  , m_creator                         ( right.creator       ()       )
  , m_hasOscillated                   ( right.hasOscillated ()       )
  , m_isSignal                        ( right.isSignal      ()       )
  , m_signalBarcode                   ( right.signalBarcode ()       )
  , m_pHepMCEvent                     ( right.pHepMCEvent   ()       )
  , m_motherMCParticle                ( right.motherMCParticle()     )
{
  clear();
  for( const_iterator it = right.begin() ; right.end() != it ; ++it )
  {  push_back( (*it)->clone() ) ; }
#ifdef GIGA_DEBUG
  GiGaTrajectoryLocal::s_Counter.increment () ;
#endif
}
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
}
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
{
  return (void*) GiGaTrajectoryLocal::s_Allocator.MallocSingle();
}
// ============================================================================

// ============================================================================
/// overloaded delete operator
// ============================================================================
void  GiGaTrajectory::operator delete(void* traj )
{
  GiGaTrajectoryLocal::s_Allocator.FreeSingle( (GiGaTrajectory*) traj );
}
// ============================================================================

// ============================================================================
/// Draw the trajectory (G4)
// ============================================================================
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
}
// ============================================================================


// ============================================================================
/** 'almost' unconditionally append the step
 *  @param step  step to be appended
 *  @return flag
 */
// ============================================================================
bool GiGaTrajectory::appendStep ( const G4Step* step )
{
  if( 0 == step ) { return false ; }

  const G4StepPoint* point = step  -> GetPostStepPoint () ;
  double             time  = point -> GetGlobalTime    () ;

  if( !finite( time ) )
  {
    std::cout << " GiGaTrajectory appendStep ERROR "
              << std::string( 65 , '*' )     << std::endl ;
    GiGaUtil::DumpG4Step( std::cout , step ) ;
    std::cout << " GiGaTrajectory  appendStep ERROR Time is not finite"
              << " action : skip the step "  << std::endl << std::flush ;
    return false ;                                             // RETURN
    // throw GiGaException ( "GiGaTrajectory(): Time is not finite ") ; }
  }

  if ( empty() ||
       // fAlive != step->GetTrack()->GetTrackStatus()                  ||
       // point -> GetProcessDefinedStep () != back() -> GetProcess  () ||
       point -> GetGlobalTime         () != back() -> GetTime     () ||
       point -> GetPosition           () != back() -> GetPosition ()  )
  {
    GiGaTrajectoryPoint* p =
      new GiGaTrajectoryPoint ( point -> GetPosition           ()   ,
                                point -> GetGlobalTime         ()   ,
                                HepLorentzVector
                                ( point -> GetMomentum         () ,
                                  point -> GetTotalEnergy      () ) ,
                                point -> GetProcessDefinedStep ()   ) ;

    push_back( p );
  }
  else  { return false ; }                                     // RETURN

  ///
  return true ;
}
// ============================================================================

// ============================================================================
/// Append the step (G4VTrajectory)
// ============================================================================
void GiGaTrajectory::AppendStep      ( const G4Step*  step )
{
  bool append = false;
  // the first point
  if      ( empty()                                      ) { append = true ; }
  // if last point
  else if ( fAlive != step->GetTrack()->GetTrackStatus() ) { append = true ; }
  // for optical photons also the reflection/refraction step must be appended
  else if ( fGeomBoundary == step->GetPostStepPoint()->GetStepStatus()  &&
            step->GetTrack()->GetDefinition        () ==
            G4OpticalPhoton::OpticalPhoton         ()    ) { append = true ; }
  ///
  if ( append ) { appendStep ( step ) ; }
}

// ============================================================================
void GiGaTrajectory::ShowTrajectory  ( std::ostream& stream ) const
{
  stream << " GiGaTrajectory "
         << " trackID/ParentID "
         << trackID  () << "/"
         << parentID ()
         << " #point: "     << size ()
         << " Particle: "   << GetParticleName()
         << " Momentum: "   << momentum () << std::endl ;
  const G4VProcess* p = creator() ;
  if( 0 != p )
  {
    stream << " CreatorProcess  "
           << p -> GetProcessType() << "/"
           << G4VProcess::GetProcessTypeName ( p->GetProcessType() ) << "/"
           << p -> GetProcessName() << "/"
           << System::typeinfoName( typeid( *p ) ) << std::endl ;
  }

  for ( const_iterator it = begin() ; it != end() ; ++it )
  {
    const GiGaTrajectoryPoint* point = *it ;
    if( 0 == point ) { continue ; }
    stream << " \tPoint "
           << " #: "      << ( it - begin() )
           << " 3D,T: "   << point -> fourVector  ()
           << " Mom: "    << point -> momentum    () ;
    const G4VProcess* p = point -> process() ;
    if( 0 != p )
    {
      stream << " Process  "
             << p -> GetProcessType() << "/"
             << G4VProcess::GetProcessTypeName ( p->GetProcessType() ) << "/"
             << p -> GetProcessName() << "/"
             << System::typeinfoName( typeid( *p ) ) ;
    }
    stream << std::endl ;
  }

}
// ============================================================================

// ============================================================================
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
}
// ============================================================================


// ============================================================================
/// get particle name
// ============================================================================
G4String      GiGaTrajectory::GetParticleName () const
{
  if( 0 == partDef() )
    { throw GiGaException(" GiGaTrajectory: G4ParticleDefinition is NULL"); }
  return partDef()->GetParticleName();
}
// ============================================================================


// ============================================================================
/// get particle charge
// ============================================================================
G4double      GiGaTrajectory::GetCharge       () const
{
  if( 0 == partDef() )
    { throw GiGaException(" GiGaTrajectory: G4ParticleDefinition is NULL"); }
  return partDef()->GetPDGCharge();
}

// ============================================================================
/// get particle encoding
// ============================================================================
G4int         GiGaTrajectory::GetPDGEncoding  () const
{
  if( 0 == partDef() )
    { throw GiGaException(" GiGaTrajectory: G4ParticleDefinition is NULL"); }
  return partDef()->GetPDGEncoding();
}
// ============================================================================

// ============================================================================
G4ThreeVector GiGaTrajectory::GetInitialMomentum () const
{
  return momentum();
}
// ============================================================================

// ============================================================================
/// get the name of the creator process
// ============================================================================
const std::string& GiGaTrajectory::processname() const
{
  static const std::string s_unknown = "Unknown/NULL";
  if ( 0 == m_creator ) { return s_unknown ; }
  return m_creator->GetProcessName() ;
}
// ============================================================================

// ============================================================================
// The END
// ============================================================================









