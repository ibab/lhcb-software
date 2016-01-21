
#include "Event/PackedTwoProngVertex.h"
#include "Event/TwoProngVertex.h"
#include "Event/StandardPacker.h"
// local
#include "UnpackTwoProngVertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : UnpackTwoProngVertex
//
// 2009-01-21 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( UnpackTwoProngVertex )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
UnpackTwoProngVertex::UnpackTwoProngVertex( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::PackedTwoProngVertexLocation::Default );
  declareProperty( "OutputName", m_outputName = LHCb::TwoProngVertexLocation::Default );
  declareProperty( "AlwaysCreateOutput",      m_alwaysOutput = false     );
}
//=============================================================================
// Destructor
//=============================================================================
UnpackTwoProngVertex::~UnpackTwoProngVertex() {} 

//=============================================================================
// Main execution
//=============================================================================
StatusCode UnpackTwoProngVertex::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // If input does not exist, and we aren't making the output regardless, just return
  if ( !m_alwaysOutput && !exist<LHCb::PackedTwoProngVertices>(m_inputName) )
    return StatusCode::SUCCESS;

  const LHCb::PackedTwoProngVertices* dst = 
    getOrCreate<LHCb::PackedTwoProngVertices,LHCb::PackedTwoProngVertices>( m_inputName );
  
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Size of PackedTwoProngVertices = " << dst->vertices().size() << endmsg;

  auto* newTwoProngVertices = new LHCb::TwoProngVertices();
  newTwoProngVertices->reserve(dst->vertices().size());
  put( newTwoProngVertices, m_outputName );

  // packing version
  const char pVer = dst->packingVersion();

  StandardPacker pack(this);
  
  for ( const auto& src : dst->vertices() )
  {

    auto* vert = new LHCb::TwoProngVertex( );
    newTwoProngVertices->insert( vert, src.key );

    vert->setTechnique( (LHCb::RecVertex::RecVertexType) src.technique );
    vert->setChi2AndDoF( pack.fltPacked( src.chi2), src.nDoF );
    vert->setPosition( Gaudi::XYZPoint( pack.position( src.x ), pack.position( src.y ), pack.position( src.z ) ) );

    //== Store the Tracks
    int hintID;
    int key;
    for ( int kk = src.firstTrack; src.lastTrack > kk; ++kk ) 
    {
      const long long trk = *(dst->refs().begin()+kk);
      if ( ( 0==pVer && pack.hintAndKey32( trk, dst, newTwoProngVertices, hintID, key ) ) ||
           ( 0!=pVer && pack.hintAndKey64( trk, dst, newTwoProngVertices, hintID, key ) ) )
      {
        SmartRef<LHCb::Track> ref( newTwoProngVertices, hintID, key );
        vert->addToTracks( ref );
      }
      else { Error( "Corrupt TwoProngVertex Track SmartRef detected." ).ignore(); }
    }

    //== Handles the ExtraInfo
    for ( int kEx = src.firstInfo; src.lastInfo > kEx; ++kEx ) {
      const std::pair<int,int>& info = *(dst->extras().begin()+kEx);
      vert->addInfo( info.first, pack.fltPacked( info.second ) );
    }

    //== Momentum of the two prongs    
    double pA = pack.energy( src.pA );
    double pB = pack.energy( src.pB );
    vert->setMomA( ROOT::Math::SVector<double,3>( pack.slope( src.txA ), pack.slope( src.tyA ) , 1.0/pA ) );
    vert->setMomB( ROOT::Math::SVector<double,3>( pack.slope( src.txB ), pack.slope( src.tyB ) , 1.0/pB ) );

    // convariance Matrix
    double err0 = pack.position( src.cov00 );
    double err1 = pack.position( src.cov11 );
    double err2 = pack.position( src.cov22 );
    double err3 = pack.slope(    src.cov33 );
    double err4 = pack.slope(    src.cov44 );
    double err5 = pack.energy(   src.cov55 ) / fabs( pA ) * 1.e-5;
    double err6 = pack.slope(    src.cov66 );
    double err7 = pack.slope(    src.cov77 );
    double err8 = pack.energy(   src.cov88 ) / fabs( pB ) * 1.e-5;

    Gaudi::SymMatrix3x3 cov;
    cov(0,0) = err0 * err0;
    cov(1,0) = err1 * err0 * pack.fraction( src.cov10 );
    cov(1,1) = err1 * err1;
    cov(2,0) = err2 * err0 * pack.fraction( src.cov20 );
    cov(2,1) = err2 * err1 * pack.fraction( src.cov21 );
    cov(2,2) = err2 * err2;
    vert->setCovMatrix( cov );

    cov(0,0) = err3 * err3;
    cov(1,0) = err4 * err3 * pack.fraction( src.cov43 );
    cov(1,1) = err4 * err4;
    cov(2,0) = err5 * err3 * pack.fraction( src.cov53 );
    cov(2,1) = err5 * err4 * pack.fraction( src.cov54 );
    cov(2,2) = err5 * err5;
    vert->momcovA() = cov;

    cov(0,0) = err6 * err6;
    cov(1,0) = err7 * err6 * pack.fraction( src.cov76 );
    cov(1,1) = err7 * err7;
    cov(2,0) = err8 * err6 * pack.fraction( src.cov86 );
    cov(2,1) = err8 * err7 * pack.fraction( src.cov87 );
    cov(2,2) = err8 * err8;
    vert->momcovB() = cov;

    ROOT::Math::SMatrix<double,3,3> mat;
    mat(0,0) = err3 * err0 * pack.fraction( src.cov30 );
    mat(0,1) = err3 * err1 * pack.fraction( src.cov31 );
    mat(0,2) = err3 * err2 * pack.fraction( src.cov32 );
    mat(1,0) = err4 * err0 * pack.fraction( src.cov40 );
    mat(1,1) = err4 * err1 * pack.fraction( src.cov41 );
    mat(1,2) = err4 * err2 * pack.fraction( src.cov42 );
    mat(2,0) = err5 * err0 * pack.fraction( src.cov50 );
    mat(2,1) = err5 * err1 * pack.fraction( src.cov51 );
    mat(2,2) = err5 * err2 * pack.fraction( src.cov52 );
    vert->momposcovA() = mat;
    
    mat(0,0) = err6 * err0 * pack.fraction( src.cov60 );
    mat(0,1) = err6 * err1 * pack.fraction( src.cov61 );
    mat(0,2) = err6 * err2 * pack.fraction( src.cov62 );
    mat(1,0) = err7 * err0 * pack.fraction( src.cov70 );
    mat(1,1) = err7 * err1 * pack.fraction( src.cov71 );
    mat(1,2) = err7 * err2 * pack.fraction( src.cov72 );
    mat(2,0) = err8 * err0 * pack.fraction( src.cov80 );
    mat(2,1) = err8 * err1 * pack.fraction( src.cov81 );
    mat(2,2) = err8 * err2 * pack.fraction( src.cov82 );
    vert->momposcovB() = mat;
    
    mat(0,0) = err6 * err3 * pack.fraction( src.cov63 );
    mat(0,1) = err6 * err4 * pack.fraction( src.cov64 );
    mat(0,2) = err6 * err5 * pack.fraction( src.cov65 );
    mat(1,0) = err7 * err3 * pack.fraction( src.cov73 );
    mat(1,1) = err7 * err4 * pack.fraction( src.cov74 );
    mat(1,2) = err7 * err5 * pack.fraction( src.cov75 );
    mat(2,0) = err8 * err3 * pack.fraction( src.cov83 );
    mat(2,1) = err8 * err4 * pack.fraction( src.cov84 );
    mat(2,2) = err8 * err5 * pack.fraction( src.cov85 );
    vert->mommomcov() = mat;


    //== Unpack the ParticleID
    std::vector<LHCb::ParticleID> pids;
    for ( int kk = src.firstPid; src.lastPid > kk; ++kk )
    {
      pids.emplace_back( LHCb::ParticleID(*(dst->refs().begin()+kk)) );
    }
    vert->setCompatiblePIDs( pids );

  }

  return StatusCode::SUCCESS;
}
//=============================================================================
