
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

#include "FitTrack_dict.h"

using namespace seal::reflect;

//------------------------------------------------------------------------------
class FitTrack_dict
//------------------------------------------------------------------------------
{
public:
  FitTrack_dict();
};  

//------------------------------------------------------------------------------
static void* FitTrack_nMeasurements_0const(void* v)
//------------------------------------------------------------------------------
{
  static unsigned int ret;
  ret = ((FitTrack*)v)->nMeasurements();
  return &ret;
}

//------------------------------------------------------------------------------
static void FitTrack_addToMeasurements_1(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((FitTrack*)v)->addToMeasurements((Measurement*)argList[0]);
}

//------------------------------------------------------------------------------
static void FitTrack_sortMeasurements_2(void* v)
//------------------------------------------------------------------------------
{
  ((FitTrack*)v)->sortMeasurements();
}

//------------------------------------------------------------------------------
static void* FitTrack_closestState_3(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  State & ret = ((FitTrack*)v)->closestState(*(double*)argList[0]);
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void* FitTrack_closestState_4const(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  const State & ret = ((FitTrack*)v)->closestState(*(double*)argList[0]);
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void* FitTrack_closestState_5(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  State & ret = ((FitTrack*)v)->closestState(*(const HepPlane3D*)argList[0]);
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void* FitTrack_closestState_6const(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  const State & ret = ((FitTrack*)v)->closestState(*(const HepPlane3D*)argList[0]);
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void* FitTrack_hasStateAt_7const(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  static bool ret;
  ret = ((FitTrack*)v)->hasStateAt(*(const State::Location*)argList[0]);
  return &ret;
}

//------------------------------------------------------------------------------
static void* FitTrack_stateAt_8(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  return (State*) ((FitTrack*)v)->stateAt(*(const State::Location*)argList[0]);
}

//------------------------------------------------------------------------------
static void* FitTrack_stateAt_9const(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  return (State*) ((FitTrack*)v)->stateAt(*(const State::Location*)argList[0]);
}

//------------------------------------------------------------------------------
static void FitTrack_reset_10(void* v)
//------------------------------------------------------------------------------
{
  ((FitTrack*)v)->reset();
}

//------------------------------------------------------------------------------
static void* FitTrack_clone_11const(void* v)
//------------------------------------------------------------------------------
{
  return (FitTrack*) ((FitTrack*)v)->clone();
}

//------------------------------------------------------------------------------
static void* FitTrack_producedByAlgo_12const(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  static bool ret;
  ret = ((FitTrack*)v)->producedByAlgo(*(const FitTrack::HistoryFlag*)argList[0]);
  return &ret;
}

//------------------------------------------------------------------------------
static void FitTrack_setProducedByAlgo_13(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((FitTrack*)v)->setProducedByAlgo(*(const FitTrack::HistoryFlag*)argList[0]);
}


//------------------------------------------------------------------------------
static void* FitTrack_nodes_14(void* v)
//------------------------------------------------------------------------------
{
  const std::vector<Node*>& ret = ((FitTrack*)v)->nodes();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void FitTrack_setNodes_15(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((FitTrack*)v)->setNodes(*(const std::vector<Node*>*)argList[0]);
}

//------------------------------------------------------------------------------
static void* FitTrack_measurements_16(void* v)
//------------------------------------------------------------------------------
{
  const std::vector<Measurement*>& ret = ((FitTrack*)v)->measurements();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void FitTrack_setMeasurements_17(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((FitTrack*)v)->setMeasurements(*(const std::vector<Measurement*>*)argList[0]);
}


//------------------------------------------------------------------------------
static void* FitTrack_constructor_0(void* mem)
//------------------------------------------------------------------------------
{
  return new(mem) FitTrack();
}


//------------------------------------------------------------------------------
static void FitTrack_destructor(void* mem)
//------------------------------------------------------------------------------
{
  ((FitTrack*)mem)->~FitTrack();
}


//------------------------------------------------------------------------------
int FitTrack_to_Track(void* mem)
//------------------------------------------------------------------------------
{
  return (int)(Track*)(FitTrack*)mem - (int)(FitTrack*)mem;
}



//------------------------------------------------------------------------------
FitTrack_dict::FitTrack_dict()
//------------------------------------------------------------------------------
{
  ClassBuilder metaC("FitTrack",
                     "FitTrack is the class for tracking software.",
                     typeid(FitTrack),
                     sizeof(FitTrack),
                     std::vector<const std::type_info*>(),
                     false,
                     NOCONTAINER,
                     0,
                     PUBLIC | VIRTUAL);


  static char clid_txt[64];
  sprintf(clid_txt,"%08X-0000-0000-0000-000000000000",10020);
  metaC.addProperty("ClassID", (char*)clid_txt);

  metaC.addProperty("Author", "Jose Hernando, Eduardo Rodrigues");

  FitTrack* cl = new FitTrack();
  metaC.addSuperClass("Track", 0, FitTrack_to_Track);
  delete cl;


  metaC.addMethod("FitTrack",
                  "default constructor",
                  (System::typeinfoName(typeid(FitTrack))).c_str(),
                  FitTrack_constructor_0,
                  PUBLIC);


  metaC.addMethod("~FitTrack",
                  "default destructor",
                  FitTrack_destructor,
                  PUBLIC | VIRTUAL);


  metaC.addMethod("nMeasurements",
                  "number of measurements on track",
                  (std::string("unsigned int")).c_str(),
                  FitTrack_nMeasurements_0const,
                  PUBLIC | CONST);

  metaC.addMethod("addToMeasurements",
                  "add a measurement to the list associated to the track",
                  (System::typeinfoName(typeid(Measurement*))).c_str(),
                  FitTrack_addToMeasurements_1,
                  PUBLIC);

  metaC.addMethod("sortMeasurements",
                  "sort the measurements by increasing z",
                  FitTrack_sortMeasurements_2,
                  PUBLIC);

  metaC.addMethod("closestState",
                  "Retrieve the reference to the state closest to the given z-position",
                  (System::typeinfoName(typeid(State ))+std::string("&")).c_str(),
                  (std::string("double")).c_str(),
                  FitTrack_closestState_3,
                  PUBLIC | VIRTUAL);

  metaC.addMethod("closestState",
                  "Retrieve the reference to the state closest to the given z-position",
                  (System::typeinfoName(typeid(State ))+std::string("&")).c_str(),
                  (std::string("double")).c_str(),
                  FitTrack_closestState_4const,
                  PUBLIC | CONST | VIRTUAL);

  metaC.addMethod("closestState",
                  "Retrieve the reference to the state closest to the given plane",
                  (System::typeinfoName(typeid(State ))+std::string("&")).c_str(),
                  (System::typeinfoName(typeid(const HepPlane3D))+std::string("&")).c_str(),
                  FitTrack_closestState_5,
                  PUBLIC | VIRTUAL);

  metaC.addMethod("closestState",
                  "Retrieve the reference to the state closest to the given plane",
                  (System::typeinfoName(typeid(State ))+std::string("&")).c_str(),
                  (System::typeinfoName(typeid(const HepPlane3D))+std::string("&")).c_str(),
                  FitTrack_closestState_6const,
                  PUBLIC | CONST | VIRTUAL);

  metaC.addMethod("hasStateAt",
                  "check the existence of a state at a certain predefined location. See State class.",
                  (std::string("bool")).c_str(),
                  (System::typeinfoName(typeid(const State::Location))+std::string("&")).c_str(),
                  FitTrack_hasStateAt_7const,
                  PUBLIC | CONST);

  metaC.addMethod("stateAt",
                  "Retrieve the pointer to the state at a certain predefined location. See State class.",
                  (System::typeinfoName(typeid(State*))).c_str(),
                  (System::typeinfoName(typeid(const State::Location))+std::string("&")).c_str(),
                  FitTrack_stateAt_8,
                  PUBLIC | VIRTUAL);

  metaC.addMethod("stateAt",
                  "Retrieve the pointer to the state at a certain predefined location. See State class.",
                  (System::typeinfoName(typeid(State*))).c_str(),
                  (System::typeinfoName(typeid(const State::Location))+std::string("&")).c_str(),
                  FitTrack_stateAt_9const,
                  PUBLIC | CONST | VIRTUAL);

  metaC.addMethod("reset",
                  "Clear the fit track before re-use",
                  FitTrack_reset_10,
                  PUBLIC | VIRTUAL);

  metaC.addMethod("clone",
                  "Clone the fit track",
                  (System::typeinfoName(typeid(FitTrack*))).c_str(),
                  FitTrack_clone_11const,
                  PUBLIC | CONST | VIRTUAL);

  metaC.addMethod("producedByAlgo",
                  "Check whether the fit track was produced by a given algorithm",
                  (std::string("bool")).c_str(),
                  (System::typeinfoName(typeid(const FitTrack::HistoryFlag))+std::string("&")).c_str(),
                  FitTrack_producedByAlgo_12const,
                  PUBLIC | CONST | VIRTUAL);

  metaC.addMethod("setProducedByAlgo",
                  "Update the name of the algorithm that produced the fit track",
                  (System::typeinfoName(typeid(const FitTrack::HistoryFlag))+std::string("&")).c_str(),
                  FitTrack_setProducedByAlgo_13,
                  PUBLIC | VIRTUAL);

 
  metaC.addField("nodes",
                 (std::string("std::vector<Node*>")).c_str(),
                 "container of Nodes",
                 OffsetOf(FitTrack, m_nodes),
                 PROTECTED);

  metaC.addField("measurements",
                 (std::string("std::vector<Measurement*>")).c_str(),
                 "container of Measurements",
                 OffsetOf(FitTrack, m_measurements),
                 PROTECTED);


  metaC.addMethod("nodes",
                  "container of Nodes",
                  (std::string("std::vector<Node*>&")).c_str(),
                  FitTrack_nodes_14,
                  PUBLIC);

  metaC.addMethod("setNodes",
                  "container of Nodes",
                  (std::string("const std::vector<Node*>&")).c_str(),
                  FitTrack_setNodes_15,
                  PUBLIC);

  metaC.addMethod("measurements",
                  "container of Measurements",
                  (std::string("std::vector<Measurement*>&")).c_str(),
                  FitTrack_measurements_16,
                  PUBLIC);

  metaC.addMethod("setMeasurements",
                  "container of Measurements",
                  (std::string("const std::vector<Measurement*>&")).c_str(),
                  FitTrack_setMeasurements_17,
                  PUBLIC);



  metaC.build();
}

namespace 
{
  struct _InitDict
  {
    _InitDict()
    {
      FitTrack_dict();
      GaudiDict::KeyedObjectDict<int>();
      GaudiDict::SmartRefDict<FitTrack>();
      GaudiDict::SmartRefVectorDict<FitTrack>();
      GaudiDict::VectorDict<FitTrack*>();
      GaudiDict::ObjectVectorDict<FitTrack>();
      GaudiDict::KeyedContainerDict<FitTrack>();
      Class::resolvePending();
    }
  };

  static _InitDict __init;
}

void* __init_InitDict(FitTrack* /* dummy */ )
{
  return &__init;
}
