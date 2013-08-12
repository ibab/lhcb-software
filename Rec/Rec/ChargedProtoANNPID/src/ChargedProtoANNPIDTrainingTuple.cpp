
//-----------------------------------------------------------------------------
/** @file ChargedProtoANNPIDTrainingTuplee.cpp
 *
 * Implementation file for algorithm ChargedProtoANNPIDTrainingTuple
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2010-03-09
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "ChargedProtoANNPIDTrainingTuple.h"

//-----------------------------------------------------------------------------

using namespace ANNGlobalPID;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedProtoANNPIDTrainingTuple::
ChargedProtoANNPIDTrainingTuple( const std::string& name,
                                 ISvcLocator* pSvcLocator )
  : ChargedProtoANNPIDAlgBase ( name , pSvcLocator ),
    m_tuple                   ( NULL ) { }

//=============================================================================
// Destructor
//=============================================================================
ChargedProtoANNPIDTrainingTuple::~ChargedProtoANNPIDTrainingTuple() { }

//=============================================================================
// Initialization
//=============================================================================
StatusCode ChargedProtoANNPIDTrainingTuple::initialize()
{
  const StatusCode sc = ChargedProtoANNPIDAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  m_tuple =
    tool<IChargedProtoANNPIDTupleTool>( "ANNGlobalPID::ChargedProtoANNPIDTupleTool",
                                        "Tuple", this );

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ChargedProtoANNPIDTrainingTuple::execute()
{
  StatusCode sc = StatusCode::SUCCESS;

  // Load the charged ProtoParticles
  LHCb::ProtoParticles * protos = getIfExists<LHCb::ProtoParticles>( m_protoPath );
  if( !protos ) return Warning("No ProtoParticles at '" + m_protoPath + "'", StatusCode::SUCCESS);
  

  // Loop over all ProtoParticles
  for ( LHCb::ProtoParticles::const_iterator iP = protos->begin();
        iP != protos->end(); ++iP )
  {
    sc = sc && m_tuple->fill( *iP );
  }

  return sc;
}

//=============================================================================

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ChargedProtoANNPIDTrainingTuple )

//=============================================================================

