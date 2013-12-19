// $Id: UnpackMCVertex.cpp,v 1.5 2009-11-07 12:20:39 jonrob Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/MCVertex.h"
#include "Event/StandardPacker.h"
#include "Event/PackedMCVertex.h"
// local
#include "UnpackMCVertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : UnpackMCVertex
//
// 2005-03-18 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( UnpackMCVertex )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
UnpackMCVertex::UnpackMCVertex( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::PackedMCVertexLocation::Default );
  declareProperty( "OutputName", m_outputName = LHCb::MCVertexLocation::Default );
  declareProperty( "AlwaysCreateOutput",         m_alwaysOutput = false     );
  //setProperty( "OutputLevel", 1 );
}
//=============================================================================
// Destructor
//=============================================================================
UnpackMCVertex::~UnpackMCVertex() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode UnpackMCVertex::execute() {

  // CRJ : If packed data does not exist just return (by default). Needed for packing of 
  //     : spillover which is not neccessarily available for each event
  if ( !m_alwaysOutput && !exist<LHCb::PackedMCVertices>(m_inputName) ) return StatusCode::SUCCESS;

  const LHCb::PackedMCVertices* dst = 
    getOrCreate<LHCb::PackedMCVertices,LHCb::PackedMCVertices>( m_inputName );

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Size of PackedMCVertices = " << dst->end() - dst->begin() << endmsg;

  LHCb::MCVertices* newMCVertices = new LHCb::MCVertices();
  put( newMCVertices, m_outputName );

  StandardPacker pack;
  
  newMCVertices->reserve( dst->size() );
  for ( std::vector<LHCb::PackedMCVertex>::const_iterator itS = dst->begin();
        dst->end() != itS; ++itS ) {
    const LHCb::PackedMCVertex& src = (*itS);

    LHCb::MCVertex* vert = new LHCb::MCVertex( );
    newMCVertices->insert( vert, src.key );
    const Gaudi::XYZPoint pt( pack.position( src.x ), pack.position( src.y ), pack.position( src.z) );
    vert->setPosition( pt );
    vert->setTime( src.tof );
    vert->setType( (LHCb::MCVertex::MCVertexType) src.type );

    int hintID;
    int key;
    if ( -1 != src.mother ) {
      pack.hintAndKey( src.mother, dst, newMCVertices, hintID, key );
      SmartRef<LHCb::MCParticle> ref( newMCVertices, hintID, key );
      vert->setMother( ref );
    }
    
    std::vector<int>::const_iterator itI;
    for ( itI = src.products.begin() ; src.products.end() != itI ; ++itI ) {
      pack.hintAndKey( *itI, dst, newMCVertices, hintID, key );
      SmartRef<LHCb::MCParticle> ref( newMCVertices, hintID, key );
      vert->addToProducts( ref );
    }
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
