// $Id: STPerformanceMonitor.cpp,v 1.3 2008-08-15 08:23:02 mneedham Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// LHCbKernel
#include "Kernel/STDetSwitch.h"
#include "Kernel/STTell1ID.h"
#include "Kernel/ISTReadoutTool.h"

// STDet
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"

// Event
#include "Event/STSummary.h"

// local
#include "STPerformanceMonitor.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STPerformanceMonitor );

//--------------------------------------------------------------------
//
//  STPerformanceMonitor : Monitor the STPerformances
//
//--------------------------------------------------------------------

STPerformanceMonitor::STPerformanceMonitor( const std::string& name, 
                                ISvcLocator* pSvcLocator ) :
  GaudiHistoAlg(name, pSvcLocator),
  m_tracker(0),
  m_event(0) 
{
  // constructer
  declareProperty("DetType", m_detType = "TT" );
  declareProperty("summaryLocation",m_summaryLocation = STSummaryLocation::TTSummary );
  declareProperty("readoutToolName",m_readoutToolName = "ITReadoutTool" );
  declareProperty("InputData", m_clusterLocation = STClusterLocation::TTClusters);
  declareProperty("ExpectedEvents", m_expectedEvents = 100000);
}

STPerformanceMonitor::~STPerformanceMonitor()
{
  // destructer
}

StatusCode STPerformanceMonitor::initialize()
{
  // Set the top directory to IT or TT.
  if( "" == histoTopDir() ) setHistoTopDir(m_detType+"/");

  // Initialize GaudiHistoAlg
  StatusCode sc = GaudiHistoAlg::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);
    
  // detector element     
  m_tracker = getDet<DeSTDetector>(DeSTDetLocation::location(m_detType));
 
  // flip  
  STDetSwitch::flip(m_detType,m_summaryLocation); 
  STDetSwitch::flip(m_detType,m_readoutToolName);
  STDetSwitch::flip(m_detType,m_clusterLocation);
  
  // readout tool
  m_readoutTool = tool<ISTReadoutTool>(m_readoutToolName,m_readoutToolName);

  return StatusCode::SUCCESS;
}

StatusCode STPerformanceMonitor::execute()
{

  ++m_event;

  // retrieve the event summary
  const STSummary* summary = get<STSummary>(m_summaryLocation);

  // get the clusters
  const STClusters* clusterCont = get<STClusters>(m_clusterLocation);

  // get list of missing sectors
  std::vector<DeSTSector*> deadSectors;

  // some are lost because of errors in decoding
  getDeadSectors(deadSectors,summary->banksWithError());
  
  // some were lost [ie tell1 not readout] 
  getDeadSectors(deadSectors,summary->missingBanks());

  // then all we have to do is loop over the sectors + count
  double frac = 0.0;
  const DeSTDetector::Sectors& sectors = m_tracker->sectors();
  for (DeSTDetector::Sectors::const_iterator iterS = sectors.begin();
       iterS != sectors.end(); ++iterS){
    std::vector<DeSTSector*>::const_iterator testIter = std::find(deadSectors.begin(),
                                                                  deadSectors.end(),*iterS);
    testIter != deadSectors.end() ? frac += 0 : frac += (*iterS)->fractionActive() ;
  } // iterS

  frac /= sectors.size(); 
  plot(frac, 1,"active fraction", 0., 1., 200);

  plot2D(m_event, frac, 11,"active fraction versus time", 0., m_expectedEvents, 0., 1., 50, 200);

  // get the occupancy
  const double occ = clusterCont->size()/(m_tracker->nStrip()*frac); 
  plot(occ, 2,"occupancy", 0., 1., 200);
  plot2D(m_event, occ, 12,"occ versus time", 0., m_expectedEvents, 0., 0.5e-3, 50, 200);


  return StatusCode::SUCCESS;
}

void STPerformanceMonitor::getDeadSectors(std::vector<DeSTSector*>& deadSectors, 
                                          const std::vector<unsigned int>& banks) const {

  for (std::vector<unsigned int>::const_iterator iter= banks.begin(); 
       iter != banks.end(); ++iter){
    // sectors
    std::vector<DeSTSector*> sectors = m_readoutTool->sectors(STTell1ID(*iter));  
    deadSectors.insert(deadSectors.begin(),sectors.begin(),sectors.end());
  } // iter
}



