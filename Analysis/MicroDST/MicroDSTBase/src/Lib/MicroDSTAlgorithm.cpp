// $Id: MicroDSTAlgorithm.cpp,v 1.3 2009-08-17 19:13:18 jpalac Exp $
// Include files

// from Gaudi
#include <GaudiKernel/AlgFactory.h>
#include <GaudiKernel/KeyedContainer.h>
// local
#include "MicroDST/MicroDSTAlgorithm.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MicroDSTAlgorithm
//
// 2007-10-11 : Juan PALACIOS
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MicroDSTAlgorithm::MicroDSTAlgorithm( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : MicroDSTCommon<GaudiAlgorithm> ( name , pSvcLocator ),
    m_inputTESLocation(""),
    m_inputTESLocations()
{
  this->declareProperty( "InputLocation", m_inputTESLocation );
  m_inputTESLocations.clear();
  this->declareProperty( "InputLocations", m_inputTESLocations );
}

//=============================================================================
// Destructor
//=============================================================================
MicroDSTAlgorithm::~MicroDSTAlgorithm() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode MicroDSTAlgorithm::initialize()
{
  StatusCode sc = MicroDSTCommon<GaudiAlgorithm>::initialize();
  if ( sc.isFailure() ) return sc;

  if ( !m_inputTESLocations.empty() && !m_inputTESLocation.empty() )
  {
    sc = Error("You have set both InputLocation AND InputLocations properties");
  }

  if ( m_inputTESLocations.empty() )
  {
    this->setInputTESLocation(m_inputTESLocation);
  }

  return sc;
}

//=============================================================================

StatusCode MicroDSTAlgorithm::execute()
{
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MicroDSTAlgorithm::finalize()
{
  return MicroDSTCommon<GaudiAlgorithm>::finalize();
}
//=============================================================================
