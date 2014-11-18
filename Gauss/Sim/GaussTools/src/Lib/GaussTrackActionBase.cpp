// $Id: GaussTrackActionBase.cpp,v 1.3 2004-04-20 04:27:15 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2004/02/22 16:51:54  ibelyaev
//  few minor fixes
//
// Revision 1.1  2004/02/20 19:35:30  ibelyaev
//  major update
// 
// ============================================================================
// Include files 
// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

// GiGa
#include "GiGa/GiGaUtil.h"

// Geant4
#include "G4TrackingManager.hh"

// GaussTools 
#include "GaussTools/GaussTrajectory.h"
#include "GaussTools/GaussTrackInformation.h"

// local
#include "GaussTools/GaussTrackActionBase.h"
#include "GaussTools/createGaussTrajectory.h"
// ============================================================================

/** @file 
 *  Implementation file for class : GaussTrackActionBase
 *
 *  @date 2004-02-19 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */

// ============================================================================
/** standard constructor 
 *  @see GiGaTrackActionBase 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GaussTrackActionBase::GaussTrackActionBase
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaTrackActionBase ( type , name , parent )
  //
  , m_nTrajectories  ( 0 )
  , m_nTrackInfos    ( 0 )
  , m_marked         ( 0 ) 
{}


// ============================================================================
/// Destructor
// ============================================================================
GaussTrackActionBase::~GaussTrackActionBase() {}
// ============================================================================

// ============================================================================
/** initialize the track action  
 *  @see GiGaTrackActionBase 
 *  @see GiGaBase 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode GaussTrackActionBase::initialize () 
{ return GiGaTrackActionBase::initialize() ; }
// ============================================================================

// ============================================================================
/** finalise the track action  
 *  @see GiGaTrackActionBase 
 *  @see GiGaBase 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode GaussTrackActionBase::finalize   () 
{
  if ( 0 != m_marked        ) 
  { 
    always () << " Number of marked  GaussTrajectories       " ;
    always () .stream() << m_marked         ;
    always () << "('" << name() << "')"     ;
    always () << endreq ;
  }
  
  if ( 0 != m_nTrajectories ) 
  { 
    always () << " Number of created GaussTrajectories       " ;
    always () .stream() << m_nTrajectories  ;
    always () << "('" << name() << "')"     ;
    always () << endreq ;
  }
  
  if ( 0 != m_nTrackInfos  ) 
  { 
    always () << " Number of created GaussTrackInformation   " ;
    always () . stream() << m_nTrackInfos  ;
    always () << "('" << name() << "')"     ;
    always () << endreq ;
  }
  
  return GiGaTrackActionBase::finalize   () ; 
}
// ============================================================================

// ============================================================================
/// get the valid Gauss Trajectory
// ============================================================================
GaussTrajectory* GaussTrackActionBase::trajectory () const 
{
  if ( 0 == trackMgr() ) 
  {
    Error ( "trajectory(): G4TrackingManager* points to NULL" ) ;
    return 0 ;                                                      // RETURN 
  }

  // get the track 
  G4Track* track = trackMgr()->GetTrack() ;
  if ( 0 == track      ) 
  {
    Error ( "trajectory(): G4Track*           points to NULL" ) ;
    return 0 ;                                                      // RETURN
  }
  
  GaussTrajectory* gau = 0 ;
  G4VTrajectory*   g4  = trackMgr()->GimmeTrajectory() ;
  if ( 0 == g4 ) 
  {
    gau = createGaussTrajectory( track ) ;
    ++m_nTrajectories ;
    trackMgr() -> SetTrajectory( gau )  ;
    return gau ;                                                  // RETURN 
  }
  
  gau = gaussTrajectory( g4 ) ;
  if ( 0 == gau ) 
  { Error ( "trajectory(): invalid cast to GaussTrajectory") ; }

  return gau ;
}


// ============================================================================
// get the valid Gauss track information 
// ============================================================================
GaussTrackInformation* GaussTrackActionBase::trackInfo     () const 
{
  if ( 0 == trackMgr() ) 
  { 
    Error( "trackInfo(): G4TrackingManager* points to NULL"); 
    return 0 ;
  };
  return trackInfo( trackMgr() -> GetTrack() ) ;
}


// ============================================================================
// get the valid Gauss tarck information 
// ============================================================================
GaussTrackInformation* GaussTrackActionBase::trackInfo    
( G4Track*  track ) const
{
  
  if ( 0 == track    ) 
  { 
    Error( "trackInfo(): G4Track*           points to NULL"); 
    return 0 ;                                                   // RETURN 
  };
  
  GaussTrackInformation*   gau  = 0 ;
  G4VUserTrackInformation* g4   =  track->GetUserInformation() ;
  if ( 0 == g4 ) 
  {
    gau = new GaussTrackInformation() ;
    ++m_nTrackInfos ;
    track->SetUserInformation( gau ) ;
    return gau ;                                                 // RETURN 
  }
  
  GiGaUtil::FastCast<G4VUserTrackInformation,GaussTrackInformation> cast;
  gau = cast( g4 ) ;
  if( 0 == gau ) 
  { Error ( "trackInfo(): invalid cast to GaussTrajckInformation") ; }

  return gau ;
}


// ============================================================================
// store children tracks S
// ============================================================================
StatusCode GaussTrackActionBase::storeChildren () const 
{
  G4TrackVector* children = trackMgr()->GimmeSecondaries();
  if( 0 == children ) { return StatusCode::FAILURE ; }              // OK 
  
  // loop over all children
  for ( unsigned int i = 0 ; i < children->size(); ++i  )
  {
    G4Track* child  = (*children)[i] ;
    if( 0 == child  ) { continue ; } 
    
    GaussTrackInformation* info = trackInfo( child ) ;
    
    mark ( info ) ;
  }
  
  return StatusCode::SUCCESS ;
}


StatusCode GaussTrackActionBase::mark
( GaussTrackInformation* info ) const
{
  if ( 0 == info ) 
  { return Error ( "mark(): GaussTrackInformation* points to NULL!" ) ; }
  else if ( info ->    toBeStored() )  { }
  else    { info -> setToBeStored( true ) ; ++m_marked ; }
  
  return StatusCode::SUCCESS ;
}

// ============================================================================
// The END 
// ============================================================================
