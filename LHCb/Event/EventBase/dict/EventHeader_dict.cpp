
//   **************************************************************************
//   *                                                                        *
//   *                      ! ! ! A T T E N T I O N ! ! !                     *
//   *                                                                        *
//   *  This file was created automatically by GaudiObjDesc, please do not    *
//   *  delete it or edit it by hand.                                         *
//   *                                                                        *
//   *  If you want to change this file, first change the corresponding       *
//   *  xml-file and rerun the tools from GaudiObjDesc (or run make if you    *
//   *  are using it from inside a Gaudi-package).                            *
//   *                                                                        *
//   **************************************************************************

// Include files
#include "ReflectionBuilder/ReflectionBuilder.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/KeyedDictionary.h"


#include <string>
#include <typeinfo>

#include "EventHeader_dict.h"

using namespace seal::reflect;

//------------------------------------------------------------------------------
class EventHeader_dict
//------------------------------------------------------------------------------
{
public:
  EventHeader_dict();
};  

//------------------------------------------------------------------------------
static void* EventHeader_evtNum_0(void* v)
//------------------------------------------------------------------------------
{
  static long ret;
  ret = ((EventHeader*)v)->evtNum();
  return &ret;
}

//------------------------------------------------------------------------------
static void EventHeader_setEvtNum_1(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((EventHeader*)v)->setEvtNum(*(long*)argList[0]);
}

//------------------------------------------------------------------------------
static void* EventHeader_runNum_2(void* v)
//------------------------------------------------------------------------------
{
  static long ret;
  ret = ((EventHeader*)v)->runNum();
  return &ret;
}

//------------------------------------------------------------------------------
static void EventHeader_setRunNum_3(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((EventHeader*)v)->setRunNum(*(long*)argList[0]);
}


//------------------------------------------------------------------------------
static void* EventHeader_constructor_0(void* mem)
//------------------------------------------------------------------------------
{
  return new(mem) EventHeader();
}


//------------------------------------------------------------------------------
static void EventHeader_destructor(void* mem)
//------------------------------------------------------------------------------
{
  ((EventHeader*)mem)->~EventHeader();
}


//------------------------------------------------------------------------------
int EventHeader_to_DataObject(void* mem)
//------------------------------------------------------------------------------
{
  return (int)(DataObject*)(EventHeader*)mem - (int)(EventHeader*)mem;
}



//------------------------------------------------------------------------------
EventHeader_dict::EventHeader_dict()
//------------------------------------------------------------------------------
{
  ClassBuilder metaC("EventHeader",
                     "Global event header, created when event is first created",
                     typeid(EventHeader),
                     sizeof(EventHeader),
                     std::vector<const std::type_info*>(),
                     false,
                     NOCONTAINER,
                     0,
                     PUBLIC | VIRTUAL);


  static char clid_txt[64];
  sprintf(clid_txt,"%08X-0000-0000-0000-000000000000",101);
  metaC.addProperty("ClassID", (char*)clid_txt);

  metaC.addProperty("Author", "Marco Cattaneo");

  EventHeader* cl = new EventHeader();
  metaC.addSuperClass("DataObject", 0, EventHeader_to_DataObject);
  delete cl;


  metaC.addMethod("EventHeader",
                  "default constructor",
                  (System::typeinfoName(typeid(EventHeader))).c_str(),
                  EventHeader_constructor_0,
                  PUBLIC);


  metaC.addMethod("~EventHeader",
                  "default destructor",
                  EventHeader_destructor,
                  PUBLIC | VIRTUAL);


  metaC.addField("evtNum",
                 (std::string("long")).c_str(),
                 "Event number",
                 OffsetOf(EventHeader, m_evtNum),
                 PRIVATE);

  metaC.addField("runNum",
                 (std::string("long")).c_str(),
                 "Run number",
                 OffsetOf(EventHeader, m_runNum),
                 PRIVATE);

  metaC.addField("evtTime",
                 (System::typeinfoName(typeid(TimePoint))).c_str(),
                 "Reserved for time when event was created: DAQ or event generator",
                 OffsetOf(EventHeader, m_evtTime),
                 PRIVATE);


  metaC.addMethod("evtNum",
                  "Event number",
                  (std::string("long")).c_str(),
                  EventHeader_evtNum_0,
                  PUBLIC);

  metaC.addMethod("setEvtNum",
                  "Event number",
                  (std::string("long")).c_str(),
                  EventHeader_setEvtNum_1,
                  PUBLIC);

  metaC.addMethod("runNum",
                  "Run number",
                  (std::string("long")).c_str(),
                  EventHeader_runNum_2,
                  PUBLIC);

  metaC.addMethod("setRunNum",
                  "Run number",
                  (std::string("long")).c_str(),
                  EventHeader_setRunNum_3,
                  PUBLIC);



  metaC.build();
}

namespace 
{
  struct _InitDict
  {
    _InitDict()
    {
      EventHeader_dict();
      GaudiDict::SmartRefDict<EventHeader>();
      GaudiDict::SmartRefVectorDict<EventHeader>();
      Class::resolvePending();
    }
  };

  static _InitDict __init;
}

void* __init_InitDict(EventHeader* /* dummy */ )
{
  return &__init;
}
