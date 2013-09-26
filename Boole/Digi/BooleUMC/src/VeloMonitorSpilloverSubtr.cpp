// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/ToolFactory.h"
// LHCbKernel includes
#include "Kernel/VeloChannelID.h"
// Event
#include "Event/VeloDigit.h"
// local
#include "VeloMonitorSpilloverSubtr.h"

// Boost
#include "boost/assign/list_of.hpp"
#include <boost/foreach.hpp>

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : VeloMonitorSpilloverSubtr
//
// 2009-09-11 : 
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VeloMonitorSpilloverSubtr )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloMonitorSpilloverSubtr::VeloMonitorSpilloverSubtr( const std::string& name,
                                                      ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{
  std::vector<std::string> tmp = boost::assign::list_of("/")("/Prev/");
  declareProperty("SpillVector", m_spillNames = tmp);
  declareProperty("InputLocation", m_inputLocation = 
		  LHCb::VeloDigitLocation::Default );

}
//=============================================================================
// Destructor
//=============================================================================
VeloMonitorSpilloverSubtr::~VeloMonitorSpilloverSubtr() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode VeloMonitorSpilloverSubtr::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiHistoAlg

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  /// set path
  if ("" == histoTopDir() ) setHistoTopDir("Velo/");

  m_spillPaths.clear();
  // Load spill paths
  std::vector<std::string>::const_iterator iSpillName = m_spillNames.begin();
  while (iSpillName != m_spillNames.end()) {
    m_spillPaths.push_back("/Event"+(*iSpillName)+m_inputLocation);
    ++iSpillName;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode VeloMonitorSpilloverSubtr::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // Get input containers
  if( !exist<VeloDigits>(m_spillPaths[0]) ){
    return Error("No VeloDigits for current sampling time ",StatusCode::SUCCESS,10);
  }    
  VeloDigits* digitContCentr = get<VeloDigits>(m_spillPaths[0]);
  if( !exist<VeloDigits>(m_spillPaths[1]) ){
    return Error(" No VeloDigits for previous sampling time ",StatusCode::SUCCESS,10);
  }    
  VeloDigits* digitContPrev  = get<VeloDigits>(m_spillPaths[1]);
  for(LHCb::VeloDigits::const_iterator ic = digitContCentr->begin(); 
      ic != digitContCentr->end(); ic++) {
    LHCb::VeloDigit* dCentr = *ic;
    short int q = dCentr->adcValue();
    LHCb::VeloDigit* dPrev = digitContPrev->object(dCentr->key());
    short int q_prev = 0;
    if( dPrev ){
      q_prev = dPrev->adcValue();
    }
    plot2D( double(q), double(q_prev), 1, " q_prev vs q ", -20., 130., -20., 130.,150,150);      
  }
    
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode VeloMonitorSpilloverSubtr::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiHistoAlg::finalize();  // must be called after all other actions
}

//=============================================================================
