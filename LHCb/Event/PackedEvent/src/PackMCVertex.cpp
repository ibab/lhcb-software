// $Id: PackMCVertex.cpp,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/MCVertex.h"
#include "Kernel/StandardPacker.h"
#include "Event/PackedMCVertex.h"
// local
#include "PackMCVertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PackMCVertex
//
// 2005-03-18 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<PackMCVertex>          s_factory ;
const        IAlgFactory& PackMCVertexFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PackMCVertex::PackMCVertex( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = MCVertexLocation::Default );
  declareProperty( "OutputName", m_outputName = PackedMCVertexLocation::Default );
}
//=============================================================================
// Destructor
//=============================================================================
PackMCVertex::~PackMCVertex() {}; 

//=============================================================================
// Main execution
//=============================================================================
StatusCode PackMCVertex::execute() {

  debug() << "==> Execute" << endmsg;

  MCVertices* verts = get<MCVertices>( m_inputName );
  debug() << m_inputName << " contains " << verts->size()
          << " MCVertices to convert." << endreq;
  
  StandardPacker pack;
  
  PackedMCVertices* out = new PackedMCVertices();
  put( out, m_outputName );

  for ( MCVertices::const_iterator itV = verts->begin();
        verts->end() != itV; ++itV ) {
    MCVertex* vert = (*itV);
    PackedMCVertex newVert;

    newVert.key  = vert->key();
    newVert.x    = pack.position( vert->position().x() );
    newVert.y    = pack.position( vert->position().y() );
    newVert.z    = pack.position( vert->position().z() );
    newVert.tof  = vert->timeOfFlight();   // What scale ?
    newVert.type = vert->type();

    newVert.mother = -1;
    if ( 0 != vert->mother() ) {
      newVert.mother = pack.reference( out, vert->mother()->parent(), 
                                       vert->mother()->key() );
    }
    for ( SmartRefVector<MCParticle>::const_iterator itP = vert->products().begin();
          vert->products().end() != itP; ++itP ) {
      int ref = pack.reference( out, (*itP)->parent(), (*itP)->key() );
      newVert.products.push_back( ref );
    }
    if ( 0 != vert->collision() ) {
      newVert.collision = pack.reference( out, 
                                          vert->collision()->parent(), 
                                          vert->collision()->key() );
    } else {
      newVert.collision = -1;
    }    
    out->addEntry( newVert );
  }

  // Clear the registry address of the unpacked container, to prevent reloading
  verts->registry()->setAddress( 0 );
  
  return StatusCode::SUCCESS;
};

//=============================================================================
