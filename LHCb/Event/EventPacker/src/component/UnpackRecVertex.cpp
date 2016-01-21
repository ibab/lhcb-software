// Include files 

// from Gaudi
#include "Event/RecVertex.h"
#include "Event/PackedRecVertex.h"
#include "Event/WeightsVector.h"
#include "Event/StandardPacker.h"
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

  const auto * dst = getOrCreate<LHCb::PackedRecVertices,LHCb::PackedRecVertices>(m_inputName);

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Size of PackedRecVertices = " << dst->vertices().size() << endmsg;

  auto * newRecVertices = new LHCb::RecVertices();
  newRecVertices->reserve(dst->vertices().size());
  put( newRecVertices, m_outputName );

  const LHCb::RecVertexPacker rvPacker(*dynamic_cast<GaudiAlgorithm*>(this));

  for ( const auto & src : dst->vertices() )
  {
    auto * vert = new LHCb::RecVertex( );
    newRecVertices->insert( vert, src.key );
    rvPacker.unpack( src, *vert, *dst, *newRecVertices );
  }

  // If the packed version is < 2, we need to manually load the weights vectors and update ...
  if ( (int)dst->version() < 2 )
  {
    const auto * weightsV = getIfExists<LHCb::WeightsVectors>(m_weightsLoc);
    if ( weightsV )
    {
      if ( msgLevel(MSG::DEBUG) )
        debug() << "Old pre-Weights PVs found. Will load weights vector and update"
                << endmsg;

      // loop over PVs and load the weights for each
      for ( auto * RV : *newRecVertices )
      {
        const auto * weights = weightsV->object(RV->key());
        if ( weights )
        {

          // build a map with the new weights for each track
          std::map<const LHCb::Track*,float> trksWeights;
          for ( const auto & tk : RV->tracks() )
          {
            // Find the weight for this track
            float wgt = 1.0;
            for ( const auto & w : weights->weights() )
            {
              if ( w.first == tk->key() )
              {
                wgt = w.second;
                break;
              }
            }
            // save the weight in the map
            trksWeights[tk] = wgt;
          }

          // Clear all existing tracks in the PV
          RV->clearTracks();

          // Add the tracks back, with the new weights
          for ( const auto & TW : trksWeights )
          {
            if ( msgLevel(MSG::DEBUG) )
              debug() << " -> Setting Track " << TW.first->key() << " weight " << TW.second
                      << endmsg;
            RV->addToTracks( TW.first, TW.second );
          }
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
