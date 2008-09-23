// $Id: OldFilterDesktop.cpp,v 1.1 2008-09-23 13:52:23 pkoppenb Exp $

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "Kernel/IFilterCriterion.h"
#include "Kernel/IPlotTool.h"
// local
#include "OldFilterDesktop.h"

//-----------------------------------------------------------------------------
// Implementation file for class : OldFilterDesktop
//
// 2004-11-26 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory

DECLARE_ALGORITHM_FACTORY( OldFilterDesktop );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OldFilterDesktop::OldFilterDesktop( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
  , m_selection()
  , m_inputPlots()
  , m_outputPlots()
{
  // use 
  declareProperty( "FilterCriterion", m_filterCriterion = "LoKi::Hybrid::FilterCriterion" );
  // Options for plot tool.
  declareProperty( "InputPlotTool", m_inputPlotTool = "LoKi::Hybrid::PlotTool/InputPlots" );
  declareProperty( "OutputPlotTool", m_outputPlotTool = "LoKi::Hybrid::PlotTool/OutputPlots" );
  declareProperty( "InputPlotsPath", m_inputPlotsPath = "" );
  declareProperty( "OutputPlotsPath", m_outputPlotsPath = "" );
  declareProperty( "OutputLocation" , m_outputLocation = "" ) ;
  declareProperty( "CloneFinalState" , m_cloneFinalState = false ) ;
  declareProperty( "CloneDaughters" , m_cloneDaughters = false ) ;
  declareProperty( "CloneTree", m_cloneTree = false ) ;
  //  declareProperty ( "MakePlots" , m_makePlots = false) ;
  StatusCode sc = setProperty ( "HistoProduce", "0" ) ; // overwrites GaudiHistoAlg.cpp
  if (!sc) Exception("Could not reset property HistoProduce");

}
//=============================================================================
// Destructor
//=============================================================================
OldFilterDesktop::~OldFilterDesktop() {};

//=============================================================================
// Initialization
//=============================================================================
StatusCode OldFilterDesktop::initialize() {
  StatusCode sc = DVAlgorithm::initialize();
  if (!sc) return sc;

  debug() << "==> Initialize" << endmsg;

  m_selection = tool<IFilterCriterion>(m_filterCriterion, "Filter", this ) ;

  // histogramming
  //  GaudiHistoAlg::setProduceHistos( m_makePlots );

  if ( produceHistos()){
    if ( m_inputPlotTool != "none" ){
      m_inputPlots = tool<IPlotTool>(m_inputPlotTool,this);
      if( !m_inputPlots ) {
        err() << "Unable to get " << m_inputPlotTool << endreq;
        return StatusCode::FAILURE;
      }
      if (m_inputPlotsPath == "") m_inputPlotsPath = "I"+name();
      else m_inputPlots->setPath(m_inputPlotsPath);
      if (msgLevel(MSG::DEBUG)) debug() << "Input plots will be in " << m_inputPlotsPath << endmsg ;
    }
    m_outputPlots = tool<IPlotTool>(m_outputPlotTool,this);
    if ( m_outputPlotTool != "none" ){
      if( !m_outputPlots ) {
        err() << "Unable to get " << m_outputPlotTool << endreq;
        return StatusCode::FAILURE;
      }
      if (m_outputPlotsPath == "") m_outputPlotsPath = "O"+name();
      else m_outputPlots->setPath(m_outputPlotsPath);
      if (msgLevel(MSG::DEBUG)) debug() << "Output plots will be in " << m_outputPlotsPath << endmsg ;
    }
  }

  if ( int(m_cloneTree)+int(m_cloneFinalState)+int(m_cloneDaughters) > 1)
  {
    err() << "CloneTree, CloneDaughters, and CloneFinalState are incompatible. You probably just want CloneTree." << endmsg ;
      return StatusCode::FAILURE;
  }
  
  if ( m_cloneTree )  debug() << "Will clone whole decay tree" << endmsg ;
  else if (m_cloneFinalState ) info() << "Will clone final state particles instead of mother" << endmsg ;
  else if (m_cloneDaughters ) info() << "Will clone daughters instead of mother" << endmsg ;
  else debug() << "Will clone mother only" << endmsg ;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode OldFilterDesktop::execute() {

  // code goes here
  const LHCb::Particle::ConstVector PV = desktop()->particles();
  StatusCode sc = makePlots(PV,m_inputPlots); // make plots
  if (!sc) return sc;

  LHCb::Particle::ConstVector accepted;
  for ( LHCb::Particle::ConstVector::const_iterator ip = PV.begin() ; ip != PV.end() ; ++ip ){
    if ( m_selection->isSatisfied(*ip) ) {
      if (!m_cloneFinalState && !m_cloneDaughters) accepted.push_back(*ip) ;
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << "Particle " << (*ip)->particleID().pid() << " with momentum "
                  << (*ip)->momentum() << " m=" << (*ip)->measuredMass() << " is accepted" << endreq ;
      }
      if ( m_cloneTree || m_cloneFinalState || m_cloneDaughters){
        LHCb::Particle::ConstVector dauts ;
        if ( m_cloneDaughters ) dauts = (*ip)->daughtersVector();
        else if ( m_cloneTree ) dauts = descendants()->descendants(*ip);
        else if ( m_cloneFinalState ) dauts = descendants()->finalStates(*ip);
        for ( LHCb::Particle::ConstVector::const_iterator id = dauts.begin() ; id != dauts.end() ; ++id ){
          if (msgLevel(MSG::VERBOSE)) verbose() << "Getting descendant " << (*id)->particleID().pid() << " " << (*id)->momentum() 
                                                << endmsg ;
          accepted.push_back(*id);
        }
      }      
    } else {
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << "Particle " << (*ip)->particleID().pid() << " with momentum "
                  << (*ip)->momentum() << " m=" << (*ip)->measuredMass() << " is rejected" << endreq ;
        verbose() << " position : " << (*ip)->referencePoint() << ", error : " << (*ip)->posCovMatrix() << endreq ;
      }
    }
  }

  sc = desktop()->cloneTrees(accepted);
  if (!sc) return sc ;
  sc = makePlots(accepted,m_outputPlots);
  if (!sc) return sc;

  const bool empty = accepted.empty();
  if ( !empty && msgLevel(MSG::DEBUG) )
  {
    debug() << "Saved " << accepted.size() << " from "
            << PV.size() << " candidates" << endreq;
  }
  setFilterPassed(!empty);

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode OldFilterDesktop::finalize() {

  debug() << "==> Finalize" << endmsg;

  return DVAlgorithm::finalize();
}

//=============================================================================
// Plotting
//=============================================================================
StatusCode OldFilterDesktop::makePlots(const LHCb::Particle::ConstVector& PV,
                                    IPlotTool* PT){

  if (!produceHistos()) return StatusCode::SUCCESS;
  if (!PT) return StatusCode::SUCCESS;
  debug() << "Plotting " << endmsg;

  return PT->fillPlots(PV) ;
}
