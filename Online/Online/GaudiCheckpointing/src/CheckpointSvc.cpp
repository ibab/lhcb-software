//  ============================================================
//
//  CheckpointSvc.h
//  ------------------------------------------------------------
//
//  Package   : GaudiCheckpointing
//
//  Author    : Markus Frank
//
//  ===========================================================
// $Id: CheckpointSvc.cpp,v 1.7 2010-03-03 13:16:49 frankb Exp $
#ifndef GAUDICHECKPOINTING_CHECKPOINTSVC_H
#define GAUDICHECKPOINTING_CHECKPOINTSVC_H

// Include files from Gaudi
#include "GaudiKernel/Service.h" 
#include "GaudiKernel/IIncidentListener.h" 

extern "C" {
#include "dis.hxx"
}

#include <map>

// Forward declarations
class IIncidentSvc;

/*
 *    LHCb namespace declaration
 */
namespace LHCb  {

  // Forward declarations
  class CheckpointSvc;
  class ITaskFSM;

  /** @class CheckpointSvc CheckpointSvc.h tests/CheckpointSvc.h
   *
   *  @author Markus Frank
   *  @date   2005-10-13
   */
  class CheckpointSvc : public Service, public IIncidentListener   {
    typedef std::map<int,int> Children;
    typedef void*             Files;

    /// Property: Name of the checkpoint file to be created
    std::string               m_checkPoint;
    /// Property: Partition name used to build the child instance name
    std::string               m_partition;
    /// Property: Task type used to build the child instance name
    std::string               m_taskType;
    /// Property: Utgid patters used to build the child instance name
    std::string               m_utgid;
    /// Property: Number of instances to be forked.
    int                       m_numInstances;
    /// Property: printout level for the checkpoint/restore mechanism    
    int                       m_printLvl;
    /// Property: Set to 1 if the child processes should become session leaders
    bool                      m_childSessions;
    /// Property: Set to 1 if the file descriptor table should be dump during child restart
    bool                      m_dumpFD;
    /// Property: Distribute children according to number of cores
    bool                      m_useCores;
    /// Property: Exit progam after producing the checkpoint file
    bool                      m_exit;

    /// Internal flag to identify the master process with respect to children
    bool                      m_masterProcess;
    /// Internal flag to indicate child checking
    bool                      m_restartChildren;
    /// Internal flag to indicate if chldren should be killed in releaseChildren
    bool                      m_killChildren;
    /// Reference to the IncidentSvc instance
    IIncidentSvc             *m_incidentSvc;
    /// Reference to the steering FSM unit (DimTaskFSM)
    ITaskFSM                 *m_fsm;
    /// Opaque file buffer to restore child environment
    Files                     m_files;
    /// Chilren map
    Children                  m_children;

  protected:

    /// Create checkpoint file from running process instance
    int saveCheckpoint();

    /// Checkpoint main process instance. Stops dim
    int stopMainInstance();
    /// Resume main process instance from checkpoint. Restarts dim
    int resumeMainInstance();
    /// Fork child process as an image of the parent
    int forkChild(int which);
    /// Watch children while running. If a child dies, restart it
    int watchChildren();
    /// Collect resources from children, which have already exited
    int waitChildren();
    /// Parse job options file (given by environment RESTARTOPTS) when restarting after checkpoint.
    int parseRestartOptions();
    /// Access the number of cors in the machines
    int numCores() const;
    /// Perform actions after writing the checkpoint file
    int finishCheckpoint();
    /// Perform all action necessary to properly startup the process after restore from the checkpoint file
    int finishRestore();

    /// Execute child process
    int execChild();

    /// Helper to build Child UTGID
    std::string buildChildUTGID(int which) const;

  public:
    /// Standard constructor
    CheckpointSvc(const std::string& nam,ISvcLocator* pSvc);

    /// Destructor
    virtual ~CheckpointSvc()  {    }

    /// IInterface overload: queryInterface
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

    /// Service overload: Initialize the service
    virtual StatusCode initialize();

    /// Service overload: Start the service
    virtual StatusCode start();

    /// Service overload: stop the service
    virtual StatusCode stop();

    /// Service overload: finalize the service
    virtual StatusCode finalize();

    /// Incident handler implemenentation: Inform that a new incident has occured
    virtual void handle(const Incident& inc);

    /// Set flag for child checking
    void restartChildren(bool value) { m_restartChildren=value; }

    /// Release children. We are no longer the owner of them!
    void releaseChildren();
  };
}
#endif // GAUDICHECKPOINTING_CHECKPOINTSVC_H
//  ============================================================
//
//  CheckpointSvc.cpp
//  ------------------------------------------------------------
//
//  Package   : GaudiCheckpointing
//
//  Author    : Markus Frank
//
//  ===========================================================
// $Id: CheckpointSvc.cpp,v 1.7 2010-03-03 13:16:49 frankb Exp $

#include "GaudiKernel/IJobOptionsSvc.h" 
#include "GaudiKernel/IIncidentSvc.h" 
#include "GaudiKernel/MsgStream.h" 
#include "GaudiOnline/ITaskFSM.h"
#include "CPP/CheckpointRestoreWrapper.h"
#include "RTL/rtl.h"

#include <cerrno>
#include <cstdlib>
#include <fcntl.h>
#ifdef _WIN32
  #include <io.h>
  typedef int pid_t;
  static inline  int   setenv(const char*, const char*, int) { return -1; }
  static inline  pid_t waitpid(int, int*, int) { return -1; }
  static inline  int   killpg(int,int) { return -1;}
  static inline  int   getpgrp() { return -1; }
  static inline  int   setsid() { return -1; }
  #define WUNTRACED     1
  #define WCONTINUED    1
  #define WNOHANG       1
  #define STDIN_FILENO  1
  #define STDOUT_FILENO 2
  #define STDERR_FILENO 3
  #define S_IWUSR       _S_IWRITE 
  #define S_IRUSR       _S_IREAD
  static const int S_IRWXU = (S_IREAD|S_IWRITE);
  static const int S_IRWXG = 0;
  static const int S_IRWXO = 0;

#else
  #include <unistd.h>
  #include <sys/wait.h>
#endif
#include <sys/stat.h>
#include <signal.h>

using namespace std;

#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,CheckpointSvc)

typedef CheckpointRestoreWrapper CHKPT;
#define CHKPT_get CheckpointRestoreWrapper__instance
#define MARKER "=============================================="

namespace  {

  /** @class Command
    *
    *  @author M.Frank
    */
  class Command : public DimCommand  {
    /// Reference to checkpoint service
    LHCb::CheckpointSvc* m_check;
    /// Reference to main task control instance
    LHCb::ITaskFSM*      m_fsm;
  public:
    /// Constructor
    Command(const std::string& nam, LHCb::CheckpointSvc* svc, LHCb::ITaskFSM* fsm) 
      : DimCommand(nam.c_str(), (char*)"C"), m_check(svc), m_fsm(fsm) { }
    /// DimCommand overload: handle DIM commands
    virtual void commandHandler()   {
      using namespace LHCb;
      // Decouple as quickly as possible from the DIM command loop !
      std::string cmd = getString();
      if      ( cmd == "configure"  ) {
	// This may never occur again for the checkpointed process!
	m_fsm->output(MSG::FATAL,"Logic error: A checkpointed process may never be initialized a second time!");
	m_fsm->setTargetState(ITaskFSM::ST_READY);
	m_fsm->declareState(ITaskFSM::ST_READY);
	m_check->restartChildren(false);
      }
      else if ( cmd == "start"      ) {
	m_fsm->setTargetState(ITaskFSM::ST_RUNNING);
	m_fsm->declareState(ITaskFSM::ST_RUNNING);
	m_check->restartChildren(true);
      }
      else if ( cmd == "stop"       ) {
	m_fsm->setTargetState(ITaskFSM::ST_STOPPED);
	m_fsm->declareState  (ITaskFSM::ST_STOPPED);
	m_check->restartChildren(false);
      }
      else if ( cmd == "reset"      ) {
	m_fsm->setTargetState(ITaskFSM::ST_NOT_READY);
	m_fsm->declareState(ITaskFSM::ST_NOT_READY);
	m_check->restartChildren(false);
      }
      else if ( cmd == "unload"     ) {
	m_fsm->setTargetState(ITaskFSM::ST_UNKNOWN);
	m_fsm->declareState(ITaskFSM::ST_UNKNOWN);	
	// Sleep for 1 second, then exit
	::lib_rtl_sleep(1000);
	m_check->releaseChildren();
	::exit(0);
      }
      else {
	m_fsm->declareState(ITaskFSM::ST_ERROR);
	m_fsm->declareSubState(ITaskFSM::UNKNOWN_ACTION);
      }
    }
  };
}

/// Standard constructor
CheckpointSvc::CheckpointSvc(const string& nam,ISvcLocator* pSvc) 
  : Service(nam,pSvc), m_incidentSvc(0), m_fsm(0), m_files(0)
{
  m_masterProcess = false;
  m_restartChildren = false;
  declareProperty("NumberOfInstances",  m_numInstances  = 0);
  declareProperty("UseCores",           m_useCores      = false);
  declareProperty("ChildSessions",      m_childSessions = false);
  declareProperty("DumpFiles",          m_dumpFD        = false);
  declareProperty("Checkpoint",         m_checkPoint    = "");
  declareProperty("PrintLevel",         m_printLvl      = MSG::WARNING);
  declareProperty("Partition",          m_partition     = "");
  declareProperty("TaskType",           m_taskType      = "Gaudi");
  declareProperty("UtgidPattern",       m_utgid         = "%N_%T_%02d");
  declareProperty("ExitAfterCheckpoint",m_exit          = true);
  declareProperty("KillChildren",       m_killChildren  = false);
}

/// IInterface implementation : queryInterface
StatusCode CheckpointSvc::queryInterface(const InterfaceID& riid, void** ppIf)   {
  if ( IIncidentListener::interfaceID().versionMatch(riid) ) {
    *ppIf = (IRunable*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return Service::queryInterface(riid, ppIf);
}


/// Service overload: Initialize the service
StatusCode CheckpointSvc::initialize() {
  StatusCode sc = Service::initialize();
  MsgStream log(msgSvc(),name());
  if ( sc.isSuccess() ) {
    typedef void* (*instance_t)();
    string proc = RTL::processName();
    instance_t func = 0;
    sc = System::getProcedureByName(0,"DimTaskFSM_instance",(System::EntryPoint*)&func);
    if( !sc.isSuccess() ) {
      log << MSG::FATAL << "Cannot access function: DimTaskFSM_instance" << endmsg;
      return sc;
    }
    log << MSG::INFO << RTL::processName() << "> Reconnect handle:" << (unsigned long)func;
    m_fsm = (ITaskFSM*)func();
    log << " Interactor: " << (unsigned long) m_fsm << endmsg;
    if ( !m_checkPoint.empty() || m_numInstances != 0 ) {
      CHKPT* chkpt =  CHKPT_get();
      chkpt->setPrint(m_printLvl);
    }
    sc = serviceLocator()->service("IncidentSvc",m_incidentSvc,true);
    if( !sc.isSuccess() ) {
      log << MSG::FATAL << "Service [IncidentSvc] not found" << endmsg;
      return sc;
    }
    m_incidentSvc->addListener(this,"APP_INITIALIZED");
    m_incidentSvc->addListener(this,"APP_STARTED");
    m_incidentSvc->addListener(this,"APP_STOPPED");
  }
  return sc;
}

/// Service overload: Start the service
StatusCode CheckpointSvc::start() {
  StatusCode sc = Service::start();
  if ( !sc.isSuccess() ) {
    MsgStream log(msgSvc(),name());
    log << MSG::FATAL << "Failed to start service base class." << endmsg;
    return sc;
  }
  if ( m_useCores || (m_numInstances != 0) )   {
    stopMainInstance();
    int n_child = m_useCores ? numCores() + m_numInstances : m_numInstances;
    for(int i=0; i<n_child; ++i)    {
      pid_t pid = forkChild(i+1);
      if ( 0 == pid )   {
	return execChild();
      }
    }
    m_restartChildren = true;
    m_state = m_targetState; // Update internal Gaudi FSM
    resumeMainInstance();    // And restore execution
    m_fsm->setTargetState(ITaskFSM::ST_RUNNING);
    m_fsm->declareState(ITaskFSM::ST_RUNNING);
    return watchChildren();  // Will never return for the parent's instance!
  }
  return sc;
}

/// Service overload: stop the service
StatusCode CheckpointSvc::stop() {
  waitChildren();
  return Service::stop();
}

/// Service overload: finalize the service
StatusCode CheckpointSvc::finalize() {
  if ( m_incidentSvc ) {
    m_incidentSvc->removeListener(this);
    m_incidentSvc->release();
    m_incidentSvc = 0;
  }
  m_fsm = 0;
  waitChildren();
  m_children.clear();
  return Service::finalize();
}

namespace {
  void string_replace(std::string& s, const std::string& pattern, const std::string& value) {
    size_t idx = string::npos;
    while( (idx=s.find(pattern)) != string::npos) {
      s.replace(idx,pattern.length(),value);
    }
  }
  string upper(const string& s) {
    string r=s;
    for(size_t i=0; i<s.length(); ++i)
      r[i] = char(toupper(s[i]));
    return r;
  }
  string lower(const string& s) {
    string r=s;
    for(size_t i=0; i<s.length(); ++i)
      r[i] = char(tolower(s[i]));
    return r;
  }
}

/// Helper to build Child UTGID
string CheckpointSvc::buildChildUTGID(int which) const {
  const string& node = RTL::nodeNameShort();
  string n = m_utgid;
  if ( !n.empty() ) {
    string_replace(n,"%PP",upper(m_partition));
    string_replace(n,"%P",m_partition);
    string_replace(n,"%p",lower(m_partition));
    string_replace(n,"%NN",upper(node));
    string_replace(n,"%N",node);
    string_replace(n,"%n",lower(node));
    string_replace(n,"%TT",upper(m_taskType));
    string_replace(n,"%T",m_taskType);
    string_replace(n,"%t",lower(m_taskType));
    char txt[1024];
    ::sprintf(txt,n.c_str(),which);
    n = txt;
  }
  else {
    n = RTL::processName();
  }
  return n;
}

/// Access the number of cors in the machines
int CheckpointSvc::numCores() const {
  int n_core=-1, fd = ::open("/proc/stat",O_RDONLY);
  if ( fd ) {
    char buff[2048], *q=0, *p=0;
    int rc = ::read(fd,buff,sizeof(buff));
    ::close(fd);
    if ( rc > 0 ) {
      for(q=p=buff; p<buff+rc;++p) {
	if ( *p == '\n' ) {
	  if( *(short*)q == *(short*)"cpu" ) {
	    if      ( q[3]==' ' ) n_core = 1;
	    else if ( q[4]==' ' ) n_core = (q[3]-'0') + 1;
	    else if ( q[5]==' ' ) n_core = (q[3]-'0')*10 + (q[4]-'0') + 1;
	  }
	  else if ( *(int*)q == *(int*)"intr" ) break;
	  q = p+1;
	}
      }
    }
  }
  return n_core;
}

/// Release children. We are no longer the owner of them!
void CheckpointSvc::releaseChildren() {
  if ( m_killChildren && !m_childSessions ) {
    // If the children are not in a process group themselves,
    // we can try and send them a SIGTERM to stop....
    int proc_group = getpgrp();
    killpg(proc_group,SIGTERM);
  }
}

/// Perform actions after writing the checkpoint file
int CheckpointSvc::finishCheckpoint() {
  if ( m_exit ) {
    const char* txt = "\n=                   Checkpoint finished. Process now exiting.\n";
    ::write(STDOUT_FILENO,MARKER,strlen(MARKER));
    ::write(STDOUT_FILENO,MARKER,strlen(MARKER));
    ::write(STDOUT_FILENO,txt,strlen(txt));
    ::write(STDOUT_FILENO,MARKER,strlen(MARKER));
    ::write(STDOUT_FILENO,MARKER,strlen(MARKER));
    ::write(STDOUT_FILENO,"\n",2);
    ::exit(0);
  }
  return StatusCode::SUCCESS;
}

/// Perform all action necessary to properly startup the process after restore from the checkpoint file
int CheckpointSvc::finishRestore() {
  CHKPT* chkpt =  CHKPT_get();
  chkpt->resume();
  chkpt->updateEnv();
  RTL::RTL_reset();
  StatusCode sc = parseRestartOptions();
  if ( !sc.isSuccess() ) {
    MsgStream err(msgSvc(),name());
    err << MSG::FATAL << "Failed to parse job options after checkpoint restart." << endmsg;
    return sc.getCode();
  }
  MsgStream log(msgSvc(),name());
  log << MSG::INFO;
  log << "Update process environment and restart options." << endmsg;
  log << "Stop threads after restart from checkpoint. " << endmsg;
  chkpt->stop();
  return StatusCode::SUCCESS;
}

/// Parse job options file (given by environment RESTARTOPTS) when restarting after checkpoint.
int CheckpointSvc::parseRestartOptions()    {
  const char* env = ::getenv("RESTARTOPTS");
  if ( env ) {
    const char* opt_path = ::getenv("JOBOPTSEARCHPATH");
    SmartIF<IJobOptionsSvc> jos(serviceLocator()->service("JobOptionsSvc",true));
    if( !jos.isValid() ) {
      throw GaudiException("Service [JobOptionsSvc] not found", name(), StatusCode::FAILURE);
    }
    // set first generic Properties
    StatusCode sc = jos->readOptions(env,opt_path ? opt_path : "");
    if( sc.isSuccess() )   {
      SmartIF<IProperty> prp(msgSvc());
      sc = jos->setMyProperties("MessageSvc", prp);
      if ( sc.isSuccess() ) {
     	sc = setProperties();
	if ( sc.isSuccess() ) {
	  RTL::RTL_reset();
	  string utgid = buildChildUTGID(0);
	  ::setenv("UTGID",utgid.c_str(),1);
	  const char* dns = ::getenv("DIM_DNS_NODE");
	  MsgStream log(msgSvc(),name());
	  log << MSG::INFO << "Processed RESTARTOPTS:" << env 
	      << " Process:" << RTL::processName() 
	      << " Node:" << RTL::nodeNameShort() 
	      << " DNS:" << (const char*)(dns ? dns : "???????")
	      << endmsg;
	  return StatusCode::SUCCESS;
	}
	MsgStream err(msgSvc(),name());
	err << MSG::FATAL << "Failed to update properties of MessageSvc." << endmsg;
	return sc.getCode();
      }
    }
    MsgStream err(msgSvc(),name());
    err << MSG::FATAL << "Failed to process RESTARTOPTS:" << env << endmsg;
    return sc.getCode();
  }
  return StatusCode::SUCCESS;
}

/// Create checkpoint file from running process instance
int CheckpointSvc::saveCheckpoint() {
  if ( !m_checkPoint.empty() ) {{
      MsgStream log(msgSvc(),name());
      log << MSG::ALWAYS << MARKER << " WRITING  CHECKPOINT " << endmsg;
    }
    int fd = ::open(m_checkPoint.c_str(),O_CREAT|O_TRUNC|O_WRONLY,S_IRWXU|S_IRWXG|S_IRWXO);
    if ( fd > 0 ) {
      CHKPT* chkpt =  CHKPT_get();
      int ret = chkpt->checkpoint(fd);
      ::close(fd);
      MsgStream log(msgSvc(),name());
      log << MSG::INFO << MARKER << " FINISHED CHECKPOINT " << endmsg;
      log << MSG::INFO << "Wrote checkpoint with " << ret << " bytes to " << m_checkPoint << endmsg;
      return StatusCode::SUCCESS;
    }
    else {
      MsgStream err(msgSvc(),name());
      err << MSG::ERROR << "Failed to create checkpoint file:" << m_checkPoint << endmsg;
    }
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

/// Checkpoint main process instance. Stops dim
int CheckpointSvc::stopMainInstance() {
  MsgStream log(msgSvc(),name());
  CHKPT* chkpt =  CHKPT_get();
  m_fsm->disconnectDIM();
  ::dis_stop_serving();
  ::dim_stop();
  ::lib_rtl_usleep(10000);
  chkpt->stop();
  if ( m_files ) ::free(m_files);
  m_files = 0;
  chkpt->getFileDescriptors(&m_files);
  if ( m_dumpFD ) {
    chkpt->dumpFileDescriptors(m_files);
  }
  return 1;
}

/// Resume main process instance from checkpoint. Restarts dim
int CheckpointSvc::resumeMainInstance() {
  CHKPT* chkpt =  CHKPT_get();
  string proc  = RTL::processName();
  const char* dns = 0;
  // Let the paret resume its work
  chkpt->resume();
  dns = ::getenv("DIM_DNS_NODE");
  MsgStream log(msgSvc(),name()); 

  log << MSG::INFO;
  log << "ProcessName:" << proc << " ";
  if ( dns ) {
    log << "DIM_DNS_NODE:" << dns << " ";
    ::dis_set_dns_node((char*)dns);
    ::dic_set_dns_node((char*)dns);
  }
  log << endmsg;
  ::dim_init();
  //
  // We have to overload the underlying dim command, since for the 
  // forker instance all Gaudi actions are over....
  // This new command make sure nothing will happen 
  // ever again inside Gaudi.
  //
  Command* command = 0;
  if ( m_useCores || (m_numInstances != 0) )   {
    if ( FSMState() == Gaudi::StateMachine::RUNNING )    {
      command = new Command(proc,this,m_fsm);
    }
  }
  m_fsm->connectDIM(command);
  ::dis_start_serving((char*)proc.c_str());
  return 1;
}

/// Fork child process as an image of the parent
int CheckpointSvc::forkChild(int which) {
  string proc  = RTL::processName();
  string utgid = buildChildUTGID(which);
  CHKPT* chkpt =  CHKPT_get();

  ::setenv("UTGID",utgid.c_str(),1);
  pid_t pid = chkpt->forkInstance();
  if ( pid == 0 ) {             // Child
    m_masterProcess = false;    // Flag child locally
  }
  else if ( pid>0 ) {
    m_masterProcess = true;
    m_children[which] = pid;
    ::setenv("UTGID",proc.c_str(),1);
  }
  else if (pid < 0)    {        // failed to fork
    MsgStream log(msgSvc(),name());
    log << MSG::FATAL << "Failed to fork child:" << ::strerror(errno) << endmsg;
    ::exit(1);
  }
  return pid;
}

/// Execute child process
int CheckpointSvc::execChild() {
  CHKPT* chkpt =  CHKPT_get();
  MsgStream log(msgSvc(),name());
  log << MSG::ALWAYS;
  for(int j=0; j<3; ++j) {
    if (dup2(j,j) < 0) {
      log << "Error dup fd:" << j << " " << ::strerror(errno) << endmsg;
    }
  }
  chkpt->setFileDescriptors(m_files);
  if ( m_childSessions ) {
    pid_t sid = ::setsid();
    if (sid < 0) {
      exit(EXIT_FAILURE);
    }
  }
  /// Need to reset RTL to get proper processnames etc.
  RTL::RTL_reset();
  const char* dns = ::getenv("DIM_DNS_NODE");
  if ( dns ) {
    ::dis_set_dns_node((char*)dns);
    ::dic_set_dns_node((char*)dns);
  }
  // Now it should be save to restart the children
  chkpt->startChild();
  string proc = RTL::processName();
  ::dim_init();
  ::dis_start_serving((char*)proc.c_str());
  m_fsm->connectDIM(0);
  return StatusCode::SUCCESS;
}

/// Watch children while running. If a child dies, restart it
int CheckpointSvc::watchChildren() {
  bool run_loop = true;
  while(run_loop)   {
    ::lib_rtl_sleep(2000);
    int count = waitChildren();
    if ( count > 0 && m_restartChildren ) {
      stopMainInstance();
      for(Children::const_iterator i=m_children.begin(); i!=m_children.end();++i) {
	int id  = (*i).first;
	if ( (*i).second == -1 ) {
	  int pid = forkChild(id);
	  if ( 0 == pid )    {
	    // This is a child process. Exit here and ensure start() is completed!
	    return execChild();
	  }
	  // Parents continue with the loop until all dead childen are restarted!
	}
      }
      resumeMainInstance();
    }
    else {
      MsgStream log(msgSvc(),name());
      log << MSG::INFO << "watchChildren:";
      for(Children::const_iterator i=m_children.begin(); i!=m_children.end();++i) {
	if ( (*i).second != -1 )
	  log << (*i).second << " ";
	else
	  log << "No." << (*i).first << "/DEAD ";
      }
      log << endmsg;
    }
  }
  return StatusCode::SUCCESS;
}

/// Collect resources from children, which have already exited
int CheckpointSvc::waitChildren() {
  pid_t w_pid = 0;
  int count = 0;
  do   {
    int status = 0;
    w_pid = ::waitpid(0, &status, WUNTRACED | WCONTINUED | WNOHANG);
    if ( w_pid > 0 ) {
      for(Children::const_iterator i=m_children.begin(); i!=m_children.end();++i) {
	int id  = (*i).first;
	int pid = (*i).second;
	if ( pid == w_pid ) {
	  m_children[id] = -1;
	  ++count;
	}
      }
    }
  } while (w_pid>0);
  if ( count == 0 ) {
    for(Children::const_iterator i=m_children.begin(); i!=m_children.end();++i) {
      if ( (*i).second == -1 ) ++count;
    }
  }
  return count;
}

/// Incident handler implemenentation: Inform that a new incident has occured
void CheckpointSvc::handle(const Incident& inc) {
  {
    MsgStream log(msgSvc(),name());
    log << MSG::INFO << "Got incident from:" << inc.source() << ": " << inc.type() << endmsg;
  }
  if ( inc.type() == "APP_INITIALIZED" ) {
    if ( !m_checkPoint.empty() ) {
      StatusCode sc;
      stopMainInstance();
      if ( !(sc=saveCheckpoint()).isSuccess() ) {
	MsgStream log(msgSvc(),name());
	log << MSG::FATAL << "Failed to create checkpoint for process:" 
	    << RTL::processName() << endmsg;
	throw GaudiException("Failed to save checkpoint:"+m_checkPoint, name(), StatusCode::FAILURE);
      }
      CHKPT* chkpt =  CHKPT_get();
      int typ = chkpt->restartType();
      sc = ( typ == 1 ) ? finishRestore() : finishCheckpoint();
      if ( !sc.isSuccess() ) {
	throw GaudiException("Failed to continue from checkpoint.", name(), StatusCode::FAILURE);
      }
      resumeMainInstance();
    }
  }
  else if ( inc.type() == "APP_STOPPED" ) {
  }
}
