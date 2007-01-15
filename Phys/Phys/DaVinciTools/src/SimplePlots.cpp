// $Id: SimplePlots.cpp,v 1.5 2007-01-15 10:32:38 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "SimplePlots.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SimplePlots
//
// 2004-10-22 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( SimplePlots );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SimplePlots::SimplePlots( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
    , m_plots(0)
{
  declareProperty( "PlotTool", m_plotTool = "SimplePlotTool/Plots"  );
}
//=============================================================================
// Destructor
//=============================================================================
SimplePlots::~SimplePlots() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode SimplePlots::initialize() {

  StatusCode sc = DVAlgorithm::initialize() ;
  if (!sc) return sc ;
  m_plots = tool<IPlotTool>(m_plotTool,this);
  if (!m_plots) {
    err() << "Unable to retrieve Plot Tool " << m_plotTool << endmsg;
    return StatusCode::FAILURE;
  }
  m_plots->setPath(name());

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode SimplePlots::execute() {

  debug() << "==> Execute" << endmsg;

  // code goes here  
  const LHCb::Particle::ConstVector PP = desktop()->particles() ;
  StatusCode sc = m_plots->fillPlots(PP);
  if (!sc) {
    err() << "Error from PlotTool" << endmsg;
    return sc;
  }
  
  setFilterPassed(true);   // Mandatory. Set to true if event is accepted.
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode SimplePlots::finalize() {

  debug() << "==> Finalize" << endmsg;

  return DVAlgorithm::finalize() ;
}
