// $Id: GaussTrackActionByEnergyType.cpp,v 1.3 2004-04-20 04:27:15 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2004/04/05 13:18:35  gcorti
// do not save photoelectrons by default
//
// Revision 1.1  2004/02/20 19:35:26  ibelyaev
//  major update
// 
// ============================================================================
// Include files 
// ============================================================================
// CLHEP
// ============================================================================
#include "CLHEP/Units/SystemOfUnits.h"
// ============================================================================
// Geant4
// ============================================================================
#include "G4TrackingManager.hh"
#include "G4VProcess.hh"
#include "G4ProcessType.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
// ============================================================================
// GiGa
// ============================================================================
#include "GiGa/GiGaMACROs.h"
// ============================================================================
// GaussTools 
// ============================================================================
#include "GaussTools/GaussTrajectory.h"
#include "GaussTools/GaussTrackInformation.h"
// ============================================================================
// local
// ============================================================================
#include "GaussTrackActionByEnergyType.h"
// ============================================================================

/** @file 
 *  Implementation file for class GaussTrackActionByEnergyType
 *
 *  @date 2004-02-19 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */

// ============================================================================
IMPLEMENT_GiGaFactory( GaussTrackActionByEnergyType );
// ============================================================================

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
GaussTrackActionByEnergyType::GaussTrackActionByEnergyType
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GaussTrackActionZ   ( type , name , parent ) 
  //
  , m_ownNames   () 
  , m_ownTypes   () 
  , m_threshold  ( 1 * GeV )
  , m_rejectRICHphe ( true )
{
  declareProperty ( "OwnTypes"    , m_ownNames  ) ;
  declareProperty ( "Threshold"   , m_threshold ) ;
  declareProperty ( "RejectRICHPhotoelectrons", m_rejectRICHphe );
};
// ============================================================================

// ============================================================================
/// Destructor
// ============================================================================
GaussTrackActionByEnergyType::~GaussTrackActionByEnergyType() {}
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
StatusCode GaussTrackActionByEnergyType::initialize () 
{
  StatusCode sc = GaussTrackActionZ::initialize() ;
  if( sc.isFailure() ) { return sc ; }
  
  m_ownTypes   .clear() ;
  
  if ( m_ownNames.empty() ) 
  {
    Warning (" Empty type lists. no action");
    return StatusCode::SUCCESS ;
  }
  
  G4ParticleTable* table = G4ParticleTable::GetParticleTable();
  if ( 0 == table ) 
  { return Error ( "G4ParticleTable* points to NULL!" ) ; }
  
  // own types 
  for( Names::const_iterator it = m_ownNames.begin() ; 
       m_ownNames.end() != it ; ++it ) 
  {
    const G4ParticleDefinition* def = table->FindParticle( *it ) ; 
    if ( 0 == def )
    { return Error ( "G4ParticleDefintion* is NULL for '" + (*it) + "'" ) ; }
    m_ownTypes.push_back( def ) ; 
  }
  
  std::sort ( m_ownTypes   .begin () , m_ownTypes   .end () );
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
/** perform action 
 *  @see G4VUserTrackingAction
 *  @param pointer to new track opbject 
 */
// ============================================================================
void GaussTrackActionByEnergyType::PreUserTrackingAction  
( const G4Track* track ) 
{
  // no action 
  if ( m_ownTypes.empty()                               ) { return ; } // RETURN
  
  if ( 0 == track || track->GetKineticEnergy() < threshold() ) { return ; }

  if ( 0 == trackMgr() ) 
  { Error ( "Pre..: G4TrackingManager* points to NULL!" ) ; return ; } // RETURN
  
  if      ( track -> GetVertexPosition().z() < zMin()   ) { return ; } // RETURN
  else if ( track -> GetVertexPosition().z() > zMax()   ) { return ; } // RETURN
  
  if( m_rejectRICHphe ) {
    const G4VProcess* process  = track->GetCreatorProcess() ;
    if ( 0 == process ) 
    { Error ( "Pre..: G4VProcess         points to NULL!" ) ; return ; } 
    
    if ( "RichHpdPhotoelectricProcess" == process->GetProcessName() ) {
      Warning ( "Pre..: RichHpdPhotoelectricProcess particles not kept" );
      return;
    } 
  }
  
  // get the trajectory 
  GaussTrajectory* tr = trajectory() ;
  
  // check the validity 
  if( 0 == tr ) 
  { Error ( "Pre...: GaussTrajectory*       points to NULL" ) ; return ; }
  
  // check the track information
  GaussTrackInformation*   info = trackInfo() ;
  if( 0 == info ) 
  { Error ( "Pre...: GaussTrackInformation* points to NULL" ) ; return ; }
  
  if ( info -> toBeStored() ) { return ; }
   
  if ( storeOwn( track ) ) { mark( info ) ; } 

  
};
// ============================================================================

// ============================================================================
/** perform action 
 *  @see G4VUserTrackingAction
 *  @param pointer to new track opbject 
 */
// ============================================================================
void GaussTrackActionByEnergyType::PostUserTrackingAction  
( const G4Track* /* track */ ) {}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
