// $Id: UnpackCaloHypo.cpp,v 1.2 2008-12-09 08:13:23 ocallot Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Kernel/StandardPacker.h"
#include "Event/PackedCaloHypo.h"
#include "Event/CaloHypo.h"

// local
#include "UnpackCaloHypo.h"

//-----------------------------------------------------------------------------
// Implementation file for class : UnpackCaloHypo
//
// 2008-11-14 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( UnpackCaloHypo );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
UnpackCaloHypo::UnpackCaloHypo( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::PackedCaloHypoLocation::Electrons );
  declareProperty( "OutputName", m_outputName = LHCb::CaloHypoLocation::Electrons );
}
//=============================================================================
// Destructor
//=============================================================================
UnpackCaloHypo::~UnpackCaloHypo() {} 

//=============================================================================
// Main execution
//=============================================================================
StatusCode UnpackCaloHypo::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  LHCb::PackedCaloHypos* dst = get<LHCb::PackedCaloHypos>( m_inputName );
  debug() << "Size of PackedCaloHypos = " << dst->end() - dst->begin() << endreq;

  LHCb::CaloHypos* newCaloHypos = new LHCb::CaloHypos();
  put( newCaloHypos, m_outputName );

  StandardPacker pack;
  
  for ( std::vector<LHCb::PackedCaloHypo>::const_iterator itS = dst->begin();
        dst->end() != itS; ++itS ) {
    const LHCb::PackedCaloHypo& src = (*itS);

    LHCb::CaloHypo* hypo = new LHCb::CaloHypo( );
    newCaloHypos->insert( hypo, src.key );
    hypo->setHypothesis( (LHCb::CaloHypo::Hypothesis) src.hypothesis );
    hypo->setLh( pack.fltPacked( src.lh ) );
    if ( 0. != src.z ) {
      LHCb::CaloPosition* pos = new LHCb::CaloPosition();
      hypo->setPosition( pos );
      pos->setZ( pack.position( src.z ) );
      LHCb::CaloPosition::Parameters params( pack.position( src.posX ), 
                                             pack.position( src.posY ),
                                             pack.energy( src.posE ) );
      pos->setParameters( params );
      
      double err0 = pack.position( src.cov00 );
      double err1 = pack.position( src.cov11 );
      double err2 = pack.energy( src.cov22 );
      LHCb::CaloPosition::Covariance cov;
      cov(0,0) = err0 * err0;
      cov(1,0) = err1 * err0 * pack.fraction( src.cov10 );
      cov(1,1) = err1 * err1;
      cov(2,0) = err2 * err0 * pack.fraction( src.cov20 );
      cov(2,1) = err2 * err1 * pack.fraction( src.cov21 );
      cov(2,2) = err2 * err2;
      pos->setCovariance( cov );
      
      LHCb::CaloPosition::Center cent( pack.position( src.centX ), pack.position( src.centY ) );
      pos->setCenter( cent );
      
      err0 = pack.position( src.cerr00 );
      err1 = pack.position( src.cerr11 );
      LHCb::CaloPosition::Spread spr;
      spr(0,0) = err0 * err0;
      spr(1,0) = err1 * err0 * pack.fraction( src.cerr10 );
      spr(1,1) = err1 * err1;
      pos->setSpread( spr );
    }
    int kk;
    int hintID;
    int key;
    for ( kk = src.firstDigit; src.lastDigit > kk; ++kk ) {
      int reference = *(dst->beginRefs()+kk);
      pack.hintAndKey( reference, dst, newCaloHypos, hintID, key );
      SmartRef<LHCb::CaloDigit> ref( newCaloHypos, hintID, key );
      hypo->addToDigits( ref );
    }
    for ( kk = src.firstCluster; src.lastCluster > kk; ++kk ) {
      int reference = *(dst->beginRefs()+kk);
      pack.hintAndKey( reference, dst, newCaloHypos, hintID, key );
      SmartRef<LHCb::CaloCluster> ref( newCaloHypos, hintID, key );
      hypo->addToClusters( ref );
    }
    for ( kk = src.firstHypo; src.lastHypo > kk; ++kk ) {
      int reference = *(dst->beginRefs()+kk);
      pack.hintAndKey( reference, dst, newCaloHypos, hintID, key );
      SmartRef<LHCb::CaloHypo> ref( newCaloHypos, hintID, key );
      hypo->addToHypos( ref );
    }
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
