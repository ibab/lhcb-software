/// CLHEP
#include "CLHEP/Geometry/Point3D.h"
/// G4Wrapper
#include "G4Wrapper/Particle.h"
/// Geant4 
#include "G4Track.hh"
#include "G4TrackVector.hh"
#include "G4TrackingManager.hh"
#include "G4ParticleDefinition.hh"
/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"
/// GiGa 
#include "GiGa/GiGaTrackActionFactory.h"
#include "GiGa/GiGaTrajectory.h"
#include "GiGa/GiGaTrackInformation.h"
/// local
#include "GiGaTrackActionSimple.h"
///

///////////////////////////////////////////////////////////////////////////////////////////////////
static const GiGaTrackActionFactory<GiGaTrackActionSimple>          s_GiGaTrackActionSimpleFactory;
const       IGiGaTrackActionFactory& GiGaTrackActionSimpleFactory = s_GiGaTrackActionSimpleFactory;
///////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////
GiGaTrackActionSimple::GiGaTrackActionSimple( const std::string& Name , ISvcLocator* Loc ) 
  : GiGaTrackActionBase( Name , Loc ) 
  ///  all tracks    are stored    
  , m_storeAll              ( false  ) 
  ///  all primaries are stored
  , m_storePrimaries        ( true   ) 
  ///  all tracks  with kinetic energy over threshold are stored  
  , m_storeByOwnEnergy      ( false  )  
  ///  all tracks  with given type are stored 
  , m_storeByOwnType        ( false  ) 
  ///  all tracks which has a daughter with kinetic energy over threshold are stored 
  , m_storeByChildEnergy    ( false  )  
  ///  all tracks which has a daughter of given type are stored 
  , m_storeByChildType      ( false  ) 
  ///  all tracks which are explicitely marked to be stored are stored 
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
  /// declare own properties
  declareProperty( "StoreAll"              , m_storeAll              ) ; 
  declareProperty( "StorePrimaries"        , m_storePrimaries        ) ; 
  declareProperty( "StoreByOwnEnergy"      , m_storeByOwnEnergy      ) ; 
  declareProperty( "StoreByOwnType"        , m_storeByOwnType        ) ; 
  declareProperty( "StoreByChildEnergy"    , m_storeByChildEnergy    ) ; 
  declareProperty( "StoreByChildType"      , m_storeByChildType      ) ; 
  declareProperty( "StoreMarkedTracks"     , m_storeMarkedTracks     ) ; 
  declareProperty( "OwnEnergyThreshold"    , m_ownEnergyThreshold    ) ; 
  declareProperty( "ChildEnergyThreshold"  , m_childEnergyThreshold  ) ; 
  declareProperty( "StoredOwnTypes"        , m_ownStoredTypesNames   ) ;
  declareProperty( "StoredChildTypes"      , m_childStoredTypesNames ) ;
  ///
};
/////////////////////////////////////////////////////////////////////////////////////////////////
GiGaTrackActionSimple::~GiGaTrackActionSimple()
{
  m_ownStoredTypes.clear() ; 
  m_ownStoredTypesNames.clear() ; 
  m_childStoredTypes.clear() ; 
  m_childStoredTypesNames.clear() ; 
};
/////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaTrackActionSimple::initialize () 
{ 
  ///
  StatusCode status = GiGaTrackActionBase::initialize() ; 
  if( status.isFailure() ) 
    { return Error("Could not intialize base class GiGaTrackActionBase!", status ) ; } 
  ///
  if( storeByOwnType() )
    {
      m_ownStoredTypes.clear();
      for( TypeNames::const_iterator it = m_ownStoredTypesNames.begin() ; m_ownStoredTypesNames.end() != it ; ++it )
	{
	  const G4ParticleDefinition* pd = G4Wrapper::getG4ParticleDefinition( *it ) ; 
	  if( 0 == pd ) { return Error("could not find G4ParticleDefinition for particle name='"+*it+"'!"); }
          m_ownStoredTypes.push_back( pd ) ;   
	}
      ///
      if( ownStoredTypes().empty() ) 
	{ 
	  Error("OwnTypesContainer is empty! Deactivate the 'StoreByOwnType' option!"); 
          m_storeByOwnType = false ;                                           /// NB !!!
	} 
    }
  ///
  if( storeByChildType() )
    {
      m_childStoredTypes.clear();
      for( TypeNames::const_iterator it = m_childStoredTypesNames.begin() ; m_childStoredTypesNames.end() != it ; ++it )
	{
	  const G4ParticleDefinition* pd = G4Wrapper::getG4ParticleDefinition( *it ) ; 
	  if( 0 == pd ) { return Error("could not find G4ParticleDefinition for particle name='"+*it+"'!"); }
          m_childStoredTypes.push_back( pd ) ;   
	}
      ///
      if( childStoredTypes().empty() ) 
	{ 
	  Error("ChildTypesContainer is empty! Deactivate the 'StoreByChildType' option!"); 
	  m_storeByChildType = false ;                                           /// NB !!!
	} 
    }
  ///
  return StatusCode::SUCCESS;
} ;
/////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaTrackActionSimple::finalize   () { return GiGaTrackActionBase::initialize() ; } ;
/////////////////////////////////////////////////////////////////////////////////////////////////
void GiGaTrackActionSimple::PreUserTrackingAction  ( const G4Track* track ) 
{
  /// Is the track valid? Is tracking manager valid? Does trajectory already exist?
  if( 0 == track || 0 == trackMgr() || 0 != trackMgr()->GimmeTrajectory()  ) { return ; } 
  ///
  { 
    /// attach GiGaTrackInformation to the track 
    GiGaTrackInformation* ti = new GiGaTrackInformation(); 
    ///
    if( storeByOwnEnergy() 
	&& ( track->GetKineticEnergy() > ownEnergyThreshold() ) ) {  ti->setToBeStored( true ); } 
    ///
    trackMgr()->SetUserTrackInformation( ti ); 
  }
  ///
  trackMgr()->SetStoreTrajectory( true ) ;  
  /// 
  {
    /// create GiGaTrajectory and inform Tracking Manager 
    GiGaTrajectory* traj = new GiGaTrajectory( track ) ; 
    traj->setStepMgr( trackMgr()->GetSteppingManager() ) ; 
    trackMgr()->SetTrajectory( traj ) ;
  }
  ///
};
/////////////////////////////////////////////////////////////////////////////////////////////////
void GiGaTrackActionSimple::PostUserTrackingAction ( const G4Track* track ) 
{
  ///
  /// Is the track valid? Is tracking manager valid?
  if( 0 == track || 0 == trackMgr()           )  { return ; } /// RETURN !!!
  ///
  /// store trajectory?
  /// (1) nothing to  be stored
  if ( 0 == trackMgr()->GimmeTrajectory    () )  { return ; } /// RETURN !!!
  ///
  /// (2) already marked to be stored  
  /// if (      trackMgr()->GetStoreTrajectory () )  { return ; } /// RETURN !!!  
  ///

  trackMgr()->SetStoreTrajectory( false );

  /// (3) store  all     particles ? 
  if ( storeAll () )                                        
    { trackMgr()->SetStoreTrajectory( true ) ;     return ; } /// RETURN !!!  
  ///

  /// (4) store  primary particles ? 
  if ( storePrimaries() &&  0 == track->GetParentID() )     
    { trackMgr()->SetStoreTrajectory( true ) ;     return ; } /// RETURN !!!  
  ///
  /// (5) store particles with kinetic energy over the threshold value. See also PreAction
  if( storeByOwnEnergy() 
      && ( track->GetKineticEnergy() > 
	   ownEnergyThreshold()      ) ) 
    { trackMgr()->SetStoreTrajectory( true ) ;     return ; } /// RETURN !!! 
  ///
  /// (6) store all predefined particle types: 
  if ( storeByOwnType()  
       && ( std::find( ownStoredTypes().begin() ,  
		       ownStoredTypes().end  () , 
		       track->GetDefinition  () ) 
	    != ownStoredTypes().end() )         )
    { trackMgr()->SetStoreTrajectory( true ) ;     return ; } /// RETURN !!!      
  ///
  /// (7) store the particle if it has a certain type of daughter particle 
  ///     or at least one from secondaries  particle have kinetic energy over 
  ///     threshold  
  if( storeByChildType() || storeByChildEnergy() 
      && 0 != trackMgr()->GimmeSecondaries()    )
    {
      const G4TrackVector* childrens = trackMgr()->GimmeSecondaries() ; 
      for( unsigned int index = 0 ; index < childrens->entries() ; ++index )
	{
	  const G4Track* tr = (*childrens)[index];
	  if( 0 == tr ) { continue; }
	  /// 
	  if( storeByChildEnergy() 
	      && ( tr->GetKineticEnergy() > 
		   childEnergyThreshold() ) ) 
	    { trackMgr()->SetStoreTrajectory( true ) ;   return ; } /// RETURN !!! 
	  ///
	  if( storeByChildType() 
	      && ( std::find( childStoredTypes().begin() ,  
			      childStoredTypes().end  () , 
			      tr->GetDefinition       () ) 
		   != childStoredTypes().end() )         ) 
	    { trackMgr()->SetStoreTrajectory( true ) ;   return ; } /// RETURN !!!     
	  ///
	}
    }
  ///
  /// (8) store  tracks, marked through GiGaTrackInformation class
  if( storeMarkedTracks() ) 
    {
      G4VUserTrackInformation* ui = track->GetUserInformation(); 
      GiGaTrackInformation*    gi = 
	( 0 == ui )  ? 0 : dynamic_cast<GiGaTrackInformation*> ( ui );
      if( 0 != gi && gi->toBeStored() ) 
	{ trackMgr()->SetStoreTrajectory   ( true )  ;   return ; }  /// RETURN !!! 
    }  
  ///
  /// other storing criteria:
  /// add your own storing criteria AFTER this line and 
  /// 

  /// 
  /// add your own storing criteria BEFORE This line
  
  ///  
  /// if track not to be stored , update history for its childrens
  if (      trackMgr()->GetStoreTrajectory () )         { return ; } /// RETURN !!! 
  /// if track is not to be stored stored, propagate it's parent (stored) to 
  if( 0 != trackMgr()->GimmeSecondaries() ) 
    {
      if( 0 == track->GetParentID() ) 
	{ Error("Dangerouse:Primary Particle is not requested to be stored") ; } 
      G4TrackVector* childrens = trackMgr()->GimmeSecondaries() ;
      for( unsigned int index = 0 ; index < childrens->entries() ; ++index )
	{
	  G4Track* tr = (*childrens)[index] ;
          if( 0 == tr ) { continue ; } 
	  ///
	  if     ( tr->GetParentID() != track->GetTrackID() ) 
	    { Error("Could not reconstruct properly the parent!") ; } 
	  ///
	  tr->SetParentID( track->GetParentID()  );
	  ///
	}
    }
  ///
  /// delete the trajectory by hand 
  if ( !trackMgr()->GetStoreTrajectory() )
    {
      G4VTrajectory* traj = trackMgr()->GimmeTrajectory();    
      if( 0 != traj ) { delete traj ; } 
      trackMgr()->SetTrajectory( 0 ) ;
    }
  ///
};
/////////////////////////////////////////////////////////////////////////////////////////////////

 













