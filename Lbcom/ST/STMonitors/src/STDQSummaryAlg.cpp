// $Id: STDQSummaryAlg.cpp,v 1.3 2010-02-05 15:22:33 nchiapol Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// Event
#include "Event/ODIN.h"

// local
#include "STDQSummaryAlg.h"

#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"
#include "Kernel/ISTReadoutTool.h"

// BOOST
#include <boost/assign/list_of.hpp>
#include <boost/assign/std/vector.hpp>

// fstream
#include <fstream>

DECLARE_ALGORITHM_FACTORY( STDQSummaryAlg );

using namespace LHCb;

//--------------------------------------------------------------------
//
//  STDQSummaryAlg:Summary class for STDQ
//
//--------------------------------------------------------------------

STDQSummaryAlg::STDQSummaryAlg( const std::string& name, 
                                    ISvcLocator* pSvcLocator ) :
  ST::TupleAlgBase(name, pSvcLocator),
  m_lastRunNumber(-1)
{

  declareSTConfigProperty("summaryLocation",m_summaryLocation , STSummaryLocation::TTSummary );
  declareSTConfigProperty("clusterLocation", m_clusterLocation , STClusterLocation::TTClusters);
  declareProperty("writeTxtFile", m_writeTxtFile = true);
  declareProperty("separator", m_separator = "|");
  declareProperty("writeTuple", m_writeTuple = true);
  declareProperty("outputFile",m_outputFileName = "STDQSummary.txt");

  using namespace boost::assign;
  m_txtColumns += "Run", "Events", "Clusters/evt", "#Noise/event",  "Proc Eff", "#Corrupted", "#Missing"; 
  declareProperty("txtColumns", m_txtColumns);
  
  // S/N cut
  declareProperty("threshold", m_threshold = 5);
  
  setForcedInit();
}

STDQSummaryAlg::~STDQSummaryAlg() { }




StatusCode STDQSummaryAlg::initialize()
{

  StatusCode sc = ST::TupleAlgBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  // zero everthing
  Counters = new STDQCounters;
  
  return StatusCode::SUCCESS;
}




StatusCode STDQSummaryAlg::execute(){

  bool data_available = true;
  if (!exist<STSummary>(m_summaryLocation) || !exist<STClusters>(m_clusterLocation)) {
    return Error("No Data available at given locations.", StatusCode::SUCCESS);
  }
  const STSummary* summary = get<STSummary>(m_summaryLocation);  
  const STClusters* clusters = get<STClusters>(m_clusterLocation); 
  
  // get the summary block 
  try {
    summary = get<STSummary>(m_summaryLocation);  
  } catch (...) {
    data_available = false;
  }
  // clusters
  try {
    clusters = get<STClusters>(m_clusterLocation); 
  } catch (...) {
    data_available = false;
  }

  if (!data_available) {
    return StatusCode::SUCCESS;
  }
  
  // odin
  const ODIN* odin = get<ODIN>(ODINLocation::Default);
  const int run = odin->runNumber();

  if (run != m_lastRunNumber){
    // new run output and reset
    if (m_lastRunNumber != -1) {
      outputInfo();
      resetCounters();
    }
    m_lastRunNumber = run;
  }
 
  // summarize this event
  processEvent(clusters,summary);

  return StatusCode::SUCCESS;
}




void STDQSummaryAlg::outputInfo(){
  
  // only output if there is something to output !
  if (Counters->m_event == 0) {
    info() << " Run contains no events , no DQ possible" << endmsg;
    return;
  }
  
  DataRow row;
  row.run        = m_lastRunNumber               ;
  row.event      = Counters->m_event             ;
  row.clus       = mean(Counters->m_sumClusters) ;
  row.noise      = mean(Counters->m_sumNoise)    ;
  row.procEff    = mean(Counters->m_sumEff)      ;
  row.corrupted  = sum (Counters->m_nCorrupted)  ;
  row.sumMissing = sum (Counters->m_sumMissing)  ;
  m_dataStorage.push_back(row);
}



void STDQSummaryAlg::resetCounters(){
  
  delete Counters;
  Counters = new STDQCounters;
}




void STDQSummaryAlg::processEvent(const STClusters* clusters, const STSummary* summary) const{

  const unsigned int nClus = clusters->size(); // all clusters
  const unsigned int nHigh = countHigh(clusters);    // remove noise clusters
  const unsigned int missingBanks = summary->missingBanks().size(); // MIA
  const unsigned int corrupted = summary->corruptedBanks().size(); // really rubbish banks
  const STSummary::RecoveredInfo& recovered = summary->recoveredBanks();  

  // fill the counters 
  Counters->m_event++;
  Counters->m_sumNoise    (nClus - nHigh); 
  Counters->m_sumClusters (nClus);
  Counters->m_sumEff      (calculateProcEff(corrupted,recovered)); 
  Counters->m_nCorrupted  (corrupted);
  Counters->m_sumMissing  (missingBanks);
}




unsigned int STDQSummaryAlg::countHigh(const STClusters* clusters) const {

  unsigned int nh = 0;
  STClusters::const_iterator iter  = clusters->begin();
  for ( ; iter != clusters->end() ; ++iter){
    const DeSTSector* sector = findSector((*iter)->channelID());
    const double signalToNoise = ((*iter)->totalCharge()/sector->noise((*iter)->channelID()));
    if (signalToNoise > m_threshold ) ++nh; 
  }
  return nh;
}



double STDQSummaryAlg::calculateProcEff(const unsigned int corrupted, 
                                   const STSummary::RecoveredInfo& recovered) const{

  const unsigned int nBanks = readoutTool()->nBoard();
  double eff = nBanks - corrupted; // corrupted banks are not processed
  for (STSummary::RecoveredInfo::const_iterator iter = recovered.begin(); 
       iter != recovered.end(); ++iter){
    eff -= iter->second; // how much was processed
  } // iter
  eff /= double(nBanks);
  return eff;
}




void STDQSummaryAlg::writeTxtFile() {

  std::ofstream oFile(m_outputFileName.c_str());
  if (oFile.fail() ){
    Warning("Failed to open output file"); 
    return;
  }
  
  // add a header describing each column in the txt file
  for (Strings::const_iterator iterS = m_txtColumns.begin() ; iterS != m_txtColumns.end(); ++iterS){
    writeTxtEntry(oFile, *iterS, 12);
  }
  oFile << m_separator << std::endl;

  // output to a txt file
  std::vector<DataRow>::iterator it;
  std::vector<DataRow>::iterator endIt = m_dataStorage.end();
  for(it = m_dataStorage.begin(); it < endIt; it++) {
    writeTxtEntry(oFile,it->run        ,12);      
    writeTxtEntry(oFile,it->event      ,12);
    writeTxtEntry(oFile,it->clus       ,12);
    writeTxtEntry(oFile,it->noise      ,12);
    writeTxtEntry(oFile,it->procEff    ,12);
    writeTxtEntry(oFile,it->corrupted  ,12);
    writeTxtEntry(oFile,it->sumMissing ,12);
    oFile << m_separator << std::endl; 
  }
}



void STDQSummaryAlg::writeTuple() {

  Tuple tuple = nTuple(name());
  std::vector<DataRow>::iterator it;
  std::vector<DataRow>::iterator endIt = m_dataStorage.end();
  for(it = m_dataStorage.begin(); it < endIt; it++) {
    Tuple tuple = nTuple(name());
    tuple->column("run",        it->run        );
    tuple->column("event",      it->event      );
    tuple->column("clus",       it->clus       );
    tuple->column("noise",      it->noise      );
    tuple->column("procEff",    it->procEff    );
    tuple->column("corrupted",  it->corrupted  );
    tuple->column("sumMissing", it->sumMissing );
    tuple->write();
  }
}




StatusCode STDQSummaryAlg::finalize(){

  outputInfo();

  if (m_writeTuple   == true) {
    writeTuple();
  }
  if (m_writeTxtFile == true) {
    writeTxtFile();
  }
  
  delete Counters;
  return ST::TupleAlgBase::finalize();
}

