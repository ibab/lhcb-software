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
};
// ============================================================================

// ============================================================================
/// constructor 
// ============================================================================
GaussTrajectory::GaussTrajectory ( ) 
  : GiGaTrajectory                      (   ) 
{
#ifdef GIGA_DEBUG
  GaussTrajectoryLocal::s_Counter.increment();
#endif 
};
// ============================================================================

// ============================================================================
/// constructor 
// ============================================================================
GaussTrajectory::GaussTrajectory   ( const G4Track* aTrack )
  : GiGaTrajectory(aTrack) 
{
#ifdef GIGA_DEBUG
  GaussTrajectoryLocal::s_Counter.increment();
#endif
};

// ============================================================================
/// constructor 
// ============================================================================
GaussTrajectory::GaussTrajectory ( const GiGaTrajectory & right )
  : GiGaTrajectory(right)
{
#ifdef GIGA_DEBUG
  GaussTrajectoryLocal::s_Counter.increment();
#endif
};

// ============================================================================
/// constructor 
// ============================================================================
GaussTrajectory::GaussTrajectory ( const GaussTrajectory & right )
  : GiGaTrajectory(right)
{
#ifdef GIGA_DEBUG
  GaussTrajectoryLocal::s_Counter.increment();
#endif
};

// ============================================================================
/// clone 
// ============================================================================
GiGaTrajectory* GaussTrajectory::clone() const 
{ return new GaussTrajectory( *this ); }


// ============================================================================
/// destructor 
// ============================================================================
GaussTrajectory::~GaussTrajectory()
{
#ifdef GIGA_DEBUG
  GaussTrajectoryLocal::s_Counter.decrement();
#endif
};

// ============================================================================
/// new 
// ============================================================================
void* GaussTrajectory::operator new(size_t)
{ return (void*) GaussTrajectoryLocal::s_Allocator.MallocSingle() ; } ;

// ============================================================================
/// delete 
// ============================================================================
void  GaussTrajectory::operator delete ( void* aTrajectory )
{ GaussTrajectoryLocal::
  s_Allocator.FreeSingle( (GaussTrajectory*) aTrajectory ) ; } ;

///
void GaussTrajectory::AppendStep      ( const G4Step*  step )       
{
  ///
  bool append = false;
  G4VUserTrackInformation* uinf = step->GetTrack()->GetUserInformation(); 
  GaussTrackInformation*    ginf = 
    ( 0 == uinf )  ? 0 : dynamic_cast<GaussTrackInformation*> ( uinf );
  /// 

  if     ( empty()                                       )  
    { append = true ; } 
  /// if some information is not available, 
  /// follow ordinary routine and just add the step
  //  else if( 0 == step->GetTrack()                      || 
	//   0 == stepMgr()                             || 
	//   step != stepMgr()->GetStep()               || 
	//   step->GetTrack() != stepMgr()->GetTrack    () ) 
  //  { append = true ; }   

  /// if  it is the last step, the step must be appended 
  else if ( fAlive != step->GetTrack()->GetTrackStatus() ) 
    { append = true ; }
  /// if appendStep (in GaussTrackInformation) was explicitly set:
  else if (ginf->appendStep()) 
    { 
      append=true; 
      ginf->setAppendStep(false);
    }
  
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











