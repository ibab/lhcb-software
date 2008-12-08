// $Id: ParticleMonitor.cpp,v 1.2 2008-12-08 18:12:13 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "LoKi/PhysTypes.h"
#include "LoKi/IHybridFactory.h"
#include "Kernel/IPlotTool.h"

// local
#include "ParticleMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ParticleMonitor
//
// 2008-12-04 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ParticleMonitor );
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ParticleMonitor::ParticleMonitor( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
  ,   m_mother(LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant( true ))
  ,   m_peak(LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant( true ))
  ,   m_sideband(LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant( false ))
{
  declareProperty( "MotherCut", m_motherCut = "ALL", 
                   "The cut to be applied to all mother particle (default all)." )  ;
  // declareProperty( "FinalCut", m_finalCut = "ALL", 
  //      "The cut to be applied to final state particle before plotting (default all)." )  ;
  declareProperty( "PeakCut", m_peakCut = "ALL", 
                   "Selection of mass peak (default all)." )  ;
  declareProperty( "SideBandCut", m_sidebandCut = "False", 
      "Selection of sidebands (default none)." )  ;
  m_plotToolNames.push_back("PidPlotTool"); // default
  declareProperty( "PlotTools" , m_plotToolNames, "Names of plot tools");
}
//=============================================================================
// Destructor
//=============================================================================
ParticleMonitor::~ParticleMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode ParticleMonitor::initialize() {
  //=== The following two lines should be commented for DC04 algorithms ! ===
  StatusCode sc = DVAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  LoKi::IHybridFactory* factory = tool<LoKi::IHybridFactory> 
    ("LoKi::Hybrid::Tool/HybridFactory:PUBLIC" , this ) ;

  if (! configure ( factory, m_motherCut , m_mother  )) return StatusCode::FAILURE ;
  //  if (! configure ( factory, m_finalCut , m_final  )) return StatusCode::FAILURE ;
  if (! configure ( factory, m_peakCut , m_peak  )) return StatusCode::FAILURE ;
  if (! configure ( factory, m_sidebandCut , m_sideband  )) return StatusCode::FAILURE ;

  release(factory);

  if ( m_plotToolNames.empty() ){
    err() << "No plot Tool defined. Will do nothing." << endmsg ;
    return StatusCode::FAILURE ;
  } else {
    if (msgLevel(MSG::DEBUG)) debug() << "Using" ;
    for ( std::vector<std::string>::const_iterator s = m_plotToolNames.begin() ;
          s != m_plotToolNames.end() ; ++s){
      m_plotTools.insert(std::pair<std::string,IPlotTool*>(*s,tool<IPlotTool>(*s,this)));
      if (msgLevel(MSG::DEBUG)) debug() << " " << *s << endmsg ;
    }    
    if (msgLevel(MSG::DEBUG)) debug() << endmsg ;
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ParticleMonitor::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // code goes here  
  for ( LHCb::Particle::ConstVector::const_iterator m = desktop()->particles().begin();
        m != desktop()->particles().end(); ++m){
    counter("# Mothers")++ ;
    if ( !m_mother( *m ) )  { continue ; }   // discard particles with no cuts
    std::string trail = "mother";
    // fill mother
    if (!fillPlots(*m,trail)) return StatusCode::FAILURE;
    
    counter("# Accepted Mothers")++;
    bool peak = m_peak( *m ) ;
    bool sideband = m_sideband( *m ) ;
    if (peak) {
      counter("# Accepted Mothers in Peak")++;
      trail = "_peak";
    } else if (sideband) {
      counter("# Accepted Mothers in Sidebands")++;
      trail = "_sideband";
    } else continue ;  // ignore events ouside of sidebands
    const LHCb::Particle::ConstVector dauts = descendants()->descendants(*m);   
    for ( LHCb::Particle::ConstVector::const_iterator d = dauts.begin() ; d != dauts.end() ; ++d){
      // fill final state daughters
      if (!fillPlots(*d,trail)) return StatusCode::FAILURE;
    }
  }

  setFilterPassed(true);  // Mandatory. Set to true if event is accepted. 
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ParticleMonitor::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return DVAlgorithm::finalize(); //=== For DC04, return StatusCode::SUCCESS;
}

//=============================================================================
