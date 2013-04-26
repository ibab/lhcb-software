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
#include "FiniteStateMachine/XmlTaskConfiguration.h"
#include "FiniteStateMachine/TasklistHandlers.h"
#include "FiniteStateMachine/FmcSlave.h"
#include "FiniteStateMachine/Machine.h"
#include "XML/XMLTypes.h"
#include "RTL/strdef.h"
#include "RTL/rtl.h"

// C/C++ include files
#include <iostream>

using namespace std;
using namespace FiniteStateMachine;

/// Standard constructor
XmlTaskConfiguration::XmlTaskConfiguration(const std::string& partition, const std::string& config, const std::string& runinfo, const std::string& mode, int num_moore)
  : m_partition(partition), m_config(config), m_runinfo(runinfo), m_mode(mode), m_instances(num_moore)
{
}

/// Default destructor
XmlTaskConfiguration::~XmlTaskConfiguration()  {
}

/// Analyse the configuration file and attach the corresponding slaves to the FSM machine
bool XmlTaskConfiguration::attachTasks(Machine& machine)  {
  char text[32];
  Tasklist tasks;
  string node = RTL::str_upper(RTL::nodeNameShort());
  string mode = RTL::str_upper(m_mode);
  TasklistAnalyzer analyzer(tasks);
  const Type* type = machine.type();

  ::snprintf(text,sizeof(text),"%d",m_instances);
  DD4hep::XML::_toDictionary(Unicode("NUMBER_OF_INSTANCES"),Unicode(text));
  xml_h inventory = DD4hep::XML::DocumentHandler().load(m_config).root();
  xml_coll_t(inventory,_Unicode(task)).for_each(analyzer);
  cout << "------------------------------------ Task list -------------------------------------" << endl;
  for(Tasklist::Tasks::const_iterator i=tasks.begin(); i!=tasks.end(); ++i)  {
    Tasklist::Task* t=*i;
    size_t instances = t->instances;
    string arguments = t->arguments(), fmc_start = t->fmcStartParams(), utgid=t->utgid;
    fmc_start = RTL::str_replace(fmc_start,"${NODE}",node);
    fmc_start = RTL::str_replace(fmc_start,"${PARTITION}",m_partition);
    fmc_start = RTL::str_replace(fmc_start,"${RUNINFO}",m_runinfo);
    fmc_start = RTL::str_replace(fmc_start,"${NAME}",t->name);
    arguments = RTL::str_replace(arguments,"${NODE}",node);
    arguments = RTL::str_replace(arguments,"${PARTITION}",m_partition);
    arguments = RTL::str_replace(arguments,"${RUNINFO}",m_runinfo);
    arguments = RTL::str_replace(arguments,"${NAME}",t->name);
    utgid     = RTL::str_replace(utgid,"${NODE}",node);
    utgid     = RTL::str_replace(utgid,"${PARTITION}",m_partition);
    utgid     = RTL::str_replace(utgid,"${RUNINFO}",m_runinfo);
    utgid     = RTL::str_replace(utgid,"${NAME}",t->name);

    for(size_t i=0; i<=instances; ++i)   {
      ::snprintf(text,sizeof(text),instances>0 ? "%02d" : "%d",int(i));
      string instance_utgid = RTL::str_replace(utgid,"${INSTANCE}",text);
      FmcSlave* slave = new FmcSlave(type,instance_utgid,&machine,(m_mode != "NORMAL") && (i != 0));
      slave->setCommand(t->command);
      slave->fmc_args = RTL::str_replace(fmc_start,"${INSTANCE}",text)+ " -DUTGID="+instance_utgid;
      slave->cmd_args = RTL::str_replace(arguments,"${INSTANCE}",text);
      machine.addSlave(slave);
      cout << "+---- Task:" << t->name << " " << "UTGID:" << slave->name() << endl
	   << "|     tmStart -m " << node << " "
	   << slave->fmc_args << " " << slave->command() << " " 
	   << slave->cmd_args << endl;
    }
  }
  return true;
}

/// Analyse the configuration file and attach the corresponding slaves to the FSM machine
bool XmlTaskConfiguration::getTasks(vector<string>& task_names)  {
  char text[32];
  Tasklist tasks;
  string node = RTL::str_upper(RTL::nodeNameShort());
  TasklistAnalyzer analyzer(tasks);

  ::snprintf(text,sizeof(text),"%d",m_instances);
  DD4hep::XML::_toDictionary(Unicode("NUMBER_OF_INSTANCES"),Unicode(text));
  xml_h inventory = DD4hep::XML::DocumentHandler().load(m_config).root();
  xml_coll_t(inventory,_Unicode(task)).for_each(analyzer);
  cout << "------------------------------------ Task list -------------------------------------" << endl;
  for(Tasklist::Tasks::const_iterator i=tasks.begin(); i!=tasks.end(); ++i)  {
    Tasklist::Task* t=*i;
    size_t instances = t->instances;
    string utgid = RTL::str_replace(t->utgid,"${NODE}",node);
    utgid = RTL::str_replace(utgid,"${PARTITION}",m_partition);
    utgid = RTL::str_replace(utgid,"${RUNINFO}",m_runinfo);
    utgid = RTL::str_replace(utgid,"${NAME}",t->name);

    for(size_t i=0; i<=instances; ++i)   {
      ::snprintf(text,sizeof(text),instances>0 ? "%02d" : "%d",int(i));
      string instance_utgid = RTL::str_replace(utgid,"${INSTANCE}",text);
      task_names.push_back(instance_utgid);
    }
  }
  return true;
}
