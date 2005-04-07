
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

#include "VeloRMeasurement_dict.h"

using namespace seal::reflect;

//------------------------------------------------------------------------------
class VeloRMeasurement_dict
//------------------------------------------------------------------------------
{
public:
  VeloRMeasurement_dict();
};  

//------------------------------------------------------------------------------
static void* VeloRMeasurement_clone_0const(void* v)
//------------------------------------------------------------------------------
{
  return (VeloRMeasurement*) ((VeloRMeasurement*)v)->clone();
}


//------------------------------------------------------------------------------
static void* VeloRMeasurement_phi_1(void* v)
//------------------------------------------------------------------------------
{
  static double ret;
  ret = ((VeloRMeasurement*)v)->phi();
  return &ret;
}

//------------------------------------------------------------------------------
static void VeloRMeasurement_setPhi_2(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((VeloRMeasurement*)v)->setPhi(*(double*)argList[0]);
}

//------------------------------------------------------------------------------
static void* VeloRMeasurement_cluster_3(void* v)
//------------------------------------------------------------------------------
{
  return (VeloCluster*) ((VeloRMeasurement*)v)->cluster();
}

//------------------------------------------------------------------------------
static void VeloRMeasurement_setCluster_4(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((VeloRMeasurement*)v)->setCluster(*(VeloCluster**)argList[0]);
}


//------------------------------------------------------------------------------
static void* VeloRMeasurement_constructor_1(void* mem, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  return new(mem) VeloRMeasurement(*(VeloCluster*)argList[0],
                                   *(DeVelo*)argList[1],
                                   *(double*)argList[2]);
}

//------------------------------------------------------------------------------
static void* VeloRMeasurement_constructor_0(void* mem)
//------------------------------------------------------------------------------
{
  return new(mem) VeloRMeasurement();
}


//------------------------------------------------------------------------------
static void VeloRMeasurement_destructor(void* mem)
//------------------------------------------------------------------------------
{
  ((VeloRMeasurement*)mem)->~VeloRMeasurement();
}


//------------------------------------------------------------------------------
int VeloRMeasurement_to_Measurement(void* mem)
//------------------------------------------------------------------------------
{
  return (int)(Measurement*)(VeloRMeasurement*)mem - (int)(VeloRMeasurement*)mem;
}



//------------------------------------------------------------------------------
VeloRMeasurement_dict::VeloRMeasurement_dict()
//------------------------------------------------------------------------------
{
  ClassBuilder metaC("VeloRMeasurement",
                     "VeloRMeasurement is a measurement made from a VeloRCluster.",
                     typeid(VeloRMeasurement),
                     sizeof(VeloRMeasurement),
                     std::vector<const std::type_info*>(),
                     false,
                     NOCONTAINER,
                     0,
                     PUBLIC | VIRTUAL);


  metaC.addProperty("Author", "Jose Hernando, Eduardo Rodrigues");

  VeloRMeasurement* cl = new VeloRMeasurement();
  metaC.addSuperClass("Measurement", 0, VeloRMeasurement_to_Measurement);
  delete cl;


  metaC.addMethod("VeloRMeasurement",
                  "Constructor from an VeloRCluster",
                  (System::typeinfoName(typeid(VeloRMeasurement))).c_str(),
                  (System::typeinfoName(typeid(VeloCluster))+std::string("&")+";"+System::typeinfoName(typeid(DeVelo))+std::string("&")+";"+std::string("double")).c_str(),
                  VeloRMeasurement_constructor_1,
                  PUBLIC);

  metaC.addMethod("VeloRMeasurement",
                  "default constructor",
                  (System::typeinfoName(typeid(VeloRMeasurement))).c_str(),
                  VeloRMeasurement_constructor_0,
                  PUBLIC);


  metaC.addMethod("~VeloRMeasurement",
                  "default destructor",
                  VeloRMeasurement_destructor,
                  PUBLIC | VIRTUAL);


  metaC.addMethod("clone",
                  "Clone the VeloRMeasurement",
                  (System::typeinfoName(typeid(VeloRMeasurement*))).c_str(),
                  VeloRMeasurement_clone_0const,
                  PUBLIC | CONST);

 
  metaC.addField("phi",
                 (std::string("double")).c_str(),
                 "the phi of the Velo R measurement",
                 OffsetOf(VeloRMeasurement, m_phi),
                 PRIVATE);

  metaC.addField("cluster",
                 (System::typeinfoName(typeid(VeloCluster*))).c_str(),
                 "Pointer to the VeloRCluster",
                 OffsetOf(VeloRMeasurement, m_cluster),
                 PRIVATE);


  metaC.addMethod("phi",
                  "the phi of the Velo R measurement",
                  (std::string("double")).c_str(),
                  VeloRMeasurement_phi_1,
                  PUBLIC);

  metaC.addMethod("setPhi",
                  "the phi of the Velo R measurement",
                  (std::string("double")).c_str(),
                  VeloRMeasurement_setPhi_2,
                  PUBLIC);

  metaC.addMethod("cluster",
                  "Pointer to the VeloRCluster",
                  (System::typeinfoName(typeid(VeloCluster*))).c_str(),
                  VeloRMeasurement_cluster_3,
                  PUBLIC);

  metaC.addMethod("setCluster",
                  "Pointer to the VeloRCluster",
                  (System::typeinfoName(typeid(VeloCluster*))).c_str(),
                  VeloRMeasurement_setCluster_4,
                  PUBLIC);



  metaC.build();
}

namespace 
{
  struct _InitDict
  {
    _InitDict()
    {
      VeloRMeasurement_dict();
      Class::resolvePending();
    }
  };

  static _InitDict __init;
}

void* __init_InitDict(VeloRMeasurement* /* dummy */ )
{
  return &__init;
}
