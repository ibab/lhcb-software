// $Id: Generation.cpp,v 1.1 2005-10-03 10:28:25 robbep Exp $
// Include files 

// local
#include "Generation.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/RndmGenerators.h"

#include "Event/HepMCEvent.h"
#include "Event/GenHeader.h"
#include "Event/HardInfo.h"

#include "HepMC/GenEvent.h"

#include "Generators/HepMCUtils.h"
#include "Generators/ISampleGenerationTool.h"
#include "Generators/IPileUpTool.h"
#include "Generators/IDecayTool.h" 
#include "Generators/IVertexSmearingTool.h"

// Gaudi Common Flat Random Number generator
Rndm::Numbers randgaudi ;

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
    m_decayTool            ( 0 ) ,
    m_nEvents              ( 0 ) , 
    m_nAcceptedEvents      ( 0 ) ,
    m_nInteractions        ( 0 ) , 
    m_nAcceptedInteractions( 0 ) {
    // Generation Method
    declareProperty ( "SampleGenerationTool" , 
                      m_sampleGenerationToolName = "MinimumBias" ) ;
    declareProperty ( "EventType"          , m_eventType = 30000000 ) ;

    // Location of the output of the generation
    declareProperty ( "HepMCEventLocation" , 
                      m_hepMCEventLocation = HepMCEventLocation::Default ) ;
    declareProperty ( "GenHeaderLocation"  ,
                      m_genHeaderLocation = GenHeaderLocation::Default ) ;
    declareProperty ( "HardInfoLocation" ,
                      m_hardInfoLocation = HardInfoLocation::Default ) ;
    
    // Tool name to generate the event
    declareProperty( "PileUpTool" , m_pileUpToolName = "VariableLuminosity" ) ;
    // Tool name to decay particles in the event
    declareProperty( "DecayTool"  , m_decayToolName = "EvtGenDecay" ) ;
    // Tool name to smear vertex
    declareProperty( "VertexSmearingTool" , 
                     m_vertexSmearingToolName = "BeamSpotSmearVertex" ) ;
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
  m_pileUpTool = tool< IPileUpTool >( m_pileUpToolName , this ) ;

  // Retrieve generation method tool
  m_sampleGenerationTool = 
    tool< ISampleGenerationTool >( m_sampleGenerationToolName , this ) ;
  
  // Retrieve generation method tool
  m_vertexSmearingTool = 
    tool< IVertexSmearingTool >( m_vertexSmearingToolName , this ) ;
  
  // Retrieve decay tool
  if ( "" != m_decayToolName ) m_decayTool = 
    tool< IDecayTool >( m_decayToolName ) ;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Generation::execute() {
  debug() << "Processing event type " << m_eventType << endmsg ;
  StatusCode sc = StatusCode::SUCCESS ;

  unsigned int nPileUp ;
  double       currentLuminosity ;
  EventVector  theHepMCs    ; EventVector::iterator itEvent ;
  HardVector   theHardInfos ; HardVector::iterator  itHard  ;

  // Generate a set of interaction until a good one is found
  bool goodEvent = false ;
  while ( ! goodEvent ) {
    // Clear the vectors which will contain the HepMC events and Hard infos
    for ( itEvent = theHepMCs.begin() ; theHepMCs.end() != itEvent ;
          ++itEvent ) delete (*itEvent) ;
    for ( itHard  = theHardInfos.begin() ; theHardInfos.end() != itHard ;
          ++itHard ) delete (*itHard) ;
    theHepMCs.clear() ; theHardInfos.clear() ;
    
    // Compute the number of pile-up interactions to generate 
    nPileUp = m_pileUpTool -> numberOfPileUp( currentLuminosity ) ;
    
    // generate a set of Pile up interactions according to the requested type
    // of event
    goodEvent = m_sampleGenerationTool -> generate( nPileUp , theHepMCs , 
                                                    theHardInfos ) ;
    
    m_nEvents++ ;
    m_nInteractions += nPileUp ;
  }  

  m_nAcceptedEvents++ ;
  m_nAcceptedInteractions += nPileUp ;

  // Now store the event in Gaudi event store
  GenHeader * theGenHeader = new GenHeader() ;
  theGenHeader -> setLuminosity( currentLuminosity ) ;
  theGenHeader -> setEvType( m_eventType ) ;
  
  sc = put( theGenHeader , m_genHeaderLocation ) ;
  if ( ! sc.isSuccess() ) return Error( "Cannot store GenHeader object" ) ;

  HepMCEvents * eventVector = new HepMCEvents() ;
  for ( itEvent = theHepMCs.begin() ; theHepMCs.end() != itEvent ; 
        ++itEvent ) {
    // Decay the particles in the event which have been left stable by 
    // the production generator
    if ( 0 != m_decayTool ) sc = decayEvent( *itEvent ) ;    
    // Smear vertex
    sc = m_vertexSmearingTool -> smearVertex( *itEvent ) ;
    eventVector -> insert( *itEvent ) ;
  }


  sc = put( eventVector , m_hepMCEventLocation ) ;
  if ( ! sc.isSuccess() ) return Error( "Cannot store HepMC object" ) ;

  HardInfos * hardVector = new HardInfos() ;
  for ( itHard = theHardInfos.begin() ; theHardInfos.end() != itHard ;
        ++itHard ) {
    hardVector -> insert( *itHard ) ;
  }  
  sc = put( hardVector , m_hardInfoLocation ) ;
  if ( ! sc.isSuccess() ) return Error( "Cannot store HardInfo object" ) ;

  return sc ;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Generation::finalize() {
  debug( ) << "==> Finalize" << endmsg ;
  // Print the various counters
  m_pileUpTool -> printPileUpCounters( ) ; 
  info( ) << "Number of generated events: " << m_nEvents << endmsg ;
  info( ) << "Number of accepted events: " << m_nAcceptedEvents << endmsg ;
  info( ) << "Number of generated interactions: " << m_nInteractions 
          << endmsg ;
  info( ) << "Number of interactions in accepted events: " 
          << m_nAcceptedInteractions << endmsg ;
  m_sampleGenerationTool -> printCounters() ;

  return GaudiAlgorithm::finalize( ) ; // Finalize base class
}

//=============================================================================
// Decay in the event all particles which have been left stable by the
// production generator
//=============================================================================
StatusCode Generation::decayEvent( HepMCEvent * theEvent ) {
  m_decayTool -> disableFlip() ;
  StatusCode sc = StatusCode::SUCCESS ;
  
  GenParticles theHepMCVector ;
  HepMC::GenEvent * pEvt = theEvent -> pGenEvt() ;
  HepMCUtils::SortHepMC( theHepMCVector , pEvt -> particles_size() ,
                         pEvt -> particles_begin() , pEvt -> particles_end() );
  GenParticles::iterator itp ;
  for ( itp = theHepMCVector.begin() ; itp != theHepMCVector.end() ; ++itp ) {
    HepMC::GenParticle * thePart = (*itp) ;
    if ( 1 == thePart -> status() ) {
      if ( m_decayTool -> isKnownToDecayTool( thePart -> pdg_id() ) ) {
        thePart -> set_status( 888 ) ;
        sc = m_decayTool -> generateDecay( pEvt , thePart ) ;
      }
    } 
  }  
  return sc ;
}


