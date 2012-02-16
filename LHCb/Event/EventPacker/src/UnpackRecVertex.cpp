// $Id: UnpackRecVertex.cpp,v 1.5 2009-11-07 12:20:39 jonrob Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "Event/RecVertex.h"
#include "Event/PackedRecVertex.h" 
#include "Kernel/StandardPacker.h"
// local
#include "UnpackRecVertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : UnpackRecVertex
//
// 2008-11-17 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( UnpackRecVertex )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
UnpackRecVertex::UnpackRecVertex( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::PackedRecVertexLocation::Primary );
  declareProperty( "OutputName", m_outputName = LHCb::RecVertexLocation::Primary );
  declareProperty( "AlwaysCreateOutput",         m_alwaysOutput = false     );
}
//=============================================================================
// Destructor
//=============================================================================
UnpackRecVertex::~UnpackRecVertex() {} 

//=============================================================================
// Main execution
//=============================================================================
StatusCode UnpackRecVertex::execute() 
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // If input does not exist, and we aren't making the output regardless, just return
  if ( !m_alwaysOutput && !exist<LHCb::PackedRecVertices>(m_inputName) ) return StatusCode::SUCCESS;

  const LHCb::PackedRecVertices* dst = 
    getOrCreate<LHCb::PackedRecVertices,LHCb::PackedRecVertices>( m_inputName );

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Size of PackedRecVertices = " << dst->end() - dst->begin() << endmsg;

  LHCb::RecVertices* newRecVertices = new LHCb::RecVertices();
  newRecVertices->reserve(dst->vertices().size());
  put( newRecVertices, m_outputName );

  static const LHCb::RecVertexPacker rvPacker;
  
  for ( std::vector<LHCb::PackedRecVertex>::const_iterator itS = dst->begin();
        dst->end() != itS; ++itS ) 
  {
    const LHCb::PackedRecVertex& src = (*itS);

    LHCb::RecVertex* vert = new LHCb::RecVertex( );
    newRecVertices->insert( vert, src.key );

    rvPacker.unpack( src, *vert, *dst, *newRecVertices );
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
