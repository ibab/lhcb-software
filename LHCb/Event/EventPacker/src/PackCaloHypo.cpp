// $Id: PackCaloHypo.cpp,v 1.2 2008-11-19 10:08:54 ocallot Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/CaloHypo.h"
#include "Event/PackedCaloHypo.h"
#include "Kernel/StandardPacker.h"
// local
#include "PackCaloHypo.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PackCaloHypo
//
// 2008-11-12 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PackCaloHypo );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PackCaloHypo::PackCaloHypo( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::CaloHypoLocation::Electrons );
  declareProperty( "OutputName", m_outputName = LHCb::PackedCaloHypoLocation::Electrons );
}
//=============================================================================
// Destructor
//=============================================================================
PackCaloHypo::~PackCaloHypo() {} 

//=============================================================================
// Main execution
//=============================================================================
StatusCode PackCaloHypo::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  LHCb::CaloHypos* hypo = get<LHCb::CaloHypos>( m_inputName );
  
  LHCb::PackedCaloHypos* out = new LHCb::PackedCaloHypos();
  put( out, m_outputName );
  out->setVersion( 1 );

  StandardPacker pack;
  
  for ( LHCb::CaloHypos::const_iterator itH = hypo->begin(); hypo->end() != itH; ++itH ) {
    LHCb::PackedCaloHypo pH;
    pH.key        = (*itH)->key();
    pH.hypothesis = (*itH)->hypothesis();
    pH.lh         = pack.logPacked( (*itH)->lh() );
    if ( 0 == (*itH)->position() ) {
      pH.z    = 0;
      pH.posX = 0;
      pH.posY = 0;
      pH.posE = 0;
      pH.cov00 = 0;
      pH.cov10 = 0;
      pH.cov20 = 0;
      pH.cov11 = 0;
      pH.cov21 = 0;
      pH.cov22 = 0;
      pH.centX = 0;
      pH.centY = 0;
      pH.cerr00 = 0;
      pH.cerr10 = 0;
      pH.cerr11 = 0;
    } else {
      LHCb::CaloPosition* pos = (*itH)->position();
      pH.z    = pack.position( pos->z() );
      pH.posX = pack.position( pos->x() );
      pH.posY = pack.position( pos->y() );
      pH.posE = pack.energy( pos->e() );

      // convariance Matrix
      double err0 = sqrt( pos->covariance()(0,0) );
      double err1 = sqrt( pos->covariance()(1,1) );
      double err2 = sqrt( pos->covariance()(2,2) );
  
      pH.cov00 = pack.position( err0 );
      pH.cov11 = pack.position( err1 );
      pH.cov22 = pack.energy  ( err2 );
      pH.cov10 = pack.fraction( pos->covariance()(1,0)/err1/err0 );
      pH.cov20 = pack.fraction( pos->covariance()(2,0)/err2/err0 );
      pH.cov21 = pack.fraction( pos->covariance()(2,1)/err2/err1 );

      pH.centX = pack.position( pos->center()(0) );
      pH.centY = pack.position( pos->center()(1) );

      err0 = sqrt( pos->spread()(0,0) );
      err1 = sqrt( pos->spread()(1,1) );
      
      pH.cerr00 = pack.position( err0 );
      pH.cerr11 = pack.position( err1 );
      pH.cerr10 = pack.fraction( pos->spread()(1,0)/err1/err0 );
    }

    //== Store the CaloDigits
    pH.firstDigit = out->sizeRef();
    for ( SmartRefVector<LHCb::CaloDigit>::const_iterator itD = (*itH)->digits().begin();
          (*itH)->digits().end() != itD; ++itD ) {
      int myRef = pack.reference( out, (*itD)->parent(), (*itD)->key().all() );
      out->addRef( myRef );
    }
    pH.lastDigit = out->sizeRef();

    //== Store the CaloClusters
    pH.firstCluster = out->sizeRef();
    for ( SmartRefVector<LHCb::CaloCluster>::const_iterator itC = (*itH)->clusters().begin();
          (*itH)->clusters().end() != itC; ++itC ) {
      int myRef = pack.reference( out, (*itC)->parent(), (*itC)->key() );
      out->addRef( myRef );
    }
    pH.lastCluster = out->sizeRef();

    //== Store the CaloHypos
    pH.firstHypo = out->sizeRef();
    for ( SmartRefVector<LHCb::CaloHypo>::const_iterator itO = (*itH)->hypos().begin();
          (*itH)->hypos().end() != itO; ++itO ) {
      int myRef = pack.reference( out, (*itO)->parent(), (*itO)->key() );
      out->addRef( myRef );
    }
    pH.lastHypo = out->sizeRef();

    out->addEntry( pH );
  }
 
  // Clear the registry address of the unpacked container, to prevent reloading
  hypo->registry()->setAddress( 0 );
  
  return StatusCode::SUCCESS;
}
//=============================================================================
