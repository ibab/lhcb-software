// $Id: AnalysisTask.cpp,v 1.10 2009-03-04 09:33:52 ggiacomo Exp $


// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
// local
#include "OMAlib/OMAlib.h"
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
  m_anaTaskname = m_anaTaskname+"_"+m_partition;

  // use HistDB if requested
  if(m_useDB)
    openDBSession( m_DBpw, m_DBuser, m_DB );

  if( "default" != m_myRefRoot) 
    setRefRoot(m_myRefRoot);


  if ( ! m_inputFiles.empty() ) {
    // single shot: analyze these files now 
    std::vector<std::string>::iterator iF;
    for(iF =  m_inputFiles.begin() ; iF != m_inputFiles.end() ; iF++) {
      info() <<"Analyzing histograms on file "<<*iF<<endmsg;
      analyze( *iF , m_inputTasks.empty() ? "unknownTask" : m_inputTasks[0] );
    }
  }
  else {
    std::vector<std::string>::iterator iF;
    if(m_inputTasks.size()>0) {
      if (m_inputTasks[0]=="any") 
        getAllTasks();
    }
    for(iF =  m_inputTasks.begin() ; iF != m_inputTasks.end() ; iF++) {
      info()<< "listening to latest saveset for task "<<*iF << endmsg;
      m_saveset.push_back( new SavesetFinder(this, *iF, m_partition) );
    }
  }
  
  return StatusCode::SUCCESS;
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
  return GaudiHistoAlg::finalize();  
}

void AnalysisTask::getAllTasks() {
  if(m_histDB) {
    m_inputTasks.clear();
    m_histDB->getTasks(m_inputTasks);
  }
}
