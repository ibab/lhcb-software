// $Id: HltL0GlobalMonitor.cpp,v 1.2 2010-03-26 14:01:20 albrecht Exp $
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
// 2010-03-24 : Johannes Albrecht
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
  declareProperty("ODIN",              m_ODINLocation = LHCb::ODINLocation::Default);
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

  m_L0Input         = book1D("L0 channel",-0.5,20.5,21);
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

  LHCb::L0DUChannel::Map channels = l0du->configuration()->channels();

  //define the bin labels
  unsigned int L0TCK = l0du->tck();
  if (L0TCK != m_lastL0TCK && m_L0Input!=0) {
      std::vector< std::pair<unsigned, std::string> > labels;
      for(LHCb::L0DUChannel::Map::iterator i = channels.begin();i!=channels.end();++i){
	
	std::string name=i->first;
	if( i->second->decisionType() == LHCb::L0DUDecision::Disable ){
	  name="disabled | "+name;
	}
	labels.push_back(std::make_pair( i->second->id(),name  ));
      }
      labels.push_back(std::make_pair( 18, "B1gas * ODIN BE"));
      labels.push_back(std::make_pair( 19, "B2gas * ODIN EB"));
      setBinLabels( m_L0Input, labels );
      m_lastL0TCK = L0TCK;
  }
  
  if (!l0du->decision()) return;
  
  LHCb::ODIN* odin = fetch<LHCb::ODIN>( LHCb::ODINLocation::Default);
  if (odin == 0) return;

  //fill the histogram containing the l0 channels
  for(LHCb::L0DUChannel::Map::iterator i = channels.begin();i!=channels.end();++i){
      fill( m_L0Input, i->second->id(), l0du->channelDecision( i->second->id() ) );

      if(i->second->id()==16 
	 && odin->bunchCrossingType() == LHCb::ODIN::Beam1 )
	fill( m_L0Input, 18, l0du->channelDecision( i->second->id() ) );

      if(i->second->id()==17 
	 && odin->bunchCrossingType() == LHCb::ODIN::Beam2 )
	fill( m_L0Input, 19, l0du->channelDecision( i->second->id() ) );

  }

};
