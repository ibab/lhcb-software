// $Id: SimplePlots.cpp,v 1.1 2005-01-06 10:37:47 pkoppenb Exp $
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
static const  AlgFactory<SimplePlots>          s_factory ;
const        IAlgFactory& SimplePlotsFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SimplePlots::SimplePlots( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
    , m_plots(0)
{
}
//=============================================================================
// Destructor
//=============================================================================
SimplePlots::~SimplePlots() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode SimplePlots::initialize() {

  debug() << "==> Initialize" << endmsg;
  m_plots = tool<IPlotTool>("SimplePlotTool","Plots",this);
  if (!m_plots) {
    err() << "Unable to retrieve SimplePlotTool" << endmsg;
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
  const ParticleVector PP = desktop()->particles() ;
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

  return  StatusCode::SUCCESS;
}
