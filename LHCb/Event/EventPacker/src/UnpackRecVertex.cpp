// $Id: UnpackRecVertex.cpp,v 1.5 2009-11-07 12:20:39 jonrob Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "Event/RecVertex.h"
#include "Event/PackedRecVertex.h" 
#include "Event/WeightsVector.h"
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
  declareProperty( "WeightsVector", m_weightsLoc = LHCb::WeightsVectorLocation::Default );
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
  if ( !m_alwaysOutput && !exist<LHCb::PackedRecVertices>(m_inputName) ) 
    return StatusCode::SUCCESS;

  const LHCb::PackedRecVertices* dst = 
    getOrCreate<LHCb::PackedRecVertices,LHCb::PackedRecVertices>( m_inputName );

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Size of PackedRecVertices = " << dst->vertices().size() << endmsg;

  LHCb::RecVertices* newRecVertices = new LHCb::RecVertices();
  newRecVertices->reserve(dst->vertices().size());
  put( newRecVertices, m_outputName );

  const LHCb::RecVertexPacker rvPacker(*dynamic_cast<GaudiAlgorithm*>(this));
  
  for ( std::vector<LHCb::PackedRecVertex>::const_iterator itS = dst->vertices().begin();
        dst->vertices().end() != itS; ++itS ) 
  {
    const LHCb::PackedRecVertex& src = (*itS);

    LHCb::RecVertex* vert = new LHCb::RecVertex( );
    newRecVertices->insert( vert, src.key );

    rvPacker.unpack( src, *vert, *dst, *newRecVertices );
  }

  // If the packed version is < 2, we need to manually load the weights vectors and update ...
  if ( (int)dst->version() < 2 && 
       exist<LHCb::WeightsVectors>(m_weightsLoc) )
  {
    const LHCb::WeightsVectors * weightsV = get<LHCb::WeightsVectors>(m_weightsLoc);

    // loop over PVs and load the weights for each
    for ( LHCb::RecVertices::iterator iRV = newRecVertices->begin();
          iRV != newRecVertices->end(); ++iRV )
    {
      const LHCb::WeightsVector * weights = weightsV->object((*iRV)->key());
      if ( weights )
      {
        std::map<const LHCb::Track*,float> trksWeights;
        for ( SmartRefVector<LHCb::Track>::const_iterator iTk = (*iRV)->tracks().begin();
              iTk != (*iRV)->tracks().end(); ++iTk )
        {
          float wgt = 1.0;
          // Find the weight for this track
          for ( std::vector<std::pair<int,float> >::const_iterator iWW = weights->weights().begin();
                iWW != weights->weights().end(); ++iWW )
          {
            if ( iWW->first == (*iTk)->key() )
            {
              wgt = iWW->second; 
              break;
            }
          }
          trksWeights[*iTk] = wgt;
        }
        (*iRV)->clearTracks();
        for ( std::map<const LHCb::Track*,float>::const_iterator iTW = trksWeights.begin();
              iTW != trksWeights.end(); ++iTW )
        {
          (*iRV)->addToTracks( iTW->first, iTW->second );
        }
      }
    }

  }

  return StatusCode::SUCCESS;
}

//=============================================================================
