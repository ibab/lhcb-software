/*============================================================
**
**      Finite state machine implementation to control 
**      and manipulate process groups
**
**  AUTHORS:
**
**      M.Frank  CERN/LHCb
**
**==========================================================*/
// Framework include files
#include "FiniteStateMachine/TasklistHandlers.h"
#include "XML/XMLTypes.h"

// C/C++ include files
#include <iostream>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::XML;
using namespace FiniteStateMachine;

DECLARE_UNICODE_TAG(tasks_inventory);
DECLARE_UNICODE_TAG(task);
DECLARE_UNICODE_TAG(command);
DECLARE_UNICODE_TAG(argument);
DECLARE_UNICODE_TAG(fmcparam);
DECLARE_UNICODE_TAG(ioparam);
DECLARE_UNICODE_TAG(user);
DECLARE_UNICODE_TAG(group);
DECLARE_UNICODE_TAG(instances);
DECLARE_UNICODE_TAG(timeout);
DECLARE_UNICODE_TAG(from);
DECLARE_UNICODE_TAG(to);

namespace FiniteStateMachine {
  struct TaskParams : public xml_h {
    TaskParams(xml_h h) : xml_h(h) {}
    std::string name()  const     { return  attr<std::string>(_U(name));  }
    std::string user()  const     { return  hasAttr(_U(user))  ? attr<std::string>(_U(user))  : string(); }
    std::string group() const     { return  hasAttr(_U(group)) ? attr<std::string>(_U(group)) : string(); }
    size_t      instances() const { return  hasAttr(_U(instances)) ? attr<int>(_U(instances)) : 0; }
    std::string command() const   { return  child(_U(command)).text();   }
    bool hasValue() const         { return  hasAttr(_U(value));           }
    std::string value() const     { return  attr<std::string>(_U(value)); }
    std::string from() const      { return  attr<std::string>(_U(from)); }
    std::string to() const        { return  attr<std::string>(_U(to)); }
    int timeout() const           { return  attr<int>(_U(timeout)); }
  };
}

/// Action operator when analyzing data
void TasklistPrinter::operator()(const xml_h& h)  {
  TaskParams t = h;
  cout << "+-- Task:" << t.name() << "[" << t.user() << "," << t.group() << "] " << t.instances() << " instances"
       << endl << "|   Command:" << t.command()
       << endl << "|   IO:     ";
  xml_coll_t(t,_U(ioparam)).for_each(Args());
  cout << endl << "|   Args:   ";
  xml_coll_t(t,_U(argument)).for_each(Args());
  cout << endl << "|   FMC:    ";
  xml_coll_t(t,_U(fmcparam)).for_each(Params());
  cout << endl 
       << "+-----------------------------------------------------------------------------------" 
       << endl;
}

/// Action operator when analyzing data
void TasklistPrinter::Args::operator()(const xml_h& h)  {
  TaskParams a = h;
  cout << a.name() << "=" << a.value() << " ";
}

/// Action operator when analyzing data
void TasklistPrinter::Timeouts::operator()(const xml_h& h)  {
  TaskParams a = h;
  cout << endl << "   Timeout: " << a.from() << " -> " << a.to() << " = " << a.timeout();
}

/// Action operator when analyzing data
void TasklistPrinter::Params::operator()(const xml_h& h)  {
  TaskParams p = h;
  string nam = p.name();
  string val = p.hasValue() ? p.value() : string();
  if      ( nam == "define"                 ) cout << " -D " << val;
  else if ( nam == "utgid"                  ) cout << " -u " << val;
  else if ( nam == "workdirectory"          ) cout << " -w " << val;
  else if ( nam == "priority"               ) cout << " -r " << val;
  else if ( nam == "nice"                   ) cout << " -p " << val;
  else if ( nam == "stderr" && val.empty()  ) cout << " -e";
  else if ( nam == "stdout" && val.empty()  ) cout << " -o";
  else if ( nam == "stderr"                 ) cout << " -E " << val;
  else if ( nam == "stdout"                 ) cout << " -O " << val;
  else if ( nam == "affinity"               ) cout << " -a " << val;
  else if ( nam == "daemon"                 ) cout << " -d ";
}

/// Action operator when analyzing data
void TasklistAnalyzer::operator()(const xml_h& h)  {
  TaskParams t = h;
  Task* task = new Task();
  tasks.tasks.push_back(task);
  task->name    = t.name();
  task->user    = t.user();
  task->group   = t.group();
  task->command = t.command();
  task->instances = t.instances();
  xml_coll_t(t,_U(argument)).for_each(Args(task));
  xml_coll_t(t,_U(fmcparam)).for_each(Params(task));
  xml_coll_t(t,_U(ioparam)).for_each(Params(task));
}

/// Action operator when analyzing data
void TasklistAnalyzer::Args::operator()(const xml_h& h)  {
  TaskParams a = h;
  string val = a.hasValue() ? a.value() : string();
  task->args.push_back(make_pair(a.name(),val));
}

/// Action operator when analyzing data
void TasklistAnalyzer::Timeouts::operator()(const xml_h& h)  {
  TaskParams a = h;
  task->timeouts.push_back(Tasklist::Timeout(a.from(),a.to(),a.timeout()));
}

/// Action operator when analyzing data
void TasklistAnalyzer::Params::operator()(const xml_h& h)  {
  TaskParams p = h;
  string fmc_par, nam = p.name(), val = p.hasValue() ? p.value() : string();
  if      ( nam == "define"                 ) fmc_par = "-D " + val;
  else if ( nam == "workdirectory"          ) fmc_par = "-w " + val;
  else if ( nam == "priority"               ) fmc_par = "-r " + val;
  else if ( nam == "nice"                   ) fmc_par = "-p " + val;
  else if ( nam == "stderr" && val.empty()  ) fmc_par = "-e";
  else if ( nam == "stdout" && val.empty()  ) fmc_par = "-o";
  else if ( nam == "stderr"                 ) fmc_par = "-E " + val;
  else if ( nam == "stdout"                 ) fmc_par = "-O " + val;
  else if ( nam == "affinity"               ) fmc_par = "-a " + val;
  else if ( nam == "daemon"                 ) fmc_par = "-d ";
  else if ( nam == "utgid"                  ) task->utgid = val;
  else if ( nam == "output"                 ) task->ioParams.push_back(make_pair(nam,val));
  else if ( nam == "input"                  ) task->ioParams.push_back(make_pair(nam,val));

  if ( !fmc_par.empty() )  {
    task->fmcParams.push_back(make_pair(nam,fmc_par));
  }
}
