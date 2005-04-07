
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

#include "VeloPhiMeasurement_dict.h"

using namespace seal::reflect;

//------------------------------------------------------------------------------
class VeloPhiMeasurement_dict
//------------------------------------------------------------------------------
{
public:
  VeloPhiMeasurement_dict();
};  

//------------------------------------------------------------------------------
static void* VeloPhiMeasurement_clone_0const(void* v)
//------------------------------------------------------------------------------
{
  return (VeloPhiMeasurement*) ((VeloPhiMeasurement*)v)->clone();
}


//------------------------------------------------------------------------------
static void* VeloPhiMeasurement_cluster_1(void* v)
//------------------------------------------------------------------------------
{
  return (VeloCluster*) ((VeloPhiMeasurement*)v)->cluster();
}

//------------------------------------------------------------------------------
static void VeloPhiMeasurement_setCluster_2(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((VeloPhiMeasurement*)v)->setCluster(*(VeloCluster**)argList[0]);
}


//------------------------------------------------------------------------------
static void* VeloPhiMeasurement_constructor_1(void* mem, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  return new(mem) VeloPhiMeasurement(*(VeloCluster*)argList[0],
                                     *(DeVelo*)argList[1]);
}

//------------------------------------------------------------------------------
static void* VeloPhiMeasurement_constructor_0(void* mem)
//------------------------------------------------------------------------------
{
  return new(mem) VeloPhiMeasurement();
}


//------------------------------------------------------------------------------
static void VeloPhiMeasurement_destructor(void* mem)
//------------------------------------------------------------------------------
{
  ((VeloPhiMeasurement*)mem)->~VeloPhiMeasurement();
}


//------------------------------------------------------------------------------
int VeloPhiMeasurement_to_Measurement(void* mem)
//------------------------------------------------------------------------------
{
  return (int)(Measurement*)(VeloPhiMeasurement*)mem - (int)(VeloPhiMeasurement*)mem;
}



//------------------------------------------------------------------------------
VeloPhiMeasurement_dict::VeloPhiMeasurement_dict()
//------------------------------------------------------------------------------
{
  ClassBuilder metaC("VeloPhiMeasurement",
                     "VeloPhiMeasurement is a measurement made from a VeloPhiCluster.",
                     typeid(VeloPhiMeasurement),
                     sizeof(VeloPhiMeasurement),
                     std::vector<const std::type_info*>(),
                     false,
                     NOCONTAINER,
                     0,
                     PUBLIC | VIRTUAL);


  metaC.addProperty("Author", "Jose Hernando, Eduardo Rodrigues");

  VeloPhiMeasurement* cl = new VeloPhiMeasurement();
  metaC.addSuperClass("Measurement", 0, VeloPhiMeasurement_to_Measurement);
  delete cl;


  metaC.addMethod("VeloPhiMeasurement",
                  "Constructor from an VeloPhiCluster",
                  (System::typeinfoName(typeid(VeloPhiMeasurement))).c_str(),
                  (System::typeinfoName(typeid(VeloCluster))+std::string("&")+";"+System::typeinfoName(typeid(DeVelo))+std::string("&")).c_str(),
                  VeloPhiMeasurement_constructor_1,
                  PUBLIC);

  metaC.addMethod("VeloPhiMeasurement",
                  "default constructor",
                  (System::typeinfoName(typeid(VeloPhiMeasurement))).c_str(),
                  VeloPhiMeasurement_constructor_0,
                  PUBLIC);


  metaC.addMethod("~VeloPhiMeasurement",
                  "default destructor",
                  VeloPhiMeasurement_destructor,
                  PUBLIC | VIRTUAL);


  metaC.addMethod("clone",
                  "Clone the VeloPhiMeasurement",
                  (System::typeinfoName(typeid(VeloPhiMeasurement*))).c_str(),
                  VeloPhiMeasurement_clone_0const,
                  PUBLIC | CONST);

 
  metaC.addField("cluster",
                 (System::typeinfoName(typeid(VeloCluster*))).c_str(),
                 "Pointer to the VeloPhiCluster",
                 OffsetOf(VeloPhiMeasurement, m_cluster),
                 PRIVATE);


  metaC.addMethod("cluster",
                  "Pointer to the VeloPhiCluster",
                  (System::typeinfoName(typeid(VeloCluster*))).c_str(),
                  VeloPhiMeasurement_cluster_1,
                  PUBLIC);

  metaC.addMethod("setCluster",
                  "Pointer to the VeloPhiCluster",
                  (System::typeinfoName(typeid(VeloCluster*))).c_str(),
                  VeloPhiMeasurement_setCluster_2,
                  PUBLIC);



  metaC.build();
}

namespace 
{
  struct _InitDict
  {
    _InitDict()
    {
      VeloPhiMeasurement_dict();
      Class::resolvePending();
    }
  };

  static _InitDict __init;
}

void* __init_InitDict(VeloPhiMeasurement* /* dummy */ )
{
  return &__init;
}
