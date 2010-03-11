// $Id: HltL0GlobalMonitor.cpp,v 1.1 2010-03-11 15:53:23 albrecht Exp $
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
#include "HltL0GlobalMonitor.h"
// ============================================================================

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltL0GlobalMonitor
//
// 2007-05-16 : Bruno Souza de Paula
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltL0GlobalMonitor );

// utilities


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltL0GlobalMonitor::HltL0GlobalMonitor( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : HltBaseAlg ( name , pSvcLocator )
  , m_events(0)
  , m_lastL0TCK(0)
{
  declareProperty("L0DUReport",        m_L0DUReportLocation = LHCb::L0DUReportLocation::Default);
  }
//=============================================================================
// Destructor
//=============================================================================
HltL0GlobalMonitor::~HltL0GlobalMonitor() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltL0GlobalMonitor::initialize() {
  StatusCode sc = HltBaseAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_L0Input         = book1D("L0 channel",-0.5,18.5,19);
  // this code may break when the enums are no longer directly exposing the hardware
 

  declareInfo("COUNTER_TO_RATE[L0Accept]",counter("L0Accept"),"L0Accept");
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltL0GlobalMonitor::execute() {  

  LHCb::L0DUReport*   l0du = fetch<LHCb::L0DUReport>( m_L0DUReportLocation );
  
  monitorL0DU(l0du);
  
  counter("#events")++;
  m_events++;

  return StatusCode::SUCCESS;
  
}


//==============================================================================
void HltL0GlobalMonitor::monitorL0DU(const LHCb::L0DUReport* l0du) {
  if (l0du == 0) return;

  counter("L0Accept") += l0du->decision();
  counter("L0Forced") += l0du->forceBit();

  if (!l0du->decision()) return;

  LHCb::L0DUChannel::Map channels = l0du->configuration()->channels();
  for(LHCb::L0DUChannel::Map::iterator i = channels.begin();i!=channels.end();++i){
      fill( m_L0Input, i->second->id(), l0du->channelDecision( i->second->id() ) );
  }

  unsigned int L0TCK = l0du->tck();
  if (L0TCK != m_lastL0TCK && m_L0Input!=0) {
      std::vector< std::pair<unsigned, std::string> > labels;
      for(LHCb::L0DUChannel::Map::iterator i = channels.begin();i!=channels.end();++i){
        labels.push_back(std::make_pair( i->second->id(), i->first ));
      }
      setBinLabels( m_L0Input, labels );
      m_lastL0TCK = L0TCK;
  }
};
