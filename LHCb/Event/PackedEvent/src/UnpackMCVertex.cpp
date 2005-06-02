// $Id: UnpackMCVertex.cpp,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/MCVertex.h"
#include "Kernel/StandardPacker.h"
#include "Event/PackedMCVertex.h"
// local
#include "UnpackMCVertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : UnpackMCVertex
//
// 2005-03-18 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<UnpackMCVertex>          s_factory ;
const        IAlgFactory& UnpackMCVertexFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
UnpackMCVertex::UnpackMCVertex( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = PackedMCVertexLocation::Default );
  declareProperty( "OutputName", m_outputName = MCVertexLocation::Default );
}
//=============================================================================
// Destructor
//=============================================================================
UnpackMCVertex::~UnpackMCVertex() {}; 

//=============================================================================
// Main execution
//=============================================================================
StatusCode UnpackMCVertex::execute() {

  debug() << "==> Execute" << endmsg;

  PackedMCVertices* dst = get<PackedMCVertices>( m_inputName );
  debug() << "Size of PackedMCVertices = " << dst->end() - dst->begin() << endreq;

  MCVertices* newMCVertices = new MCVertices();
  put( newMCVertices, m_outputName );

  StandardPacker pack;
  
  for ( std::vector<PackedMCVertex>::const_iterator itS = dst->begin();
        dst->end() != itS; ++itS ) {
    const PackedMCVertex& src = (*itS);

    MCVertex* vert = new MCVertex( );
    newMCVertices->insert( vert, src.key );
    HepPoint3D pt( pack.position( src.x ), pack.position( src.y ), pack.position( src.z) );
    vert->setPosition( pt );
    vert->setTimeOfFlight( src.tof );
    vert->setType( (MCVertex::MCVertexType) src.type );

    int hintID;
    int key;
    if ( -1 != src.mother ) {
      pack.hintAndKey( src.mother, dst, newMCVertices, hintID, key );
      SmartRef<MCParticle> ref( newMCVertices, hintID, key );
      vert->setMother( ref );
    }
    
    std::vector<int>::const_iterator itI;
    for ( itI = src.products.begin() ; src.products.end() != itI ; ++itI ) {
      pack.hintAndKey( *itI, dst, newMCVertices, hintID, key );
      SmartRef<MCParticle> ref( newMCVertices, hintID, key );
      vert->addToProducts( ref );
    }
    if ( -1 != src.collision ) {
      pack.hintAndKey( src.collision, dst, newMCVertices, hintID, key );
      SmartRef<Collision> refc( newMCVertices, hintID, key );
      vert->setCollision( refc );
    }
  }
  
  return StatusCode::SUCCESS;
};

//=============================================================================
