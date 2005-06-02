// $Id: PackTrStateL.cpp,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/TrStateL.h"
#include "Kernel/StandardPacker.h"
#include "Event/PackedTrStateL.h"
// local
#include "PackTrStateL.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PackTrStateL
//
// 2005-03-16 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<PackTrStateL>          s_factory ;
const        IAlgFactory& PackTrStateLFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PackTrStateL::PackTrStateL( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = TrStateLLocation::Default );
  declareProperty( "OutputName", m_outputName = PackedTrStateLLocation::Default );
}
//=============================================================================
// Destructor
//=============================================================================
PackTrStateL::~PackTrStateL() {}; 

//=============================================================================
// Main execution
//=============================================================================
StatusCode PackTrStateL::execute() {

  debug() << "==> Execute" << endmsg;

  TrStateLs* states = get<TrStateLs>( m_inputName );
  debug() << m_inputName << " contains " << states->size()
          << " states to convert." << endreq;

  StandardPacker pack;
  
  PackedTrStateLs* out = new PackedTrStateLs();
  put( out, m_outputName );

  for ( TrStateLs::const_iterator itS = states->begin();
        states->end() != itS; ++itS ) {
    TrStateL* state = (*itS);
    
    if ( MSG::VERBOSE >= msgLevel() ) {
      verbose() << format( " State %12.4f %12.4f %12.4f %12.8f %12.8f",
                           state->z(), state->x(), state->y(), 
                           state->tx(), state->ty() ) 
                << endreq;
    }
    
    PackedTrStateL newState;
    
    newState.key  = state->key();
    newState.x    = pack.position( state->x()  );
    newState.y    = pack.position( state->y()  );
    newState.z    = pack.position( state->z()  );
    newState.tx   = pack.slope   ( state->tx() );
    newState.ty   = pack.slope   ( state->ty() );

    // convariance Matrix
    std::vector<double> err;
    err.push_back( sqrt( state->stateCov()[0][0] ) );
    err.push_back( sqrt( state->stateCov()[1][1] ) );
    err.push_back( sqrt( state->stateCov()[2][2] ) );
    err.push_back( sqrt( state->stateCov()[3][3] ) );
    
    newState.cov_00 = pack.position( err[0] );
    newState.cov_11 = pack.position( err[1] );
    newState.cov_22 = pack.slope   ( err[2] );
    newState.cov_33 = pack.slope   ( err[3] );
    newState.cov_10 = pack.fraction( state->stateCov()[1][0]/err[1]/err[0] );
    newState.cov_20 = pack.fraction( state->stateCov()[2][0]/err[2]/err[0] );
    newState.cov_21 = pack.fraction( state->stateCov()[2][1]/err[2]/err[1] );
    newState.cov_30 = pack.fraction( state->stateCov()[3][0]/err[3]/err[0] );
    newState.cov_31 = pack.fraction( state->stateCov()[3][1]/err[3]/err[1] );
    newState.cov_32 = pack.fraction( state->stateCov()[3][2]/err[3]/err[2] );

    out->addEntry( newState );
  }

  // Clear the registry address of the unpacked container, to prevent reloading
  states->registry()->setAddress( 0 );
  
  return StatusCode::SUCCESS;
};

//=============================================================================
