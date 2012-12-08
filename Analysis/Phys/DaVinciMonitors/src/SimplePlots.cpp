// $Id: SimplePlots.cpp,v 1.1.1.1 2008-12-05 16:41:05 pkoppenb Exp $
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
DECLARE_ALGORITHM_FACTORY( SimplePlots )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SimplePlots::SimplePlots( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : DaVinciAlgorithm ( name , pSvcLocator )
    , m_plots(0)
{
  declareProperty( "PlotTool", m_plotTool = "LoKi::Hybrid::PlotTool//Plots"  );
}
//=============================================================================
// Destructor
//=============================================================================
SimplePlots::~SimplePlots() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode SimplePlots::initialize() 
{
  StatusCode sc = DaVinciAlgorithm::initialize() ;
  if (!sc) return sc ;
 
  m_plots = tool<IPlotTool>(m_plotTool,this);
  if (!m_plots) {
    err() << "Unable to retrieve Plot Tool " << m_plotTool << endmsg;
    return StatusCode::FAILURE;
  }

  return m_plots->setPath(name());
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode SimplePlots::execute() {

  debug() << "==> Execute" << endmsg;

  // code goes here  
  const LHCb::Particle::ConstVector PP = this->i_particles() ;
  StatusCode sc = m_plots->fillPlots(PP);
  if (!sc) {
    err() << "Error from PlotTool" << endmsg;
    return sc;
  }
  
  setFilterPassed(true);   // Mandatory. Set to true if event is accepted.
  return StatusCode::SUCCESS;
}
