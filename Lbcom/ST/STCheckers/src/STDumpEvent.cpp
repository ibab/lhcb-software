// local
#include "STDumpEvent.h"

#include "Event/STCluster.h"
#include "Event/STSummary.h"
#include "Event/ODIN.h"

DECLARE_ALGORITHM_FACTORY( STDumpEvent )

using namespace LHCb;

//--------------------------------------------------------------------
//
//  STDumpEvent : Prints out the full geometry tree
//
//--------------------------------------------------------------------

STDumpEvent::STDumpEvent( const std::string& name, 
                        ISvcLocator* pSvcLocator ) :
  ST::AlgBase(name, pSvcLocator)
{
  // constructer
  declareProperty("FullDetail", m_fullDetail = false );
  declareSTConfigProperty("clusterLocation", m_clusterLocation , STClusterLocation::TTClusters);
  declareSTConfigProperty("summaryLocation", m_summaryLocation , STSummaryLocation::TTSummary);

}

STDumpEvent::~STDumpEvent()
{
  // destructer
}


StatusCode STDumpEvent::execute()
{
  // execute once per event

  // retrieve clusters
  const STClusters* clusterCont = get<STClusters>(m_clusterLocation);
  const STSummary* summary = get<STSummary>(m_summaryLocation);
  const ODIN* odin = get<ODIN>(ODINLocation::Default);     

  info() << "*** ST Information for run " << odin->runNumber() << " event "  
         << odin->eventNumber() << endmsg;
  info() << *summary << endmsg; 
  
  if (m_fullDetail == true){
    info() << "Cluster Information:" <<  endmsg;
     STClusters::const_iterator iterClus = clusterCont->begin();
     for( ; iterClus != clusterCont->end(); ++iterClus) info() << **iterClus << endmsg;
  } // full detail
  info() << "End of Event Info " << endmsg; 

  return StatusCode::SUCCESS;
}


