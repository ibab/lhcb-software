// $Id: CompareCaloHypo.cpp,v 1.2 2009-07-09 09:44:16 cattanem Exp $
// Include files

// local
#include "CompareCaloHypo.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CompareCaloHypo
//
// 2008-11-14 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CompareCaloHypo )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CompareCaloHypo::CompareCaloHypo( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::CaloHypoLocation::Electrons );
  declareProperty( "TestName"  , m_testName   = LHCb::CaloHypoLocation::Electrons+"Test" );
}

//=============================================================================
// Destructor
//=============================================================================
CompareCaloHypo::~CompareCaloHypo() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CompareCaloHypo::execute()
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
 
  LHCb::CaloHypos* old  = get<LHCb::CaloHypos>( m_inputName  );
  LHCb::CaloHypos* test = get<LHCb::CaloHypos>( m_testName  );

  // compare and return
  const LHCb::CaloHypoPacker packer(*this);
  return packer.check( *old, *test );
}
//=============================================================================
