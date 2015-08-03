#include "Event/TwoProngVertex.h"
#include "Event/PackedTwoProngVertex.h"
#include "Event/StandardPacker.h"
// local
#include "PackTwoProngVertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PackTwoProngVertex
//
// 2009-01-21 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PackTwoProngVertex )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  PackTwoProngVertex::PackTwoProngVertex( const std::string& name,
                                          ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName",  m_inputName  = LHCb::TwoProngVertexLocation::Default );
  declareProperty( "OutputName", m_outputName = LHCb::PackedTwoProngVertexLocation::Default );
  declareProperty( "AlwaysCreateOutput",         m_alwaysOutput = false     );
}

//=============================================================================
// Destructor
//=============================================================================
PackTwoProngVertex::~PackTwoProngVertex() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PackTwoProngVertex::execute()
{

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // If input does not exist, and we aren't making the output regardless, just return
  if ( !m_alwaysOutput && !exist<LHCb::TwoProngVertices>(m_inputName) ) return StatusCode::SUCCESS;

  LHCb::TwoProngVertices* verts = getOrCreate<LHCb::TwoProngVertices,LHCb::TwoProngVertices>( m_inputName );
  LHCb::PackedTwoProngVertices* out = new LHCb::PackedTwoProngVertices();
  out->vertices().reserve(verts->size());
  put( out, m_outputName );
  out->setVersion( 1 );

  // packing version
  const char pVer = LHCb::PackedTwoProngVertices::defaultPackingVersion();
  out->setPackingVersion(pVer);

  StandardPacker pack(this);

  for ( const LHCb::TwoProngVertex* vert : *verts )
  {
  
    // save the packed vertex
    out->vertices().push_back( LHCb::PackedTwoProngVertex() );
    LHCb::PackedTwoProngVertex & pVert = out->vertices().back();

    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "Found vertex key " << vert->key() << endmsg;

    pVert.key        = vert->key();
    pVert.technique  = vert->technique();
    pVert.chi2       = pack.fltPacked( vert->chi2() );
    pVert.nDoF       = vert->nDoF();
    pVert.x          = pack.position( vert->position().x() );
    pVert.y          = pack.position( vert->position().y() );
    pVert.z          = pack.position( vert->position().z() );

    double pA = 1./vert->momA()(2);
    double pB = 1./vert->momB()(2);

    pVert.txA        = pack.slope( vert->momA()(0) );
    pVert.tyA        = pack.slope( vert->momA()(1) );
    pVert.pA         = pack.energy( pA );

    pVert.txB        = pack.slope( vert->momB()(0) );
    pVert.tyB        = pack.slope( vert->momB()(1) );
    pVert.pB         = pack.energy( pB );

    // convariance Matrix
    double err0 = std::sqrt( vert->covMatrix()(0,0) );
    double err1 = std::sqrt( vert->covMatrix()(1,1) );
    double err2 = std::sqrt( vert->covMatrix()(2,2) );
    double err3 = std::sqrt( vert->momcovA()(0,0) );
    double err4 = std::sqrt( vert->momcovA()(1,1) );
    double err5 = std::sqrt( vert->momcovA()(2,2) );
    double err6 = std::sqrt( vert->momcovB()(0,0) );
    double err7 = std::sqrt( vert->momcovB()(1,1) );
    double err8 = std::sqrt( vert->momcovB()(2,2) );

    pVert.cov00 = pack.position(  err0 );
    pVert.cov11 = pack.position(  err1 );
    pVert.cov22 = pack.position(  err2 );
    pVert.cov33 = pack.slope(     err3 );
    pVert.cov44 = pack.slope(     err4 );
    pVert.cov55 = pack.energy(    err5 * fabs( pA ) * 1.e5 );
    pVert.cov66 = pack.slope(     err6 );
    pVert.cov77 = pack.slope(     err7 );
    pVert.cov88 = pack.energy(    err8 * fabs( pB ) * 1.e5 );

    pVert.cov10 = pack.fraction( vert->covMatrix()(1,0)/err1/err0 );

    pVert.cov20 = pack.fraction( vert->covMatrix()(2,0)/err2/err0 );
    pVert.cov21 = pack.fraction( vert->covMatrix()(2,1)/err2/err1 );

    pVert.cov30 = pack.fraction( vert->momposcovA()(0,0)/err3/err0 );
    pVert.cov31 = pack.fraction( vert->momposcovA()(0,1)/err3/err1 );
    pVert.cov32 = pack.fraction( vert->momposcovA()(0,2)/err3/err2 );

    pVert.cov40 = pack.fraction( vert->momposcovA()(1,0)/err4/err0 );
    pVert.cov41 = pack.fraction( vert->momposcovA()(1,1)/err4/err1 );
    pVert.cov42 = pack.fraction( vert->momposcovA()(1,2)/err4/err2 );
    pVert.cov43 = pack.fraction( vert->momcovA()   (1,0)/err4/err3 );

    pVert.cov50 = pack.fraction( vert->momposcovA()(2,0)/err5/err0 );
    pVert.cov51 = pack.fraction( vert->momposcovA()(2,1)/err5/err1 );
    pVert.cov52 = pack.fraction( vert->momposcovA()(2,2)/err5/err2 );
    pVert.cov53 = pack.fraction( vert->momcovA()   (2,0)/err5/err3 );
    pVert.cov54 = pack.fraction( vert->momcovA()   (2,1)/err5/err4 );

    pVert.cov60 = pack.fraction( vert->momposcovB()(0,0)/err6/err0 );
    pVert.cov61 = pack.fraction( vert->momposcovB()(0,1)/err6/err1 );
    pVert.cov62 = pack.fraction( vert->momposcovB()(0,2)/err6/err2 );
    pVert.cov63 = pack.fraction( vert->mommomcov() (0,0)/err6/err3 );
    pVert.cov64 = pack.fraction( vert->mommomcov() (0,1)/err6/err4 );
    pVert.cov65 = pack.fraction( vert->mommomcov() (0,2)/err6/err5 );

    pVert.cov70 = pack.fraction( vert->momposcovB()(1,0)/err7/err0 );
    pVert.cov71 = pack.fraction( vert->momposcovB()(1,1)/err7/err1 );
    pVert.cov72 = pack.fraction( vert->momposcovB()(1,2)/err7/err2 );
    pVert.cov73 = pack.fraction( vert->mommomcov() (1,0)/err7/err3 );
    pVert.cov74 = pack.fraction( vert->mommomcov() (1,1)/err7/err4 );
    pVert.cov75 = pack.fraction( vert->mommomcov() (1,2)/err7/err5 );
    pVert.cov76 = pack.fraction( vert->momcovB()   (1,0)/err7/err6 );

    pVert.cov80 = pack.fraction( vert->momposcovB()(2,0)/err8/err0 );
    pVert.cov81 = pack.fraction( vert->momposcovB()(2,1)/err8/err1 );
    pVert.cov82 = pack.fraction( vert->momposcovB()(2,2)/err8/err2 );
    pVert.cov83 = pack.fraction( vert->mommomcov() (2,0)/err8/err3 );
    pVert.cov84 = pack.fraction( vert->mommomcov() (2,1)/err8/err4 );
    pVert.cov85 = pack.fraction( vert->mommomcov() (2,2)/err8/err5 );
    pVert.cov86 = pack.fraction( vert->momcovB()   (2,0)/err8/err6 );
    pVert.cov87 = pack.fraction( vert->momcovB()   (2,1)/err8/err7 );

    //== Store the Tracks
    pVert.firstTrack = out->refs().size();
    for ( SmartRefVector<LHCb::Track>::const_iterator itT = vert->tracks().begin();
          vert->tracks().end() != itT; ++itT ) 
    {
      out->refs().push_back( 0==pVer ? 
                             pack.reference32( out, (*itT)->parent(), (*itT)->key() ) :
                             pack.reference64( out, (*itT)->parent(), (*itT)->key() ) );
    }
    pVert.lastTrack = out->refs().size();

    //== Store the ParticleID
    pVert.firstPid = out->refs().size();
    for ( std::vector<LHCb::ParticleID>::const_iterator itP = vert->compatiblePIDs().begin();
          vert->compatiblePIDs().end() != itP; ++itP ) 
    {
      out->refs().push_back( (*itP).pid() );
    }
    pVert.lastPid = out->refs().size();

    //== Handles the ExtraInfo
    pVert.firstInfo = out->extras().size();
    for ( GaudiUtils::VectorMap<int,double>::iterator itE = vert->extraInfo().begin();
          vert->extraInfo().end() != itE; ++itE ) {
      out->extras().push_back( std::make_pair((*itE).first,pack.fltPacked((*itE).second)) );
    }
    pVert.lastInfo = out->extras().size();

  }

  return StatusCode::SUCCESS;
}

//=============================================================================
