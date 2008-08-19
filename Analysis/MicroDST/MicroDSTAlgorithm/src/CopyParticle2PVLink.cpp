// $Id: CopyParticle2PVLink.cpp,v 1.9 2008-08-19 19:33:34 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// from LHCb
#include "Event/RecVertex.h"
#include "Event/Particle.h"

// local
#include "CopyParticle2PVLink.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CopyParticle2PVLink
//
// 2007-10-15 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CopyParticle2PVLink );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CopyParticle2PVLink::CopyParticle2PVLink( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : 
  MicroDSTAlgorithm ( name , pSvcLocator )
{
}
//=============================================================================
// Destructor
//=============================================================================
CopyParticle2PVLink::~CopyParticle2PVLink() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CopyParticle2PVLink::initialize() {

  StatusCode sc = MicroDSTAlgorithm::initialize(); // must be executed first

  debug() << "==> Initialize" << endmsg;

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  verbose() << "inputTESLocation() is " << inputTESLocation() << endmsg;

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode CopyParticle2PVLink::execute() {

  debug() << "==> Execute" << endmsg;

  verbose() << "Checking input location " << inputTESLocation() << endmsg;
  
  setFilterPassed(true);

  if (exist<Particle2Vertex::Table>(inputTESLocation()) )
  {
    verbose() << "Retrieving relations table from " 
              << inputTESLocation() << endmsg;
    const Particle2Vertex::Table* table = get<Particle2Vertex::Table>(inputTESLocation());
    if (table) {
      verbose() << "found table!" << endmsg;
      Particle2Vertex::Table* cloneTable = tableOfClones(table);
       verbose() << "Going to store relations table from " << inputTESLocation()
                << " into " << fullOutputTESLocation() << endmsg;
      put( cloneTable, fullOutputTESLocation() );
      return StatusCode::SUCCESS;
    }
    return StatusCode::FAILURE;
  } else {
    verbose() << "Found no table at " << inputTESLocation() << ". storing empty table"<< endmsg;
    Particle2Vertex::Table* cloneTable = new Particle2Vertex::Table();
    put( cloneTable, fullOutputTESLocation() );
    return StatusCode::SUCCESS;
  }

}
//=============================================================================
Particle2Vertex::Table* CopyParticle2PVLink::tableOfClones(const Particle2Vertex::Table* table) 
{

  Particle2Vertex::Table* cloneTable = new Particle2Vertex::Table();

  Particle2Vertex::Range relations = table->relations();
  verbose() << "Found " << relations.size() 
            << " Particle -> PV relations" << endmsg;

  for (Particle2Vertex::Range::const_iterator iRel = relations.begin();
       iRel != relations.end();
       ++iRel ) {
    const LHCb::Particle* part = iRel->from();
    if (part) {
      Particle2Vertex::Range range = table->relations(part);
      for (Particle2Vertex::Range::const_iterator iRange = range.begin();
           iRange != range.end();
           ++iRange) {
        verbose() << "\nParticle with PID "    
                  << part->particleID().pid() 
                  << " is related to PV at " << (iRange->to())->position()
                  << " #tracks "             
                  << (dynamic_cast<const LHCb::RecVertex*>(iRange->to()))->tracks().size() 
        
                  << " with weight "         << iRange->weight() << endmsg;
        const LHCb::Particle* clonedParticle = 
          getStoredClone<LHCb::Particle>(part);
        const LHCb::RecVertex* clonedVertex = 
          getStoredClone<LHCb::RecVertex>(dynamic_cast<LHCb::RecVertex*>( iRange->to()) );
        if (clonedParticle&&clonedVertex) {
          cloneTable->relate(clonedParticle, clonedVertex, iRange->weight());
          verbose() << "\nclonedParticle with PID "    
                    << clonedParticle->particleID().pid() 
                    << " is related to cloned PV at " << clonedVertex->position()
                    << " #tracks "             
                    << clonedVertex->tracks().size()
                    << " with weight "         << iRange->weight() << endmsg;
        } // if cloned particle AND cloned vertex
      } // loop on related vertices
    } // if From Particle* found
  } // loop on all relations
  
  return cloneTable;

}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode CopyParticle2PVLink::finalize() {

  debug() << "==> Finalize" << endmsg;

  return MicroDSTAlgorithm::finalize();  // must be called after all other actions
}
//=============================================================================
