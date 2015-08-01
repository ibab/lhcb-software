// Include files 

// local
#include "DaughtersInLHCbKeepOnlySignal.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/SystemOfUnits.h"

// from Kernel
#include "Kernel/ParticleID.h"
#include "GaudiKernel/Vector4DTypes.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

// from HepMC
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

// from Generators
#include "GenEvent/HepMCUtils.h"
#include "MCInterfaces/IDecayTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DaughtersInLHCbKeepOnlySignal
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( DaughtersInLHCbKeepOnlySignal )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DaughtersInLHCbKeepOnlySignal::DaughtersInLHCbKeepOnlySignal( const std::string& type,
                                                              const std::string& name,
                                                              const IInterface* parent )
: GaudiTool ( type, name , parent ) , 
  m_decayTool( 0 ) , m_nSignalBeforeCut( 0 ) , 
  m_signalQuark     ( LHCb::ParticleID::down ) , 
  m_ppSvc( 0 ) 
{
  declareInterface< IGenCutTool >( this ) ;
  declareProperty( "ChargedThetaMin" , m_chargedThetaMin = 10 * Gaudi::Units::mrad ) ;
  declareProperty( "ChargedThetaMax" , m_chargedThetaMax = 400 * Gaudi::Units::mrad ) ;
  declareProperty( "NeutralThetaMin" , m_neutralThetaMin = 5 * Gaudi::Units::mrad ) ;
  declareProperty( "NeutralThetaMax" , m_neutralThetaMax = 400 * Gaudi::Units::mrad ) ;
  declareProperty( "DecayTool"       , m_decayToolName = "EvtGenDecay" ) ;
  declareProperty( "SignalPID"       , m_signalPID = 443 ) ;
}

//=============================================================================
// Destructor 
//=============================================================================
DaughtersInLHCbKeepOnlySignal::~DaughtersInLHCbKeepOnlySignal( ) { ; }

//=============================================================================
// Initialize
//=============================================================================
StatusCode DaughtersInLHCbKeepOnlySignal::initialize( ) {

  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTool

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize and retrieve "
                                      << m_decayToolName << " tool" << endmsg;

  if ( "" != m_decayToolName )
    m_decayTool = tool< IDecayTool >( m_decayToolName ) ;

  m_decayTool -> setSignal( m_signalPID ) ;

  m_ppSvc =
    svc< LHCb::IParticlePropertySvc >( "LHCb::ParticlePropertySvc" ) ;

  const LHCb::ParticleProperty * prop = m_ppSvc -> find( LHCb::ParticleID( m_signalPID ) ) ;
  LHCb::ParticleID pid( prop -> pdgID() ) ;
  if ( pid.hasCharm() ) m_signalQuark = LHCb::ParticleID::charm ;
  else if ( pid.hasBottom() ) m_signalQuark = LHCb::ParticleID::bottom ;  

  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode DaughtersInLHCbKeepOnlySignal::finalize( ) {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  if ( 0 != m_decayTool ) release( m_decayTool );
  if ( 0 != m_ppSvc ) release( m_ppSvc ) ;

  info() << "*****************************************" << std::endl 
         << "************** Signal Counters *********" << std::endl 
         << "Number of generated signal particles = " 
         << m_nSignalBeforeCut << endmsg ;

  return GaudiTool::finalize();
}

//=============================================================================
// Acceptance function
//=============================================================================
bool DaughtersInLHCbKeepOnlySignal::applyCut( ParticleVector & theParticleVector ,
                                              const HepMC::GenEvent * theEvent ,
                                              const LHCb::GenCollision * /* theHardInfo */ )
  const {
  // First decay all particles heavier than the signal
  m_decayTool -> disableFlip() ;
  StatusCode sc ;
  
  HepMCUtils::ParticleSet particleSet ;  

  HepMC::GenEvent::particle_const_iterator it ;
  switch ( m_signalQuark ) {
    
  case LHCb::ParticleID::bottom: // decay only B
    for ( it = theEvent -> particles_begin() ; 
          it != theEvent -> particles_end() ; ++it )
      if ( LHCb::ParticleID( (*it) -> pdg_id() ).hasQuark( m_signalQuark ) ) 
        particleSet.insert( *it ) ;
    break ;
    
  case LHCb::ParticleID::charm: // decay B + D
    for ( it = theEvent -> particles_begin() ;
          it != theEvent -> particles_end() ; ++it ) {
      LHCb::ParticleID pid( (*it) -> pdg_id() ) ;
      if ( ( pid.hasQuark( m_signalQuark ) ) || 
           ( pid.hasQuark( LHCb::ParticleID::bottom ) ) ) 
        particleSet.insert( *it ) ;
    }
    break ;

  default:
    if ( 15 == LHCb::ParticleID(m_signalPID).abspid() ) // tau ?
    {
      for ( it = theEvent -> particles_begin() ;
            it != theEvent -> particles_end() ; ++it ) {
        LHCb::ParticleID pid( (*it) -> pdg_id() ) ;
        if ( ( pid.hasQuark( LHCb::ParticleID::charm  ) ) || 
             ( pid.hasQuark( LHCb::ParticleID::bottom ) ) ) 
          particleSet.insert( *it ) ;
      }
      break ; 
    }
    else 
    {
      // decay all what is heavier than the signal
      for ( it = theEvent -> particles_begin() ;
            it != theEvent -> particles_end() ; ++it ) {
        LHCb::ParticleID pid( (*it) -> pdg_id() ) ;
        if ( (*it) -> generated_mass() > 
             m_ppSvc -> find( LHCb::ParticleID( m_signalPID ) ) -> mass() )
          particleSet.insert( *it ) ;
        // if signal is KS then decay also K0
        else if ( ( m_signalPID == 310 ) && ( pid.abspid() == 311 ) )
          particleSet.insert( *it ) ;
      }      
    }
    break ; 
  }

  for ( HepMCUtils::ParticleSet::iterator itHeavy = particleSet.begin() ; 
        itHeavy != particleSet.end() ; ++itHeavy ) 
    
    if ( ( LHCb::HepMCEvent::StableInProdGen == (*itHeavy) -> status() ) && 
         ( m_signalPID != abs( (*itHeavy) -> pdg_id() ) ) ) {
      
      if ( m_decayTool -> isKnownToDecayTool( (*itHeavy) -> pdg_id() ) ) {
        sc = m_decayTool -> generateDecayWithLimit( *itHeavy , m_signalPID ) ;
        if ( ! sc.isSuccess() ) return sc ;
      } 
    }

  // Check whether the signal is in the Event or not
  //--------------------------------------------------------------------
  theParticleVector.clear( ) ;
  for ( it = theEvent -> particles_begin() ; 
        it != theEvent -> particles_end() ; ++it )
    if ( abs( (*it) -> pdg_id() ) == abs( m_signalPID ) ) 
      if ( LHCb::HepMCEvent::DocumentationParticle != (*it) -> status() ) 
        theParticleVector.push_back( *it ) ;
  std::sort( theParticleVector.begin() , theParticleVector.end() , 
             HepMCUtils::compareHepMCParticles ) ;
  
  if ( theParticleVector.empty() ) return false ;

  m_nSignalBeforeCut++ ;

  // decay the signal particle
  //--------------------------------------------------------------------  
  bool hasFlipped = false ;
  HepMC::GenParticle * theSignal ;
  theSignal = theParticleVector.front() ;  
  if ( theParticleVector.size() > 1 )
    theParticleVector.erase( theParticleVector.begin()+1 , theParticleVector.end() ) ;
  sc = m_decayTool -> generateSignalDecay( theSignal , hasFlipped ) ;
  if ( ! sc.isSuccess() ) {
    Error( "Skip event" ) ;
    return false ;
  }

  // apply cut
  //--------------------------------------------------------------------  
  ParticleVector::iterator itp ;
  for ( itp = theParticleVector.begin() ; 
        itp != theParticleVector.end() ; ) {    
    if ( ! passCuts( *itp ) ) {
      itp = theParticleVector.erase( itp ) ;
    } else ++itp ;
  }

  if ( theParticleVector.empty() ) return false ;
  std::vector< HepMC::GenVertex * > vertexVector ;
  
  HepMC::GenVertex * theNewVertex = copyHepMCParticle( theSignal , vertexVector ) ;
  std::pair< HepMC::GenParticle * , HepMC::GenParticle * >  beam = 
    theEvent -> beam_particles();
  HepMC::GenParticle * b1 = new HepMC::GenParticle( *beam.first ) ;
  HepMC::GenParticle * b2 = new HepMC::GenParticle( *beam.second ) ;
  theNewVertex -> add_particle_in( b1 ) ;
  theNewVertex -> add_particle_in( b2 ) ;

  // clear event
  HepMC::GenEvent* newEvent = const_cast< HepMC::GenEvent *>( theEvent );
  newEvent -> clear() ;

  newEvent -> add_vertex( theNewVertex ) ;
  std::vector< HepMC::GenVertex * >::iterator vIt ;
  for ( vIt = vertexVector.begin() ; vertexVector.end() != vIt ; ++vIt )
    newEvent -> add_vertex( *vIt ) ;

  newEvent -> set_beam_particles( b1 , b2 ) ;

  return true ;
}

//=============================================================================
// Functions to test if all daughters are in acceptance
//=============================================================================
bool DaughtersInLHCbKeepOnlySignal::passCuts( const HepMC::GenParticle * theSignal  ) const {
  HepMC::GenVertex * EV = theSignal -> end_vertex() ;
  if ( 0 == EV ) return true ;

  // check if pz of the signal is positive
  if ( theSignal -> momentum().pz() < 0. ) return false ;

  typedef std::vector< HepMC::GenParticle * > Particles ;
  Particles stables ;
  HepMC::GenVertex::particle_iterator iter ;

  for ( iter = EV -> particles_begin( HepMC::descendants ) ; 
        iter != EV -> particles_end( HepMC::descendants ) ; ++iter ) {
    if ( 0 == (*iter) -> end_vertex() ) stables.push_back( *iter ) ;
  }  

  if ( stables.empty() )
    Exception( "Signal has no stable daughters !" ) ;

  double angle( 0. ) ;
  double firstpz = stables.front() -> momentum().pz() ;

  debug() << "New event" << endmsg ;

  for ( Particles::const_iterator it = stables.begin() ; it != stables.end() ;
        ++it ) {

    debug() << "Check particle " << (*it) -> pdg_id() << " with angle " 
            << (*it) -> momentum().theta() / Gaudi::Units::mrad 
            << " mrad." << endmsg ;
   
    // Remove neutrinos
    if ( ( 12 == abs( (*it) -> pdg_id() ) ) || 
         ( 14 == abs( (*it) -> pdg_id() ) ) || 
         ( 16 == abs( (*it) -> pdg_id() ) ) ) continue ;
 
    // Don't use daughters of Lambda and KS:
    HepMC::GenParticle * theParent ;
    theParent = 
      *( (*it) -> production_vertex() -> particles_in_const_begin() ) ;
    if ( 3122 == abs( theParent -> pdg_id() ) ) continue ;
    if ( 310 == theParent -> pdg_id() ) continue ;

    // Consider only gammas from pi0 and eta
    if ( 22 == (*it) -> pdg_id() ) {
      if ( ( 111 != theParent -> pdg_id() ) &&
           ( 221 != theParent -> pdg_id() ) ) continue ;
    }

    // All particles in same direction
    if ( 0 > ( firstpz * ( (*it) -> momentum().pz() ) ) ) return false ;
      
    angle = (*it) -> momentum().theta() ;
    
    LHCb::ParticleID pid( (*it) -> pdg_id() ) ;
    if ( 0 == pid.threeCharge() ) {
      if ( fabs( sin( angle ) ) > fabs( sin( m_neutralThetaMax ) ) ) 
        return false ;
      if ( fabs( sin( angle ) ) < fabs( sin( m_neutralThetaMin ) ) ) 
        return false ;
    } else {
      if ( fabs( sin( angle ) ) > fabs( sin( m_chargedThetaMax ) ) ) 
        return false ;
      if ( fabs( sin( angle ) ) < fabs( sin( m_chargedThetaMin ) ) ) 
        return false ;
    }
  }

  debug() << "Event passed !" << endmsg ;
  
  return true ;
}

//=================================================================
// Copy HepMC Particle into a new event
//=================================================================

HepMC::GenVertex * DaughtersInLHCbKeepOnlySignal::copyHepMCParticle( const HepMC::GenParticle * theSignal ,
                                                                     std::vector< HepMC::GenVertex *> & vV ) 
  const {
  if ( 0 == theSignal -> production_vertex() ) 
  {
    Error( "Signal particle has no production vertex." ) ;
    return 0 ;
  }
  
  // create a new vertex and a new HepMC Particle for the root particle
  // (a copy of which will be associated to the new HepMC event) 

  HepMC::GenVertex * newVertex =
    new HepMC::GenVertex( theSignal -> production_vertex() -> position() ) ;
  
  HepMC::GenParticle * theNewParticle =
    new HepMC::GenParticle( theSignal -> momentum() , theSignal -> pdg_id() ,
                            theSignal -> status() ) ;
  
  newVertex -> add_particle_out( theNewParticle ) ;
  
  // Associate the new particle to the HepMC event
  // and copy all tree to the new HepMC event
  fillTree( theNewParticle , theSignal , vV ) ;
  
  return newVertex ;
}

//=================================================================
// Copy a HepMC Particle with all descendents
//=================================================================

void DaughtersInLHCbKeepOnlySignal::fillTree( HepMC::GenParticle * theNewParticle ,
                                              const HepMC::GenParticle * theOldParticle  ,
                                              std::vector< HepMC::GenVertex *> & vV  ) 
  const {
  //
  // Copy theOldParticle to theNewParticle in theEvent
  // theNewParticle already exist and is created outside this function
  HepMC::GenVertex * oVertex = theOldParticle -> end_vertex() ;
  if ( 0 != oVertex ) {
    // Create decay vertex and associate it to theNewParticle
    HepMC::GenVertex * newVertex =
      new HepMC::GenVertex( oVertex -> position() ) ;
    newVertex -> add_particle_in( theNewParticle ) ;
    vV.push_back( newVertex ) ;

    // loop over child particle of this vertex after sorting them
    std::list< const HepMC::GenParticle * > outParticles ;
    for ( HepMC::GenVertex::particles_out_const_iterator itP = 
            oVertex -> particles_out_const_begin() ; 
          itP != oVertex -> particles_out_const_end() ; ++itP )
      outParticles.push_back( (*itP ) ) ;

    outParticles.sort( HepMCUtils::compareHepMCParticles ) ;

    std::list< const HepMC::GenParticle * >::const_iterator child ;
    for ( child = outParticles.begin( ) ; child != outParticles.end( ) ; 
          ++child ) {
      
      // Create a new particle for each daughter of theOldParticle
      HepMC::GenParticle * newPart =
        new HepMC::GenParticle ( (*child) -> momentum () ,
                                 (*child) -> pdg_id ()   ,
                                 (*child) -> status ()   ) ;
      newVertex -> add_particle_out( newPart ) ;
      
      const HepMC::GenParticle * theChild = (*child) ;
      // Recursive call : fill the event with the daughters
      fillTree( newPart , theChild , vV ) ;      
    }
  }
}
