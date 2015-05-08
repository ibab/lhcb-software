/*============================================================
**
**      Ctrl process management on a HLT worker node
**
**  AUTHORS:
**
**      M.Frank
**
**==========================================================*/
#include "RTL/rtl.h"
#include "XML/XMLTypes.h"
#include "Controller/TasklistHandlers.h"

// C/C++ include files
#include <iostream>
#include <signal.h>

using namespace std;
using namespace FiniteStateMachine;

extern "C" int fsm_xml_test(int argc, char** argv)  {
  string name;
  Tasklist tasks;
  RTL::CLI cli(argc, argv, 0);
  TasklistAnalyzer analyzer(tasks);
  
  cli.getopt("name",1,name);

  xml_h inventory = DD4hep::XML::DocumentHandler().load(name).root();
  DD4hep::XML::_toDictionary(Unicode("NUMBER_OF_INSTANCES"),Unicode("12"));
  xml_coll_t(inventory,_Unicode(task)).for_each(TasklistPrinter());
  xml_coll_t(inventory,_Unicode(task)).for_each(analyzer);

  cout << "------------------------------------ Task list -------------------------------------" << endl;
  for(Tasklist::Tasks::const_iterator i=tasks.begin(); i!=tasks.end(); ++i)  {
    Tasklist::Task* t=*i;
    cout << "+---- Task:" << t->name << endl
	 << "|     tmStart -u " << t->name << " "
	 << t->fmcStartParams() << " " << t->command << " " 
	 << t->arguments() << endl;
    cout << "|     tmStop  -u " << t->name << " -s " << SIGTERM << endl;
    cout << "|     tmKill  -u " << t->name << " -s " << SIGKILL << endl;
  }
  return 1;
}
