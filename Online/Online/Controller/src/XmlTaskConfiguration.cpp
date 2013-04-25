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
#include "RTL/rtl.h"

// C/C++ include files
#include <iostream>

using namespace std;
using namespace FiniteStateMachine;

static string _replace(const string& src, const string& pattern, const string& replacement)   {
  string res = src;
  for(size_t id=res.find(pattern); id != string::npos; id = res.find(pattern) )
    res.replace(id,pattern.length(),replacement);
  return res;
}

/// Standard constructor
XmlTaskConfiguration::XmlTaskConfiguration(const string& config) : m_config(config) 
{
}

/// Default destructor
XmlTaskConfiguration::~XmlTaskConfiguration()  {
}

/// Analyse the configuration file and attach the corresponding slaves to the FSM machine
bool XmlTaskConfiguration::attachTasks(Machine& machine, const string& partition, 
				       const string& runinfo, const string& mode, 
				       int num_moore)
{
  char text[32];
  Tasklist tasks;
  TasklistAnalyzer analyzer(tasks);
  const Type* type = machine.type();

  ::snprintf(text,sizeof(text),"%d",int(num_moore));
  DD4hep::XML::_toDictionary(Unicode("NUMBER_OF_INSTANCES"),Unicode(text));
  xml_h inventory = DD4hep::XML::DocumentHandler().load(m_config).root();
  xml_coll_t(inventory,_Unicode(task)).for_each(analyzer);
  cout << "------------------------------------ Task list -------------------------------------" << endl;
  for(Tasklist::Tasks::const_iterator i=tasks.begin(); i!=tasks.end(); ++i)  {
    Tasklist::Task* t=*i;
    size_t instances = t->instances;
    string arguments = t->arguments(), fmc_start = t->fmcStartParams(), utgid=t->utgid;
    fmc_start = _replace(fmc_start,"${NODE}",RTL::nodeNameShort());
    fmc_start = _replace(fmc_start,"${PARTITION}",partition);
    fmc_start = _replace(fmc_start,"${RUNINFO}",runinfo);
    fmc_start = _replace(fmc_start,"${NAME}",t->name);
    arguments = _replace(arguments,"${NODE}",RTL::nodeNameShort());
    arguments = _replace(arguments,"${PARTITION}",partition);
    arguments = _replace(arguments,"${RUNINFO}",runinfo);
    arguments = _replace(arguments,"${NAME}",t->name);
    utgid = _replace(utgid,"${NODE}",RTL::nodeNameShort());
    utgid = _replace(utgid,"${PARTITION}",partition);
    utgid = _replace(utgid,"${RUNINFO}",runinfo);
    utgid = _replace(utgid,"${NAME}",t->name);

    for(size_t i=0; i<=instances; ++i)   {
      ::snprintf(text,sizeof(text),"%02d",int(i));
      string instance_utgid = _replace(utgid,"${INSTANCE}",text);
      FmcSlave* slave = new FmcSlave(type,instance_utgid,&machine,(mode != "NORMAL") && (i != 0));
      slave->setCommand(t->command);
      slave->fmc_args = _replace(fmc_start,"${INSTANCE}",text);
      slave->cmd_args = _replace(arguments,"${INSTANCE}",text);
      machine.addSlave(slave);
      cout << "+---- Task:" << t->name << " " << "UTGID:" << slave->name() << endl
	   << "|     tmStart "
	   << slave->fmc_args << " " << slave->command() << " " 
	   << slave->cmd_args << endl;
    }
  }
  return false;
}

