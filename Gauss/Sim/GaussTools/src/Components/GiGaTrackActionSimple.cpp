// $Id: GiGaTrackActionSimple.cpp,v 1.2 2003-02-14 18:14:49 witoldp Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/12/12 15:19:33  witoldp
// major repackaging
//
// Revision 1.5  2002/12/07 21:19:14  ibelyaev
//  few optimization updates
//
// Revision 1.4  2002/12/07 14:41:44  ibelyaev
//  add new Calo stuff
//
// ============================================================================
#define GIGA_GIGATRACKACKTIONSIMPLE_CPP 1 
// ============================================================================
/// CLHEP
#include "CLHEP/Geometry/Point3D.h"
/// Geant4 
#include "G4Track.hh"
#include "G4TrackVector.hh"
#include "G4TrackingManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"
/// GiGa 
#include "GiGa/GiGaMACROs.h"
// GaussTools
#include "GaussTools/GaussTrajectory.h"
#include "GaussTools/GaussTrackInformation.h"
/// local
#include "GiGaTrackActionSimple.h"
///

/** @file 
 *  
 *  Implementation of class GiGaTrackActionSimple
 *
 *  @author Vanya Belyaev
 *  @author Witek Pokorski
 */

// ============================================================================
/// factory business 
// ============================================================================
IMPLEMENT_GiGaFactory( GiGaTrackActionSimple );
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
GiGaTrackActionSimple::GiGaTrackActionSimple
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaTrackActionBase( type , name , parent ) 
  ///  all tracks    are stored    
  , m_storeAll              ( false  ) 
  ///  all primaries are stored
  , m_storePrimaries        ( true   ) 
  ///  all tracks that created a hit are stored
  , m_storeHitTracks        ( false  )
  ///  all tracks  with kinetic energy over threshold are stored  
  , m_storeByOwnEnergy      ( false  )  
  ///  all tracks  with given type are stored 
  , m_storeByOwnType        ( false  ) 
  ///  all tracks which has a daughter with kinetic 
  ///  energy over threshold are stored 
  , m_storeByChildEnergy    ( false  )  
  ///  all tracks which has a daughter of given type are stored 
  , m_storeByChildType      ( false  ) 
  ///  all tracks which are explicitely marked to be stored are stored 
  ///  this is for ex used for tracks that generated hits in tracking devices
  , m_storeMarkedTracks     ( true   )
  /// threshold for own kinetic energy 
  , m_ownEnergyThreshold    ( 10*TeV ) 
  /// threshold for child kinetic energy 
  , m_childEnergyThreshold  ( 10*TeV ) 
  /// container of names of own   types 
  , m_ownStoredTypesNames   (        )   
  /// container of names of child types 
  , m_childStoredTypesNames (        ) 
  /// container of definitions of own   types 
  , m_ownStoredTypes        (        ) 
  /// container of definitions  of child types 
  , m_childStoredTypes      (        )  
  ///
{
  // declare own properties
  declareProperty( "StoreAll"              , m_storeAll              ) ; 
  declareProperty( "StorePrimaries"        , m_storePrimaries        ) ;
  //  declareProperty( "StoreHitTracks"        , m_storeHitTracks        ) ;
  declareProperty( "StoreByOwnEnergy"      , m_storeByOwnEnergy      ) ; 
  declareProperty( "StoreByOwnType"        , m_storeByOwnType        ) ; 
  declareProperty( "StoreByChildEnergy"    , m_storeByChildEnergy    ) ; 
  declareProperty( "StoreByChildType"      , m_storeByChildType      ) ; 
  declareProperty( "StoreMarkedTracks"     , m_storeMarkedTracks     ) ; 
  declareProperty( "OwnEnergyThreshold"    , m_ownEnergyThreshold    ) ; 
  declareProperty( "ChildEnergyThreshold"  , m_childEnergyThreshold  ) ; 
  declareProperty( "StoredOwnTypes"        , m_ownStoredTypesNames   ) ;
  declareProperty( "StoredChildTypes"      , m_childStoredTypesNames ) ;
};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GiGaTrackActionSimple::~GiGaTrackActionSimple()
{
  m_ownStoredTypes.clear() ; 
  m_ownStoredTypesNames.clear() ; 
  m_childStoredTypes.clear() ; 
  m_childStoredTypesNames.clear() ; 

};
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
StatusCode GiGaTrackActionSimple::initialize () 
{
  // initialize the base 
  StatusCode status = GiGaTrackActionBase::initialize() ; 
  if( status.isFailure() ) 
    { return Error("Could not intialize base class GiGaTrackActionBase!", 
                   status ) ; } 
  //
  if( m_storeByOwnType )
    {
      m_ownStoredTypes.clear();
      G4ParticleTable* table = G4ParticleTable::GetParticleTable();
      if( 0 == table ) 
        { return Error("G4ParticleTable* points to NULL!" ) ; } 
      for( TypeNames::const_iterator it = m_ownStoredTypesNames.begin() ; 
           m_ownStoredTypesNames.end() != it ; ++it )
        {
          const G4ParticleDefinition* pd = table->FindParticle( *it ) ; 
          if( 0 == pd )
            { return Error(std::string("could not find G4ParticleDefinition")+ 
                           "for particle name='"+*it+"'!"); }
          m_ownStoredTypes.push_back( pd ) ;   
        }
      //
      if( ownStoredTypes().empty() ) 
        { 
          Warning(std::string("OwnTypesContainer is empty! Deactivate the ") 
                  + "'StoreByOwnType' option!"); 
          m_storeByOwnType = false ;  ///< NB !!!
        } 
    }
  //
  if( m_storeByChildType )
    {
      m_childStoredTypes.clear();
      G4ParticleTable* table = G4ParticleTable::GetParticleTable();
      if( 0 == table ) 
        { return Error("G4ParticleTable* points to NULL!" ) ; } 
      for( TypeNames::const_iterator it = m_childStoredTypesNames.begin() ; 
           m_childStoredTypesNames.end() != it ; ++it )
        {
          const G4ParticleDefinition* pd = table->FindParticle( *it ) ; 
          if( 0 == pd ) 
            { return Error(std::string("could not find G4ParticleDefinition ") 
                           + "for particle name='"+*it+"'!"); }
          m_childStoredTypes.push_back( pd ) ;   
        }
      //
      if( childStoredTypes().empty() ) 
        { 
          Warning(std::string("ChildTypesContainer") + 
                  " is empty! Deactivate the 'StoreByChildType' option!"); 
          m_storeByChildType = false ;   ///< NB !!!
        } 
    }
  ///
  return Print("Initialized successfully" , 
               StatusCode::SUCCESS        , MSG::VERBOSE);
} ;
// ============================================================================

// ============================================================================
/** finalize the action object 
 *  @see GiGaTrackActionBase 
 *  @see GiGaBase 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @return status code
 */ 
// ============================================================================
StatusCode GiGaTrackActionSimple::finalize   () 
{ 
  m_ownStoredTypes.clear() ; 
  m_ownStoredTypesNames.clear() ; 
  m_childStoredTypes.clear() ; 
  m_childStoredTypesNames.clear() ; 
  // finilize the base 
  return GiGaTrackActionBase::finalize() ;
};
// ============================================================================

// ============================================================================
/**  perform action 
 *   @param pointer to new track opbject 
 */
// ============================================================================
void GiGaTrackActionSimple::PreUserTrackingAction  (const G4Track* track ) 
{
  // Is the track valid? Is tracking manager valid? 
  // Does trajectory already exist?
  if( 0 == track || 0 == trackMgr() || 0 != trackMgr()->GimmeTrajectory()  ) 
    { return ; } 
  { 
    // attach GaussTrackInformation to the track 
    GaussTrackInformation* ti = new GaussTrackInformation(); 
    //
    if( storeByOwnEnergy() 
        && ( track->GetKineticEnergy() > ownEnergyThreshold() ) ) 
      {  ti->setToBeStored( true ); } 
    //
    trackMgr()->SetUserTrackInformation( ti );
  }
  //
  trackMgr()->SetStoreTrajectory( true ) ;  
  //
  {
    // create GaussTrajectory and inform Tracking Manager 
    GaussTrajectory* traj = new GaussTrajectory( track ) ; 
    // traj->setStepMgr( trackMgr()->GetSteppingManager() ) ; 
    trackMgr()->SetTrajectory( traj ) ;
  }
  //
};

// ============================================================================
/**  perform action 
 *  @see G4UserTrackingAction
 *   @param pointer to  track opbject 
 */
// ============================================================================
void GiGaTrackActionSimple::PostUserTrackingAction ( const G4Track* track ) 
{
  // Is the track valid? Is tracking manager valid?
  if( 0 == track || 0 == trackMgr()           )  { return ; } /// RETURN !!!
  // store trajectory?
  // (1) nothing to  be stored
  if ( 0 == trackMgr()->GimmeTrajectory    () )  { return ; } /// RETURN !!!
  // (2) already marked to be stored  
  // if (      trackMgr()->GetStoreTrajectory () )  { return ; } /// RETURN  
  //
  trackMgr()->SetStoreTrajectory( false );
  // (3) store  all     particles ? 
  if ( m_storeAll )                                        
    { trackMgr()->SetStoreTrajectory( true ) ;     return ; } /// RETURN !!!  
  // (4) store  primary particles ? 
  if ( m_storePrimaries &&  0 == track->GetParentID() )     
    { trackMgr()->SetStoreTrajectory( true ) ;     return ; } /// RETURN !!!  
  // store particles that created hits ?
  //  if( m_storeHitTracks ) 
  // { trackMgr()->SetStoreTrajectory( true ) ;     return ; } /// RETURN !!!  
  // (5) store particles with kinetic energy over the threshold value. 
  //     See also PreAction
  if( m_storeByOwnEnergy 
      && ( track->GetKineticEnergy() > 
           m_ownEnergyThreshold      ) ) 
    { trackMgr()->SetStoreTrajectory( true ) ;     return ; } /// RETURN !!! 
  // (6) store all predefined particle types: 
  if ( m_storeByOwnType  
       && ( std::find( m_ownStoredTypes.begin() ,  
                       m_ownStoredTypes.end  () , 
                       track->GetDefinition  () ) 
            != m_ownStoredTypes.end() )         )
    { trackMgr()->SetStoreTrajectory( true ) ;     return ; } /// RETURN !!!
  // (7) store the particle if it has a certain type of daughter particle 
  //     or at least one from secondaries  particle have kinetic energy over 
  //     threshold  
  if( m_storeByChildType || m_storeByChildEnergy 
      && 0 != trackMgr()->GimmeSecondaries()    )
    {
      const G4TrackVector* childrens = trackMgr()->GimmeSecondaries() ; 
      for( unsigned int index = 0 ; index < childrens->size() ; ++index )
        {
          const G4Track* tr = (*childrens)[index];
          if( 0 == tr ) { continue; }
          // 
          if( m_storeByChildEnergy 
              && ( tr->GetKineticEnergy() > 
                   m_childEnergyThreshold ) ) 
            { trackMgr()->SetStoreTrajectory( true ) ;   return ; } /// RETURN 
          //
          if( m_storeByChildType
              && ( std::find( m_childStoredTypes.begin() ,  
                              m_childStoredTypes.end  () , 
                              tr->GetDefinition       () ) 
                   != m_childStoredTypes.end() )         ) 
            { trackMgr()->SetStoreTrajectory( true ) ;   return ; } /// RETURN
          ///
        }
    }
  // (8) store  tracks, marked through GaussTrackInformation class
  if( m_storeMarkedTracks ) 
    {
      G4VUserTrackInformation* ui = track->GetUserInformation(); 
      GaussTrackInformation*    gi = 
        ( 0 == ui )  ? 0 : static_cast<GaussTrackInformation*> ( ui );
      if( 0 != gi && gi->toBeStored() ) 
        { trackMgr()->SetStoreTrajectory   ( true )  ;   return ; }  /// RETURN 
    }  
  // other storing criteria:
  // add your own storing criteria AFTER this line and 
  // ........
  // add your own storing criteria BEFORE This line
  
  // check if track is to be stored 
  if (      trackMgr()->GetStoreTrajectory () )         { return ; } /// RETURN 
  
  // if track is not to be stored, propagate it's parent ID (stored) to its
  // secondaries
  if( 0 != trackMgr()->GimmeSecondaries() ) 
    {
      if( 0 == track->GetParentID() ) 
        { Error("Dangerouse:Primary Particle is not requested to be stored");}
      
      G4TrackVector* childrens = trackMgr()->GimmeSecondaries() ;
      for( unsigned int index = 0 ; index < childrens->size() ; ++index )
        {
          G4Track* tr = (*childrens)[index] ;
          if( 0 == tr ) { continue ; } 
          //
          if     ( tr->GetParentID() != track->GetTrackID() ) 
            { Error("Could not reconstruct properly the parent!") ; } 
          //
          tr->SetParentID( track->GetParentID() );
          //
        }      
    }
  // also update the trackID in the hits
  G4VUserTrackInformation* uinf = track->GetUserInformation(); 
  GaussTrackInformation*    ginf = 
    ( 0 == uinf )  ? 0 : static_cast<GaussTrackInformation*> ( uinf );
  ginf->updateHitsTrackID( track->GetParentID() );
  
  // delete the trajectory by hand 
  if ( !trackMgr()->GetStoreTrajectory() )
    {
      G4VTrajectory* traj = trackMgr()->GimmeTrajectory();    
      if( 0 != traj ) { delete traj ; } 
      trackMgr()->SetTrajectory( 0 ) ;
    }
}; 
// ============================================================================

// ============================================================================
// The END 
// ============================================================================











