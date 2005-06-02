// $Id: UnpackTrStateP.cpp,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/TrStateP.h"
#include "Kernel/StandardPacker.h"
#include "Event/PackedTrStateP.h"
// local
#include "UnpackTrStateP.h"

//-----------------------------------------------------------------------------
// Implementation file for class : UnpackTrStateP
//
// 2005-03-16 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<UnpackTrStateP>          s_factory ;
const        IAlgFactory& UnpackTrStatePFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
UnpackTrStateP::UnpackTrStateP( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = PackedTrStatePLocation::Default );
  declareProperty( "OutputName", m_outputName = TrStatePLocation::Default );
}
//=============================================================================
// Destructor
//=============================================================================
UnpackTrStateP::~UnpackTrStateP() {}; 

//=============================================================================
// Main execution
//=============================================================================
StatusCode UnpackTrStateP::execute() {

  debug() << "==> Execute" << endmsg;

  PackedTrStatePs* dst = get<PackedTrStatePs>( m_inputName );
  debug() << "Size of PackedTrStatePs = " << dst->end() - dst->begin() << endreq;

  TrStatePs* newStates = new TrStatePs();
  put( newStates, m_outputName );

  StandardPacker pack;
  
  for ( std::vector<PackedTrStateP>::const_iterator itS = dst->begin();
        dst->end() != itS; ++itS ) {
    const PackedTrStateP& src = (*itS);

    HepVector stateVector(5);
    HepSymMatrix stateCov(5);
    
    double z =  pack.position( src.z );
    //== Fill state vector
    stateVector[0] =  pack.position( src.x );
    stateVector[1] =  pack.position( src.y );
    stateVector[2] =  pack.slope( src.tx );
    stateVector[3] =  pack.slope( src.ty );
    double p = pack.energy( src.p );
    stateVector[4] = 1./p;
    
    //== Fill covariance matrix
    std::vector<double> err;
    err.push_back( pack.position( src.cov_00  ) );
    err.push_back( pack.position( src.cov_11  ) );
    err.push_back( pack.slope   ( src.cov_22  ) );
    err.push_back( pack.slope   ( src.cov_33  ) );
    err.push_back( pack.energy  ( src.cov_44  ) / p / p );
    
    stateCov.fast(1,1) = err[0] * err[0];
    stateCov.fast(2,2) = err[1] * err[1];
    stateCov.fast(3,3) = err[2] * err[2];
    stateCov.fast(4,4) = err[3] * err[3];
    stateCov.fast(5,5) = err[4] * err[4];
    stateCov.fast(2,1) = err[1] * err[0] * pack.fraction( src.cov_10 );
    stateCov.fast(3,1) = err[2] * err[0] * pack.fraction( src.cov_20 );
    stateCov.fast(3,2) = err[2] * err[1] * pack.fraction( src.cov_21 );
    stateCov.fast(4,1) = err[3] * err[0] * pack.fraction( src.cov_30 );
    stateCov.fast(4,2) = err[3] * err[1] * pack.fraction( src.cov_31 );
    stateCov.fast(4,3) = err[3] * err[2] * pack.fraction( src.cov_32 );
    stateCov.fast(5,1) = err[4] * err[0] * pack.fraction( src.cov_40 );
    stateCov.fast(5,2) = err[4] * err[1] * pack.fraction( src.cov_41 );
    stateCov.fast(5,3) = err[4] * err[2] * pack.fraction( src.cov_42 );
    stateCov.fast(5,4) = err[4] * err[3] * pack.fraction( src.cov_43 );
    
    TrStateP* state = new TrStateP( z, stateVector, stateCov );
    newStates->insert( state, src.key );
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
