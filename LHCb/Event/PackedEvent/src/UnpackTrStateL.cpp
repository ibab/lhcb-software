// $Id: UnpackTrStateL.cpp,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/TrStateL.h"
#include "Kernel/StandardPacker.h"
#include "Event/PackedTrStateL.h"
// local
#include "UnpackTrStateL.h"

//-----------------------------------------------------------------------------
// Implementation file for class : UnpackTrStateL
//
// 2005-03-16 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<UnpackTrStateL>          s_factory ;
const        IAlgFactory& UnpackTrStateLFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
UnpackTrStateL::UnpackTrStateL( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = PackedTrStateLLocation::Default );
  declareProperty( "OutputName", m_outputName = TrStateLLocation::Default );
}
//=============================================================================
// Destructor
//=============================================================================
UnpackTrStateL::~UnpackTrStateL() {}; 

//=============================================================================
// Main execution
//=============================================================================
StatusCode UnpackTrStateL::execute() {

  debug() << "==> Execute" << endmsg;

  PackedTrStateLs* dst = get<PackedTrStateLs>( m_inputName );
  debug() << "Size of PackedTrStateLs = " << dst->end() - dst->begin() << endreq;

  TrStateLs* newStates = new TrStateLs();
  put( newStates, m_outputName );

  StandardPacker pack;
  
  for ( std::vector<PackedTrStateL>::const_iterator itS = dst->begin();
        dst->end() != itS; ++itS ) {
    const PackedTrStateL& src = (*itS);

    HepVector stateVector(4);
    HepSymMatrix stateCov(4);
    
    double z =  pack.position( src.z );
    //== Fill state vector
    stateVector[0] =  pack.position( src.x );
    stateVector[1] =  pack.position( src.y );
    stateVector[2] =  pack.slope( src.tx );
    stateVector[3] =  pack.slope( src.ty );
    //== Fill covariance matrix
    std::vector<double> err;
    err.push_back( pack.position( src.cov_00  ) );
    err.push_back( pack.position( src.cov_11  ) );
    err.push_back( pack.slope   ( src.cov_22  ) );
    err.push_back( pack.slope   ( src.cov_33  ) );
    
    stateCov.fast(1,1) = err[0] * err[0];
    stateCov.fast(2,2) = err[1] * err[1];
    stateCov.fast(3,3) = err[2] * err[2];
    stateCov.fast(4,4) = err[3] * err[3];
    stateCov.fast(2,1) = err[1] * err[0] * pack.fraction( src.cov_10 );
    stateCov.fast(3,1) = err[2] * err[0] * pack.fraction( src.cov_20 );
    stateCov.fast(3,2) = err[2] * err[1] * pack.fraction( src.cov_21 );
    stateCov.fast(4,1) = err[3] * err[0] * pack.fraction( src.cov_30 );
    stateCov.fast(4,2) = err[3] * err[1] * pack.fraction( src.cov_31 );
    stateCov.fast(4,3) = err[3] * err[2] * pack.fraction( src.cov_32 );
    
    TrStateL* state = new TrStateL( z, stateVector, stateCov );
    newStates->insert( state, src.key );
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
