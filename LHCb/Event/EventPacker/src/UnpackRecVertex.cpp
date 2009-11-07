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
DECLARE_ALGORITHM_FACTORY( UnpackRecVertex );


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
StatusCode UnpackRecVertex::execute() {

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

  StandardPacker pack;
  
  for ( std::vector<LHCb::PackedRecVertex>::const_iterator itS = dst->begin();
        dst->end() != itS; ++itS ) {
    const LHCb::PackedRecVertex& src = (*itS);

    LHCb::RecVertex* vert = new LHCb::RecVertex( );
    newRecVertices->insert( vert, src.key );

    vert->setTechnique( (LHCb::RecVertex::RecVertexType) src.technique );
    vert->setChi2AndDoF( pack.fltPacked( src.chi2), src.nDoF );
    Gaudi::XYZPoint pos( pack.position( src.x ), pack.position( src.y ), pack.position( src.z ) );
    vert->setPosition( pos );

    // convariance Matrix
    double err0 = pack.position( src.cov00 );
    double err1 = pack.position( src.cov11 );
    double err2 = pack.position( src.cov22 );
    Gaudi::SymMatrix3x3  cov;
    cov(0,0) = err0 * err0;
    cov(1,0) = err1 * err0 * pack.fraction( src.cov10 );
    cov(1,1) = err1 * err1;
    cov(2,0) = err2 * err0 * pack.fraction( src.cov20 );
    cov(2,1) = err2 * err1 * pack.fraction( src.cov21 );
    cov(2,2) = err2 * err2;
    vert->setCovMatrix( cov );

    //== Store the Tracks
    int kk;
    int hintID;
    int key;
    for ( kk = src.firstTrack; src.lastTrack > kk; ++kk ) {
      int trk = *(dst->beginRefs()+kk);
      pack.hintAndKey( trk, dst, newRecVertices, hintID, key );
      SmartRef<LHCb::Track> ref( newRecVertices, hintID, key );
      vert->addToTracks( ref );
    }

    //== Handles the ExtraInfo
    for ( int kEx = src.firstInfo; src.lastInfo > kEx; ++kEx ) {
      std::pair<int,int> info = *(dst->beginExtra()+kEx);
      vert->addInfo( info.first, pack.fltPacked( info.second ) );
    }
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
