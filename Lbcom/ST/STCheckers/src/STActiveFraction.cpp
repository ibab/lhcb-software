// Include files 

// local
#include "STActiveFraction.h"

// STDet
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"
#include "STDet/DeTTSector.h"
#include "STDet/DeUTSector.h"

#include "TH2D.h"
#include "TGraph.h"
#include "AIDA/IHistogram1D.h"

//-----------------------------------------------------------------------------
// Implementation file for class : STActiveFraction
//
// 2011-07-20 : Mark Tobin
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
namespace ST {
  DECLARE_ALGORITHM_FACTORY( STActiveFraction )
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ST::STActiveFraction::STActiveFraction( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : ST::HistoAlgBase(name, pSvcLocator),
    m_event(0) 
{
  // This is the start time, step size and number of steps
  declareProperty("StartTime", m_startTime=0);
  declareProperty("TimeStep",  m_timeStep=0);
  declareProperty("Steps",  m_nSteps=0);
}
//=============================================================================
// Destructor
//=============================================================================
ST::STActiveFraction::~STActiveFraction() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode ST::STActiveFraction::initialize() {

  StatusCode sc = ST::HistoAlgBase::initialize();
  if( "" == histoTopDir() ) setHistoTopDir(this->detType()+"/");

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  
  return StatusCode::SUCCESS;

}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ST::STActiveFraction::execute() {

  if ( msgLevel(MSG::DEBUG) ) {
    debug() << "==> Execute" << endmsg;
    debug() << m_event << endmsg;
  
    // get the disabled sectors
    const DeSTDetector::Sectors disabledSec = tracker()->disabledSectors();
    if (disabledSec.size() != 0u){  
      for (DeSTDetector::Sectors::const_iterator iterS = disabledSec.begin(); 
           iterS != disabledSec.end(); ++iterS){    
        debug() << "disabled " << (*iterS)->nickname() << endmsg;  
      } // for      
    }
    else {
      debug() << "All sectors enabled " << endmsg;
    }
  
    // get the disabled beetles
    std::vector<LHCb::STChannelID> disabledB = tracker()->disabledBeetles();
    if (disabledB.size() != 0u){  
      for (std::vector<LHCb::STChannelID>::const_iterator iterB = disabledB.begin(); 
           iterB != disabledB.end(); ++iterB){    
        debug() << "disabled " << uniqueBeetle(*iterB) << endmsg;  
      } // for      
    }
    else {
      debug() << "All beetles enabled " << endmsg;
    }
  } // MSG::DEBUG

  // active fraction
  info () << m_event << " fraction active " << tracker()->fractionActive() 
          << " @ " << (m_startTime+m_event*m_timeStep)/1000000000 << endmsg; 
  m_time.push_back((m_startTime+m_event*m_timeStep)/1000000000);
  m_active.push_back(tracker()->fractionActive()*100.);

  m_event++;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ST::STActiveFraction::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  TH2D* h = new TH2D(("Frame"+detType()).c_str(),"",100,m_time[0],m_time.back(),10, 95., 100.);
  h->GetXaxis()->SetTimeDisplay(1);
  h->GetXaxis()->SetTimeOffset(-m_time[0]);
  h->GetYaxis()->SetTitle("Fraction active (%)");
  TGraph* g = new TGraph(m_time.size(), &m_time[0], &m_active[0]);
  g->SetName(("FractionVsTime"+detType()).c_str());
  g->Write();
  return ST::HistoAlgBase::finalize();  // must be called after all other actions
}

//=============================================================================
