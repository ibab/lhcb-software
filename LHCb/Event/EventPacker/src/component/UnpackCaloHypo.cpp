// $Id: UnpackCaloHypo.cpp,v 1.4 2009-11-07 12:20:39 jonrob Exp $
// Include files 
#include "Event/StandardPacker.h"
#include "Event/PackedCaloHypo.h"
#include "Event/CaloHypo.h"

// local
#include "UnpackCaloHypo.h"

//-----------------------------------------------------------------------------
// Implementation file for class : UnpackCaloHypo
//
// 2008-11-14 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
UnpackCaloHypo::UnpackCaloHypo( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::PackedCaloHypoLocation::Electrons );
  declareProperty( "OutputName", m_outputName = LHCb::CaloHypoLocation::Electrons );
  declareProperty( "AlwaysCreateOutput", m_alwaysOutput = false );
}

//=============================================================================
// Destructor
//=============================================================================
UnpackCaloHypo::~UnpackCaloHypo() {} 

//=============================================================================
// Main execution
//=============================================================================
StatusCode UnpackCaloHypo::execute() 
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // If input does not exist, and we aren't making the output regardless, just return
  if ( !m_alwaysOutput && !exist<LHCb::PackedCaloHypos>(m_inputName) )
    return StatusCode::SUCCESS;

  const auto* dst =
    getOrCreate<LHCb::PackedCaloHypos,LHCb::PackedCaloHypos>( m_inputName );

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Size of PackedCaloHypos = " << dst->hypos().size() << endmsg;

  auto* newCaloHypos = new LHCb::CaloHypos();
  put( newCaloHypos, m_outputName );

   // unpack
  const LHCb::CaloHypoPacker packer(*this);
  packer.unpack( *dst, *newCaloHypos );

  return StatusCode::SUCCESS;
}

//=============================================================================

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( UnpackCaloHypo )

//=============================================================================
