// ============================================================================
#define GIGA_GaussPostTrackAction_CPP 1 
// ============================================================================
/// CLHEP
#include "CLHEP/Geometry/Point3D.h"
/// Geant4 
#include "G4Track.hh"
#include "G4TrackVector.hh"
#include "G4TrackingManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4PrimaryParticle.hh"
/// GaudiKernel
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"
/// GiGa 
//#include "GiGa/GiGaMACROs.h"
#include "GiGaCnv/GiGaPrimaryParticleInformation.h"
// GaussTools
#include "GaussTools/GaussTrajectory.h"
#include "GaussTools/GaussTrackInformation.h"
/// local
#include "GaussPostTrackAction.h"
///

/** @file 
 *  
 *  Implementation of class GaussPostTrackAction
 *
 *  @author Vanya Belyaev
 *  @author Witek Pokorski
 */

// ============================================================================
/// factory business 
// ============================================================================
DECLARE_TOOL_FACTORY( GaussPostTrackAction );
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
GaussPostTrackAction::GaussPostTrackAction
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaTrackActionBase( type , name , parent ) 
  ///  all tracks    are stored    
    , m_storeAll              ( false  ) 
  ///  all primaries are stored
    , m_storePrimaries        ( true   ) 
  /// all forced decay products are stored
    , m_storeDecayProducts    ( true   )
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
    , m_storeBySecondariesProcess ( false )
    , m_childStoredProcess()
 ///
    , m_storeByOwnProcess ( false )
    , m_ownStoredProcess()
    , m_storeUpToZmax( true )
    , m_zMaxToStore(12280.0 * mm)
    , m_rejectRICHphe ( true )
    , m_rejectOptPhot ( true )
{
  // declare own properties
  declareProperty( "StoreAll"              , m_storeAll              ) ; 
  declareProperty( "StorePrimaries"        , m_storePrimaries        ) ;
  declareProperty( "StoreForcedDecays"     , m_storeDecayProducts    ) ;
  declareProperty( "StoreByOwnEnergy"      , m_storeByOwnEnergy      ) ;
  declareProperty( "OwnEnergyThreshold"    , m_ownEnergyThreshold    ) ;
  declareProperty( "StoreByOwnType"        , m_storeByOwnType        ) ; 
  declareProperty( "StoreByChildEnergy"    , m_storeByChildEnergy    ) ; 
  declareProperty( "StoreByChildType"      , m_storeByChildType      ) ; 
  declareProperty( "StoreMarkedTracks"     , m_storeMarkedTracks     ) ; 
  declareProperty( "ChildEnergyThreshold"  , m_childEnergyThreshold  ) ; 
  declareProperty( "StoredOwnTypes"        , m_ownStoredTypesNames   ) ;
  declareProperty( "StoredChildTypes"      , m_childStoredTypesNames ) ;
  declareProperty( "StoreByChildProcess"   , m_storeBySecondariesProcess);
  declareProperty( "StoredChildProcesses"  , m_childStoredProcess);
  declareProperty( "StoreByOwnProcess"     , m_storeByOwnProcess);
  declareProperty( "StoredOwnProcesses"    , m_ownStoredProcess);
  declareProperty( "StoreUpToZ"            , m_storeUpToZmax);
  declareProperty( "ZmaxForStoring"        , m_zMaxToStore);
  declareProperty( "RejectRICHPhotoelectrons", m_rejectRICHphe );
  declareProperty( "RejectOpticalPhotons"   , m_rejectOptPhot );

};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GaussPostTrackAction::~GaussPostTrackAction()
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
StatusCode GaussPostTrackAction::initialize () 
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
StatusCode GaussPostTrackAction::finalize   () 
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
 *  @see G4UserTrackingAction
 *   @param pointer to  track opbject 
 */
// ============================================================================
void GaussPostTrackAction::PreUserTrackingAction ( const G4Track* track )
{
  // Is the track valid? Is tracking manager valid? 
  if( 0 == track || 0 == trackMgr() ) 
    { return ; }  

  // check if GaussTrackInformation already exists and if not
  // attach it to the track
  G4VUserTrackInformation* uinf = track->GetUserInformation();
  GaussTrackInformation* ti;
  
  if(uinf)
    {
      ti = (GaussTrackInformation*) uinf;
    }
  else
    {
      ti = new GaussTrackInformation(); 
      trackMgr()->SetUserTrackInformation(ti);
    }
  //  
  if( storeByOwnEnergy() 
      && ( track->GetKineticEnergy() > ownEnergyThreshold() ) ) {
    if( m_storeUpToZmax 
        && (track->GetVertexPosition().z() > m_zMaxToStore) ) {
      return;
    }    
    ti->setToBeStored( true ); 
  }
  //
};

// ============================================================================
/**  perform action 
 *  @see G4UserTrackingAction
 *   @param pointer to  track object 
 */
// ============================================================================
void GaussPostTrackAction::PostUserTrackingAction ( const G4Track* track ) 
{
  // Is the track valid? Is tracking manager valid?
  if( 0 == track || 0 == trackMgr()           )  { return ; } /// RETURN !!!
  // store trajectory?
  // (1) nothing to  be stored
  if ( 0 == trackMgr()->GimmeTrajectory    () )  { return ; } /// RETURN !!!
  // (2) already marked to be stored  
  // if (      trackMgr()->GetStoreTrajectory () )  { return ; } /// RETURN  
  //
  
  G4VUserTrackInformation* ui = track->GetUserInformation(); 
  GaussTrackInformation* gi = (GaussTrackInformation*) ui;

  trackMgr()->SetStoreTrajectory( false );
  // if only to a certain z, check z and set flag
  bool zstore = true;
  if( m_storeUpToZmax && (track->GetVertexPosition().z() > m_zMaxToStore) ) { 
    zstore = false; 
  }
  
  if( m_rejectRICHphe ) {
    const G4VProcess* process  = track->GetCreatorProcess() ;
    if ( 0 != process ) {
      if ( ("RichHpdPhotoelectricProcess" ==   process->GetProcessName()) ||
         ( "RichHpdSiEnergyLossProcess"  ==   process->GetProcessName()) ||
         ( "RichPmtPhotoelectricProcess" ==   process->GetProcessName()) ||
           ( "RichPmtSiEnergyLossProcess"  ==   process->GetProcessName()) ||
           ( "TorchTBMcpPhotoElectricProcess"  ==   process->GetProcessName()) ||
           ( "TorchTBMcpEnergyLossProcess"  ==   process->GetProcessName())  ) {
        Warning ( 
        "RichHpd/Pmt/Mcp PhotoelectricProcess  RichHpd/Pmt/Mcp SiEnergyLossProcess particles not kept",
                  StatusCode::SUCCESS, 0 );
        return;
      }
    }
  }
  
  if( m_rejectOptPhot ) {
    if ( track->GetDefinition()->GetParticleName() == "opticalphoton") {
//       Warning ( "OpticalPhotons not kept", StatusCode::SUCCESS, 0 );
      return;
    }
  }    

  // if reject rich photoelectrons check and store
  // (3) store  all     particles ? 
  if ( m_storeAll && zstore ) {      
    trackMgr()->SetStoreTrajectory( true );
    gi->setStoreHepMC(true); // flag for storing in HepMC (Witek)
    setProcess( track ) ;
    fillGaussTrackInformation( track ) ;
    return ; 
  } /// RETURN !!!  

  // (3.5) store forced-decay products 
  if (m_storeDecayProducts && 
      track->GetDynamicParticle()->GetPreAssignedDecayProducts()) {
    if ( 0 != trackMgr()->GimmeSecondaries() ) {
      G4TrackVector* childrens = trackMgr()->GimmeSecondaries();

      for (unsigned int index = 0; index < childrens->size(); ++index) {
        G4Track* dtr = (*childrens)[index] ;
	if( 0 == dtr ) { continue ; } 
	if( !(dtr->GetDynamicParticle()->GetPreAssignedDecayProducts())) {
	  //
	  // check if GaussTrackInformation already exists and if not
	  // attach it to the track
	  G4VUserTrackInformation* duinf = dtr->GetUserInformation();
	  GaussTrackInformation* dti;

	  if(duinf) {
	    dti = (GaussTrackInformation*) duinf;
	  } else {
	    dti = new GaussTrackInformation(); 
	    dtr->SetUserInformation(dti);
	  }   
	  dti->setToBeStored(true);
	}
      }
      trackMgr()->SetStoreTrajectory( true );
      gi->setStoreHepMC(true); // flag for storing in HepMC (Witek)

      setProcess( track ) ;
      fillGaussTrackInformation( track ) ;
      return ; 
    }
  }
  
  // (4) store  primary particles ? 
  if ( m_storePrimaries &&  0 == track->GetParentID() ) { 
    setProcess( track ) ;
    trackMgr()->SetStoreTrajectory( true );
    gi->setStoreHepMC(true); // flag for storing in HepMC (Witek)
      
    fillGaussTrackInformation( track ) ;
    return; 
  } /// RETURN !!!  
  
  // (5) store particles with kinetic energy over the threshold value. 
  //     See also PreAction
  if( m_storeByOwnEnergy 
      && ( track->GetKineticEnergy() > m_ownEnergyThreshold      ) 
      && zstore ) { 
    setProcess( track ) ;
    fillGaussTrackInformation( track ) ;      
    trackMgr()->SetStoreTrajectory( true ) ;     
    gi->setStoreHepMC(true); // flag for storing in HepMC (Witek)

    return ; 
  } /// RETURN !!! 
  
  // (6) store all predefined particle types: 
  if ( m_storeByOwnType  
       && ( std::find( m_ownStoredTypes.begin() ,  
                       m_ownStoredTypes.end  () , 
                       track->GetDefinition  () ) 
            != m_ownStoredTypes.end() )
       && zstore ) { 
    setProcess( track ) ;
    fillGaussTrackInformation( track ) ;
    trackMgr()->SetStoreTrajectory( true );     
    gi->setStoreHepMC(true); // flag for storing in HepMC (Witek)
    return; 
  } /// RETURN !!!
  
  // (7) store the particle if it has a certain type of daughter particle 
  //     or at least one from secondaries  particle have kinetic energy over 
  //     threshold  
  if( m_storeByChildType || (m_storeByChildEnergy 
                             && 0 != trackMgr()->GimmeSecondaries()    
                             && zstore )) {
    const G4TrackVector* childrens = trackMgr()->GimmeSecondaries() ; 
    for( unsigned int index = 0 ; index < childrens->size() ; ++index ) {
      const G4Track* tr = (*childrens)[index];
      if( 0 == tr ) { continue; }
      // 
      if( m_storeByChildEnergy 
	  && ( tr->GetKineticEnergy() > 
		m_childEnergyThreshold ) ) { 
	setProcess( track ) ;
	fillGaussTrackInformation( track ) ;      
	trackMgr()->SetStoreTrajectory( true );   
	gi->setStoreHepMC(true); // flag for storing in HepMC (Witek)
	return; 
      } /// RETURN 
          //
      if( m_storeByChildType
	  && ( std::find( m_childStoredTypes.begin() ,  
			  m_childStoredTypes.end  () , 
			  tr->GetDefinition       () ) 
		!= m_childStoredTypes.end() )         
	  && zstore ) { 
	setProcess( track ) ;
        fillGaussTrackInformation( track ) ;      
	trackMgr()->SetStoreTrajectory( true );   
	gi->setStoreHepMC(true); // flag for storing in HepMC (Witek)
	return; 
      } /// RETURN
	    ///
    }
  }

  // (7.5) store tracks according to creator process of its daughters
  if( m_storeBySecondariesProcess && 0 != trackMgr()->GimmeSecondaries()
      && zstore ) {
    const G4TrackVector* childrens = trackMgr()->GimmeSecondaries() ; 
    for( unsigned int index = 0 ; index < childrens->size() ; ++index ) {
      const G4Track* tr = (*childrens)[index];
      if( 0 == tr ) { continue; }
      // 
      if(std::find(m_childStoredProcess.begin(), m_childStoredProcess.end(), 
		   tr->GetCreatorProcess()->GetProcessName()) 
		  != m_childStoredProcess.end()) { 
	setProcess( track ) ;
        fillGaussTrackInformation( track ) ;      
	trackMgr()->SetStoreTrajectory( true );   
	gi->setStoreHepMC(true); // flag for storing in HepMC (Witek)
	return; /// RETURN
      } 
	///
    }
  }
  
  // (7.6) store tracks according to creator process 
  if( m_storeByOwnProcess && zstore ) {
    if(std::find(m_ownStoredProcess.begin(), m_ownStoredProcess.end(), 
		track->GetCreatorProcess()->GetProcessName()) 
	      != m_ownStoredProcess.end()) { 
      setProcess( track ) ;    
      fillGaussTrackInformation( track ) ;
      trackMgr()->SetStoreTrajectory( true );   
      gi->setStoreHepMC(true); // flag for storing in HepMC (Witek)
      return; /// RETURN
    } 
  }
  
  // (8) store  tracks, marked through GaussTrackInformation class
  if( m_storeMarkedTracks ) {
    if( 0 != gi && gi->toBeStored() ) { 
      setProcess( track ) ;
      fillGaussTrackInformation( track ) ;          
      trackMgr()->SetStoreTrajectory( true );   
      gi->setStoreHepMC(true); // flag for storing in HepMC (Witek)
      return; 
    }  /// RETURN 
  }
  
  // check if track is to be stored ???????????????????????????????????????
  if (trackMgr()->GetStoreTrajectory()) { 
    gi->setStoreHepMC(true);
    return;
  } /// RETURN 
  
  // if track is not to be stored, propagate it's parent ID (stored) to its
  // secondaries
  //if (!(gi->storeHepMC()))  // in the future
  {
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
        // set the flag saying that the direct mother is not stored
        G4VUserTrackInformation* duinf = tr->GetUserInformation();
        GaussTrackInformation* dti; 
        if(duinf)
        {
          dti = (GaussTrackInformation*) duinf;
        }
        else
        {
          dti = new GaussTrackInformation(); 
          tr->SetUserInformation(dti);
        }   
        dti->setDirectParent(false); 
        //
      }      
    }
    // also update the trackID in the hits
    //  G4VUserTrackInformation* uinf = track->GetUserInformation(); 
    //  GaussTrackInformation* ginf = (GaussTrackInformation*) uinf;
    //  ginf->updateHitsTrackID( track->GetParentID() );
    gi->updateHitsTrackID( track->GetParentID() );
  }
  
  // delete the trajectory by hand 
  if ( !trackMgr()->GetStoreTrajectory() )
    {
      G4VTrajectory* traj = trackMgr()->GimmeTrajectory();    
      if( 0 != traj ) { delete traj ; } 
      trackMgr()->SetTrajectory( 0 ) ;
    }
}; 
// ============================================================================
// Fill Track User information from Primary Info from Generator part
// (oscillation, signal barcode, HepMC Event link, etc...
// ============================================================================
void GaussPostTrackAction::fillGaussTrackInformation( const G4Track* track ) 
  const { 
  const G4DynamicParticle * dynamicParticle = track -> GetDynamicParticle() ;
  if ( 0 == dynamicParticle ) return ;
  
  G4PrimaryParticle * primaryParticle = dynamicParticle -> GetPrimaryParticle() ;
  if ( 0 == primaryParticle ) return ;
  
  G4VUserPrimaryParticleInformation * g4UserInfo = 
    track->GetDynamicParticle()->GetPrimaryParticle()->GetUserInformation() ;
  if ( 0 == g4UserInfo ) return ;
  
  GiGaPrimaryParticleInformation * uInf =
    ( GiGaPrimaryParticleInformation *) g4UserInfo ;
  GaussTrajectory* traj=
    (GaussTrajectory*) (trackMgr()->GimmeTrajectory());
             
  if ( uInf -> hasOscillated() ) traj->setHasOscillated( true ) ;
  if ( uInf -> isSignal() ) traj->setIsSignal( true ) ;
  traj->setSignalBarcode( uInf -> signalBarcode() ) ;
  traj->setHepMCEvent( uInf -> pHepMCEvent() ) ;
  traj->setMotherMCParticle( uInf -> motherMCParticle() ) ;
}

// ==============================================================================
// Set track creation processus
// ==============================================================================
void GaussPostTrackAction::setProcess( const G4Track * track ) const {
  const G4VProcess * process = track -> GetCreatorProcess() ;
  if ( 0 == process ) return ;
  GaussTrajectory * traj = (GaussTrajectory*) (trackMgr()->GimmeTrajectory()) ;
  traj->setProcessName( process -> GetProcessName() ) ;
}
