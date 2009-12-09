// $Id: AnalysisTask.cpp,v 1.22 2009-12-09 08:42:49 ggiacomo Exp $


// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
// local
#include "OMAlib/OMAlib.h"
#include "OMAlib/OMAAlgorithms.h"
#include "OMAlib/AnalysisTask.h"
#include "OMAlib/SavesetFinder.h"

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
    m_inputTasks(0)
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
  declareProperty ( "StopAlgSequence", m_stayHere = true);
  //
  declareProperty ( "RICHclustersDir", m_RICHClDir= "/home/ryoung");
}

AnalysisTask::~AnalysisTask() {
  std::vector<SavesetFinder*>::iterator iSS;
  for(iSS = m_saveset.begin() ; iSS != m_saveset.end() ; iSS++) {
    delete (*iSS);
  }
} 


StatusCode AnalysisTask::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); 
  if ( sc.isFailure() ) return sc;  
  setMsgStream(&(always()));
  // add the partition name to the analysis task identifier
  m_anaTaskname = m_anaTaskname+"/"+m_partition;
  debug() << "Initializing Analysis Task "<<m_anaTaskname << endmsg;


  if ( ! m_inputFiles.empty() ) {
    // offline mode: switch off message dim publishing 
    m_doPublish = false;
  }
  else { // online mode
    startMessagePublishing();
  }

  // use HistDB if requested
  openDBSession();

  if( "default" != m_myRefRoot) {
    setRefRoot(m_myRefRoot);
  }

  if (! m_textLogName.empty()) {
    m_textLog=true;
    openLog();
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

    if (m_stayHere) {
      // stay here until killed
      while (1) {
        sleep(99999);
      }
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
