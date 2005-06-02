// $Id: PackTrStateP.cpp,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/TrStateP.h"
#include "Kernel/StandardPacker.h"
#include "Event/PackedTrStateP.h"
// local
#include "PackTrStateP.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PackTrStateP
//
// 2005-03-16 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<PackTrStateP>          s_factory ;
const        IAlgFactory& PackTrStatePFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PackTrStateP::PackTrStateP( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = TrStatePLocation::Default );
  declareProperty( "OutputName", m_outputName = PackedTrStatePLocation::Default );
}
//=============================================================================
// Destructor
//=============================================================================
PackTrStateP::~PackTrStateP() {}; 

//=============================================================================
// Main execution
//=============================================================================
StatusCode PackTrStateP::execute() {

  debug() << "==> Execute" << endmsg;

  TrStatePs* statePs = get<TrStatePs>( m_inputName );
  debug() << m_inputName << " contains " << statePs->size()
          << " states to convert." << endreq;
  
  StandardPacker pack;

  PackedTrStatePs* out = new PackedTrStatePs();
  put( out, m_outputName );

  for ( TrStatePs::const_iterator itP = statePs->begin();
        statePs->end() != itP; ++itP ) {
    TrStateP* state = (*itP);
    if ( MSG::VERBOSE >= msgLevel() ) {
      verbose() << format( " State %12.4f %12.4f %12.4f %12.8f %12.8f %12.8",
                           state->z(), state->x(), state->y(), 
                           state->tx(), state->ty(), state->qDivP() ) 
                << endreq;
    }

    PackedTrStateP newState;
    
    newState.key  = state->key();
    newState.x    = pack.position( state->x()  );
    newState.y    = pack.position( state->y()  );
    newState.z    = pack.position( state->z()  );
    newState.tx   = pack.slope   ( state->tx() );
    newState.ty   = pack.slope   ( state->ty() );
    double p = 0.;
    if ( 0 != state->qDivP() ) p = 1./ state->qDivP();
    newState.p    = pack.energy  ( p );

    // convariance Matrix
    std::vector<double> err;
    err.push_back( sqrt( state->stateCov()[0][0] ) );
    err.push_back( sqrt( state->stateCov()[1][1] ) );
    err.push_back( sqrt( state->stateCov()[2][2] ) );
    err.push_back( sqrt( state->stateCov()[3][3] ) );
    err.push_back( sqrt( state->stateCov()[4][4] ) );
    
    newState.cov_00 = pack.position( err[0] );
    newState.cov_11 = pack.position( err[1] );
    newState.cov_22 = pack.slope   ( err[2] );
    newState.cov_33 = pack.slope   ( err[3] );
    newState.cov_44 = pack.energy  ( p * p * err[4] );  // d(1/p) = dp/p2

    newState.cov_10 = pack.fraction( state->stateCov()[1][0]/err[1]/err[0] );
    newState.cov_20 = pack.fraction( state->stateCov()[2][0]/err[2]/err[0] );
    newState.cov_21 = pack.fraction( state->stateCov()[2][1]/err[2]/err[1] );
    newState.cov_30 = pack.fraction( state->stateCov()[3][0]/err[3]/err[0] );
    newState.cov_31 = pack.fraction( state->stateCov()[3][1]/err[3]/err[1] );
    newState.cov_32 = pack.fraction( state->stateCov()[3][2]/err[3]/err[2] );
    newState.cov_40 = pack.fraction( state->stateCov()[4][0]/err[4]/err[0] );
    newState.cov_41 = pack.fraction( state->stateCov()[4][1]/err[4]/err[1] );
    newState.cov_42 = pack.fraction( state->stateCov()[4][2]/err[4]/err[2] );
    newState.cov_43 = pack.fraction( state->stateCov()[4][3]/err[4]/err[3] );

    out->addEntry( newState );
  }

  // Clear the registry address of the unpacked container, to prevent reloading
  statePs->registry()->setAddress( 0 );
  
  return StatusCode::SUCCESS;
};
//=============================================================================
