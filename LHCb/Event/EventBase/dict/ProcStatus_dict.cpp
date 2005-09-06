
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

#include "ProcStatus_dict.h"

using namespace seal::reflect;

//------------------------------------------------------------------------------
class ProcStatus_dict
//------------------------------------------------------------------------------
{
public:
  ProcStatus_dict();
};  

//------------------------------------------------------------------------------
static void ProcStatus_addAlgorithmStatus_0(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((ProcStatus*)v)->addAlgorithmStatus(*(const std::string*)argList[0],
                                       *(int*)argList[1]);
}

//------------------------------------------------------------------------------
static void* ProcStatus_algorithmStatus_1(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  static int ret;
  ret = ((ProcStatus*)v)->algorithmStatus(*(const std::string*)argList[0]);
  return &ret;
}

//------------------------------------------------------------------------------
static void* ProcStatus_listOfAlgorithms_2(void* v)
//------------------------------------------------------------------------------
{
  std::vector<std::string>& ret = ((ProcStatus*)v)->listOfAlgorithms();
  return (void*)&ret;
}


//------------------------------------------------------------------------------
static void* ProcStatus_aborted_3(void* v)
//------------------------------------------------------------------------------
{
  static bool ret;
  ret = ((ProcStatus*)v)->aborted();
  return &ret;
}

//------------------------------------------------------------------------------
static void ProcStatus_setAborted_4(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((ProcStatus*)v)->setAborted(*(bool*)argList[0]);
}

//------------------------------------------------------------------------------
static void* ProcStatus_algs_5(void* v)
//------------------------------------------------------------------------------
{
  const std::vector<std::string>& ret = ((ProcStatus*)v)->algs();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void ProcStatus_setAlgs_6(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((ProcStatus*)v)->setAlgs(*(const std::vector<std::string>*)argList[0]);
}

//------------------------------------------------------------------------------
static void* ProcStatus_stat_7(void* v)
//------------------------------------------------------------------------------
{
  const std::vector<int>& ret = ((ProcStatus*)v)->stat();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void ProcStatus_setStat_8(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((ProcStatus*)v)->setStat(*(const std::vector<int>*)argList[0]);
}


//------------------------------------------------------------------------------
static void* ProcStatus_constructor_0(void* mem)
//------------------------------------------------------------------------------
{
  return new(mem) ProcStatus();
}


//------------------------------------------------------------------------------
static void ProcStatus_destructor(void* mem)
//------------------------------------------------------------------------------
{
  ((ProcStatus*)mem)->~ProcStatus();
}


//------------------------------------------------------------------------------
int ProcStatus_to_DataObject(void* mem)
//------------------------------------------------------------------------------
{
  return (int)(DataObject*)(ProcStatus*)mem - (int)(ProcStatus*)mem;
}



//------------------------------------------------------------------------------
ProcStatus_dict::ProcStatus_dict()
//------------------------------------------------------------------------------
{
  ClassBuilder metaC("ProcStatus",
                     "Record the status of the Brunel processing",
                     typeid(ProcStatus),
                     sizeof(ProcStatus),
                     std::vector<const std::type_info*>(),
                     false,
                     NOCONTAINER,
                     0,
                     PUBLIC | VIRTUAL);


  static char clid_txt[64];
  sprintf(clid_txt,"%08X-0000-0000-0000-000000000000",5001);
  metaC.addProperty("ClassID", (char*)clid_txt);

  metaC.addProperty("Author", "Olivier Callot");

  ProcStatus* cl = new ProcStatus();
  metaC.addSuperClass("DataObject", 0, ProcStatus_to_DataObject);
  delete cl;


  metaC.addMethod("ProcStatus",
                  "default constructor",
                  (System::typeinfoName(typeid(ProcStatus))).c_str(),
                  ProcStatus_constructor_0,
                  PUBLIC);


  metaC.addMethod("~ProcStatus",
                  "default destructor",
                  ProcStatus_destructor,
                  PUBLIC | VIRTUAL);


  metaC.addMethod("addAlgorithmStatus",
                  "Add an entry for an algorithm",
                  (std::string("const std::basic_string<char> &")+";"+std::string("int")).c_str(),
                  ProcStatus_addAlgorithmStatus_0,
                  PUBLIC);

  metaC.addMethod("algorithmStatus",
                  "Status of this algorithm",
                  (std::string("int")).c_str(),
                  (std::string("const std::basic_string<char> &")).c_str(),
                  ProcStatus_algorithmStatus_1,
                  PUBLIC);

  metaC.addMethod("listOfAlgorithms",
                  "returns the list of algorithms for which there is a status",
                  (std::string("std::vector<std::basic_string<char> >&")).c_str(),
                  ProcStatus_listOfAlgorithms_2,
                  PUBLIC);

 
  metaC.addField("aborted",
                 (std::string("bool")).c_str(),
                 "Indicate if the processing should be aborted",
                 OffsetOf(ProcStatus, m_aborted),
                 PRIVATE);

  metaC.addField("algs",
                 (std::string("std::vector<std::basic_string<char> >")).c_str(),
                 "List of algorithms for which there is a status",
                 OffsetOf(ProcStatus, m_algs),
                 PRIVATE);

  metaC.addField("stat",
                 (std::string("std::vector<int>")).c_str(),
                 "list of status. Just for proper Event Model handling",
                 OffsetOf(ProcStatus, m_stat),
                 PRIVATE);


  metaC.addMethod("aborted",
                  "Indicate if the processing should be aborted",
                  (std::string("bool")).c_str(),
                  ProcStatus_aborted_3,
                  PUBLIC);

  metaC.addMethod("setAborted",
                  "Indicate if the processing should be aborted",
                  (std::string("bool")).c_str(),
                  ProcStatus_setAborted_4,
                  PUBLIC);

  metaC.addMethod("algs",
                  "List of algorithms for which there is a status",
                  (std::string("std::vector<std::basic_string<char> >&")).c_str(),
                  ProcStatus_algs_5,
                  PUBLIC);

  metaC.addMethod("setAlgs",
                  "List of algorithms for which there is a status",
                  (std::string("const std::vector<std::basic_string<char> >&")).c_str(),
                  ProcStatus_setAlgs_6,
                  PUBLIC);

  metaC.addMethod("stat",
                  "list of status. Just for proper Event Model handling",
                  (std::string("std::vector<int>&")).c_str(),
                  ProcStatus_stat_7,
                  PUBLIC);

  metaC.addMethod("setStat",
                  "list of status. Just for proper Event Model handling",
                  (std::string("const std::vector<int>&")).c_str(),
                  ProcStatus_setStat_8,
                  PUBLIC);



  metaC.build();
}

namespace 
{
  struct _InitDict
  {
    _InitDict()
    {
      ProcStatus_dict();
      GaudiDict::SmartRefDict<ProcStatus>();
      GaudiDict::SmartRefVectorDict<ProcStatus>();
      Class::resolvePending();
    }
  };

  static _InitDict __init;
}

void* __init_InitDict(ProcStatus* /* dummy */ )
{
  return &__init;
}
