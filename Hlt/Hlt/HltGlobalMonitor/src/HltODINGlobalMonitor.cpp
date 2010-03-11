// $Id: HltODINGlobalMonitor.cpp,v 1.1 2010-03-11 15:53:57 albrecht Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD&STL
// ============================================================================
#include <string>
#include <vector>
// ============================================================================
// BOOST
// ============================================================================
#include "boost/assign/list_of.hpp"
#include "boost/algorithm/string/erase.hpp"

// ============================================================================
// AIDA
// ============================================================================
#include "AIDA/IHistogram1D.h"
#include "AIDA/IProfile1D.h"
#include "AIDA/IAxis.h"

// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IIncidentSvc.h"
// ============================================================================
// GaudiUtils
// ============================================================================
#include "GaudiUtils/HistoStats.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/HltDecReports.h"
// ============================================================================
// Memory Usage
// ============================================================================
#include "GaudiKernel/Timing.h"
#include "GaudiKernel/Memory.h"
// ============================================================================
// HLT
// ============================================================================
#include "HltBase/HltHistogramUtilities.h"
using namespace Hlt::HistogramUtilities;
// ============================================================================
// local
// ============================================================================
#include "HltODINGlobalMonitor.h"
// ============================================================================

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltODINGlobalMonitor
//
// 2007-05-16 : Bruno Souza de Paula
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltODINGlobalMonitor );

// utilities


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltODINGlobalMonitor::HltODINGlobalMonitor( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : HltBaseAlg ( name , pSvcLocator )
  , m_events(0)
{
  declareProperty("ODIN",              m_ODINLocation = LHCb::ODINLocation::Default);
  }
//=============================================================================
// Destructor
//=============================================================================
HltODINGlobalMonitor::~HltODINGlobalMonitor() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltODINGlobalMonitor::initialize() {
  StatusCode sc = HltBaseAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_odin            = book1D("ODIN trigger type",  "ODIN trigger Type ",-0.5, 7.5, 8);
  std::vector<std::pair<unsigned,std::string> > odinLabels = boost::assign::list_of< std::pair<unsigned,std::string> >
    (ODIN::PhysicsTrigger,    "Physics")
    (ODIN::BeamGasTrigger,    "BeamGas")
    (ODIN::LumiTrigger,       "Lumi")
    (ODIN::TechnicalTrigger,  "Technical")
    (ODIN::AuxiliaryTrigger,  "Auxiliary")
    (ODIN::NonZSupTrigger,    "NonZSup")
    (ODIN::TimingTrigger,     "Timing")
    (ODIN::CalibrationTrigger,"Calibration");
  if (!setBinLabels( m_odin, odinLabels )) {
    error() << "failed to set binlables on ODIN hist" << endmsg;
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltODINGlobalMonitor::execute() {  

  LHCb::ODIN*         odin = fetch<LHCb::ODIN>( LHCb::ODINLocation::Default);
   monitorODIN(odin);
  counter("#events")++;
  m_events++;

  return StatusCode::SUCCESS;
  
}

//==============================================================================
void HltODINGlobalMonitor::monitorODIN(const LHCb::ODIN* odin) {
  if (odin == 0 ) return;
  counter("ODIN::Lumi")    += (odin->triggerType()==ODIN::LumiTrigger);
  counter("ODIN::NotLumi") += (odin->triggerType()!=ODIN::LumiTrigger);
  fill(m_odin, odin->triggerType(), 1.);
  return;
}
