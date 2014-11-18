// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $
// ============================================================================

// ============================================================================
#define  GAUSSTOOLS_GAUSSTRAJECTORY_CPP 1 
// ============================================================================
/// from GaussTools
#include "GaussTools/GaussTrajectory.h"
#include "GaussTools/GaussTrackInformation.h"
#include "GiGa/GiGaUtil.h"
///
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4SteppingManager.hh"

// ============================================================================
namespace GaussTrajectoryLocal
{
  //
  G4Allocator<GaussTrajectory> s_Allocator ;
  //
#ifdef GIGA_DEBUG
  /** @var s_Counter
   *  static instance ounter for class GaussTrajectory
   */
  static GiGaUtil::InstanceCounter<GaussTrajectory> s_Counter;
#endif 
}


// ============================================================================
/// constructor 
// ============================================================================
GaussTrajectory::GaussTrajectory ( ) 
  : GiGaTrajectory()
{
#ifdef GIGA_DEBUG
  GaussTrajectoryLocal::s_Counter.increment();
#endif 
}


// ============================================================================
/// constructor 
// ============================================================================
GaussTrajectory::GaussTrajectory   ( const G4Track* aTrack )
  : GiGaTrajectory ( aTrack ) 
{
#ifdef GIGA_DEBUG
  GaussTrajectoryLocal::s_Counter.increment();
#endif
}

// ============================================================================
/// constructor 
// ============================================================================
GaussTrajectory::GaussTrajectory ( const GiGaTrajectory & right )
  : GiGaTrajectory ( right )
{
#ifdef GIGA_DEBUG
  GaussTrajectoryLocal::s_Counter.increment();
#endif
}

// ============================================================================
/// constructor 
// ============================================================================
GaussTrajectory::GaussTrajectory ( const GaussTrajectory & right )
  : GiGaTrajectory ( right )
{
#ifdef GIGA_DEBUG
  GaussTrajectoryLocal::s_Counter.increment();
#endif
}

// ============================================================================
/// clone 
// ============================================================================
GiGaTrajectory* GaussTrajectory::clone() const { 
  return new GaussTrajectory( *this ); 
}


// ============================================================================
/// destructor 
// ============================================================================
GaussTrajectory::~GaussTrajectory()
{
#ifdef GIGA_DEBUG
  GaussTrajectoryLocal::s_Counter.decrement();
#endif
}

// ============================================================================
/// new 
// ============================================================================
void* GaussTrajectory::operator new(size_t) { 
  return (void*) GaussTrajectoryLocal::s_Allocator.MallocSingle() ; 
} 

// ============================================================================
/// delete 
// ============================================================================
void  GaussTrajectory::operator delete ( void* aTrajectory )
{ GaussTrajectoryLocal::
  s_Allocator.FreeSingle( (GaussTrajectory*) aTrajectory ) ; 
}

// ============================================================================
/// append the step 
// ============================================================================
void GaussTrajectory::AppendStep      ( const G4Step*  step )       
{

  // size before append 
  const size_t size_0  = size() ;

  // 1) 'append' using GiGaTrajectory class 
  GiGaTrajectory::AppendStep( step ) ;
  
  // 2) appended? 
  if ( size() != size_0 ) { return ; }                          // RETURN

  // 3) specific "append" action 
  
  G4VUserTrackInformation* uinf 
    = step->GetTrack()->GetUserInformation();
  if ( 0 == uinf       ) { return  ; }                          // RETURN 
  
  GaussTrackInformation* ginf = gaussTrackInformation( uinf ) ;
  if ( 0 == ginf       ) { return  ; }                          // RETURN 
  
  if ( ginf -> appendStep() ) 
  { 
    appendStep( step ) ;
    ginf -> setAppendStep( false ) ;
  }
}



// ============================================================================
// The END 
// ============================================================================
