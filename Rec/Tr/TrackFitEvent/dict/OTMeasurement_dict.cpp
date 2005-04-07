
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

#include <string>
#include <typeinfo>

#include "OTMeasurement_dict.h"

using namespace seal::reflect;

//------------------------------------------------------------------------------
class OTMeasurement_dict
//------------------------------------------------------------------------------
{
public:
  OTMeasurement_dict();
};  

//------------------------------------------------------------------------------
static void* OTMeasurement_clone_0const(void* v)
//------------------------------------------------------------------------------
{
  return (OTMeasurement*) ((OTMeasurement*)v)->clone();
}


//------------------------------------------------------------------------------
static void* OTMeasurement_ambiguity_1(void* v)
//------------------------------------------------------------------------------
{
  static int ret;
  ret = ((OTMeasurement*)v)->ambiguity();
  return &ret;
}

//------------------------------------------------------------------------------
static void OTMeasurement_setAmbiguity_2(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((OTMeasurement*)v)->setAmbiguity(*(int*)argList[0]);
}

//------------------------------------------------------------------------------
static void* OTMeasurement_tu_3(void* v)
//------------------------------------------------------------------------------
{
  static double ret;
  ret = ((OTMeasurement*)v)->tu();
  return &ret;
}

//------------------------------------------------------------------------------
static void OTMeasurement_setTu_4(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((OTMeasurement*)v)->setTu(*(double*)argList[0]);
}

//------------------------------------------------------------------------------
static void* OTMeasurement_time_5(void* v)
//------------------------------------------------------------------------------
{
  return (OTTime*) ((OTMeasurement*)v)->time();
}

//------------------------------------------------------------------------------
static void OTMeasurement_setTime_6(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((OTMeasurement*)v)->setTime(*(OTTime**)argList[0]);
}


//------------------------------------------------------------------------------
static void* OTMeasurement_constructor_1(void* mem, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  return new(mem) OTMeasurement(*(OTTime*)argList[0],
                                *(DeOTDetector*)argList[1],
                                *(int*)argList[2],
                                *(double*)argList[3]);
}

//------------------------------------------------------------------------------
static void* OTMeasurement_constructor_0(void* mem)
//------------------------------------------------------------------------------
{
  return new(mem) OTMeasurement();
}


//------------------------------------------------------------------------------
static void OTMeasurement_destructor(void* mem)
//------------------------------------------------------------------------------
{
  ((OTMeasurement*)mem)->~OTMeasurement();
}


//------------------------------------------------------------------------------
int OTMeasurement_to_Measurement(void* mem)
//------------------------------------------------------------------------------
{
  return (int)(Measurement*)(OTMeasurement*)mem - (int)(OTMeasurement*)mem;
}



//------------------------------------------------------------------------------
OTMeasurement_dict::OTMeasurement_dict()
//------------------------------------------------------------------------------
{
  ClassBuilder metaC("OTMeasurement",
                     "OTMeasurement is a measurement made from a OTTime.",
                     typeid(OTMeasurement),
                     sizeof(OTMeasurement),
                     std::vector<const std::type_info*>(),
                     false,
                     NOCONTAINER,
                     0,
                     PUBLIC | VIRTUAL);


  metaC.addProperty("Author", "Jose Hernando, Eduardo Rodrigues");

  OTMeasurement* cl = new OTMeasurement();
  metaC.addSuperClass("Measurement", 0, OTMeasurement_to_Measurement);
  delete cl;


  metaC.addMethod("OTMeasurement",
                  "Constructor from an OTTime",
                  (System::typeinfoName(typeid(OTMeasurement))).c_str(),
                  (System::typeinfoName(typeid(OTTime))+std::string("&")+";"+System::typeinfoName(typeid(DeOTDetector))+std::string("&")+";"+std::string("int")+";"+std::string("double")).c_str(),
                  OTMeasurement_constructor_1,
                  PUBLIC);

  metaC.addMethod("OTMeasurement",
                  "default constructor",
                  (System::typeinfoName(typeid(OTMeasurement))).c_str(),
                  OTMeasurement_constructor_0,
                  PUBLIC);


  metaC.addMethod("~OTMeasurement",
                  "default destructor",
                  OTMeasurement_destructor,
                  PUBLIC | VIRTUAL);


  metaC.addMethod("clone",
                  "Clone the OTMeasurement",
                  (System::typeinfoName(typeid(OTMeasurement*))).c_str(),
                  OTMeasurement_clone_0const,
                  PUBLIC | CONST);

 
  metaC.addField("ambiguity",
                 (std::string("int")).c_str(),
                 "the ambiguity (+1/-1) of the measurement",
                 OffsetOf(OTMeasurement, m_ambiguity),
                 PRIVATE);

  metaC.addField("tu",
                 (std::string("double")).c_str(),
                 "the slope in u of the measurement",
                 OffsetOf(OTMeasurement, m_tu),
                 PRIVATE);

  metaC.addField("time",
                 (System::typeinfoName(typeid(OTTime*))).c_str(),
                 "Pointer to the OTTime",
                 OffsetOf(OTMeasurement, m_time),
                 PRIVATE);


  metaC.addMethod("ambiguity",
                  "the ambiguity (+1/-1) of the measurement",
                  (std::string("int")).c_str(),
                  OTMeasurement_ambiguity_1,
                  PUBLIC);

  metaC.addMethod("setAmbiguity",
                  "the ambiguity (+1/-1) of the measurement",
                  (std::string("int")).c_str(),
                  OTMeasurement_setAmbiguity_2,
                  PUBLIC);

  metaC.addMethod("tu",
                  "the slope in u of the measurement",
                  (std::string("double")).c_str(),
                  OTMeasurement_tu_3,
                  PUBLIC);

  metaC.addMethod("setTu",
                  "the slope in u of the measurement",
                  (std::string("double")).c_str(),
                  OTMeasurement_setTu_4,
                  PUBLIC);

  metaC.addMethod("time",
                  "Pointer to the OTTime",
                  (System::typeinfoName(typeid(OTTime*))).c_str(),
                  OTMeasurement_time_5,
                  PUBLIC);

  metaC.addMethod("setTime",
                  "Pointer to the OTTime",
                  (System::typeinfoName(typeid(OTTime*))).c_str(),
                  OTMeasurement_setTime_6,
                  PUBLIC);



  metaC.build();
}

namespace 
{
  struct _InitDict
  {
    _InitDict()
    {
      OTMeasurement_dict();
      Class::resolvePending();
    }
  };

  static _InitDict __init;
}

void* __init_InitDict(OTMeasurement* /* dummy */ )
{
  return &__init;
}
