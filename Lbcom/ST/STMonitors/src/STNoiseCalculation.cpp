// Include files 

#include "DetDesc/Condition.h"

// local
#include "STNoiseCalculation.h"
#include "Kernel/ISTNoiseCalculationTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : STNoiseCalculation
//
// 2009-10-01 : Mark Tobin
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
namespace ST{
  DECLARE_ALGORITHM_FACTORY( STNoiseCalculation )
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ST::STNoiseCalculation::STNoiseCalculation( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_noiseTool(0)
{
  /// Noise calculation tool
  declareProperty("NoiseToolType",m_noiseToolType="ST::STNoiseCalculationTool");
  declareProperty("NoiseToolName",m_noiseToolName,"TTNoiseCalculationTool");
}

//=============================================================================
// Destructor
//=============================================================================
ST::STNoiseCalculation::~STNoiseCalculation() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode ST::STNoiseCalculation::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_noiseTool = tool<ST::ISTNoiseCalculationTool>(m_noiseToolType, m_noiseToolName);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ST::STNoiseCalculation::execute() {

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  StatusCode sc = m_noiseTool->updateNoise();
  return sc;//StatusCode::SUCCESS;
}

//=============================================================================
