// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $
// ============================================================================

// ============================================================================
#define  GAUSSTOOLS_GAUSSTRAJECTORY_CPP 1 
// ============================================================================
/// from GaussTools
#include "GaussTools/GaussTrajectory.h"
#include "GaussTools/GaussTrackInformation.h"
///
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4SteppingManager.hh"


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











