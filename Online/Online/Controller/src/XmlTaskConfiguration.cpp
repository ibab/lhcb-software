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
#include "RTL/Sys.h"
#include "RTL/rtl.h"

// C/C++ include files
#include <iostream>
#include <sstream>
#include <climits>

using namespace std;
using namespace FiniteStateMachine;

/// Standard constructor
XmlTaskConfiguration::XmlTaskConfiguration(const string& part, const string& cfg, const string& info, const string& mode, int num_moore)
  : m_partition(part), m_config(cfg), m_runinfo(info), m_mode(mode), m_instances(num_moore)
{
}

/// Default destructor
XmlTaskConfiguration::~XmlTaskConfiguration()  {
}

/// Analyse the configuration file and attach the corresponding slaves to the FSM machine
bool XmlTaskConfiguration::attachTasks(Machine& machine, const string& slave_type, bool bind_cpu)  {
  char text[32], instances_text[64];
  Tasklist tasks;
  string node = RTL::str_upper(RTL::nodeNameShort());
  string mode = RTL::str_upper(m_mode);
  TasklistAnalyzer analyzer(tasks);
  const Type* type = machine.type();
  int print_level = TypedObject::INFO;
  map<int,string> affinity_args;
  size_t num_sockets = 1;

  affinity_args[0] = "";
  if ( bind_cpu )  {
    vector<RTL::SystemCPU> cpu_info;
    RTL::read(cpu_info);
    set<int> physical_cpu_slots = RTL::physical_cpu_ids(cpu_info);
    num_sockets = physical_cpu_slots.size();
    for(int i=0, n=int(num_sockets); i<n; ++i)   {
      stringstream str;
      set<int> pslots = RTL::processor_ids(cpu_info,i);
      for(std::set<int>::const_iterator it=pslots.begin(); it!=pslots.end();++it) 
	str << " -a " << (*it);
      affinity_args[i] = str.str();
    }
  }

  ::snprintf(instances_text,sizeof(instances_text),"%d",m_instances);
  DD4hep::XML::_toDictionary(Unicode("NUMBER_OF_INSTANCES"),Unicode(instances_text));
  xml_h inventory = DD4hep::XML::DocumentHandler().load(m_config).root();
  xml_coll_t(inventory,_Unicode(task)).for_each(analyzer);
  machine.display(print_level,"XmlConfig","------------------------------------ Task list -------------------------------------");
  for(Tasklist::Tasks::const_iterator i=tasks.begin(); i!=tasks.end(); ++i)  {
    Tasklist::Task* t = *i;
    size_t  instances = t->instances;
    string  arguments = t->arguments(), fmc_start = t->fmcStartParams(), utgid=t->utgid;
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
    map<int,int> num_instance, task_instance;

    // Init counters
    for(size_t j=0; j<=num_sockets; ++j) {
      num_instance[j] = 0;
      task_instance[j] = 0;
    }
    // Determine the number of task instances per CPU slot
    for(size_t j=0; j<=instances; ++j) 
      ++num_instance[j%num_sockets];
    // Create 'real' and 'internal' slaves
    for(size_t j=0; j<=instances; ++j)   {
      DimSlave* slave = 0;
      int cpu_slot = j%num_sockets;
      int task_id  = task_instance[cpu_slot];
      bool forking = (m_mode != "NORMAL") && (instances != 0);
      bool internal_slave = forking && j>=num_sockets;      
      if ( instances>0 )
	::snprintf(text,sizeof(text),"%1d%02d",cpu_slot,task_id);
      else
	::snprintf(text,sizeof(text),"%d",int(j));
      string instance_utgid = RTL::str_replace(utgid,"${INSTANCE}",text);
      string instance_args  = RTL::str_replace(arguments,"${INSTANCE}",text);
      string instance_fmc   = RTL::str_replace(fmc_start,"${INSTANCE}",text)+ " -DUTGID="+instance_utgid;
      string cmd = t->command;

      ++task_instance[cpu_slot];
      if ( internal_slave )
	::snprintf(instances_text,sizeof(instances_text),"0");
      else
	::snprintf(instances_text,sizeof(instances_text),"%d",int(num_instance[cpu_slot]-1));

      instance_args += " -instances=";
      instance_args += instances_text;
      machine.display(print_level,"XmlConfig","+---- Task:%s UTGID: %s. %s %s [%s slave]",
		      t->name.c_str(), instance_utgid.c_str(),
		      !internal_slave ? "Total number of processes to be forked:" : "",
		      !internal_slave ? instances_text : "",
		      !internal_slave ? "PHYSICAL" : "INTERNAL");
      if ( slave_type == "NativeDimSlave" )  {
	slave = new NativeDimSlave(type,instance_utgid,&machine,internal_slave);
	char wd[PATH_MAX];
	if ( 0 == ::getcwd(wd,sizeof(wd)) )  {
	  machine.display(print_level,"XmlConfig","|     CANNOT attach slave %s. "
			  "Failed to retrieve current working directory: %s",
			  instance_utgid.c_str(),RTL::errorString());
	  return false;
	}
	cmd = wd + string("/") + t->command;
	if ( 0 != ::access(cmd.c_str(),X_OK) )  {
	  machine.display(print_level,"XmlConfig","|     CANNOT attach slave %s. "
			  "File %s is not executable: %s",
			  instance_utgid.c_str(),cmd.c_str(),RTL::errorString());
	  return false;
	}
	slave->setArgs(instance_utgid+" "+instance_args);
	slave->cloneEnv();
      }
      else  {
	FmcSlave* s = new FmcSlave(type,instance_utgid,&machine,internal_slave);
	string affinity = (forking && !internal_slave) ? affinity_args[cpu_slot] : string("");
	s->setFmcArgs(instance_fmc+affinity);
	s->setArgs(instance_args);
	slave = s;
	if ( !internal_slave )  
	  machine.display(print_level,"XmlConfig","|     tmStart -m %s %s %s %s %s",
			  node.c_str(),instance_fmc.c_str(),affinity.c_str(),
			  cmd.c_str(),instance_args.c_str());
      }
      slave->setCommand(cmd);
      slave->setInstanceTag(int(j));
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
	  machine.display(machine.ERROR,slave->c_name(),"Type: %s -- Cannot set timeout for action %s = %d seconds [%s]",
			  type->c_name(), it->action.c_str(), tmo, "No transition found");
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
