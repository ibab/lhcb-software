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
#include "Controller/XmlTaskConfiguration.h"
#include "Controller/TasklistHandlers.h"
#include "Controller/NativeDimSlave.h"
#include "Controller/FmcSlave.h"
#include "FiniteStateMachine/Machine.h"
#include "XML/XMLTypes.h"
#include "RTL/strdef.h"
#include "RTL/rtl.h"

// C/C++ include files
#include <iostream>
#include <climits>

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
bool XmlTaskConfiguration::attachTasks(Machine& machine, const string& slave_type)  {
  char text[32];
  Tasklist tasks;
  string node = RTL::str_upper(RTL::nodeNameShort());
  string mode = RTL::str_upper(m_mode);
  TasklistAnalyzer analyzer(tasks);
  const Type* type = machine.type();
  int ALWAYS = TypedObject::ALWAYS;

  ::snprintf(text,sizeof(text),"%d",m_instances);
  DD4hep::XML::_toDictionary(Unicode("NUMBER_OF_INSTANCES"),Unicode(text));
  xml_h inventory = DD4hep::XML::DocumentHandler().load(m_config).root();
  xml_coll_t(inventory,_Unicode(task)).for_each(analyzer);
  machine.display(ALWAYS,"------------------------------------ Task list -------------------------------------");
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
      DimSlave* slave = 0;
      ::snprintf(text,sizeof(text),instances>0 ? "%02d" : "%d",int(i));
      string instance_utgid = RTL::str_replace(utgid,"${INSTANCE}",text);
      string instance_args = RTL::str_replace(arguments,"${INSTANCE}",text);
      string instance_fmc = RTL::str_replace(fmc_start,"${INSTANCE}",text)+ " -DUTGID="+instance_utgid;
      string cmd = t->command;
      machine.display(ALWAYS,"+---- Task:%s UTGID %s",t->name.c_str(),instance_utgid.c_str());
      if ( slave_type == "NativeDimSlave" )  {
	slave = new NativeDimSlave(type,instance_utgid,&machine);
	char wd[PATH_MAX];
	if ( 0 == ::getcwd(wd,sizeof(wd)) )  {
	  machine.display(ALWAYS,"|     CANNOT attch slave %s. Failed to retrieve current working directory: %s",
			  instance_utgid.c_str(),RTL::errorString());
	  return false;
	}
	cmd = wd + string("/") + t->command;
	if ( 0 != ::access(cmd.c_str(),X_OK) )  {
	  machine.display(ALWAYS,"|     CANNOT attch slave %s. File %s is not executable: %s",
			  instance_utgid.c_str(),cmd.c_str(),RTL::errorString());
	  return false;
	}
	slave->setArgs(instance_utgid+" "+instance_args);
	slave->setCommand(cmd);
	slave->cloneEnv();
      }
      else  {
	FmcSlave* s = new FmcSlave(type,instance_utgid,&machine,(m_mode != "NORMAL") && (i != 0));
	s->setFmcArgs(instance_fmc);
	s->setArgs(instance_args);
	slave = s;
	machine.display(ALWAYS,"|     tmStart -m %s %s %s %s",node.c_str(),instance_fmc.c_str(),
			cmd.c_str(),instance_args.c_str());
      }
      slave->setCommand(cmd);
      machine.addSlave(slave);
      const Type::States& states = type->states();
      for(Tasklist::Timeouts::const_iterator it=t->timeouts.begin(); it!=t->timeouts.end(); ++it)   {
	bool found = false;
	int tmo = (*it).timeout;
	if ( (*it).action == "Any" )  {
	  slave->setTimeout(tmo);
	  continue;
	}
	for(Type::States::const_iterator is=states.begin(); is!=states.end(); ++is)  {
	  const State::Transitions& tr = (*is).second->outgoing();
	  for(State::Transitions::const_iterator itr=tr.begin(); itr!=tr.end(); ++itr)  {
	    const Transition* tr = *itr;
	    if ( tr->name() == (*it).action )  {
	      slave->addTimeout(tr,tmo);
	      found = true;
	    }
	  }
	}
	if ( !found )  {
	  machine.display(machine.ERROR,"%s> Type: %s -- Cannot set timeout for action %s = %d seconds [%s]",
			  slave->c_name(), type->c_name(), it->action.c_str(), tmo, "No transition found");
	}
      }
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
