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
#include "Controller/Tasklist.h"

using namespace std;
using namespace FiniteStateMachine;

/// Constructor
Tasklist::Task::Task() : instances(0)  {
}

/// Standard destructor
Tasklist::Task::~Task() {
}

/// String with the FMC parameters propery concatenated
string Tasklist::Task::fmcStartParams() const  {
  string res = "";
  if ( !group.empty() ) res += " -g "+group;
  if ( !user.empty()  ) res += " -n "+user;
  for(Params::const_iterator i=fmcParams.begin(); i!=fmcParams.end(); ++i)
    res += " "+(*i).second;
  return res;
}

/// String with the argument list propery concatenated
string Tasklist::Task::arguments() const  {
  string res;
  for(Params::const_iterator i=args.begin(); i!=args.end(); )  {
    res += (*i).first;
    if ( !(*i).second.empty() ) res += "="+(*i).second;
    ++i;
    if ( i != args.end() ) res += " ";
  }
  return res;
}

/// Constructor
Tasklist::Tasklist() {
}

/// Standard destructor
Tasklist::~Tasklist()   {
  for (Tasklist::Tasks::iterator i=tasks.begin(); i!=tasks.end(); ++i)
    delete (*i);
  tasks.clear();
}
