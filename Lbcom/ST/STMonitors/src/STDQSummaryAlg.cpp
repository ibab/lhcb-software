// Event
#include "Event/ODIN.h"

// local
#include "STDQSummaryAlg.h"

#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"
#include "Kernel/ISTReadoutTool.h"

// fstream
#include <fstream>


DECLARE_ALGORITHM_FACTORY( STDQSummaryAlg )

using namespace LHCb;

//--------------------------------------------------------------------
//
//  STDQSummaryAlg:Summary class for STDQ
//
//--------------------------------------------------------------------

STDQSummaryAlg::STDQSummaryAlg( const std::string& name,
                                    ISvcLocator* pSvcLocator ) :
  ST::TupleAlgBase(name, pSvcLocator)
{

  declareSTConfigProperty("summaryLocation",m_summaryLocation , STSummaryLocation::TTSummary );
  declareSTConfigProperty("clusterLocation", m_clusterLocation , STClusterLocation::TTClusters);
  declareProperty("writeTxtFile", m_writeTxtFile = true);
  declareProperty("separator",    m_separator = "|");
  declareProperty("writeTuple",   m_writeTuple = true);
  declareSTConfigProperty("outputFile",   m_outputFileName , "TTDQSummary.txt");
  declareProperty("minADC",       m_minADC = 15);
  declareProperty("maxADC",       m_maxADC = 45);
  declareProperty("fpPrecision",  m_fpPrecision = 2);

  // S/N cut
  declareProperty("threshold", m_threshold = 6);

  setForcedInit();
}

StatusCode STDQSummaryAlg::execute(){

  const STSummary* summary = getIfExists<STSummary>(m_summaryLocation);
  const STClusters* clusters = getIfExists<STClusters>(m_clusterLocation);
  if ( !summary || !clusters ) {
    return Error("No Data available at given locations.", StatusCode::SUCCESS);
  }

  // odin
  const ODIN* odin = get<ODIN>(ODINLocation::Default);
  const int run = odin->runNumber();

  if (run != m_lastRunNumber){
    // new run output and reset
    if (m_lastRunNumber != -1) outputInfo();
    resetCounters(odin->eventTime());
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
  row.runDate    = Counters->m_RunDate           ;
  row.event      = Counters->m_event             ;
  row.clus       = mean(Counters->m_sumClusters) ;
  row.noise      = mean(Counters->m_sumNoise)    ;
  row.procEff    = 1-mean(Counters->m_sumEff)    ;
  row.error      = sum (Counters->m_nError)      ;
  row.corrupted  = sum (Counters->m_nCorrupted)  ;
  row.sumMissing = sum (Counters->m_sumMissing)  ;
  row.chargeMPV  = Counters->chargeMPV()         ;
  m_dataStorage.push_back(row);
}

void STDQSummaryAlg::resetCounters(Gaudi::Time time){
  Counters.reset( new STDQCounters(m_minADC, m_maxADC) );
  Counters->m_RunDate  = std::to_string(time.year(0));
  Counters->m_RunDate += "-"+std::to_string(time.month(0)+1);
  Counters->m_RunDate += "-"+std::to_string(time.day(0));
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
  for (const auto& cluster : *clusters) {
    const DeSTSector* sector = findSector(cluster->channelID());
    const double signalToNoise = (cluster->totalCharge()/sector->noise(cluster->channelID()));
    Counters->addCharge(cluster->totalCharge());
    if (signalToNoise > m_threshold ) ++nh;
  }
  return nh;
}

double STDQSummaryAlg::calculateProcEff(const unsigned int corrupted,
                                   const STSummary::RecoveredInfo& recovered) const{

  const unsigned int nBanks = readoutTool()->nBoard();
  double eff = nBanks - corrupted; // corrupted banks are not processed
  eff = std::accumulate( recovered.begin(), recovered.end(), eff,
                         [](double e, const std::pair<const unsigned int,double>&r) {
                             return e-r.second;// how much was processed
  });
  return eff/double(nBanks);
}

void STDQSummaryAlg::writeTxtFile() {

  std::ofstream oFile(m_outputFileName);
  if (oFile.fail() ){
    Warning("Failed to open output file");
    return;
  }

  oFile <<  std::setprecision(m_fpPrecision);
  // add a header describing each column in the txt file
  for (const auto& c : STDQCounters::m_txtColumns ) writeTxtEntry(oFile, c);
  oFile << m_separator << std::endl;

  // output to a txt file
  for(const auto& r : m_dataStorage ) {
    writeTxtEntry(oFile,r.run        );
    writeTxtEntry(oFile,r.runDate    );
    writeTxtEntry(oFile,r.event      );
    writeTxtEntry(oFile,r.clus       );
    writeTxtEntry(oFile,r.noise      );
    writeTxtEntry(oFile, std::abs(r.procEff) > 1e-20 ? r.procEff : 0);
    writeTxtEntry(oFile,r.error      );
    writeTxtEntry(oFile,r.corrupted  );
    writeTxtEntry(oFile,r.sumMissing );
    writeTxtEntry(oFile,r.chargeMPV  );
    oFile << m_separator << " " << m_separator << std::endl;
  }
}

void STDQSummaryAlg::writeTuple() {
  Tuple tuple = nTuple(name());
  for(const auto& r : m_dataStorage) {
    tuple->column("run",        r.run        );
//    tuple->column("runDate",    r.runDate    );
    tuple->column("event",      r.event      );
    tuple->column("clus",       r.clus       );
    tuple->column("noise",      r.noise      );
    tuple->column("procInEff",  r.procEff    );
    tuple->column("ErrorBanks", r.error      );
    tuple->column("corrupted",  r.corrupted  );
    tuple->column("sumMissing", r.sumMissing );
    tuple->column("chargeMPV",  r.chargeMPV  );
    tuple->write();
  }
}

StatusCode STDQSummaryAlg::finalize(){
  outputInfo();
  if (m_writeTuple   ) writeTuple();
  if (m_writeTxtFile ) writeTxtFile();
  return ST::TupleAlgBase::finalize();
}

