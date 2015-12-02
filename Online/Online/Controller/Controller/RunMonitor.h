/*============================================================
**
**      Finite state machine implementation to control 
**      and manipulate process groups
**      FOR HLT2 monitoring purposes
**
**  AUTHORS:
**
**      M.Frank  CERN/LHCb
**
**==========================================================*/
#ifndef ONLINE_FINITESTATEMACHINE_HLT2MONITORING_H
#define ONLINE_FINITESTATEMACHINE_HLT2MONITORING_H

// Framework include files
#include "CPP/Event.h"
#include "CPP/Interactor.h"
#include "FiniteStateMachine/Type.h"
#include "FiniteStateMachine/State.h"
#include "FiniteStateMachine/Slave.h"
#include "FiniteStateMachine/Machine.h"
#include "FiniteStateMachine/Transition.h"

// C/C++ include files
#include <string>
#include <vector>
#include <list>
#include <map>

/* 
 *  FiniteStateMachine namespace declaration
 */
namespace FiniteStateMachine {

  class Machine;
  class RunMonitorItem;
  class RunMonitorRestore;
  class RunMonitorToDoScan;
  class RunMonitorMessenger;
  class RunMonitorController;
  class RunMonitorCoordinator;

  enum {
    ELEMENT_EXISTS  = 0,
    ELEMENT_INSERTED = 1,
    ELEMENT_INVALID = 3,
    ELEMENT_LAST
  };
  enum {
    CMD_EXECUTE = 1<<20,  // RunMonitorCoordinator, RunMonitorController, RunMonitorToDoScan
    CMD_TRANSITION,
    CMD_ADD,
    CMD_CLEAR,            // RunMonitorToDoScan
    CMD_LOAD,             // RunMonitorController, 
    CMD_PAUSE,
    CMD_FINISHED,
    CMD_FAILED,
    CMD_DONE,
    CMD_PUBLISH,
    CMD_SCAN,             // RunMonitorCoordinator, RunMonitorToDoScan, RunMonitorController
    CMD_EXIT,
    CMD_RESTART,
    CMD_DEBUG,
    CMD_NDEBUG,
    CMD_LAST
  };

  ///
  /**
   *
   * \author  M.Frank
   * \date    01/03/2013
   * \version 0.1
   */
  class RunMonitorConfig    {
  public:
    std::string name;
    std::string todo;
    std::string done;
    std::string running;
    std::string failed;
    std::string archive;
    std::string slaveType;
    std::string taskType;
    /// Data directory. Files are then found in: <dataDirectory>/<item-partition name>/<item-activity name>
    std::string dataDirectory;
    size_t numWorkers;
    /// Assumed average event size in bytes
    size_t avgEventSize;
    /// Minimal number of events per file to be taken into account for monitoring
    size_t minEventsPerFile;
    /// Minimal integrated number of events for all acceptable data files
    size_t minEventsPerRun;
    /// Number of event to be processed by each monitoring task
    size_t numEventsPerRun;

    /// Sacn interval time
    int  scanInterval;

    /// Debugging flag: Debug controller structures and interactions
    bool debugCtrl;
    /// Debugging flag: Debug scanner structures and interactions
    bool debugScanner;
    /// Debugging flag: Debug messenger structures and interactions
    bool debugMessenger;
    /// Debugging flag: Use internal slaves only (debugging only)
    bool internalOnly;
    /// Create and attach a messenger object
    bool messenger;

    Slave* (*createSlave)(RunMonitorController* controller);

  public:
    /// Standard constructor
    RunMonitorConfig();
    /// Standard destructor
    virtual ~RunMonitorConfig();
    /// Recalculate depending values
    RunMonitorConfig& config();
    /// Create working directories
    bool make_dirs();
    /// Minimal individual file size
    size_t minFileSize() const  {  return minEventsPerFile*avgEventSize; }
    /// Minimal Data size of all useful files
    size_t minDataSize() const  {  return minEventsPerRun*avgEventSize;  }
  };

  /// Class to describe an activity in terms of a FSM roundtrip
  /**
   * The FSM roundtrip is defined by a queue of FSM states to be
   * executed in sequence and an optional callback executed
   * after completion or failure of the transition.
   *
   * \author  M.Frank
   * \date    01/03/2013
   * \version 0.1
   */
  class RunMonitorItem : public TypedObject  {
  public:
    typedef std::map<std::string,std::string> Properties;
    Properties  properties;
    std::string runInfo;
    std::string dataDirectory;
    std::string eventSelectorOpts;
    long        runNumber;

  protected:
    
  public:
    /// Standard constructor
    RunMonitorItem(const std::string& nam);
    /// Standard destructor
    virtual ~RunMonitorItem();
    /// Check for property existence
    bool hasProperty(const std::string& key) const;
    /// Property access by key. Returns "Unknown property:"+key if not present
    std::string property(const std::string& key) const;
    /// Print item content
    void dump() const;
    /// Dump to file
    void toFile(int fd) const;
    /// UTGID encoding
    std::string utgid(const std::string& activity) const;
    /// Build single run item from XML file
    static RunMonitorItem* fromXMLFile(const std::string& item_name, const std::string& path);
  };    // End class RunMonitorItem

 
  ///
  /**
   *
   * \author  M.Frank
   * \date    01/03/2013
   * \version 0.1
   */
  class RunMonitorController : public CPP::Interactor, public TypedObject   {
  public:
    typedef FSM::ErrCond ErrCond;
  protected:
    CPP::Interactor*   m_master;
    RunMonitorItem*    m_work;

    /// Static cache of the pointer to the error state
    const State*       m_errorState;
    /// Reference to machine object
    Machine*           m_machine;
    /// Currently executing state transition
    size_t             m_current;
    /// Slave pointer
    Slave*             m_slave;
    /// Reference to configuration object
    const RunMonitorConfig* m_config;
  public:
    /// Standard constructor
    RunMonitorController(CPP::Interactor* master, Machine* mach, const RunMonitorConfig* config);
    /// Standard destructor
    virtual ~RunMonitorController();

    Machine* machine()  const {  return m_machine; }
    RunMonitorItem* work()  const {  return m_work; }
    const RunMonitorConfig* config()  const {  return m_config;  }

    /// Print outgoing transitions for the machine in the requested state
    void printOutgoingTransitions(const State* st)   const;

    /// Print outgoing transitions for the machine in the current state
    void printOutgoingTransitions()   const;

    /// Print slaves, which are in a annomal status
    void printSlaves(bool annomal_only=true)  const;

    /// Detach work item from this controller
    void detachWork(RunMonitorItem* w)   {  m_work = w; }

    /// Prepare FSM machine and slave task to work on a new work item
    void prepareTask(RunMonitorItem* w);

    /// Interactor Interrupt handling routine
    virtual void handle(const CPP::Event& ev);

    /// Invoke a transition from the current state (if exists) by name
    ErrCond invoke_transition(const std::string& transition);

    /// Move FSM machine to the requested state by name
    ErrCond go_to_state(const std::string& state);

    /// FSM machine callback on the transition to ERROR
    ErrCond daq_err();

    /// Callback to load slave task and start FSM round-trip
    ErrCond load();
    
    /// FSM machine callback on the transition 'configure' from NOT_READY to READY
    ErrCond configure();
    
    /// FSM machine callback on the transition 'start' from READY to RUNNING
    ErrCond start();

    /// FSM machine callback on the transition 'stop' from RUNNING/PAUSE to READY
    ErrCond stop();

    /// FSM machine callback on reach the NOT_READY/OFFLINE state
    ErrCond reset();

    /// FSM machine callback on the transition 'unload' from NOT_READY to OFFLINE
    ErrCond unload();

    /// FSM callback on enter the state OFFLINE ie. when the slave finished work or got killed.
    ErrCond done();

    /// FSM machine callback on entering the state 'PAUSED'
    ErrCond paused();

    /// Generic FAILURE callback of the FSM machine
    ErrCond fail();

    /// Publish state information when transition is completed
    virtual ErrCond publish();
  };

  ///
  /**
   *
   * \author  M.Frank
   * \date    01/03/2013
   * \version 0.1
   */
  class RunMonitorCoordinator : public CPP::Interactor, public TypedObject   {
  public:
    friend class RunMonitorMessenger;

    typedef std::list<RunMonitorController*> RunMonitorers;
    typedef std::list<RunMonitorItem*> RunMonitorItems;
    typedef RunMonitorItems RunMonitorToDo;
    typedef RunMonitorItems RunMonitorRunning;
    typedef RunMonitorItems RunMonitorFinished;
    typedef RunMonitorItems RunMonitorFailed;

  protected:
    /// RunMonitor to do
    RunMonitorToDo       m_todo;
    /// RunMonitor to do
    RunMonitorRunning    m_running;
    /// RunMonitor finished and in state publishing
    RunMonitorFinished   m_done;
    /// RunMonitor finished and in state publishing
    RunMonitorFailed     m_failed;

    /// Controller container with busy actors
    RunMonitorers        m_busy;
    /// Controller container with idle actors
    RunMonitorers        m_idle;
    /// Interactor: Reference to the scanners of the todo list
    RunMonitorToDoScan*  m_scanner;
    /// Interactor: Reference to the scanners of the todo list
    RunMonitorMessenger* m_messenger;
    /// Interactor: Reference to restoration object after crash
    RunMonitorRestore*   m_restore;
    /// Reference to configuration object
    RunMonitorConfig*    m_config;

  public:

    /// Standard constructor
    RunMonitorCoordinator(const Type *typ, RunMonitorConfig* config);

    /// Standard destructor
    virtual ~RunMonitorCoordinator();

    /// Prepare monitoring item for its usefulness, and prepare all necessary actions
    bool useMonitorItem(RunMonitorItem* w);

    /// Add a new workflow instanve to the 'todo' list
    int addTodo(RunMonitorItem* work);
    /// Clear the 'todo' work item list
    void clearTodo();

    /// Clear the 'finished' work item list
    void clearDone();
    /// Adopt scanner.
    void adoptScanner(RunMonitorToDoScan* scanner);
    /// Adopt restore object after crash.
    void adoptRestore(RunMonitorRestore* restore);
    /// Adopt messenger object for DIM interface.
    void adoptMessenger(RunMonitorMessenger* messenger);

    /// Handle running work item
    bool prepareTask(RunMonitorItem* item);
    /// Handle failed work item
    void handleFailed(RunMonitorItem* item);
    /// Handle successfully completed work item
    void handleDone(RunMonitorItem* item);

    /// Interactor callback to handle external interrupts
    /**  @arg  ev    [Event,read-only]   Event structure to be handled
     */
    virtual void handle(const CPP::Event& event);
    /// IOC handler
    virtual void handleIoc(const CPP::Event& event);
    /// Network handler
    virtual void handleNetEvent(const CPP::Event& event);
    /// Timer handler
    virtual void handleTimeEvent(const CPP::Event& event);

    /// Run the entire object (never returns)
    void run(bool start_timer);
  };   // End class RunMonitorCoordinator

  /// Class to restore connections and resume work after a job restart.
  /**
   *
   * \author  M.Frank
   * \date    01/03/2013
   * \version 0.1
   */
  class RunMonitorRestore : public CPP::Interactor, public TypedObject  {
  public:
  protected:
    /// Main coordination object between the controllers, the slaves and other collaborating objects
    RunMonitorCoordinator* m_coordinator;
    /// General configuration object
    const RunMonitorConfig* m_config;

  public:
    /// Standard constructor
    RunMonitorRestore(RunMonitorCoordinator* coord, const RunMonitorConfig* cfg);
    /// Standard destructor
    virtual ~RunMonitorRestore();
    /// Interactor callback to handle external interrupts
    /**  @arg  ev    [Event,read-only]   Event structure to be handled
     */
    virtual void handle(const CPP::Event& event);
    /// Interactor callback to handle IocSensor interrupts
    virtual void handleIoc(const CPP::Event& event);
    /// Scan the work directory and re-establish work items
    virtual void scan();
  };

  /// Class to scan the todo directory with run-related todo items
  /**
   *
   * \author  M.Frank
   * \date    01/03/2013
   * \version 0.1
   */
  class RunMonitorToDoScan : public CPP::Interactor, public TypedObject  {
  public:
  protected:
    /// General configuration object
    const RunMonitorConfig* m_config;

    /// Build single run item to be worked on
    RunMonitorItem* buildItem(const char* fname)  const;

  public:
    /// Standard constructor
    RunMonitorToDoScan(const RunMonitorConfig* cfg);
    /// Standard destructor
    virtual ~RunMonitorToDoScan();
    /// Interactor callback to handle external interrupts
    /**  @arg  ev    [Event,read-only]   Event structure to be handled
     */
    virtual void handle(const CPP::Event& event);
    /// Scan the work directory and send the work to the master
    virtual void scan(CPP::Interactor* requestor);
  };

  /// Class to handle external request interrupts 
  /**
   *
   * \author  M.Frank
   * \date    01/03/2013
   * \version 0.1
   */
  class RunMonitorMessenger : public CPP::Interactor  {
  public:
    typedef std::list<RunMonitorItem*> RunMonitorItems;
    typedef std::pair<int, std::pair< std::string, const RunMonitorItems*> > PubItem;

  protected:
    /// Main coordination object between the controllers, the slaves and other collaborating objects
    RunMonitorCoordinator* m_coordinator;
    /// General configuration object
    const RunMonitorConfig* m_config;
    /// Publishing items
    PubItem m_run, m_todo, m_done, m_failed;
    /// Handle to DIM command interface
    int m_cmd;

    /// DIM callback on dis_update_service
    static void dataFeed(void* tag, void** buf, int* size, int* );
    /// DIM acllback to handle commands
    static void commandHandler(void* tag, void* address, int* size);

  public:
    /// Standard constructor
    RunMonitorMessenger(RunMonitorCoordinator* coord, const RunMonitorConfig* cfg);
    /// Standard destructor
    virtual ~RunMonitorMessenger();
    /// Interactor callback to handle external interrupts
    /**  @arg  ev    [Event,read-only]   Event structure to be handled
     */
    virtual void handle(const CPP::Event& event);
  };



  bool fsm_slave_anomaly(const Slave* s);
  std::string fsm_slave_state_string(Machine* m, bool always=false);

  ///
  /**
   *
   * \author  M.Frank
   * \date    01/03/2013
   * \version 0.1
   */
}      //  End namespace FiniteStateMachine
#endif //  ONLINE_FINITESTATEMACHINE_HLT2MONITORING_H
