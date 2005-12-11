// $Id: Generation.cpp,v 1.7 2005-12-11 23:22:30 robbep Exp $
// Include files 

// local
#include "Generation.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/RndmGenerators.h"

#include "Event/HepMCEvent.h"
#include "Event/GenHeader.h"
#include "Event/HardInfo.h"
#include "Kernel/ParticleID.h"

#include "HepMC/GenEvent.h"

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
    m_decayTool            ( 0 ) ,
    m_fullGenEventCutTool  ( 0 ) ,
    m_nEvents              ( 0 ) , 
    m_nAcceptedEvents      ( 0 ) ,
    m_nInteractions        ( 0 ) , 
    m_nAcceptedInteractions( 0 ) ,
    m_n1b                  ( 0 ) ,
    m_n3b                  ( 0 ) ,
    m_nPromptB             ( 0 ) ,
    m_n1c                  ( 0 ) ,
    m_n3c                  ( 0 ) ,
    m_nPromptC             ( 0 ) ,
    m_nbc                  ( 0 ) ,
    m_n1bAccepted          ( 0 ) ,
    m_n3bAccepted          ( 0 ) ,
    m_nPromptBAccepted     ( 0 ) ,
    m_n1cAccepted          ( 0 ) ,
    m_n3cAccepted          ( 0 ) ,
    m_nPromptCAccepted     ( 0 ) ,
    m_nbcAccepted          ( 0 ) ,
    m_nBeforeFullEvent     ( 0 ) ,
    m_nAfterFullEvent      ( 0 ) {
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
    declareProperty( "PileUpTool" , m_pileUpToolName = "FixedLuminosity" ) ;
    // Tool name to decay particles in the event
    declareProperty( "DecayTool"  , m_decayToolName = "EvtGenDecay" ) ;
    // Tool name to smear vertex
    declareProperty( "VertexSmearingTool" , 
                     m_vertexSmearingToolName = "BeamSpotSmearVertex" ) ;
    // Tool name to cut on full event
    declareProperty( "FullGenEventCutTool" , 
                     m_fullGenEventCutToolName = "" ) ;
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

  // Retrieve decay tool
  if ( "" != m_decayToolName ) m_decayTool = 
    tool< IDecayTool >( m_decayToolName ) ;

  // Retrieve generation method tool
  m_sampleGenerationTool = 
    tool< ISampleGenerationTool >( m_sampleGenerationToolName , this ) ;
  
  // Retrieve generation method tool
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

  unsigned int nPileUp ;
  double       currentLuminosity ;
  EventVector  theHepMCs    ; EventVector::iterator itEvent ;
  HardVector   theHardInfos ; HardVector::iterator  itHard  ;

  unsigned int n1b( 0 ) , n3b( 0 ) , nPromptB( 0 ) ;
  unsigned int n1c( 0 ) , n3c( 0 ) , nPromptC( 0 ) ;
  unsigned int nbc( 0 ) ;

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

    // Update interaction counters
    n1b = 0 ; n3b = 0 ; nPromptB = 0 ;
    n1c = 0 ; n3c = 0 ; nPromptC = 0 ;
    nbc = 0 ;
    for ( itEvent = theHepMCs.begin() ; itEvent != theHepMCs.end() ; 
          ++itEvent ) updateInteractionCounters( n1b, n3b, nPromptB, n1c, n3c, 
                                                 nPromptC, nbc , *itEvent ) ;
    
    m_n1b += n1b ; m_n3b += n3b ; m_nPromptB += nPromptB ;
    m_n1c += n1c ; m_n3c += n3c ; m_nPromptC += nPromptC ;
    m_nbc += nbc ;

    // Decay the event if it is a good event
    if ( ( goodEvent ) && ( 0 != m_decayTool ) ) {
      for ( itEvent = theHepMCs.begin() ; itEvent != theHepMCs.end() ;
            ++itEvent ) {
        sc = decayEvent( *itEvent ) ;
        if ( ! sc.isSuccess() ) return sc ;
        sc = m_vertexSmearingTool -> smearVertex( *itEvent ) ;
        if ( ! sc.isSuccess() ) return sc ;
      }
    }

    // Apply generator level cut on full event
    if ( m_fullGenEventCutTool ) {
      if ( goodEvent ) {
        m_nBeforeFullEvent++ ;
        goodEvent = m_fullGenEventCutTool -> studyFullEvent( theHepMCs , 
                                                             theHardInfos ) ;
        if ( goodEvent ) m_nAfterFullEvent++ ;
      }
    }
  }  

  m_nAcceptedEvents++ ;
  m_nAcceptedInteractions += nPileUp ;

  m_n1bAccepted += n1b ; m_n3bAccepted += n3b ; 
  m_nPromptBAccepted += nPromptB ;
  m_n1cAccepted += n1c ; m_n3cAccepted += n3c ; 
  m_nPromptCAccepted += nPromptC ;
  m_nbcAccepted += nbc ;

  // Now store the event in Gaudi event store
  GenHeader * theGenHeader = new GenHeader() ;
  theGenHeader -> setLuminosity( currentLuminosity ) ;
  theGenHeader -> setEvType( m_eventType ) ;
  
  sc = put( theGenHeader , m_genHeaderLocation ) ;
  if ( ! sc.isSuccess() ) return Error( "Cannot store GenHeader object" ) ;

  HepMCEvents * eventVector = new HepMCEvents() ;
  for ( itEvent = theHepMCs.begin() ; theHepMCs.end() != itEvent ; 
        ++itEvent ) eventVector -> insert( *itEvent ) ;
  sc = put( eventVector , m_hepMCEventLocation ) ;
  if ( ! sc.isSuccess() ) return Error( "Cannot store HepMC object" ) ;

  HardInfos * hardVector = new HardInfos() ;
  for ( itHard = theHardInfos.begin() ; theHardInfos.end() != itHard ;
        ++itHard ) hardVector -> insert( *itHard ) ;
  sc = put( hardVector , m_hardInfoLocation ) ;
  if ( ! sc.isSuccess() ) return Error( "Cannot store HardInfo object" ) ;

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
  printCounter( info() , "generated interactions with >= 1 b" , m_n1b ) ;
  printCounter( info() , "generated interactions with >= 3 b" , m_n3b ) ;
  printCounter( info() , "generated interactions with 1 prompt B" , 
                m_nPromptB ) ;
  printCounter( info() , "generated interactions with >= 1 c" , m_n1c ) ;
  printCounter( info() , "generated interactions with >= 3 c" , m_n3c ) ;
  printCounter( info() , "accepted interactions with b and c" , m_nbc ) ;
  printCounter( info() , "generated interactions with 1 prompt C" , 
                m_nPromptC ) ;
  
  printCounter( info() , "accepted events" , m_nAcceptedEvents ) ;
  printCounter( info() , "interactions in accepted events" , 
                m_nAcceptedInteractions ) ;
  printCounter( info() , "accepted interactions with >= 1 b", m_n1bAccepted ) ;
  printCounter( info() , "accepted interactions with >= 3 b", m_n3bAccepted ) ;
  printCounter( info() , "accepted interactions with 1 prompt B" , 
                m_nPromptBAccepted ) ;
  printCounter( info() , "accepted interactions with >= 1 c", m_n1cAccepted ) ;
  printCounter( info() , "accepted interactions with >= 3 c", m_n3cAccepted ) ;
  printCounter( info() , "accepted interactions with 1 prompt C" , 
                m_nPromptCAccepted ) ; 
  printCounter( info() , "accepted interactions with b and c", m_nbcAccepted );

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
StatusCode Generation::decayEvent( HepMCEvent * theEvent ) {
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

    if ( ( 1 ==status ) || ( ( 888 == status ) && 
                             ( 0 == thePart -> end_vertex() ) ) ) {

      if ( m_decayTool -> isKnownToDecayTool( thePart -> pdg_id() ) ) {

        if ( 1 == status ) thePart -> set_status( 888 ) ;
        else thePart -> set_status( 777 ) ;

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
void Generation::updateInteractionCounters( unsigned int & n1b , 
                                            unsigned int & n3b ,
                                            unsigned int & nPromptB ,
                                            unsigned int & n1c ,
                                            unsigned int & n3c ,
                                            unsigned int & nPromptC ,
                                            unsigned int & nbc ,
                                            const HepMCEvent * evt ) 
{
  const HepMC::GenEvent * theEvent = evt -> pGenEvt() ;
  unsigned int bQuark( 0 ) , bHadron( 0 ) , cQuark( 0 ) , cHadron( 0 ) ;
  int pdgId ;
  HepMC::GenEvent::particle_const_iterator iter ;
  for ( iter = theEvent -> particles_begin() ; 
        theEvent -> particles_end() != iter ; ++iter ) {
    if ( (*iter) -> status() >= 3 ) continue ;
    pdgId = abs( (*iter) -> pdg_id() ) ;
    ParticleID thePid( pdgId ) ;
    if ( 5 == pdgId ) bQuark++ ;
    if ( 4 == pdgId ) cQuark++ ;
    if ( thePid.hasBottom() ) bHadron++ ;
    if ( thePid.hasCharm() ) cHadron++ ;
  }
  if ( bQuark >= 1 ) n1b++ ;
  if ( bQuark >= 3 ) n3b++ ;
  if ( cQuark >= 1 ) n1c++ ;
  if ( cQuark >= 3 ) n3c++ ;
  if ( ( bQuark >= 1 ) && ( cQuark >= 1 ) ) nbc++ ;
  if ( ( 0 == bQuark ) && ( bHadron > 0 ) ) nPromptB++ ;
  if ( ( 0 == cQuark ) && ( 0 == bHadron ) && ( cHadron > 0 ) ) nPromptC++ ;
}

