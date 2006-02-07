// $Id: Generation.cpp,v 1.12 2006-02-07 00:15:32 robbep Exp $
// Include files 

// local
#include "Generation.h"

// from Boost
#include <boost/checked_delete.hpp>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/RndmGenerators.h"

// from Event
#include "Event/GenHeader.h"
#include "Event/GenCollision.h"

// from LHCb
#include "Kernel/ParticleID.h"

// from HepMC
#include "HepMC/GenEvent.h"

// from Generators
#include "Generators/ISampleGenerationTool.h"
#include "Generators/IPileUpTool.h"
#include "Generators/IDecayTool.h" 
#include "Generators/IVertexSmearingTool.h"
#include "Generators/IFullGenEventCutTool.h"
#include "Generators/GenCounters.h"

// Gaudi Common Flat Random Number generator
extern Rndm::Numbers randgaudi ;

//-----------------------------------------------------------------------------
// Implementation file for class : Generation
//
// 2005-08-11 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<Generation>          s_factory ;
const        IAlgFactory& GenerationFactory = s_factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Generation::Generation( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) ,
    m_pileUpTool           ( 0 ) ,
    m_decayTool            ( 0 ) ,
    m_sampleGenerationTool ( 0 ) ,
    m_vertexSmearingTool   ( 0 ) ,
    m_fullGenEventCutTool  ( 0 ) ,
    m_nEvents              ( 0 ) , 
    m_nAcceptedEvents      ( 0 ) ,
    m_nInteractions        ( 0 ) , 
    m_nAcceptedInteractions( 0 ) ,
    m_nBeforeFullEvent     ( 0 ) ,
    m_nAfterFullEvent      ( 0 ) {
    // Generation Method
    declareProperty ( "SampleGenerationTool" , 
                      m_sampleGenerationToolName = "MinimumBias" ) ;
    declareProperty ( "EventType"          , m_eventType = 30000000 ) ;

    // Location of the output of the generation
    declareProperty ( "HepMCEventLocation" , m_hepMCEventLocation = 
                      LHCb::HepMCEventLocation::Default ) ;
    declareProperty ( "GenHeaderLocation"  , m_genHeaderLocation = 
                      LHCb::GenHeaderLocation::Default ) ;
    declareProperty ( "GenCollisionLocation" , m_genCollisionLocation = 
                      LHCb::GenCollisionLocation::Default ) ;
    
    // Tool name to generate the event
    declareProperty( "PileUpTool" , m_pileUpToolName = "FixedLuminosity" ) ;
    // Tool name to decay particles in the event
    declareProperty( "DecayTool"  , m_decayToolName = "EvtGenDecay" ) ;
    // Tool name to smear vertex
    declareProperty( "VertexSmearingTool" , 
                     m_vertexSmearingToolName = "BeamSpotSmearVertex" ) ;
    // Tool name to cut on full event
    declareProperty( "FullGenEventCutTool" , 
                     m_fullGenEventCutToolName = "" ) ;

    // Reset counters
    m_intC.assign( 0 ) ;
    m_intCAccepted.assign( 0 ) ;

    // setup counter names
    m_intCName[ Oneb ] = "generated interactions with >= 1b" ;    
    m_intCName[ Threeb ] = "generated interactions with >= 3b" ;
    m_intCName[ PromptB ] = "generated interactions with 1 prompt B" ;
    m_intCName[ Onec ] = "generated interactions with >= 1c" ;
    m_intCName[ Threec ] = "generated interactions with >= 3c" ;
    m_intCName[ PromptC ] = "generated interactions with >= prompt C" ;
    m_intCName[ bAndc ] = "generated interactions with b and c" ;

    m_intCAcceptedName[ Oneb ] = "accepted interactions with >= 1b" ;    
    m_intCAcceptedName[ Threeb ] = "accepted interactions with >= 3b" ;
    m_intCAcceptedName[ PromptB ] = "accepted interactions with 1 prompt B" ;
    m_intCAcceptedName[ Onec ] = "accepted interactions with >= 1c" ;
    m_intCAcceptedName[ Threec ] = "accepted interactions with >= 3c" ;
    m_intCAcceptedName[ PromptC ] = "accepted interactions with >= prompt C" ;
    m_intCAcceptedName[ bAndc ] = "accepted interactions with b and c" ;
}

//=============================================================================
// Destructor
//=============================================================================
Generation::~Generation() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode Generation::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize( ) ; // Initialize base class
  if ( sc.isFailure() ) return sc ;
  debug() << "==> Initialise" << endmsg ;

  // Initialization of the Common Flat Random generator if not already done
  // This generator must be used by all external MC Generator
  if ( ! ( randgaudi ) ) {
    sc = randgaudi.initialize( randSvc( ) , Rndm::Flat( 0 , 1 ) ) ;
    if ( ! sc.isSuccess( ) )
      return Error( "Could not initialize Rndm::Flat" , sc ) ;
  }

  // Retrieve pile up tool
  if ( "" == m_pileUpToolName ) {
    info() << "No Pile Up Tool is defined. Will generate no pile-up " 
           << endmsg ;
    info() << "and fix luminosity to 2e32" << endmsg ;
  } else m_pileUpTool = tool< IPileUpTool >( m_pileUpToolName , this ) ;

  // Retrieve decay tool
  if ( "" != m_decayToolName ) m_decayTool = 
    tool< IDecayTool >( m_decayToolName ) ;

  // Retrieve generation method tool
  if ( "" == m_sampleGenerationToolName ) 
    return Error( "No Sample Generation Tool is defined. This is mandatory" ) ;
  m_sampleGenerationTool = 
    tool< ISampleGenerationTool >( m_sampleGenerationToolName , this ) ;
  
  // Retrieve generation method tool
  if ( "" != m_vertexSmearingToolName ) 
    m_vertexSmearingTool = 
      tool< IVertexSmearingTool >( m_vertexSmearingToolName , this ) ;
  
  // Retrieve full gen event cut tool
  if ( "" != m_fullGenEventCutToolName ) m_fullGenEventCutTool =
    tool< IFullGenEventCutTool >( m_fullGenEventCutToolName , this ) ;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Generation::execute() {
  debug() << "Processing event type " << m_eventType << endmsg ;
  StatusCode sc = StatusCode::SUCCESS ;

  unsigned int  nPileUp ;
  double        currentLuminosity ;
  LHCb::HepMCEvents   * theEvents     ; LHCb::HepMCEvents::iterator itEvents ;
  LHCb::GenCollisions * theCollisions ; 

  interactionCounter theIntCounter ;

  // Create containers for this event
  theEvents = new LHCb::HepMCEvents( ) ;
  theCollisions = new LHCb::GenCollisions( ) ;

  // Generate a set of interaction until a good one is found
  bool goodEvent = false ;
  while ( ! goodEvent ) {
    theEvents -> erase( theEvents -> begin() , theEvents -> end() ) ;
    theCollisions -> erase(theCollisions -> begin() ,theCollisions -> end() ) ;
    theEvents -> clear() ;     theCollisions -> clear() ;
    
    // Compute the number of pile-up interactions to generate 
    if ( 0 != m_pileUpTool ) 
      nPileUp = m_pileUpTool -> numberOfPileUp( currentLuminosity ) ;
    else { 
      // default set to 1 pile and 2.10^32 luminosity
      nPileUp = 1 ;
      currentLuminosity = 2.e32/cm2/s ;
    }

    // generate a set of Pile up interactions according to the requested type
    // of event
    goodEvent = m_sampleGenerationTool -> generate( nPileUp , theEvents , 
                                                    theCollisions ) ;

    // increase event and interactions counters
    m_nEvents++ ;    m_nInteractions += nPileUp ;

    // Update interaction counters
    theIntCounter.assign( 0 ) ;
    for ( itEvents = theEvents -> begin() ; itEvents != theEvents -> end() ; 
          ++itEvents ) updateInteractionCounters( theIntCounter , *itEvents ) ;
    
    std::transform( m_intC.begin() , m_intC.end() , theIntCounter.begin() , 
                    m_intC.begin() , std::plus< unsigned int >( ) ) ;

    // Decay the event if it is a good event
    if ( ( goodEvent ) && ( 0 != m_decayTool ) ) {
      for ( itEvents = theEvents -> begin() ; itEvents != theEvents -> end() ;
            ++itEvents ) {
        sc = decayEvent( *itEvents ) ;
        if ( ! sc.isSuccess() ) return sc ;
        sc = m_vertexSmearingTool -> smearVertex( *itEvents ) ;
        if ( ! sc.isSuccess() ) return sc ;
      }
    }

    // Apply generator level cut on full event
    if ( m_fullGenEventCutTool ) {
      if ( goodEvent ) {
        m_nBeforeFullEvent++ ;
        goodEvent = m_fullGenEventCutTool -> studyFullEvent( theEvents , 
                                                             theCollisions ) ;
        if ( goodEvent ) m_nAfterFullEvent++ ;
      }
    }
  }  

  m_nAcceptedEvents++ ;
  m_nAcceptedInteractions += nPileUp ;

  std::transform( m_intCAccepted.begin() , m_intCAccepted.end() , 
                  theIntCounter.begin() , m_intCAccepted.begin() , 
                  std::plus< unsigned int >( ) ) ;

  // Now update the header information and put the event in Gaudi event store
  LHCb::GenHeader* theGenHeader = get<LHCb::GenHeader> ( m_genHeaderLocation );
  theGenHeader -> setLuminosity( currentLuminosity ) ;
  theGenHeader -> setEvType( m_eventType ) ;
  LHCb::GenCollisions::const_iterator it ;

  for ( it = theCollisions -> begin() ; theCollisions -> end() != it ; ++it ) 
    theGenHeader -> addToCollisions( *it ) ;

  sc = put( theEvents , m_hepMCEventLocation ) ;
  if ( ! sc.isSuccess() ) return Error( "Cannot store HepMC object" ) ;

  sc = put( theCollisions , m_genCollisionLocation ) ;
  if ( ! sc.isSuccess() ) return Error( "Cannot store GenCollision object" ) ;

  return sc ;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Generation::finalize() {
  using namespace GenCounters ;
  debug( ) << "==> Finalize" << endmsg ;
  // Print the various counters
  info() << "**************************************************" << endmsg ;
  m_pileUpTool -> printPileUpCounters( ) ; 
  info() << "***********   Generation counters   **************" << std::endl ;
  printCounter( info() , "generated events" , m_nEvents ) ;
  printCounter( info() , "generated interactions" , m_nInteractions ) ;
  
  for ( unsigned int i = 0 ; i < m_intC.size() ; ++i )
    printCounter( info() , m_intCName[ i ] , m_intC[ i ] ) ;
  
  printCounter( info() , "accepted events" , m_nAcceptedEvents ) ;
  printCounter( info() , "interactions in accepted events" , 
                m_nAcceptedInteractions ) ;
  
  for ( unsigned int j = 0 ; j < m_intCAccepted.size() ; ++j ) 
    printCounter( info() , m_intCAcceptedName[ j ] , m_intCAccepted[ j ] ) ;

  printEfficiency( info() , "full event cut" , m_nAfterFullEvent , 
                   m_nBeforeFullEvent ) ;
  info() << endmsg ;

  m_sampleGenerationTool -> printCounters() ;

  return GaudiAlgorithm::finalize( ) ; // Finalize base class
}

//=============================================================================
// Decay in the event all particles which have been left stable by the
// production generator
//=============================================================================
StatusCode Generation::decayEvent( LHCb::HepMCEvent * theEvent ) {
  using namespace LHCb;
  m_decayTool -> disableFlip() ;
  StatusCode sc ;
  
  HepMC::GenEvent * pEvt = theEvent -> pGenEvt() ;
  HepMC::GenEvent::particle_iterator itp ;

  // We must use particles_begin to obtain an ordered iterator of GenParticles
  // according to the barcode: this allows to reproduce events !
  for ( itp = pEvt -> particles_begin() ; itp != pEvt -> particles_end() ; 
        ++itp ) {

    HepMC::GenParticle * thePart = (*itp) ;
    unsigned int status = thePart -> status() ;

    if ( ( HepMCEvent::StableInProdGen  == status ) || 
         ( ( HepMCEvent::DecayedByDecayGenAndProducedByProdGen == status )
           && ( 0 == thePart -> end_vertex() ) ) ) {
      
      if ( m_decayTool -> isKnownToDecayTool( thePart -> pdg_id() ) ) {

        if ( HepMCEvent::StableInProdGen == status ) 
          thePart -> 
            set_status( HepMCEvent::DecayedByDecayGenAndProducedByProdGen ) ;
        else thePart -> set_status( HepMCEvent::DecayedByDecayGen ) ;

        sc = m_decayTool -> generateDecay( thePart ) ;
        if ( ! sc.isSuccess() ) return sc ;
      }
    } 
  }  
  return StatusCode::SUCCESS ;
}

//=============================================================================
// Interaction counters
//=============================================================================
void Generation::updateInteractionCounters( interactionCounter & theCounter ,
                                            const LHCb::HepMCEvent * evt ) 
{
  const HepMC::GenEvent * theEvent = evt -> pGenEvt() ;
  unsigned int bQuark( 0 ) , bHadron( 0 ) , cQuark( 0 ) , cHadron( 0 ) ;
  int pdgId ;
  HepMC::GenEvent::particle_const_iterator iter ;
  for ( iter = theEvent -> particles_begin() ; 
        theEvent -> particles_end() != iter ; ++iter ) {
    if ( (*iter) -> status() >= LHCb::HepMCEvent::DocumentationParticle ) 
      continue ;
    pdgId = abs( (*iter) -> pdg_id() ) ;
    LHCb::ParticleID thePid( pdgId ) ;
    if ( 5 == pdgId ) bQuark++ ;
    if ( 4 == pdgId ) cQuark++ ;
    if ( thePid.hasBottom() ) bHadron++ ;
    if ( thePid.hasCharm() ) cHadron++ ;
  }
  if ( bQuark >= 1 ) theCounter[ Oneb ]++ ;
  if ( bQuark >= 3 ) theCounter[ Threeb ]++ ;
  if ( cQuark >= 1 ) theCounter[ Onec ]++ ;
  if ( cQuark >= 3 ) theCounter[ Threec ]++ ;
  if ( ( bQuark >= 1 ) && ( cQuark >= 1 ) ) theCounter[ bAndc ]++ ;
  if ( ( 0 == bQuark ) && ( bHadron > 0 ) ) theCounter[ PromptB ]++ ;
  if ( ( 0 == cQuark ) && ( 0 == bHadron ) && ( cHadron > 0 ) ) 
    theCounter[ PromptC ]++;
}

