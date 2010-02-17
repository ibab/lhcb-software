// $Id: STDQSummaryAlg.cpp,v 1.6 2010-02-17 14:20:42 nchiapol Exp $

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
//#include <boost/assign/list_of.hpp>
//#include <boost/assign/std/vector.hpp>

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
  declareProperty("separator",    m_separator = "|");
  declareProperty("writeTuple",   m_writeTuple = true);
  declareProperty("outputFile",   m_outputFileName = "STDQSummary.txt");
  declareProperty("minADC",       m_minADC = 15);
  declareProperty("maxADC",       m_maxADC = 45);

  m_txtColumns.push_back("Run");
  m_txtColumns.push_back("Events");
  m_txtColumns.push_back("Clusters/evt");
  m_txtColumns.push_back("#Noise/event");
  m_txtColumns.push_back("Proc Eff");
  m_txtColumns.push_back("#ErrorBanks");
  m_txtColumns.push_back("#Corrupted");
  m_txtColumns.push_back("#Missing");
  m_txtColumns.push_back("Charge MPV");
  m_txtColumns.push_back("Comments");
  
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
  Counters = new STDQCounters(m_minADC, m_maxADC);
  
  return StatusCode::SUCCESS;
}




StatusCode STDQSummaryAlg::execute(){

  if (!exist<STSummary>(m_summaryLocation) || !exist<STClusters>(m_clusterLocation)) {
    return Error("No Data available at given locations.", StatusCode::SUCCESS);
  }
  const STSummary* summary = get<STSummary>(m_summaryLocation);  
  const STClusters* clusters = get<STClusters>(m_clusterLocation); 
  
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
  
  STDQCounters::DataRow row;
  row.run        = m_lastRunNumber               ;
  row.event      = Counters->m_event             ;
  row.clus       = mean(Counters->m_sumClusters) ;
  row.noise      = mean(Counters->m_sumNoise)    ;
  row.procEff    = mean(Counters->m_sumEff)      ;
  row.error      = sum (Counters->m_nError)      ;
  row.corrupted  = sum (Counters->m_nCorrupted)  ;
  row.sumMissing = sum (Counters->m_sumMissing)  ;
  row.chargeMPV  = Counters->chargeMPV()         ;
  m_dataStorage.push_back(row);
}



void STDQSummaryAlg::resetCounters(){
  
  delete Counters;
  Counters = new STDQCounters(m_minADC, m_maxADC);
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
  Counters->m_nError      (summary->nErrorBanks());
  Counters->m_nCorrupted  (corrupted);
  Counters->m_sumMissing  (missingBanks);
}




unsigned int STDQSummaryAlg::countHigh(const STClusters* clusters) const {

  unsigned int nh = 0;
  STClusters::const_iterator iter  = clusters->begin();
  for ( ; iter != clusters->end() ; ++iter){
    const DeSTSector* sector = findSector((*iter)->channelID());
    const double signalToNoise = ((*iter)->totalCharge()/sector->noise((*iter)->channelID()));
    Counters->addCharge((*iter)->totalCharge());
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
  oFile << m_separator << " " << m_separator << std::endl; 
  //oFile << m_separator << std::endl;

  // output to a txt file
  std::vector<STDQCounters::DataRow>::iterator it;
  std::vector<STDQCounters::DataRow>::iterator endIt = m_dataStorage.end();
  for(it = m_dataStorage.begin(); it < endIt; it++) {
    writeTxtEntry(oFile,it->run        ,12);      
    writeTxtEntry(oFile,it->event      ,12);
    writeTxtEntry(oFile,it->clus       ,12);
    writeTxtEntry(oFile,it->noise      ,12);
    writeTxtEntry(oFile,it->procEff    ,12);
    writeTxtEntry(oFile,it->error      ,12);
    writeTxtEntry(oFile,it->corrupted  ,12);
    writeTxtEntry(oFile,it->sumMissing ,12);
    writeTxtEntry(oFile,it->chargeMPV  ,12);
    oFile << m_separator << " " << m_separator << std::endl; 
  }
}



void STDQSummaryAlg::writeTuple() {

  Tuple tuple = nTuple(name());
  std::vector<STDQCounters::DataRow>::iterator it;
  std::vector<STDQCounters::DataRow>::iterator endIt = m_dataStorage.end();
  for(it = m_dataStorage.begin(); it < endIt; it++) {
    Tuple tuple = nTuple(name());
    tuple->column("run",        it->run        );
    tuple->column("event",      it->event      );
    tuple->column("clus",       it->clus       );
    tuple->column("noise",      it->noise      );
    tuple->column("procEff",    it->procEff    );
    tuple->column("ErrorBanks", it->error      );
    tuple->column("corrupted",  it->corrupted  );
    tuple->column("sumMissing", it->sumMissing );
    tuple->column("chargeMPV",  it->chargeMPV  );
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

