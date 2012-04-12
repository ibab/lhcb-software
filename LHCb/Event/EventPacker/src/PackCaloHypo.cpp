// $Id: PackCaloHypo.cpp,v 1.5 2009-11-07 12:20:39 jonrob Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "PackCaloHypo.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PackCaloHypo
//
// 2008-11-12 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PackCaloHypo )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  PackCaloHypo::PackCaloHypo( const std::string& name,
                              ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::CaloHypoLocation::Electrons );
  declareProperty( "OutputName", m_outputName = LHCb::PackedCaloHypoLocation::Electrons );
  declareProperty( "AlwaysCreateOutput",         m_alwaysOutput = false     );
  declareProperty( "DeleteInput",                m_deleteInput  = false     );
  declareProperty( "EnableCheck",                m_enableCheck  = false     );
}

//=============================================================================
// Destructor
//=============================================================================
PackCaloHypo::~PackCaloHypo() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PackCaloHypo::execute()
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // If input does not exist, and we aren't making the output regardless, just return
  if ( !m_alwaysOutput && !exist<LHCb::CaloHypos>(m_inputName) ) return StatusCode::SUCCESS;

  // inputs
  LHCb::CaloHypos* hypos = getOrCreate<LHCb::CaloHypos,LHCb::CaloHypos>( m_inputName );

  // output
  LHCb::PackedCaloHypos* out = new LHCb::PackedCaloHypos();
  put( out, m_outputName );
  out->setVersion( 1 );

  // pack
  const LHCb::CaloHypoPacker packer(*this);
  packer.pack( *hypos, *out );

  // Packing checks
  if ( UNLIKELY(m_enableCheck) )
  {
    // make new unpacked output data object
    LHCb::CaloHypos * unpacked = new LHCb::CaloHypos();
    put( unpacked, m_inputName+"_PackingCheck" );
    
    // unpack
    packer.unpack( *out, *unpacked );
    
    // run checks
    packer.check( *hypos, *unpacked ).ignore();
    
    // clean up after checks
    evtSvc()->unregisterObject( unpacked );
    delete unpacked;
  }

  // If requested, remove the input data from the TES and delete
  if ( m_deleteInput )
  {
    evtSvc()->unregisterObject( hypos );
    delete hypos;
    hypos = NULL;
  }
  else
  {
    // Clear the registry address of the unpacked container, to prevent reloading
    hypos->registry()->setAddress( 0 );
  }

  return StatusCode::SUCCESS;
}
//=============================================================================
