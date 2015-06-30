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
#include <ctime>

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

    /// Fork time queue
    std::list<time_t>         m_forkQueue;
    /// Restore options
    std::vector<std::string>  m_restoreOptionClients;
    /// Property: Name of the checkpoint file to be created
    std::string               m_checkPoint;
    /// Property: Partition name used to build the child instance name
    std::string               m_partition;
    /// Property: Task type used to build the child instance name (default: 'Gaudi')
    std::string               m_taskType;
    /// Property: Utgid patters used to build the child instance name (default: '%N_%T_%02d')
    std::string               m_utgid;
    /// Property: lib directory for libraries contained in the checkpoint (default:/dev/shm/checkpoint)
    std::string               m_libDirectory;
    /// Property: Checkpoint save flags (0=None, 2=Save_libs[default])
    int                       m_saveFlags;
    /// Property: Checkpoint restart flags  (default: 0)
    int                       m_restartFlags;

    /// Property: Number of instances to be forked.  (default: 0)
    int                       m_numInstances;
    /// Property: printout level for the checkpoint/restore mechanism (default:WARNING)
    int                       m_printLvl;
    /// Property: first child index for fork mechanism (default: 0)
    int                       m_firstChild;
    /// Property: Exit progam after producing the checkpoint file (default: 1[true])
    int                       m_exit;
    /// Property: Wait for children with debugger for n seconds (default: 0 ms)
    int                       m_childWait;
    /// Property to make children sleep after fork ( default:250 ms)
    int                       m_childSleep;
    /// Property to force UTGID to environ and argv[0] (default: 0 [false])
    int                       m_forceUTGID;
    /// Property: Length of queue to store time-stamps to forks
    int                       m_forkQueLen;
    /// Property: Minimal time allowed to fork children with the queue length
    time_t                    m_forkDistance;
    /// Property: Invoke incident APP_INITIALIZED from service on start (default: false)
    bool                      m_invokeIncident;
    /// Property: Instance is running online and re-establishes the DIM connection
    bool                      m_connectDIM;
    /// Property: Set to 1 if the child processes should become session leaders
    bool                      m_childSessions;
    /// Property: Set to 1 if the file descriptor table should be dump during child restart
    bool                      m_dumpFD;
    /// Property: Distribute children according to number of cores
    bool                      m_useCores;

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
    /// Debug hook: set option CheckpointSvc.ChildWait to something big. Then connect with gdb
    void debugWait(int value) const;

    /// Create checkpoint file from running process instance
    int saveCheckpoint();

    /// Checkpoint main process instance. Stops dim
    int stopMainInstance();
    /// Resume main process instance from checkpoint. Restarts dim
    int resumeMainInstance(bool with_resume_children);
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
    /// Check the frequency of forking children
    void checkForkFrequency();

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

    /// Service overload: restart the service
    virtual StatusCode restart();

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
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiOnline/ITaskFSM.h"
#include "Checkpointing/Chkpt.h"
#include "Checkpointing/Namespace.h"
#include "RTL/rtl.h"

#include <cerrno>
#include <cstdlib>
#include <climits>
#include <fcntl.h>
#ifdef _WIN32
#include <io.h>
#include <sys/stat.h>
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
static const int S_IRWXU = (_S_IREAD|_S_IWRITE);
static const int S_IRWXG = 0;
static const int S_IRWXO = 0;

#else
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <syscall.h>
#endif
#include <sys/stat.h>
#include <signal.h>

using namespace std;

#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
using namespace LHCb;
DECLARE_SERVICE_FACTORY(CheckpointSvc)

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
      : DimCommand(nam.c_str(),"C"), m_check(svc), m_fsm(fsm) { }
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
      else if ( cmd == "pause"      ) {
        m_fsm->setTargetState(ITaskFSM::ST_PAUSED);
        m_fsm->declareState  (ITaskFSM::ST_PAUSED);
        m_check->restartChildren(false);
      }
      else if ( cmd == "continue"   ) {
        m_fsm->setTargetState(ITaskFSM::ST_RUNNING);
        m_fsm->declareState(ITaskFSM::ST_RUNNING);
        m_check->restartChildren(false);
      }
      else if ( cmd == "reset"      ) {
        m_fsm->setTargetState(ITaskFSM::ST_NOT_READY);
        m_fsm->declareState(ITaskFSM::ST_NOT_READY);
        m_check->restartChildren(false);
      }
      else if ( cmd == "!state" )  {
        const std::string& old_state = m_fsm->stateName();
        m_fsm->_declareState(old_state);
        return;
      }
      else if ( cmd == "unload" || cmd == "recover" || cmd == "RESET" ) {
        m_fsm->setTargetState(ITaskFSM::ST_UNKNOWN);
        m_fsm->declareState(ITaskFSM::ST_UNKNOWN);
        // Sleep for 1 second, then exit
        ::lib_rtl_sleep(1000);
        m_check->releaseChildren();
        ::_exit(EXIT_SUCCESS);
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
  m_masterProcess = true;
  m_restartChildren = false;
  declareProperty("NumberOfInstances",      m_numInstances  = 0);
  declareProperty("UseCores",               m_useCores      = false);
  declareProperty("ChildSessions",          m_childSessions = false);
  declareProperty("DumpFiles",              m_dumpFD        = false);
  declareProperty("Checkpoint",             m_checkPoint    = "");
  declareProperty("PrintLevel",             m_printLvl      = MSG::WARNING);
  declareProperty("Partition",              m_partition     = "");
  declareProperty("TaskType",               m_taskType      = "Gaudi");
  declareProperty("UtgidPattern",           m_utgid         = "%N_%T_%02d");
  declareProperty("ExitAfterCheckpoint",    m_exit          = 1);
  declareProperty("KillChildren",           m_killChildren  = false);
  declareProperty("FirstChild",             m_firstChild    = 1);
  declareProperty("ChildWait",              m_childWait     = 0);
  declareProperty("ChildSleep",             m_childSleep    = 250);
  declareProperty("ForceUtgid",             m_forceUTGID    = 0);
  declareProperty("RestoreOptionClients",   m_restoreOptionClients);
  declareProperty("CheckpointLibs",         m_libDirectory  = "/dev/shm/checkpoint");
  declareProperty("CheckpointSaveFlags",    m_saveFlags     = CHECKPOINTING_NAMESPACE::MTCP_SAVE_LIBS);
  declareProperty("CheckpointRestartFlags", m_restartFlags  = 0);
  declareProperty("InvokeIncident",         m_invokeIncident= false);
  declareProperty("ConnectToDIM",           m_connectDIM    = true);
  declareProperty("ForkQueueLength",        m_forkQueLen    = 30); // Store last 10 subsequent forks
  declareProperty("ForkTimeDistance",       m_forkDistance  = 60); // Maximally once per minute
}

/// IInterface implementation : queryInterface
StatusCode CheckpointSvc::queryInterface(const InterfaceID& riid, void** ppIf)   {
  if ( IIncidentListener::interfaceID().versionMatch(riid) ) {
    *ppIf = (IIncidentListener*)this;
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
    if ( m_connectDIM ) {
      sc = System::getProcedureByName(0,"DimTaskFSM_instance",(System::EntryPoint*)&func);
      if( !sc.isSuccess() ) {
        log << MSG::FATAL << "Cannot access function: DimTaskFSM_instance" << endmsg;
        return sc;
      }
      log << MSG::INFO << RTL::processName() << "> Reconnect handle:" << (unsigned long)func;
      m_fsm = (ITaskFSM*)func();
    }
    log << " Interactor: " << (unsigned long) m_fsm << endmsg;
    if ( m_numInstances < 0 ) {
      m_numInstances = ::atol(::getenv("NBOFSLAVES"));
    }
    if ( !m_checkPoint.empty() || m_numInstances != 0 ) {
      checkpointing_set_print_level(m_printLvl);
    }
    sc = serviceLocator()->service("IncidentSvc",m_incidentSvc,true);
    if( !sc.isSuccess() ) {
      log << MSG::FATAL << "Service [IncidentSvc] not found" << endmsg;
      return sc;
    }
    m_incidentSvc->addListener(this,"APP_INITIALIZED");
    m_incidentSvc->addListener(this,"APP_RUNNING");
    m_incidentSvc->addListener(this,"APP_STOPPED");
    m_forkQueue.clear();
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
  if ( m_invokeIncident ) {
    m_incidentSvc->fireIncident(Incident(name(),"APP_INITIALIZED"));
  }
  if ( m_masterProcess )    {  // Only the master does anything on start!
    m_forkQueue.clear();
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
      m_state = m_targetState;     // Update internal Gaudi FSM
      resumeMainInstance(true);    // And restore execution
      if ( m_connectDIM )  {
        m_fsm->setTargetState(ITaskFSM::ST_RUNNING);
        m_fsm->declareState(ITaskFSM::ST_RUNNING);
      }
      return watchChildren();  // Will never return for the parent's instance!
    }
  }
  return sc;
}

/// Service overload: stop the service
StatusCode CheckpointSvc::stop() {
  waitChildren();
  return Service::stop();
}

/// Service overload: restart the service
StatusCode CheckpointSvc::restart() {
  return StatusCode::SUCCESS;
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

//const string& node = RTL::nodeNameShort();
void CheckpointSvc::debugWait(int value)  const  {
  bool dbg;// = ( node == "hlte0103" ) && ( m_childWait>0 );
  dbg = true;
  if ( dbg )  {
    bool r = true;
    int cnt = value;
    if ( cnt>0 ) {
      char text[1024];
      ::snprintf(text,sizeof(text),"[ERROR] +++ Going into wait loop (%d seconds)"
                 " for debugging: --pid %d\n",value,(int)syscall(SYS_getpid));
      ::write(STDOUT_FILENO,text,strlen(text));
    }
    while( r && cnt>0 ) {
      --cnt;
      ::lib_rtl_sleep(1000);
    }
  }
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
    char txt[1024];
    string utgid = RTL::processName();
    if ( utgid.length()>2 ) utgid = utgid.substr(0,utgid.length()-2);
    string_replace(n,"%UTGID",utgid);
    string_replace(n,"%PP",upper(m_partition));
    string_replace(n,"%P",m_partition);
    string_replace(n,"%p",lower(m_partition));
    string_replace(n,"%NN",upper(node));
    string_replace(n,"%N",node);
    string_replace(n,"%n",lower(node));
    string_replace(n,"%TT",upper(m_taskType));
    string_replace(n,"%T",m_taskType);
    string_replace(n,"%t",lower(m_taskType));
    ::snprintf(txt,sizeof(txt),n.c_str(),which+m_firstChild);
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
  if ( fd != -1 ) {
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
    size_t len = strlen(MARKER);
    ::write(STDOUT_FILENO,MARKER,len);
    ::write(STDOUT_FILENO,MARKER,len);
    ::write(STDOUT_FILENO,txt,strlen(txt));
    ::write(STDOUT_FILENO,MARKER,len);
    ::write(STDOUT_FILENO,MARKER,len);
    ::write(STDOUT_FILENO,"\n",2);
    if ( m_exit > 1 ) ::_exit(EXIT_SUCCESS);
    ::exit(EXIT_SUCCESS);
  }
  return StatusCode::SUCCESS;
}

/// Perform all action necessary to properly startup the process after restore from the checkpoint file
int CheckpointSvc::finishRestore() {
  // %%HACK%% Sleep 3 secs to let threads pass immediate actions before we modify the
  // environment.
  ::lib_rtl_sleep(1000);

  checkpointing_update_environment();
  RTL::RTL_reset();
  StatusCode sc = parseRestartOptions();
  if ( !sc.isSuccess() ) {
    MsgStream err(msgSvc(),name());
    err << MSG::FATAL << "Failed to parse job options after checkpoint restart." << endmsg;
    return sc.getCode();
  }
  // If debugging is enabled (m_childWait>0) call debug hook
  // Do not call before, because we need the new values from the options!
  debugWait(m_childWait);

  if ( ::getenv("TEST_CHECKPOINT") )  {
    ::fprintf(stdout,"%s%s\n",MARKER,MARKER);
    ::fprintf(stdout,"=  RESTORE TEST WAS SUCCESSFUL.\n");
    ::fprintf(stdout,"=  Looks like this checkpoint is working.\n");
    ::fprintf(stdout,"=  The process will now exit.\n");
    ::fprintf(stdout,"%s%s\n",MARKER,MARKER);
    ::fflush(stdout);
    ::lib_rtl_sleep(3000);
    ::_exit(EXIT_SUCCESS);
  }
  MsgStream log(msgSvc(),name());
  log << MSG::INFO << "Updated process environment and restart options." << endmsg;
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
          string utgid = buildChildUTGID(m_firstChild);
          checkpointing_set_utgid(utgid.c_str());
          const char* dns = ::getenv("DIM_DNS_NODE");
          MsgStream log(msgSvc(),name());
          SmartIF<IAlgManager> algMgr(serviceLocator());
          for(vector<string>::const_iterator i=m_restoreOptionClients.begin(); i!=m_restoreOptionClients.end();++i) {
            const string& nam = *i;
            IAlgorithm* alg = 0;
            IProperty* prop = 0;
            sc = StatusCode::FAILURE;
            if ( (sc=algMgr->getAlgorithm(nam,alg)).isSuccess() ) {
              prp = alg;
              sc = jos->setMyProperties(alg->name(), prp);
            }
            else if ( (sc=serviceLocator()->service(nam,prop)).isSuccess() )  {
              sc = jos->setMyProperties(nam, prop);
            }
            if ( !sc.isSuccess() )  {
              log << MSG::FATAL << "Failed to update properties of:" << nam << endmsg;
              return sc;
            }
          }
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
    if ( !m_libDirectory.empty() )  {
      m_saveFlags |= CHECKPOINTING_NAMESPACE::MTCP_SAVE_LIBS;
      checkpointing_set_lib_directory(m_libDirectory.c_str());
    }
    if ( 0 != m_saveFlags ) {
      checkpointing_set_save_flags(m_saveFlags);
    }
    int fd = ::open(m_checkPoint.c_str(),O_CREAT|O_TRUNC|O_WRONLY|O_LARGEFILE,S_IRWXU|S_IRWXG|S_IRWXO);
    if ( fd != -1 ) {
      int ret = checkpointing_write_checkpoint(fd);
      write(3,"[Error] Restore complete.....\n",31);
      ::close(fd);
      if ( ret == 1 )   {
        //!!!!!!! No printing here! other threads may lock output!!!!!!!
        // MsgStream log(msgSvc(),name());
        // log << MSG::INFO << MARKER;
        // log << " FINISHED loading process from checkpoint..."
        //    << "Continue processing... " << endmsg;
      }
      else {
        MsgStream log(msgSvc(),name());
        log << MSG::INFO << MARKER << " FINISHED CHECKPOINT " << endmsg;
        if ( m_connectDIM ) {
          log << "Wrote checkpoint with " << ret << " bytes to " << m_checkPoint << endmsg;
        }
      }
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
  // First get rid of DIM
  // Debug ONLY: checkpointing_set_print_level(1);
  if ( m_connectDIM ) {
    m_fsm->disconnectDIM();
  }
  ::lib_rtl_sleep(100);
  if ( m_files ) ::free(m_files);
  m_files = 0;
  // Now stop child threads
  checkpointing_stop_process();
  checkpointing_get_file_descriptors(&m_files);
  if ( m_dumpFD ) {
    checkpointing_dump_file_descriptors(m_files);
  }
  return 1;
}

/// Resume main process instance from checkpoint. Restarts dim
int CheckpointSvc::resumeMainInstance(bool with_resume_child_threads) {
  string proc  = RTL::processName();
  const char* dns = 0;
  // Let the paret resume its work
  if ( with_resume_child_threads )   {
    checkpointing_resume_process();
    ::lib_rtl_sleep(200);
  }
  dns = ::getenv("DIM_DNS_NODE");
  MsgStream log(msgSvc(),name());
  log << MSG::INFO;
  log << "ProcessName:" << proc << " ";
  if ( dns && m_connectDIM ) {
    log << "DIM_DNS_NODE:" << dns << " ";
    ::dis_set_dns_node(dns);
    ::dic_set_dns_node(dns);
  }
  log << endmsg;
  //
  // We have to overload the underlying dim command, since for the
  // forker instance all Gaudi actions are over....
  // This new command make sure nothing will happen
  // ever again inside Gaudi.
  //
  if ( m_connectDIM ) {
    Command* command = 0;
    if ( m_useCores || (m_numInstances != 0) )   {
      if ( FSMState() == Gaudi::StateMachine::RUNNING )    {
        command = new Command(proc,this,m_fsm);
      }
    }
    m_fsm->connectDIM(command);
  }
  return 1;
}

/// Fork child process as an image of the parent
int CheckpointSvc::forkChild(int which) {
  string proc  = RTL::processName();
  string utgid = buildChildUTGID(which);

  checkpointing_set_utgid(utgid.c_str());
  pid_t pid = checkpointing_fork_process();
  if ( pid == 0 ) {             // Child
    m_masterProcess = false;    // Flag child locally
    m_numInstances = 0;
    m_useCores = false;
    if ( m_forceUTGID>0 ) {
      checkpointing_force_utgid(utgid.c_str());
    }
    if ( m_childSleep>0 ) {
      int w = which%10;
      ::lib_rtl_sleep(m_childSleep*w);
    }
  }
  else if ( pid>0 ) {
    m_masterProcess = true;
    m_children[which] = pid;
    checkpointing_set_utgid(proc.c_str());
  }
  else if (pid < 0)    {        // failed to fork
    bool do_wait = true;
    char text[512];
    int err = errno;
    ::snprintf(text,sizeof(text),"[ERROR] Failed to fork child:%s -> %s [%d]\n",utgid.c_str(),::strerror(err),err);
    while (do_wait)   {
      ::write(STDOUT_FILENO,text,strlen(text));
      ::write(STDOUT_FILENO,text,strlen(text));
      ::write(STDERR_FILENO,text,strlen(text));
      //::_exit(EXIT_FAILURE);
      ::lib_rtl_sleep(5000);
    }
  }
  return pid;
}

/// Execute child process
int CheckpointSvc::execChild() {
  for(int j=0; j<3; ++j) {
    if (dup2(j,j) < 0) {
      char text[256];
      ::snprintf(text,sizeof(text),"[ERROR] Severe error dup fd:%d %s\n",j,::strerror(errno));
      ::write(STDOUT_FILENO,text,strlen(text));
    }
  }
  /// Restore file descriptors for child process
  checkpointing_set_file_descriptors(m_files);
  /// If requested assign seperate process group to child
  if ( m_childSessions ) {
    pid_t sid = ::setsid();
    if (sid < 0) {
      char text[1024];
      ::snprintf(text,sizeof(text),"[ERROR] Cannot assign process group to child: %s\n",::strerror(errno));
      ::write(STDOUT_FILENO,text,strlen(text));
      ::_exit(EXIT_FAILURE);
    }
  }
  // If debugging is enabled (m_childWait>0) call debug hook
  debugWait(m_childWait);
  /// Need to reset RTL to get proper processnames etc. afterwards
  RTL::RTL_reset();
  /// Now it should be save to restart the children
  checkpointing_start_child();
  /// Configure DIM startup
  const char* dns = ::getenv("DIM_DNS_NODE");
  string proc = RTL::processName();
  if ( m_connectDIM ) {
    if ( dns ) {
      ::dis_set_dns_node(dns);
      ::dic_set_dns_node(dns);
    }
    m_fsm->connectDIM(0);
  }
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
          if ( m_forkQueLen > 0 )  {
            m_forkQueue.push_back(time(0));
            if ( int(m_forkQueue.size()) > m_forkQueLen ) m_forkQueue.pop_front();
          }
        }
      }
      resumeMainInstance(true);
      checkForkFrequency();
    }
    else {
      MsgStream log(msgSvc(),name());
      log << MSG::DEBUG << "watchChildren:";
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

/// Check the frequency of forking children
void CheckpointSvc::checkForkFrequency()  {
  /// Children were forked so often, that the queue is full.
  /// Check if the time distance is still OK.
  if ( m_forkQueLen > 0 && m_forkQueLen == int(m_forkQueue.size()) )   {
    long start = LONG_MAX, end = LONG_MIN;
    for(std::list<time_t>::const_iterator i=m_forkQueue.begin(); i != m_forkQueue.end(); ++i) {
      time_t stamp = *i;
      if (start>stamp) start = stamp;
      if (end<stamp) end = stamp;
    }
    if ( (end-start) < m_forkDistance )   {
      // Need to do something here: Best is to issue an error and sleep forever
      // DIM commands will still be handled though by the command, which will also
      // stop us if the flag m_restartChildren is going to false.
      MsgStream log(msgSvc(),name());
      log << MSG::FATAL << "HLT_FUCKED: Forked the last " << m_forkQueLen 
          << " children in only " << (end-start) << " seconds. THIS IS NOT NORMAL." << endmsg;
      log << MSG::FATAL << "Will stop forking until reset command." << endmsg;
      m_restartChildren = false;
    }
  }
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
      int typ = checkpointing_restart_type();
      bool restore =  typ == 1;
      sc = ( restore ) ? finishRestore() : finishCheckpoint();
      if ( !sc.isSuccess() ) {
        throw GaudiException("Failed to continue from checkpoint.", name(), StatusCode::FAILURE);
      }
      resumeMainInstance(!restore);
    }
  }
  if ( inc.type() == "APP_INITIALIZED" ) {
  }
  else if ( inc.type() == "APP_RUNNING" ) {
    // string proc  = RTL::processName();
    //    ::dis_start_serving(proc.c_str());
    ::lib_rtl_sleep(500);
    checkpointing_dump_threads();
  }
  else if ( inc.type() == "APP_STOPPED" ) {
  }
}
