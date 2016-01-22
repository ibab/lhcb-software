// Include files

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
  declareProperty( "ClearRegistry",              m_clearRegistry = true     );
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
  if ( !m_alwaysOutput && 
       !exist<LHCb::CaloHypos>(m_inputName) ) return StatusCode::SUCCESS;

  // inputs
  LHCb::CaloHypos* hypos = getOrCreate<LHCb::CaloHypos,LHCb::CaloHypos>( m_inputName );

  // output
  LHCb::PackedCaloHypos* out = new LHCb::PackedCaloHypos();
  put( out, m_outputName );
  out->setVersion( 1 );
  out->setPackingVersion( LHCb::PackedCaloHypos::defaultPackingVersion() );

  // pack
  const LHCb::CaloHypoPacker packer(this);
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
    const StatusCode sc = evtSvc()->unregisterObject( unpacked );
    if ( sc.isSuccess() ) 
    {
      delete unpacked;
    }
    else
    {
      return Error( "Failed to delete test data after unpacking check", sc );
    }
  }

  // If requested, remove the input data from the TES and delete
  if ( UNLIKELY(m_deleteInput) )
  {
    const StatusCode sc = evtSvc()->unregisterObject( hypos );
    if ( sc.isSuccess() ) 
    {
      delete hypos;
      hypos = nullptr;
    }
    else
    {
      return Error( "Failed to delete input data as requested", sc );
    }
  }
  else
  {
    // Clear the registry address of the unpacked container, to prevent reloading
    if ( m_clearRegistry ) { hypos->registry()->setAddress( 0 ); }
  }

  return StatusCode::SUCCESS;
}
//=============================================================================
