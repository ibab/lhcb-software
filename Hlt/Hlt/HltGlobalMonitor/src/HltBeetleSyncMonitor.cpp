// $Id: HltBeetleSyncMonitor.cpp,v 1.76 2010-08-26 08:45:29 graven Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD&STL
// ============================================================================
#include <string>
#include <map>
// ============================================================================
// AIDA
// ============================================================================
#include <AIDA/IHistogram2D.h>
// ============================================================================
// GaudiKernel
// ============================================================================
#include <GaudiKernel/AlgFactory.h> 
// ============================================================================
// GaudiUtils
// ============================================================================
#include <GaudiUtils/HistoStats.h>
#include <GaudiUtils/HistoLabels.h>
// ============================================================================
// Event
// ============================================================================
#include <Event/HltDecReports.h>
#include <Event/STLiteCluster.h>
#include <Event/VeloLiteCluster.h>
#include <Event/L0DUReport.h>
#include <Event/ODIN.h>
// ============================================================================
// local
// ============================================================================
#include "HltBeetleSyncMonitor.h"
// ============================================================================

//-----------------------------------------------------------------------------
// Implementation file for class : HltBeetleSyncMonitor
//
// 2010-10-13 : Roel Aaij
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(HltBeetleSyncMonitor);

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltBeetleSyncMonitor::HltBeetleSyncMonitor(const std::string& name,
                                           ISvcLocator* pSvcLocator)
   : HltBaseAlg (name , pSvcLocator)
{
  declareProperty("L0DUReportLocation", 
                  m_L0DUReportLocation = LHCb::L0DUReportLocation::Default);
  declareProperty("ODINLocation", m_ODINLocation = LHCb::ODINLocation::Default);
  declareProperty("VeloClusterLocation",
                  m_VeloLiteClusterLocation = LHCb::VeloLiteClusterLocation::Default);
  declareProperty("ITClusterLocation",
                  m_ITLiteClusterLocation = LHCb::STLiteClusterLocation::ITClusters);
}
//=============================================================================
// Destructor
//=============================================================================
HltBeetleSyncMonitor::~HltBeetleSyncMonitor()
{

}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltBeetleSyncMonitor::initialize()
{
   StatusCode sc = HltBaseAlg::initialize(); // must be executed first
   if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

   m_SPDIT = book2D("SPDvsIT", "SPD multiplicity. vs. IT hits",
                    0., 1200., 120, 0., 5000., 100);
   m_SPDVelo = book2D("SPDvsVelo", "SPD multiplicity vs. Velo hits",
                      0., 1200., 120, 0., 10000., 100);

   return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltBeetleSyncMonitor::execute() {  
   monitor();

   counter("#events")++;

   setFilterPassed(true);
   return StatusCode::SUCCESS;
}

//=============================================================================
void HltBeetleSyncMonitor::monitor() {  
   LHCb::ODIN* odin = fetch<LHCb::ODIN>(m_ODINLocation);
   LHCb::L0DUReport* l0du = fetch<LHCb::L0DUReport>(m_L0DUReportLocation);
   LHCb::STLiteCluster::FastContainer* itClusters = fetch<LHCb::STLiteCluster::FastContainer>
      (m_ITLiteClusterLocation);
   LHCb::VeloLiteCluster::FastContainer* veloClusters = fetch<LHCb::VeloLiteCluster::FastContainer>
      (m_VeloLiteClusterLocation);

   if (odin == 0 || l0du == 0 || itClusters == 0 || veloClusters == 0) {
      return;
   }

   // Check for presence of SPD in L0DU
   typedef std::map<std::string, std::pair<int, double> > dataMap_t;
   const dataMap_t& dm = l0du->dataMap();
   dataMap_t::const_iterator it = dm.find("Spd(Mult)");
   if (it == dm.end()) {
      return;
   } else {
      double nSPD = it->second.second;
      double nIT = itClusters->size();
      double nVelo = veloClusters->size();

      m_SPDIT->fill(nSPD, nIT);
      m_SPDVelo->fill(nSPD, nVelo);
   }
}
