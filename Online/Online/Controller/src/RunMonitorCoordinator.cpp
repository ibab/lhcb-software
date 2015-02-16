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

// Framework include files
#include "Controller/RunMonitor.h"
#include "FiniteStateMachine/FSMTypes.h"
#include "CPP/TimeSensor.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "RTL/readdir.h"
#include "RTL/rtl.h"
#include "dis.hxx"

// C/C++ include files
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;
using namespace FiniteStateMachine;
#define LINE_MARKS "==========================================================================================================="

namespace {
  template<typename T> void delete_items(T beg, T end)  {
    for(T i=beg; i!=end; ++i) delete (*i);
  }
}

/// Standard constructor
RunMonitorCoordinator::RunMonitorCoordinator(const Type* typ, RunMonitorConfig* cfg) 
  : Interactor(), TypedObject(typ,cfg->name),
    m_scanner(0), m_messenger(0), m_restore(0), m_config(cfg)
{
  char text[256];
  for(size_t i=0; i < m_config->numWorkers; ++i)   {
    ::snprintf(text,sizeof(text),"%s_RunMonitor_%03d",c_name(),int(i));
    Machine* mach = new Machine(type(), text);
    m_idle.push_back(new RunMonitorController(this, mach, m_config));
  }
}

/// Standard destructor
RunMonitorCoordinator::~RunMonitorCoordinator()    {
  clearTodo();
  clearDone();
  if ( m_scanner ) delete m_scanner;
  m_scanner = 0;
  if ( m_messenger ) delete m_messenger;
  m_messenger = 0;
}

/// Add a new workflow instanve to the 'todo' list
int RunMonitorCoordinator::addTodo(RunMonitorItem* work)    {
  if ( work )   {
    IocSensor::instance().send(this,CMD_ADD,work);  
    return ELEMENT_INSERTED;
  }
  return ELEMENT_INVALID;
}

/// Clear the 'todo' work item list
void RunMonitorCoordinator::clearTodo()    {
  delete_items(m_todo.begin(), m_todo.end());
}

/// Clear the 'finished' work item list
void RunMonitorCoordinator::clearDone()    {
  delete_items(m_done.begin(), m_done.end());
}

/// Adopt scanner 
void RunMonitorCoordinator::adoptScanner(RunMonitorToDoScan* scanner)    {
  if ( m_scanner ) delete m_scanner;
  m_scanner = scanner;
}

/// Adopt restore object after crash.
void RunMonitorCoordinator::adoptRestore(RunMonitorRestore* restore)   {
  if ( m_restore ) delete m_restore;
  m_restore = restore;
}

/// Adopt messenger 
void RunMonitorCoordinator::adoptMessenger(RunMonitorMessenger* messenger)    {
  if ( m_messenger ) delete m_messenger;
  m_messenger = messenger;
}

/// Prepare monitoring item for its usefulness, and prepare all necessary actions
bool RunMonitorCoordinator::useMonitorItem(RunMonitorItem* w)    {
  struct stat stat_buff;
  char data_dir[PATH_MAX], file_name[PATH_MAX];
  size_t total_bytes = 0, all_bytes = 0, num_files = 0, num_entries = 0;
  ::snprintf(data_dir,sizeof(data_dir),"%s/%s/%s/%ld",
	     m_config->dataDirectory.c_str(),
	     w->properties["partitionname"].c_str(),
	     w->properties["runtype"].c_str(),
	     w->runNumber);
  if ( 0 == ::stat(data_dir,&stat_buff) )  {
    DIR* dir = ::opendir(data_dir);
    try  {
      if ( dir )  {
	for(dirent* dp = readdir(dir); dp; dp = readdir(dir))  {
	  const char* fname = dp->d_name;
	  ++num_entries;
	  if ( *fname && ::isdigit(*fname) && ::strstr(fname,".raw") )  {
	    ::snprintf(file_name,sizeof(file_name),"%s/%s",data_dir,fname);
	    if ( 0 == ::stat(file_name,&stat_buff) && S_ISREG(stat_buff.st_mode) )  {
	      if ( size_t(stat_buff.st_size) > m_config->minFileSize() )  {
		total_bytes += stat_buff.st_size;
		++num_files;
	      }
	      else  {
		all_bytes += stat_buff.st_size;
	      }
	    }
	  }
	}
      }
    }
    catch(...)    {
      display(ALWAYS,w->c_name(),"Error scanning data directory:%s",data_dir);
    }
    if ( dir )    {
      ::closedir(dir);
    }
    /// If the files found are sufficiently large, we can start the work....
    if ( total_bytes > m_config->minDataSize() )  {
      display(ALWAYS,w->c_name(),"Work on data directory:%s #Files:%ld #Bytes:%ld",
	      data_dir, long(num_files), long(total_bytes));
      return true;
    }
    display(ALWAYS,w->c_name(),"Work item %s rejected. Directory:%s %s %s Num.Entries:%ld Files:%ld Bytes:%ld / %ld [Need:%ld]",
	    w->c_name(), data_dir, dir ? "" : "[ACCESS FAILED]", dir ? "" : RTL::errorString(),
	    long(num_entries), long(num_files),long(total_bytes),long(all_bytes),long(m_config->minDataSize()));
    return false;
  }
  display(ALWAYS,w->c_name(),"Non existing data directory:%s [%s]",data_dir,RTL::errorString());
  return false;
}

/// Interactor callback to handle external interrupts
/**  @arg  ev    [Event,read-only]   Event structure to be handled
 */
void RunMonitorCoordinator::handle(const Event& event)    {
  try {
    dim_lock();
    switch(event.eventtype) {
    case TimeEvent:
      handleTimeEvent(event);
      break;    
    case NetEvent:
      handleNetEvent(event);
      break;
    case IocEvent:
      handleIoc(event);
      break;
    default:
      break;
    }
  }
  catch(...)  {
  }
  dim_unlock();
}

/// Network handler
void RunMonitorCoordinator::handleNetEvent(const Event& /* event */)   {
}

/// Timer handler
void RunMonitorCoordinator::handleTimeEvent(const Event& event)   {
  if ( event.timer_data == (void*)CMD_SCAN )   {
    IocSensor::instance().send(this,CMD_SCAN);
    TimeSensor::instance().add(this,m_config->scanInterval,(void*)CMD_SCAN);
  }
}

/// IOC handler
void RunMonitorCoordinator::handleIoc(const Event& event)    {
  char text[64];
  static int w_id = 0;
  switch(event.type)  {

  case CMD_DEBUG:
    m_config->debugCtrl = true;
    m_config->debugScanner = true;
    m_config->debugMessenger = true;
    break;

  case CMD_NDEBUG:
    m_config->debugCtrl = false;
    m_config->debugScanner = false;
    m_config->debugMessenger = false;
    break;

  case CMD_ADD:  {
    RunMonitorItem* w = (RunMonitorItem*)event.data;
    m_todo.push_back(w);
    IocSensor::instance().send(this,CMD_EXECUTE);
    break;
  }

  case CMD_SCAN:
    if ( m_scanner )  {
      // Send command to target & ream scan timer
      if ( m_todo.empty() )  {
	IocSensor::instance().send(m_scanner, CMD_SCAN, this);
      }
      IocSensor::instance().send(this,CMD_EXECUTE);
    }
    else  {
      display(ALWAYS,c_name(),"Coordinator: Request to rearm scanner, but not command target present!");
    }
    break;

  case CMD_DONE:   {
    RunMonitorController*     c = (RunMonitorController*)event.data;
    RunMonitorers::iterator   i = find(m_busy.begin(),m_busy.end(),c);
    m_busy.erase(i);
    m_idle.push_back(c);
    display(ALWAYS,c_name(),"Controller: DONE -- Todo:%d Busy:%d Idle:%d Done:%d",
	    int(m_todo.size()), int(m_busy.size()), int(m_idle.size()), int(m_done.size()) );
    IocSensor::instance().send(this,CMD_EXECUTE,c);
    break;
  }

  case CMD_EXECUTE:
    if ( m_todo.size() > 0 && m_idle.size() > 0 )  {
      RunMonitorController* c = m_idle.front();
      for(RunMonitorToDo::iterator i=m_todo.begin(); i!=m_todo.end(); ++i)  {
	RunMonitorItem* w = (*i);
	if ( useMonitorItem(w) )   {
	  display(ALWAYS,c_name(),"%s",LINE_MARKS);
	  if ( prepareTask(w) )  {
	    c->prepareTask(w);
	    m_todo.erase(i);	    
	    m_idle.pop_front();
	    m_busy.push_back(c);
	    w->properties["FSM-Slave:PREPARED"] = fsm_slave_state_string(c->machine());
	    display(ALWAYS,c_name(),"Coordinator: Starting to work on:%s -- Todo:%d Busy:%d Idle:%d Done:%d",
		    w->c_name(), int(m_todo.size()), int(m_busy.size()), int(m_idle.size()), int(m_done.size()) );
	    IocSensor::instance().send(c,CMD_LOAD);
	    IocSensor::instance().send(m_messenger,CMD_PUBLISH);
	    return;
	  }
	  else  {
	    handleFailed(w);
	    display(ALWAYS,c_name(),"Controller: TO_IMPLEMENT: FAILED to work on:%s -- Todo:%d Busy:%d Idle:%d Done:%d",
		    w->c_name(), int(m_todo.size()), int(m_busy.size()), int(m_idle.size()), int(m_done.size()) );
	    IocSensor::instance().send(this,CMD_EXECUTE);
	    IocSensor::instance().send(m_messenger,CMD_PUBLISH);
	    return;
	  }
	}
	else   {
	  /// Check how old the run actually is.....
	  display(ALWAYS,c_name(),"Run %ld was taken started %s and still not worked on...",
		  w->runNumber, w->property("startdate").c_str());
	}
      }
    }
    break;

  case CMD_FINISHED:   {
    RunMonitorController*     c = (RunMonitorController*)event.data;
    RunMonitorItem*           w = c->work();
    c->prepareTask(0);
    if ( w ) handleDone(w);
    if ( w ) w->properties["FSM-Slave:FINAL_DONE"] = fsm_slave_state_string(c->machine());
    IocSensor::instance().send(this,CMD_DONE,c);
    break;
  }

  case CMD_FAILED:   {
    RunMonitorController*     c = (RunMonitorController*)event.data;
    RunMonitorItem*           w = c->work();
    c->detachWork(0);
    if ( w ) handleFailed(w);
    if ( w ) w->properties["FSM-Slave:FINAL_FAIL"] = fsm_slave_state_string(c->machine());
    display(ALWAYS,c_name(),"Controller: TO_IMPLEMENT: FAILED to work on:%s -- Todo:%d Busy:%d Idle:%d Done:%d",
	    w ? w->c_name() : "UNKNOWN", int(m_todo.size()), int(m_busy.size()), int(m_idle.size()), int(m_done.size()) );
    IocSensor::instance().send(c, CMD_FAILED, (void*)new string("recover"));
    break;
  }

  case CMD_RESTART:
    ::sprintf(text,"RunMonitor_%d",++w_id);
    display(ALWAYS,c_name(),"Controller: Adding new work %s ....... Todo:%d Busy:%d Idle:%d Done:%d",
	    text, int(m_todo.size()), int(m_busy.size()), int(m_idle.size()), int(m_done.size()) );
    IocSensor::instance().send(this, CMD_ADD, new RunMonitorItem(text));
    break;

  default:
    break;
  }
}

/// Handle running work item
bool RunMonitorCoordinator::prepareTask(RunMonitorItem* item)   {
  RunMonitorRunning::iterator ir = find(m_todo.begin(),m_todo.end(),item);
  string n = "/" + item->name();
  string running = m_config->running + n;
  string todo = m_config->todo + n;
  if ( ir != m_todo.end() ) m_todo.erase(ir);
  m_running.push_back(item);

  int ret = ::rename(todo.c_str(), running.c_str());
  if ( 0 != ret )   {
    display(ALWAYS,"RUNNING","Failed to rename work item from %s to %s.",todo.c_str(), running.c_str());
    return false;
  }

  char data_dir[PATH_MAX], text[PATH_MAX];
  ::snprintf(data_dir,sizeof(data_dir),"%s/%s/%s/%ld",
	     m_config->dataDirectory.c_str(),
	     item->properties["partitionname"].c_str(),
	     item->properties["runtype"].c_str(),
	     item->runNumber);
  ::snprintf(text,sizeof(text),"%s/%ld/EventSelector.opts",m_config->running.c_str(),item->runNumber);
  int fd = ::open(text,O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
  if ( fd )  {
    item->eventSelectorOpts = text;
    ::snprintf(text,sizeof(text),
	       "// Options for Run %ld  generated at %s.\n"
	       "EventSelector.Input       = \"%s\";\n"
	       "EventSelector.FilePrefix  = \"\";\n"
	       "EventSelector.DeleteFiles = false;\n"
	       "EventSelector.EvtMax      = %ld;\n"
	       "ApplicationMgr.EvtMax     = %ld;\n",
	       item->runNumber, ::lib_rtl_timestr(), data_dir, 
	       long(m_config->numEventsPerRun),
	       long(m_config->numEventsPerRun)
	       );
    int to_write = ::strlen(text);
    int wr = ::write(fd,text,to_write);
    if ( wr < to_write )  {
      printf("FAILURE: Write:%d bytes of %d bytes  %s   %s\n",wr, to_write, text,RTL::errorString());
    }
    ::close(fd);
    item->dataDirectory = data_dir;
    return true;
  }
  display(ALWAYS,item->c_name(),"Failed to write options file for:%s error:%s",
	  item->c_name(),RTL::errorString());
  return false;
}

/// Handle failed work item
void RunMonitorCoordinator::handleFailed(RunMonitorItem* item)   {
  RunMonitorRunning::iterator ir = find(m_running.begin(),m_running.end(),item);
  string n = "/" + item->name();
  string running = m_config->running + n;
  string fail = m_config->failed + n;
  char text[PATH_MAX];
  if ( ir != m_running.end() ) m_running.erase(ir);
  m_failed.push_back(item);

  /// Write out Failure summary for this run analysis
  ::snprintf(text,sizeof(text),"%s/%ld/FAILURE.txt",m_config->running.c_str(),item->runNumber);
  int fd = ::open(text,O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
  if ( fd )  {
    ::snprintf(text,sizeof(text),"# FAILURE Summary for analysis of run %ld\n",item->runNumber);
    ::write(fd,text,::strlen(text));
    item->toFile(fd);
    ::close(fd);
  }
  int ret = ::rename(running.c_str(), fail.c_str());
  if ( 0 != ret )   {
    display(ALWAYS,"FAILED","Failed to rename work item from %s to %s.",running.c_str(), fail.c_str());
  }
}

/// Handle successfully completed work item
void RunMonitorCoordinator::handleDone(RunMonitorItem* item)   {
  RunMonitorRunning::iterator ir = find(m_running.begin(),m_running.end(),item);
  string n = "/" + item->name();
  string running = m_config->running + n;
  string done = m_config->done + n;
  if ( ir != m_running.end() ) m_running.erase(ir);
  m_done.push_back(item);
  int ret = ::rename(running.c_str(), done.c_str());
  if ( 0 != ret )   {
    display(ALWAYS,"FINISHED","Failed to rename work item from %s to %s.",running.c_str(), done.c_str());
  }
}

/// Run the entire object (never returns)
void RunMonitorCoordinator::run(bool start_timer)   {
  if ( m_restore && start_timer )   {
    IocSensor::instance().send(m_restore,CMD_EXECUTE,this);
  }
  if ( m_scanner && start_timer )   {
    TimeSensor::instance().add(this,5,(void*)CMD_SCAN);
  }
  IocSensor::instance().run();
}
