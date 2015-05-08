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
#include "XML/XMLTypes.h"
#include "RTL/rtl.h"

// C/C++ include files
#include <memory>
#include <cstring>
#include <sstream>
#include <stdexcept>

using namespace std;
using namespace DD4hep;
using namespace FiniteStateMachine;

/// Standard constructor
RunMonitorItem::RunMonitorItem(const string& nam)
  : TypedObject(0,nam), runNumber(-1)
{
}

/// Standard destructor
RunMonitorItem::~RunMonitorItem()   {
}

/// Print item content
void RunMonitorItem::dump() const  {
  display(ALWAYS,c_name(),"Run:%d ",runNumber);
  for(Properties::const_iterator i=properties.begin(); i!=properties.end(); ++i)
    display(ALWAYS,c_name(),"   %s : %s",(*i).first.c_str(), (*i).second.c_str());
}

/// Check for property existence
bool RunMonitorItem::hasProperty(const std::string& key) const   {
  Properties::const_iterator i = properties.find(key);
  return i != properties.end();
}

/// Property access by key. Returns "Unknown property:"+key if not present
std::string RunMonitorItem::property(const std::string& key) const   {
  Properties::const_iterator i = properties.find(key);
  if ( i != properties.end() )  return (*i).second;
  return "Unknown property:"+key;
}

/// UTGID encoding
std::string RunMonitorItem::utgid(const std::string& activity) const   {
  char new_name[1024];
  Properties::const_iterator i = properties.find("partitionname");
  ::snprintf(new_name,sizeof(new_name),"%s_%s_Run_%08ld",
	     (*i).second.c_str(),
	     activity.c_str(),
	     runNumber);
  return new_name;
}

/// Print item content
void RunMonitorItem::toFile(int fd) const  {
  char text[1024];
  Properties::const_iterator i;
  ::snprintf(text,sizeof(text),"# Summary of run %ld\n",runNumber);
  ::write(fd,text,::strlen(text));
  for(i=properties.begin(); i!=properties.end();++i)  {
    ::snprintf(text,sizeof(text),"%s = %s\n",(*i).first.c_str(),(*i).second.c_str());
    ::write(fd,text,::strlen(text));
  }
}

/// Build single run item from XML file
RunMonitorItem* RunMonitorItem::fromXMLFile(const string& item_name, const string& path)  {
  auto_ptr<RunMonitorItem> item(new RunMonitorItem(item_name));
  try {
    XML::DocumentHolder doc(XML::DocumentHandler().load(path));
    xml_elt_t runH = doc.root();
    item->runNumber = runH.attr<int>(_Unicode(runid));
    for(xml_coll_t c(runH,_Unicode(param)); c; ++c)   {
      item->properties[c.attr<string>(_U(name))] = c.attr<string>(_U(value));
    }
    return item.release();
  }
  catch(const exception& e)  {
  }
  return 0;
}
