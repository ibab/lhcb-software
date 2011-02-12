// $Id: PackTwoProngVertex.cpp,v 1.5 2009-11-07 12:20:39 jonrob Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "Event/TwoProngVertex.h"
#include "Event/PackedTwoProngVertex.h"
#include "Kernel/StandardPacker.h"
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
StatusCode PackTwoProngVertex::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // If input does not exist, and we aren't making the output regardless, just return
  if ( !m_alwaysOutput && !exist<LHCb::TwoProngVertices>(m_inputName) ) return StatusCode::SUCCESS;

  LHCb::TwoProngVertices* verts = getOrCreate<LHCb::TwoProngVertices,LHCb::TwoProngVertices>( m_inputName );
  LHCb::PackedTwoProngVertices* out = new LHCb::PackedTwoProngVertices();
  out->vertices().reserve(verts->size());
  put( out, m_outputName );
  out->setVersion( 1 );

  StandardPacker pack;
  for (  LHCb::TwoProngVertices::const_iterator itV = verts->begin();
         verts->end() != itV ; ++itV )
  {
    LHCb::TwoProngVertex*      vert = *itV;
    LHCb::PackedTwoProngVertex pVert;
    debug() << "Found vertex key " << (*itV)->key() << endmsg;
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

    /*
      info() << format( "cov %6d", pVert.cov00 ) << endmsg;
      info() << format( "cov %6d %6d", pVert.cov10, pVert.cov11 ) << endmsg;
      info() << format( "cov %6d %6d %6d", pVert.cov20, pVert.cov21, pVert.cov22 ) << endmsg;
      info() << format( "cov %6d %6d %6d %6d", pVert.cov30, pVert.cov31, pVert.cov32, pVert.cov33 ) << endmsg;
      info() << format( "cov %6d %6d %6d %6d %6d",
      pVert.cov40, pVert.cov41, pVert.cov42, pVert.cov43, pVert.cov44 ) << endmsg;
      info() << format( "cov %6d %6d %6d %6d %6d %6d",
      pVert.cov50, pVert.cov51, pVert.cov52, pVert.cov53, pVert.cov54, pVert.cov55 ) << endmsg;
      info() << format( "cov %6d %6d %6d %6d %6d %6d %6d",
      pVert.cov60, pVert.cov61, pVert.cov62, pVert.cov63, pVert.cov64, pVert.cov65, pVert.cov66 ) << endmsg;
      info() << format( "cov %6d %6d %6d %6d %6d %6d %6d %6d",
      pVert.cov70, pVert.cov71, pVert.cov72, pVert.cov73, pVert.cov74, pVert.cov75,
      pVert.cov76, pVert.cov77 ) << endmsg;
      info() << format( "cov %6d %6d %6d %6d %6d %6d %6d %6d %6d",
      pVert.cov80, pVert.cov81, pVert.cov82, pVert.cov83, pVert.cov84, pVert.cov85,
      pVert.cov86, pVert.cov87, pVert.cov88 ) << endmsg;
    */

    //== Store the Tracks
    pVert.firstTrack = out->sizeRefs();
    for ( SmartRefVector<LHCb::Track>::const_iterator itT = vert->tracks().begin();
          vert->tracks().end() != itT; ++itT ) {
      int myRef = pack.reference( out, (*itT)->parent(), (*itT)->key() );
      out->addRef( myRef );
    }
    pVert.lastTrack = out->sizeRefs();

    //== Store the ParticleID
    pVert.firstPid = out->sizeRefs();
    for ( std::vector<LHCb::ParticleID>::const_iterator itP = vert->compatiblePIDs().begin();
          vert->compatiblePIDs().end() != itP; ++itP ) {
      out->addRef( (*itP).pid() );
    }
    pVert.lastPid = out->sizeRefs();

    //== Handles the ExtraInfo
    pVert.firstInfo = out->sizeExtra();
    for ( GaudiUtils::VectorMap<int,double>::iterator itE = vert->extraInfo().begin();
          vert->extraInfo().end() != itE; ++itE ) {
      out->addExtra( (*itE).first, pack.fltPacked( (*itE).second ) );
    }
    pVert.lastInfo = out->sizeExtra();
    out->addEntry( pVert );
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
