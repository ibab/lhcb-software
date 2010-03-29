// $Id: HltL0GlobalMonitor.cpp,v 1.3 2010-03-29 14:45:10 albrecht Exp $
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

  m_L0Input         = book1D("L0 channel",-0.5,21.5,22);

  /**
   * 2010-03-29 J.A.
   *This is to put labels on the histogram at initialize
   *Will be updated with corresponding flags during the run
   *I am not sure if the saveset has a "2010 type" L0 TCK 
   * --> be save and do it by hand
   */
  std::vector< std::pair<unsigned, std::string> > labels;
  labels.push_back(std::make_pair( 0, "HCAL"));
  labels.push_back(std::make_pair( 1, "SPD"));
  labels.push_back(std::make_pair( 2, "CALO"));
  labels.push_back(std::make_pair( 3, "MUON,minbias"));
  labels.push_back(std::make_pair( 4, "PU"));
  labels.push_back(std::make_pair( 5, "SPD40"));
  labels.push_back(std::make_pair( 6, "PU20"));
  labels.push_back(std::make_pair( 7, "Electron"));
  labels.push_back(std::make_pair( 8, "Photon"));
  labels.push_back(std::make_pair( 9, "Hadron"));
  labels.push_back(std::make_pair( 10, "Muon"));
  labels.push_back(std::make_pair( 11, "DiMuon"));
  labels.push_back(std::make_pair( 12, "Muon,lowMult"));
  labels.push_back(std::make_pair( 13, "DiMuon,lowMult"));
  labels.push_back(std::make_pair( 14, "LocalPi0"));
  labels.push_back(std::make_pair( 15, "GlobalPi0"));
  labels.push_back(std::make_pair( 16, "B1gas"));
  labels.push_back(std::make_pair( 17, "B2gas"));
  labels.push_back(std::make_pair( 18, "B1gas * ODIN BE"));
  labels.push_back(std::make_pair( 19, "B2gas * ODIN EB"));
  labels.push_back(std::make_pair( 20, "L0 Global"));
  if (!setBinLabels( m_L0Input, labels )) {
    error() << "failed to set binlables on L0 hist" << endmsg;
  }

  m_L0Input->setTitle("L0 channel summary, L0TCK: _not yet set_");
 

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
      labels.push_back(std::make_pair( 20, "L0 Global"));
      setBinLabels( m_L0Input, labels );
      m_lastL0TCK = L0TCK;
      char txt[128];
      sprintf(txt,"L0 channel summary, L0TCK: 0x%x",L0TCK);
      m_L0Input->setTitle(txt);   
  }
  
  if (!l0du->decision()) return;
  
  LHCb::ODIN* odin = fetch<LHCb::ODIN>( LHCb::ODINLocation::Default);
  if (odin == 0) return;

  bool odinBGas=false;
  bool l0Physics=false;
  //fill the histogram containing the l0 channels
  for(LHCb::L0DUChannel::Map::iterator i = channels.begin();i!=channels.end();++i){
      fill( m_L0Input, i->second->id(), l0du->channelDecision( i->second->id() ) );

      if(i->second->id()==16 
	 && odin->bunchCrossingType() == LHCb::ODIN::Beam1 ){
	fill( m_L0Input, 18, l0du->channelDecision( i->second->id() ) );
	if(l0du->channelDecision( i->second->id() )) odinBGas=true;
      }
      

      if(i->second->id()==17 
	 && odin->bunchCrossingType() == LHCb::ODIN::Beam2 ){
	fill( m_L0Input, 19, l0du->channelDecision( i->second->id() ) );
	if(l0du->channelDecision( i->second->id() )) odinBGas=true;
      }
      
      
      //get the global L0 physics decision
      //enabled, non-beam gas channel which fired..
      if( i->second->decisionType() != LHCb::L0DUDecision::Disable 
	  && i->second->id() < 16 
	  && l0du->channelDecision( i->second->id() ) ) l0Physics=true;
      

  }
  //fill the global L0 decision  
  if(odinBGas || l0Physics) fill( m_L0Input, 20,1);
  


};
