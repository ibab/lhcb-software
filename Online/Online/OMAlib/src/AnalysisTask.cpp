// $Id: AnalysisTask.cpp,v 1.26 2010-06-17 10:46:00 ggiacomo Exp $
#include <cmath>
// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
// local
#include "OMAlib/OMAlib.h"
#include "OMAlib/OMAAlgorithms.h"
#include "OMAlib/AnalysisTask.h"
#include "OMAlib/SavesetFinder.h"
#include "OMAlib/RunInfo.h"

#include <dic.hxx>

//-----------------------------------------------------------------------------
// Implementation file for class : AnalysisTask
//
// 2008-02-18 : Giacomo Graziani
//-----------------------------------------------------------------------------


AnalysisTask::AnalysisTask( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : OMAlib(NULL, name),
    GaudiHistoAlg ( name , pSvcLocator ),
    m_inputFiles(0),
    m_inputTasks(0),
    m_forceOnlineEnv(false),
    m_runInfo(NULL),
    m_runStatus(-1),
    m_runAvailable(false),
    m_conditionNames(0)
{
  declareProperty ( "useHistDB"    , m_useDB = false);
  declareProperty ( "HistDB"       , m_DB = OnlineHistDBEnv_constants::DB );
  declareProperty ( "HistDBuser"   , m_DBuser = OnlineHistDBEnv_constants::ACCOUNT );
  declareProperty ( "HistDBpw"     , m_DBpw = OnlineHistDBEnv_constants::PASSWORD );
  declareProperty ( "HistRefRoot"  , m_myRefRoot = "default");
  declareProperty ( "InputFiles"   , m_inputFiles);
  declareProperty ( "InputTasks"   , m_inputTasks);
  declareProperty ( "Partition"    , m_partition = "LHCb" );
  declareProperty ( "TextLog"      , m_textLogName = "");
  declareProperty ( "HistDBMsgPersistency", m_logToHistDB = true);
  declareProperty ( "PublishDIMmessages", m_doPublish = true);
  declareProperty ( "ForceOnlineEnv", m_forceOnlineEnv = false); // enable online tools (DIM, status checks) also in offline mode
  declareProperty ( "ChangeHistPadColors", m_padcolors = false);
  declareProperty ( "CheckDetectorStatus", m_checkStatus = true);
  declareProperty ( "CleanupMessagesAtStartup", m_messageCleanup = false);
  //
  declareProperty ( "RICHclustersDir", m_RICHClDir= "/home/ryoung");
}

AnalysisTask::~AnalysisTask() {
  std::vector<SavesetFinder*>::iterator iSS;
  for(iSS = m_saveset.begin() ; iSS != m_saveset.end() ; iSS++) {
    delete (*iSS);
  }
  if(m_runInfo) delete m_runInfo;
  std::vector<DimInfo*>::iterator iDI;
  for(iDI = m_statusInfo.begin() ; iDI != m_statusInfo.end() ; iDI++) {
    delete (*iDI);
  }
}


StatusCode AnalysisTask::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize();
  if ( sc.isFailure() ) return sc;
  setMsgStream(&(always()));
  // add the partition name to the analysis task identifier
  m_anaTaskname = m_anaTaskname+"/"+m_partition;
  debug() << "Initializing Analysis Task "<<m_anaTaskname << endmsg;


  if ( ! m_inputFiles.empty() &&  !m_forceOnlineEnv) {
    // offline mode: switch off message dim publishing and status checks unless forced
    m_doPublish = false;
    m_checkStatus = false;
  }
  else { // online mode
    bool dimOK=startMessagePublishing();
    if (!dimOK) {
      error() <<"An analysis task with the same name is already running!!!"<<endmsg;
      return StatusCode::FAILURE;
    }
  }
  // use HistDB if requested
  openDBSession();

  if(m_messageCleanup && m_doPublish) cleanupDIMMessages(); // to purge possible relic messages in Alarm Screen

  if( "default" != m_myRefRoot) {
    setRefRoot(m_myRefRoot);
  }

  if (! m_textLogName.empty()) {
    m_textLog=true;
    openLog();
  }

  // access run status if requested
  if(m_checkStatus) {
    if(m_histDB) {
      std::vector<int> bits;
      std::vector<std::string> dimservices;
      int nc = m_histDB->getConditions(bits,  m_conditionNames,  dimservices);
      for (int ic=0; ic<nc; ic++) {
        DimInfo* dimservice = new DimInfo(dimservices[ic].c_str(), (int) -1);
        m_statusInfo.push_back( dimservice );
        sleep(1); // this is needed to avoid DIM crashes!!
        if (dimservice->getInt() == -1) {
          error() << "requested to check status " << m_conditionNames[ic] <<
            " but DIM service is not available"<<endmsg;
        }
      }

      m_runInfo = new RunInfo(this);
      sleep(1); // this is needed to avoid DIM crashes!!
      if (m_runInfo->getInt() == -1) {
        error() << "run number DIM service is not available"<<endmsg;
        m_runAvailable= false;
      }
    }
  }


  // special for RICH IFB algorithm
  std::string richalg("IfbMonitor");
  ( dynamic_cast<OMAIfbMonitor*>(getAlg(richalg)) )->setOutputDir(m_RICHClDir);

  if ( ! m_inputFiles.empty() ) {
    // offline mode: analyze these files and exit
    std::vector<std::string>::iterator iF;
    std::string task =  m_inputTasks.empty() ? "any" : m_inputTasks[0];
    for(iF =  m_inputFiles.begin() ; iF != m_inputFiles.end() ; iF++) {
      info() <<"Analyzing histograms on file "<<*iF<<endmsg;
      resetMessages(task);
      analyze( *iF , task);
      refreshMessageList(task);
      sleep(5);
    }
    closeDBSession();
  }
  else { // online mode: start SavesetFinder for each requested monitoring task
    std::vector<std::string>::iterator iF;
    if(m_inputTasks.size()>0) {
      if (m_inputTasks[0]=="any")
        getAllTasks();
    }
    closeDBSession();
    for(iF =  m_inputTasks.begin() ; iF != m_inputTasks.end() ; iF++) {
      info()<< "listening to latest saveset for task "<<*iF << endmsg;
      m_saveset.push_back( new SavesetFinder(this, *iF, m_partition) );
    }
  }
  return StatusCode::SUCCESS;
}

void AnalysisTask::openDBSession() {
  if(m_useDB && NULL == m_histDB)
    OMAlib::openDBSession( m_DBpw, m_DBuser, m_DB );
}

StatusCode AnalysisTask::analyze(std::string& SaveSet,
                                 std::string Task) {
  m_savesetName=SaveSet;
  m_taskname=Task;

  return StatusCode::SUCCESS;
}

StatusCode AnalysisTask::execute() {
  return StatusCode::SUCCESS;
}


StatusCode AnalysisTask::finalize() {
  closeLog();
  return GaudiHistoAlg::finalize();
}

void AnalysisTask::getAllTasks() {
  if(m_histDB) {
    m_inputTasks.clear();
    m_histDB->getTasks(m_inputTasks);
  }
}

void AnalysisTask::checkRunStatus(int run) {
  if (!m_runAvailable) {
    info() << "Run number DIM service is now available"<<endmsg;
    m_runAvailable = true;
  }
  m_runStatus=checkStatus();
  debug() << "Start of new run "<< run << " detected: detector status is "<< m_runStatus << endmsg;
}

long AnalysisTask::checkStatus() {
  if (!m_checkStatus) return 0xffffffff; // 32 bits are ok (no check)

  long status=0;
  bool dimfound=false;
  for(unsigned int ic =0; ic< m_statusInfo.size() ; ic++) {
    int cond= m_statusInfo[ic]->getInt();
    if (cond == -1) {
      error() << "requested to check status " << m_conditionNames[ic] <<
            " but DIM service is not available"<<endmsg;
    }
    else {
      if (cond == 1)   status += (1<<ic);
      dimfound = true;
    }
  }
  if (!dimfound) status = -1;
  return status;
}
