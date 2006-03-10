//
// This File contains the implementation of the MCParticle2MCHitAlg interface
//
// C++ code for 'LHCb Tracking package(s)'
//
//   Authors: Rutger van der Eijk
//   Created: 3-7-2002

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// LHCbKernel
#include "Relations/Relation1D.h" 

// from Event/MCEvent
#include "Event/MCParticle.h"
#include "Event/MCHit.h"

// from Event/LinkerEvent
#include "Linker/LinkerWithKey.h"

// local
#include "MCParticle2MCHitAlg.h"

// Declaration of the Algorithm Factory
static const  AlgFactory<MCParticle2MCHitAlg>          s_factory ;
const        IAlgFactory& MCParticle2MCHitAlgFactory = s_factory ; 

using namespace LHCb;

MCParticle2MCHitAlg::MCParticle2MCHitAlg(const std::string& name,
                                         ISvcLocator* pSvcLocator):
  GaudiAlgorithm ( name , pSvcLocator ),
  m_inputData(),
  m_outputData( "Relations/" + LHCb::MCParticleLocation::Default + "2MCHits" )
{
  // Standard constructor, initializes variables
  declareProperty( "MCHitPath",   m_inputData );
  declareProperty( "OutputData",  m_outputData );
  declareProperty( "MakeLinker" , m_makeLinker = false );
}


MCParticle2MCHitAlg::~MCParticle2MCHitAlg()
{
  // Destructor
}; 


StatusCode MCParticle2MCHitAlg::execute()
{
  // Main execution
  debug() << "==> Execute" << endmsg;

  // get MCHits
  MCHits* mcHits = get<MCHits>( m_inputData );

  // typedef
  typedef Relation1D<MCParticle, MCHit> LocalDirectType;

  // create an association table
  LocalDirectType* table = new LocalDirectType();

  // loop over MCHits
  for(MCHits::const_iterator itHit = mcHits->begin();
      itHit != mcHits->end(); ++itHit) {
    // retrieve MCHit
    MCHit* mcHit = *itHit ;
    if (!mcHit) {
      return Error( "Failed retrieving MCHit" );
    }
    
    // retrieve associated MCParticle
    const MCParticle* mcPart = mcHit->mcParticle();

    // relate in table 
    table->relate(mcPart, mcHit);
  }
  
  // Register the table on the TES
  put( table, outputData() );
  
  // Produce also the Linker table if requested
  if ( m_makeLinker ) {
    std::string linkPath = outputData();
    if ( "/Event/Relations/" == linkPath.substr(0,17) )
      linkPath = "Link/" + linkPath.substr(17);
    else if ( "Relations/" == linkPath.substr(0,10) )
      linkPath = "Link/" + linkPath.substr(10);
    LinkerWithKey<MCParticle,MCHit> myLink( eventSvc(), msgSvc(), linkPath );
    LocalDirectType::Range ran = table->relations();
    for( LocalDirectType::Range::const_iterator it = ran.begin();
         it != ran.end(); ++it ) {
      myLink.link( it->to(), it->from(), 1. );
    }
    debug() << "Linker table stored at " << linkPath << endreq;
  }
  
  return StatusCode::SUCCESS ;
};
