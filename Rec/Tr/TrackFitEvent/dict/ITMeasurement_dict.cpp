
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

#include "ITMeasurement_dict.h"

using namespace seal::reflect;

//------------------------------------------------------------------------------
class ITMeasurement_dict
//------------------------------------------------------------------------------
{
public:
  ITMeasurement_dict();
};  

//------------------------------------------------------------------------------
static void* ITMeasurement_clone_0const(void* v)
//------------------------------------------------------------------------------
{
  return (ITMeasurement*) ((ITMeasurement*)v)->clone();
}


//------------------------------------------------------------------------------
static void* ITMeasurement_cluster_1(void* v)
//------------------------------------------------------------------------------
{
  return (ITCluster*) ((ITMeasurement*)v)->cluster();
}

//------------------------------------------------------------------------------
static void ITMeasurement_setCluster_2(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((ITMeasurement*)v)->setCluster(*(ITCluster**)argList[0]);
}


//------------------------------------------------------------------------------
static void* ITMeasurement_constructor_1(void* mem, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  return new(mem) ITMeasurement(*(ITCluster*)argList[0],
                                *(DeSTDetector*)argList[1]);
}

//------------------------------------------------------------------------------
static void* ITMeasurement_constructor_0(void* mem)
//------------------------------------------------------------------------------
{
  return new(mem) ITMeasurement();
}


//------------------------------------------------------------------------------
static void ITMeasurement_destructor(void* mem)
//------------------------------------------------------------------------------
{
  ((ITMeasurement*)mem)->~ITMeasurement();
}


//------------------------------------------------------------------------------
int ITMeasurement_to_Measurement(void* mem)
//------------------------------------------------------------------------------
{
  return (int)(Measurement*)(ITMeasurement*)mem - (int)(ITMeasurement*)mem;
}



//------------------------------------------------------------------------------
ITMeasurement_dict::ITMeasurement_dict()
//------------------------------------------------------------------------------
{
  ClassBuilder metaC("ITMeasurement",
                     "ITMeasurement is a measurement made from a IT cluster.",
                     typeid(ITMeasurement),
                     sizeof(ITMeasurement),
                     std::vector<const std::type_info*>(),
                     false,
                     NOCONTAINER,
                     0,
                     PUBLIC | VIRTUAL);


  metaC.addProperty("Author", "Jose Hernando, Eduardo Rodrigues");

  ITMeasurement* cl = new ITMeasurement();
  metaC.addSuperClass("Measurement", 0, ITMeasurement_to_Measurement);
  delete cl;


  metaC.addMethod("ITMeasurement",
                  "Constructor from an ITCluster",
                  (System::typeinfoName(typeid(ITMeasurement))).c_str(),
                  (System::typeinfoName(typeid(ITCluster))+std::string("&")+";"+System::typeinfoName(typeid(DeSTDetector))+std::string("&")).c_str(),
                  ITMeasurement_constructor_1,
                  PUBLIC);

  metaC.addMethod("ITMeasurement",
                  "default constructor",
                  (System::typeinfoName(typeid(ITMeasurement))).c_str(),
                  ITMeasurement_constructor_0,
                  PUBLIC);


  metaC.addMethod("~ITMeasurement",
                  "default destructor",
                  ITMeasurement_destructor,
                  PUBLIC | VIRTUAL);


  metaC.addMethod("clone",
                  "Clone the ITMeasurement",
                  (System::typeinfoName(typeid(ITMeasurement*))).c_str(),
                  ITMeasurement_clone_0const,
                  PUBLIC | CONST);

 
  metaC.addField("cluster",
                 (System::typeinfoName(typeid(ITCluster*))).c_str(),
                 "Pointer to the ITCluster",
                 OffsetOf(ITMeasurement, m_cluster),
                 PRIVATE);


  metaC.addMethod("cluster",
                  "Pointer to the ITCluster",
                  (System::typeinfoName(typeid(ITCluster*))).c_str(),
                  ITMeasurement_cluster_1,
                  PUBLIC);

  metaC.addMethod("setCluster",
                  "Pointer to the ITCluster",
                  (System::typeinfoName(typeid(ITCluster*))).c_str(),
                  ITMeasurement_setCluster_2,
                  PUBLIC);



  metaC.build();
}

namespace 
{
  struct _InitDict
  {
    _InitDict()
    {
      ITMeasurement_dict();
      Class::resolvePending();
    }
  };

  static _InitDict __init;
}

void* __init_InitDict(ITMeasurement* /* dummy */ )
{
  return &__init;
}
