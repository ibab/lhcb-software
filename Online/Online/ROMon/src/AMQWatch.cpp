// $Id: AMQWatch.cpp,v 1.2 2010-10-14 06:44:04 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Structure of basic items used to monitor the boot status
//               of the CPU farm(s)
//
//  Author     : M.Frank
//  Created    : 20/09/2010
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/AMQWatch.cpp,v 1.2 2010-10-14 06:44:04 frankb Exp $
#ifndef ONLINE_ROMON_AMQWATCH_H
#define ONLINE_ROMON_AMQWATCH_H

// Framework include files
#include "CPP/Interactor.h"
#include "ROMon/DataFile.h"

// C++ include files
#include <list>
#include <ctime>

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  /**@class AMQWatch AMQMon.h ROMon/AMQMon.h
   *
   * Class which represents the boot status of all subfarms
   *
   * @author M.Frank
   */
  class AMQWatch : public Interactor {
    struct ErrCount {
      time_t     stamp;
      size_t     count, errors;
      ErrCount(time_t t, int c, int e) : stamp(t), count(c), errors(e) {}
    };
    typedef std::list<ErrCount> Errors;
  protected:
    /// Monitor name
    std::string        m_name;
    /// Buffer for command string to be executed on error
    std::string        m_command;
    /// Sample length
    size_t             m_sampleSize;
    /// Maximum allowed number of errors in the sample
    size_t             m_maxErrors;
    /// Time difference between 2 scans
    int                m_scanDiff;
    /// Flag to test
    int                m_test;
    /// Data file object
    DataFile           m_file;
    /// Error hosty of the last couple of read cycles
    Errors             m_errors;

  public:
    /// Initializing constructor
    AMQWatch(const std::string& name);
    /// Default destructor
    virtual ~AMQWatch();
    /// Set source file name
    void setFileName(const std::string& fname) { m_file = fname;      }
    /// Set sample size
    void setSampleSize(size_t s)               { m_sampleSize = s;    }
    /// Set error threshold
    void setErrorThreshold(size_t s)           { m_maxErrors = s;     }
    /// Set time delay between 2 consecutive scans
    void setScanDelay(int diff)                { m_scanDiff = diff;   }
    /// Set command to be executed in the presence of errors
    void setCommand(const std::string& cmd)    { m_command = cmd;     }
    /// Set test flag (>0: test mode ON)
    void setTest(int flg)                      { m_test = flg;        }

    /// Show process configuration
    void show() const;

    /// Start the BOOT monitor service for all subfarms known
    virtual int start();
    /// Scan subfarms for new information
    virtual int scan();
    /// Check if errors are beyond threshold
    virtual bool checkOk()  const;
    /// Invoke corrective action
    virtual int executeAction();
    /// Interactor overload: Handle interaction event
    virtual void handle(const Event& ev);
  };

}
#endif // ONLINE_ROMON_AMQWATCH_H

// Framework include files
#include "ROMon/TaskSupervisorParser.h"
#include "RTL/rtl.h"
#include "RTL/time.h"
#include "NET/defs.h"
#include "CPP/Event.h"
#include "CPP/TimeSensor.h"
#include "ROMon/CPUMon.h"
#include "ROMonDefs.h"

// C++ include files
#include <fstream>
#include <cerrno>
#include <fcntl.h>
#include "sys/stat.h"

extern "C" {
#include "dis.h"
}

using namespace ROMon;
using namespace std;

enum { CMD_DATA=1 };

struct AMQDataProcessor : public DataFile::DataProcessor {
  mutable size_t  count, errors;
  AMQDataProcessor() : count(0), errors(0) {}
  bool operator()(const char* ptr) const {
    if ( ptr ) {
      ++count;
      if ( ::strstr(ptr,"stopped waiting for space in pendingMessage cursor for") ) {
        ++errors;
      }
    }
    return true;
  }
};

/// Initializing constructor
AMQWatch::AMQWatch(const std::string& n)
  : m_name(n), m_sampleSize(30), m_maxErrors(1), m_scanDiff(10), m_test(0), m_file("")
{
}

/// Default destructor
AMQWatch::~AMQWatch() {
}

/// Show process configuration
void AMQWatch::show() const {
  ::lib_rtl_output(LIB_RTL_ALWAYS,"%s> Process watcher configuration:",m_name.c_str());
  ::lib_rtl_output(LIB_RTL_ALWAYS,"%s> Log file: %s",m_name.c_str(),m_file.name().c_str());
  ::lib_rtl_output(LIB_RTL_ALWAYS,"%s> Error action: %s",m_name.c_str(), m_command.c_str());
  ::lib_rtl_output(LIB_RTL_ALWAYS,"%s> Sample size:             %d entries",m_name.c_str(),m_sampleSize);
  ::lib_rtl_output(LIB_RTL_ALWAYS,"%s> Max.No.Errors in samples %d errors ",m_name.c_str(),m_maxErrors);
  ::lib_rtl_output(LIB_RTL_ALWAYS,"%s> Time delay betweenscans: %d seconds",m_name.c_str(),m_scanDiff);
}

/// Start the LOG monitor to execute
int AMQWatch::start() {
  int res = m_file.open();
  if ( res ) {
    if ( !m_test ) m_file.seek_end();
    TimeSensor::instance().add(this,m_scanDiff,(void*)CMD_DATA);
    return 1;
  }
  return 0;
}

/// Start the LOG monitor to execute
int AMQWatch::scan()   {
  AMQDataProcessor proc;
  int res = m_file.scan(proc);
  if ( res )   {
    if ( m_file.migrated() )   {
      m_file.close();
      res = m_file.scan(proc);
    }
    while ( m_errors.size() > m_sampleSize ) m_errors.pop_front();
    m_errors.push_back(ErrCount(::time(0),proc.count,proc.errors));
  }
  return res;
}

/// Check if errors are beyond threshold
bool AMQWatch::checkOk()  const {
  size_t nErrs = 0;
  size_t nCounts = 0;
  for(Errors::const_iterator i=m_errors.begin(); i!=m_errors.end(); ++i) {
    nCounts += (*i).count;
    nErrs += (*i).errors;
  }
  ::lib_rtl_output(nErrs <= m_maxErrors ? LIB_RTL_DEBUG : LIB_RTL_ALWAYS,
                   "%s> %s: Found %d errors within %d scanned messages. Action necessary:%s",
                   m_name.c_str(), ::lib_rtl_timestr(), nErrs, nCounts, nErrs <= m_maxErrors ?  "NO" : "YES");
  return nErrs <= m_maxErrors;
}

/// Invoke corrective action
int AMQWatch::executeAction() {
  string cmd = m_test ? "echo "+m_command : m_command;
  ::lib_rtl_output(LIB_RTL_ALWAYS,"%s> Corrective action required:%s",m_name.c_str(),::lib_rtl_timestr());
  ::lib_rtl_output(LIB_RTL_ALWAYS,"%s> Executing %s action:%s",m_name.c_str(),m_test ? "TEST" : "",cmd.c_str());
  if ( ::system(cmd.c_str()) != -1 ) {
    m_errors.clear();
    return 1;
  }
  return 0;
}

/// Handle interaction event
void AMQWatch::handle(const Event& ev) {
  try {
    switch(ev.eventtype) {
    case TimeEvent:
      if (ev.timer_data == (void*)CMD_DATA ) {
        if ( scan() ) {
          if ( !checkOk() ) {
            executeAction();
          }
        }
        TimeSensor::instance().add(this,m_scanDiff,(void*)CMD_DATA);
      }
      break;
    default:
      break;
    }
  }
  catch(const exception& e) {
    ::lib_rtl_output(LIB_RTL_ERROR,"%s> Exception in callback processing:%s",m_name.c_str(),e.what());
  }
  catch(...) {
    ::lib_rtl_output(LIB_RTL_ERROR,"%s> UNKNOWN exception in callback processing.",m_name.c_str());
  }
}

static void help() {
  ::printf("No help found!\n");
  ::exit(0);
}

extern "C" int run_amq_watch(int argc, char *argv[])    {
  bool test = false;
  int max_errs = 100;
  int scan_delay = 5;
  int sample_size = 15;
  string node = RTL::nodeNameShort();
  string server = "/"+node+"/"+RTL::processName();
  string fname  = "/group/online/dataflow/cmtuser/libraries/activeMQ/apache-activemq-5.4.0/data/activemq.log";
  string cmd    = "tmKill -N "+node+" -m "+node+" -s 9 ActiveMQ";

  RTL::CLI cli(argc,argv,help);
  if ( cli.getopt("VERBOSE",4)!=0 )
    ::lib_rtl_install_printer(ro_rtl_print,(void*)LIB_RTL_VERBOSE);
  else if ( cli.getopt("DEBUG",4)!=0 )
    ::lib_rtl_install_printer(ro_rtl_print,(void*)LIB_RTL_DEBUG);
  else if ( cli.getopt("INFO",4)!=0 )
    ::lib_rtl_install_printer(ro_rtl_print,(void*)LIB_RTL_INFO);
  else if ( cli.getopt("WARNING",4)!=0 )
    ::lib_rtl_install_printer(ro_rtl_print,(void*)LIB_RTL_WARNING);
  else if ( cli.getopt("ERROR",4)!=0 )
    ::lib_rtl_install_printer(ro_rtl_print,(void*)LIB_RTL_ERROR);
  else
    ::lib_rtl_install_printer(ro_rtl_print,(void*)LIB_RTL_ERROR);

  cli.getopt("file",1,fname);
  cli.getopt("delay",1,scan_delay);
  cli.getopt("command",1,cmd);
  cli.getopt("maxerrors",1,max_errs);
  cli.getopt("samplesize",1,sample_size);
  test = cli.getopt("test",1) != 0;

  AMQWatch mon(RTL::processName());
  mon.setTest(test ? 1 : 0);
  mon.setCommand(cmd);
  mon.setFileName(fname);
  mon.setScanDelay(scan_delay);
  mon.setSampleSize(sample_size);
  mon.setErrorThreshold(max_errs);
  ::dis_start_serving((char*)server.c_str());
  print_startup(("Process watcher "+server).c_str());
  mon.show();
  mon.start();
  TimeSensor::instance().run();
  return 1;
}
