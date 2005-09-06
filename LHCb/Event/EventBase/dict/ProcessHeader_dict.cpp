
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

#include "ProcessHeader_dict.h"

using namespace seal::reflect;

//------------------------------------------------------------------------------
class ProcessHeader_dict
//------------------------------------------------------------------------------
{
public:
  ProcessHeader_dict();
};  

//------------------------------------------------------------------------------
static void* ProcessHeader_randomSeeds_0(void* v)
//------------------------------------------------------------------------------
{
  const std::vector < int >& ret = ((ProcessHeader*)v)->randomSeeds();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void ProcessHeader_setRandomSeeds_1(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((ProcessHeader*)v)->setRandomSeeds(*(const std::vector < int >*)argList[0]);
}

//------------------------------------------------------------------------------
static void* ProcessHeader_applicationName_2(void* v)
//------------------------------------------------------------------------------
{
  const std::string& ret = ((ProcessHeader*)v)->applicationName();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void ProcessHeader_setApplicationName_3(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((ProcessHeader*)v)->setApplicationName(*(const std::string*)argList[0]);
}

//------------------------------------------------------------------------------
static void* ProcessHeader_applicationVersion_4(void* v)
//------------------------------------------------------------------------------
{
  const std::string& ret = ((ProcessHeader*)v)->applicationVersion();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void ProcessHeader_setApplicationVersion_5(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((ProcessHeader*)v)->setApplicationVersion(*(const std::string*)argList[0]);
}


//------------------------------------------------------------------------------
static void* ProcessHeader_constructor_0(void* mem)
//------------------------------------------------------------------------------
{
  return new(mem) ProcessHeader();
}


//------------------------------------------------------------------------------
static void ProcessHeader_destructor(void* mem)
//------------------------------------------------------------------------------
{
  ((ProcessHeader*)mem)->~ProcessHeader();
}


//------------------------------------------------------------------------------
int ProcessHeader_to_DataObject(void* mem)
//------------------------------------------------------------------------------
{
  return (int)(DataObject*)(ProcessHeader*)mem - (int)(ProcessHeader*)mem;
}



//------------------------------------------------------------------------------
ProcessHeader_dict::ProcessHeader_dict()
//------------------------------------------------------------------------------
{
  ClassBuilder metaC("ProcessHeader",
                     "Common base class for all process headers: GenHeader, MCHeader... @todo ASK MARCO FOR ID",
                     typeid(ProcessHeader),
                     sizeof(ProcessHeader),
                     std::vector<const std::type_info*>(),
                     false,
                     NOCONTAINER,
                     0,
                     PUBLIC | VIRTUAL);


  static char clid_txt[64];
  sprintf(clid_txt,"%08X-0000-0000-0000-000000000000",199);
  metaC.addProperty("ClassID", (char*)clid_txt);

  metaC.addProperty("Author", "P. Koppenburg");

  ProcessHeader* cl = new ProcessHeader();
  metaC.addSuperClass("DataObject", 0, ProcessHeader_to_DataObject);
  delete cl;


  metaC.addMethod("ProcessHeader",
                  "default constructor",
                  (System::typeinfoName(typeid(ProcessHeader))).c_str(),
                  ProcessHeader_constructor_0,
                  PUBLIC);


  metaC.addMethod("~ProcessHeader",
                  "default destructor",
                  ProcessHeader_destructor,
                  PUBLIC | VIRTUAL);


  metaC.addField("randomSeeds",
                 (std::string("std::vector < int >")).c_str(),
                 "random number seeds for this step (int or long?)",
                 OffsetOf(ProcessHeader, m_randomSeeds),
                 PRIVATE);

  metaC.addField("applicationName",
                 (std::string("std::basic_string<char>")).c_str(),
                 "Application or Program Name (e.g. Gauss, Boole or Pythia)",
                 OffsetOf(ProcessHeader, m_applicationName),
                 PRIVATE);

  metaC.addField("applicationVersion",
                 (std::string("std::basic_string<char>")).c_str(),
                 "Application or Program version",
                 OffsetOf(ProcessHeader, m_applicationVersion),
                 PRIVATE);


  metaC.addMethod("randomSeeds",
                  "random number seeds for this step (int or long?)",
                  (std::string("std::vector < int >&")).c_str(),
                  ProcessHeader_randomSeeds_0,
                  PUBLIC);

  metaC.addMethod("setRandomSeeds",
                  "random number seeds for this step (int or long?)",
                  (std::string("const std::vector < int >&")).c_str(),
                  ProcessHeader_setRandomSeeds_1,
                  PUBLIC);

  metaC.addMethod("applicationName",
                  "Application or Program Name (e.g. Gauss, Boole or Pythia)",
                  (std::string("std::basic_string<char> &")).c_str(),
                  ProcessHeader_applicationName_2,
                  PUBLIC);

  metaC.addMethod("setApplicationName",
                  "Application or Program Name (e.g. Gauss, Boole or Pythia)",
                  (std::string("const std::basic_string<char> &")).c_str(),
                  ProcessHeader_setApplicationName_3,
                  PUBLIC);

  metaC.addMethod("applicationVersion",
                  "Application or Program version",
                  (std::string("std::basic_string<char> &")).c_str(),
                  ProcessHeader_applicationVersion_4,
                  PUBLIC);

  metaC.addMethod("setApplicationVersion",
                  "Application or Program version",
                  (std::string("const std::basic_string<char> &")).c_str(),
                  ProcessHeader_setApplicationVersion_5,
                  PUBLIC);



  metaC.build();
}

namespace 
{
  struct _InitDict
  {
    _InitDict()
    {
      ProcessHeader_dict();
      GaudiDict::SmartRefDict<ProcessHeader>();
      GaudiDict::SmartRefVectorDict<ProcessHeader>();
      Class::resolvePending();
    }
  };

  static _InitDict __init;
}

void* __init_InitDict(ProcessHeader* /* dummy */ )
{
  return &__init;
}
