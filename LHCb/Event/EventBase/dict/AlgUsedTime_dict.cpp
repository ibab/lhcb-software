
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

#include "AlgUsedTime_dict.h"

using namespace seal::reflect;

//------------------------------------------------------------------------------
class AlgUsedTime_dict
//------------------------------------------------------------------------------
{
public:
  AlgUsedTime_dict();
};  

//------------------------------------------------------------------------------
static void* AlgUsedTime_algName_0(void* v)
//------------------------------------------------------------------------------
{
  const std::string& ret = ((AlgUsedTime*)v)->algName();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void AlgUsedTime_setAlgName_1(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((AlgUsedTime*)v)->setAlgName(*(const std::string*)argList[0]);
}

//------------------------------------------------------------------------------
static void* AlgUsedTime_userTime_2(void* v)
//------------------------------------------------------------------------------
{
  static float ret;
  ret = ((AlgUsedTime*)v)->userTime();
  return &ret;
}

//------------------------------------------------------------------------------
static void AlgUsedTime_setUserTime_3(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((AlgUsedTime*)v)->setUserTime(*(float*)argList[0]);
}

//------------------------------------------------------------------------------
static void* AlgUsedTime_elapTime_4(void* v)
//------------------------------------------------------------------------------
{
  static float ret;
  ret = ((AlgUsedTime*)v)->elapTime();
  return &ret;
}

//------------------------------------------------------------------------------
static void AlgUsedTime_setElapTime_5(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((AlgUsedTime*)v)->setElapTime(*(float*)argList[0]);
}


//------------------------------------------------------------------------------
static void* AlgUsedTime_constructor_0(void* mem)
//------------------------------------------------------------------------------
{
  return new(mem) AlgUsedTime();
}


//------------------------------------------------------------------------------
static void AlgUsedTime_destructor(void* mem)
//------------------------------------------------------------------------------
{
  ((AlgUsedTime*)mem)->~AlgUsedTime();
}


//------------------------------------------------------------------------------
int AlgUsedTime_to_KeyedObjectLtintGt(void* mem)
//------------------------------------------------------------------------------
{
  return (int)(KeyedObject<int>*)(AlgUsedTime*)mem - (int)(AlgUsedTime*)mem;
}



//------------------------------------------------------------------------------
AlgUsedTime_dict::AlgUsedTime_dict()
//------------------------------------------------------------------------------
{
  ClassBuilder metaC("AlgUsedTime",
                     "store name of algorithm instance and CPU time it used",
                     typeid(AlgUsedTime),
                     sizeof(AlgUsedTime),
                     std::vector<const std::type_info*>(),
                     false,
                     NOCONTAINER,
                     0,
                     PUBLIC | VIRTUAL);


  static char clid_txt[64];
  sprintf(clid_txt,"%08X-0000-0000-0000-000000000000",5002);
  metaC.addProperty("ClassID", (char*)clid_txt);

  metaC.addProperty("Author", "Massimiliano Ferro-Luzzi");

  AlgUsedTime* cl = new AlgUsedTime();
  metaC.addSuperClass("KeyedObject<int>", 0, AlgUsedTime_to_KeyedObjectLtintGt);
  delete cl;


  metaC.addMethod("AlgUsedTime",
                  "default constructor",
                  (System::typeinfoName(typeid(AlgUsedTime))).c_str(),
                  AlgUsedTime_constructor_0,
                  PUBLIC);


  metaC.addMethod("~AlgUsedTime",
                  "default destructor",
                  AlgUsedTime_destructor,
                  PUBLIC | VIRTUAL);


  metaC.addField("algName",
                 (std::string("std::basic_string<char>")).c_str(),
                 "name of timed algorithm instance",
                 OffsetOf(AlgUsedTime, m_algName),
                 PRIVATE);

  metaC.addField("userTime",
                 (std::string("float")).c_str(),
                 "USER time consumed by algorithm instance",
                 OffsetOf(AlgUsedTime, m_userTime),
                 PRIVATE);

  metaC.addField("elapTime",
                 (std::string("float")).c_str(),
                 "ELAPSED time consumed by algorithm instance",
                 OffsetOf(AlgUsedTime, m_elapTime),
                 PRIVATE);


  metaC.addMethod("algName",
                  "name of timed algorithm instance",
                  (std::string("std::basic_string<char> &")).c_str(),
                  AlgUsedTime_algName_0,
                  PUBLIC);

  metaC.addMethod("setAlgName",
                  "name of timed algorithm instance",
                  (std::string("const std::basic_string<char> &")).c_str(),
                  AlgUsedTime_setAlgName_1,
                  PUBLIC);

  metaC.addMethod("userTime",
                  "USER time consumed by algorithm instance",
                  (std::string("float")).c_str(),
                  AlgUsedTime_userTime_2,
                  PUBLIC);

  metaC.addMethod("setUserTime",
                  "USER time consumed by algorithm instance",
                  (std::string("float")).c_str(),
                  AlgUsedTime_setUserTime_3,
                  PUBLIC);

  metaC.addMethod("elapTime",
                  "ELAPSED time consumed by algorithm instance",
                  (std::string("float")).c_str(),
                  AlgUsedTime_elapTime_4,
                  PUBLIC);

  metaC.addMethod("setElapTime",
                  "ELAPSED time consumed by algorithm instance",
                  (std::string("float")).c_str(),
                  AlgUsedTime_setElapTime_5,
                  PUBLIC);



  metaC.build();
}

namespace 
{
  struct _InitDict
  {
    _InitDict()
    {
      AlgUsedTime_dict();
      GaudiDict::KeyedObjectDict<int>();
      GaudiDict::SmartRefDict<AlgUsedTime>();
      GaudiDict::SmartRefVectorDict<AlgUsedTime>();
      GaudiDict::VectorDict<AlgUsedTime*>();
      GaudiDict::ObjectVectorDict<AlgUsedTime>();
      GaudiDict::KeyedContainerDict<AlgUsedTime>();
      Class::resolvePending();
    }
  };

  static _InitDict __init;
}

void* __init_InitDict(AlgUsedTime* /* dummy */ )
{
  return &__init;
}
