// $Id: GaussTrackActionByType.cpp,v 1.4 2007-01-12 15:36:43 ranjard Exp $ 
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

// Geant4
#include "G4TrackingManager.hh"
#include "G4VProcess.hh"
#include "G4ProcessType.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

// GiGa
//#include "GiGa/GiGaMACROs.h"

// GaussTools 
#include "GaussTools/GaussTrajectory.h"
#include "GaussTools/GaussTrackInformation.h"

// local
#include "GaussTrackActionByType.h"


/** @file 
 *  Implementation file for class GaussTrackActionByType
 *
 *  @date 2004-02-19 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GaussTrackActionByType );

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
GaussTrackActionByType::GaussTrackActionByType
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GaussTrackActionZ   ( type , name , parent ) 
  //
  , m_ownNames   () 
  , m_childNames () 
  , m_ownTypes   () 
  , m_childTypes ()
  , m_rejectRICHphe ( true )
{
  m_ownNames.push_back   ( "mu-" ) ;
  m_ownNames.push_back   ( "mu+" ) ;
  m_ownNames.push_back   ( "pi0" ) ;
  
  declareProperty ( "OwnTypes"   , m_ownNames   ) ;
  
  m_childNames.push_back ( "mu-" ) ;
  m_childNames.push_back ( "mu+" ) ;
  m_childNames.push_back ( "pi0" ) ;
  
  declareProperty ( "ChildTypes" , m_childNames ) ;

  declareProperty ( "RejectRICHPhotoelectrons", m_rejectRICHphe );

};
// ============================================================================

// ============================================================================
/// Destructor
// ============================================================================
GaussTrackActionByType::~GaussTrackActionByType() {}
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
StatusCode GaussTrackActionByType::initialize () 
{
  StatusCode sc = GaussTrackActionZ::initialize() ;
  if( sc.isFailure() ) { return sc ; }
  
  m_ownTypes   .clear() ;
  m_childTypes .clear() ;
  
  if ( m_ownNames.empty() && m_childNames.empty() ) 
  {
    Warning (" Empty type lists. no action");
    return StatusCode::SUCCESS ;
  }
  
  G4ParticleTable* table = G4ParticleTable::GetParticleTable();
  if ( 0 == table ) 
  { return Error ( "G4ParticleTable* points to NULL!" ) ; }
  
  { // own types 
    for( Names::const_iterator it = m_ownNames.begin() ; 
         m_ownNames.end() != it ; ++it ) 
    {
      const G4ParticleDefinition* def = table->FindParticle( *it ) ; 
      if ( 0 == def )
      { return Error ( "G4ParticleDefintion* is NULL for '" + (*it) + "'" ) ; }
      std::cout << " own type "
                <<  (*it)
                << " def = "
                << def->GetParticleName()
                << std::endl ;
      
      m_ownTypes.push_back( def ) ; 
    }
  }
  { // own types 
    for( Names::const_iterator it = m_childNames.begin() ; 
         m_childNames.end() != it ; ++it ) 
    {
      const G4ParticleDefinition* def = table->FindParticle( *it ) ; 
      if ( 0 == def )
      { return Error ( "G4ParticleDefintion* is NULL for '" + (*it) + "'" ) ; }
      std::cout << " own type "
                <<  (*it)
                << " def = "
                << def->GetParticleName()
                << std::endl ;
      m_childTypes.push_back( def ) ; 
    }
  }
  
  std::sort ( m_ownTypes   .begin () , m_ownTypes   .end () );
  std::sort ( m_childTypes .begin () , m_childTypes .end () );

  std::cout << " size of containers : "
            << m_ownTypes.size() 
            <<" " << m_childTypes.size() << std::endl ;
  

  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
/** perform action 
 *  @see G4VUserTrackingAction
 *  @param pointer to new track opbject 
 */
// ============================================================================
void GaussTrackActionByType::PreUserTrackingAction  ( const G4Track* track ) 
{
  // no action 
  if ( ownTypes().empty()                               ) { return ; } // RETURN
  
  if ( 0 == trackMgr() ) 
  { Error ( "Pre..: G4TrackingManager* points to NULL!" ) ; return ; } // RETURN
  
  if ( 0 == track )
  { Error ( "Pre..: G4Track*           points to NULL!" ) ; return ; } // RETURN
  
  if      ( track -> GetVertexPosition().z() < zMin()   ) { return ; } // RETURN
  else if ( track -> GetVertexPosition().z() > zMax()   ) { return ; } // RETURN
  
  if( m_rejectRICHphe ) {
    const G4VProcess* process  = track->GetCreatorProcess() ;
    if ( 0 == process ) 
    { Error ( "Pre..: G4VProcess         points to NULL!" ) ; } 
    else if ( ("RichHpdPhotoelectricProcess" == process->GetProcessName()) ||
             ("RichPmtPhotoelectricProcess" == process->GetProcessName() ) ||
              ( "TorchTBMcpPhotoElectricProcess" == process->GetProcessName() )   ) {
      Warning ( "Pre..: RichHpdPhotoelectricProcess RichPmtPhotoelectricProcess particles not kept" );
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
void GaussTrackActionByType::PostUserTrackingAction  
( const G4Track* track ) 
{
  // no action 
  if ( childTypes().empty()                             ) { return ; } // RETURN

  if ( 0 == trackMgr() ) 
  { Error ( "Pre..: G4TrackingManager* points to NULL!" ) ; return ; } // RETURN
  
  if ( 0 == trackMgr() ->GimmeSecondaries ()            ) { return ; } // RETURN
  
  if ( 0 == track ) 
  { Error ( "Pre..: G4Track*           points to NULL!" ) ; return ; } // RETURN
  
  if      ( track -> GetVertexPosition().z() < zMin()   ) { return ; } // RETURN
  else if ( track -> GetVertexPosition().z() > zMax()   ) { return ; } // RETURN
  
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
   
  bool store = false ;
  
  const G4TrackVector* children = trackMgr() -> GimmeSecondaries() ;
  for( unsigned int i  = 0 ; i  < children->size() && !store ; ++i )
  { store = storeChild( (*children)[i] ) ; }
  
  // update track info 
  if ( store ) { mark( info ) ; }
  
};
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
