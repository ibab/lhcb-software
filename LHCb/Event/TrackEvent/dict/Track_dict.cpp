
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

#include "Track_dict.h"

using namespace seal::reflect;

//------------------------------------------------------------------------------
class Track_dict
//------------------------------------------------------------------------------
{
public:
  Track_dict();
};  

//------------------------------------------------------------------------------
static void* Track_positionAndMomentum_0const(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  static StatusCode ret = ((Track*)v)->positionAndMomentum(*(HepPoint3D*)argList[0],
                                                           *(HepVector3D*)argList[1],
                                                           *(HepSymMatrix*)argList[2]);
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void* Track_position_1const(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  static StatusCode ret = ((Track*)v)->position(*(HepPoint3D*)argList[0],
                                                *(HepSymMatrix*)argList[1]);
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void* Track_slopes_2const(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  static StatusCode ret = ((Track*)v)->slopes(*(HepVector3D*)argList[0],
                                              *(HepSymMatrix*)argList[1]);
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void* Track_p_3const(void* v)
//------------------------------------------------------------------------------
{
  static double ret;
  ret = ((Track*)v)->p();
  return &ret;
}

//------------------------------------------------------------------------------
static void* Track_pt_4const(void* v)
//------------------------------------------------------------------------------
{
  static double ret;
  ret = ((Track*)v)->pt();
  return &ret;
}

//------------------------------------------------------------------------------
static void* Track_momentum_5const(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  static StatusCode ret = ((Track*)v)->momentum(*(HepVector3D*)argList[0],
                                                *(HepSymMatrix*)argList[1]);
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void* Track_posMomCovariance_6const(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  static StatusCode ret = ((Track*)v)->posMomCovariance(*(HepSymMatrix*)argList[0]);
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void* Track_charge_7const(void* v)
//------------------------------------------------------------------------------
{
  static int ret;
  ret = ((Track*)v)->charge();
  return &ret;
}

//------------------------------------------------------------------------------
static void* Track_nStates_8const(void* v)
//------------------------------------------------------------------------------
{
  static unsigned int ret;
  ret = ((Track*)v)->nStates();
  return &ret;
}

//------------------------------------------------------------------------------
static void Track_addToStates_9(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((Track*)v)->addToStates((State*)argList[0]);
}

//------------------------------------------------------------------------------
static void Track_removeFromStates_10(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((Track*)v)->removeFromStates((State*)argList[0]);
}

//------------------------------------------------------------------------------
static void* Track_closestState_11(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  State & ret = ((Track*)v)->closestState(*(double*)argList[0]);
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void* Track_closestState_12const(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  const State & ret = ((Track*)v)->closestState(*(double*)argList[0]);
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void* Track_closestState_13(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  State & ret = ((Track*)v)->closestState(*(const HepPlane3D*)argList[0]);
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void* Track_closestState_14const(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  const State & ret = ((Track*)v)->closestState(*(const HepPlane3D*)argList[0]);
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void* Track_hasStateAt_15const(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  static bool ret;
  ret = ((Track*)v)->hasStateAt(*(const State::Location*)argList[0]);
  return &ret;
}

//------------------------------------------------------------------------------
static void* Track_stateAt_16(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  return (State*) ((Track*)v)->stateAt(*(const State::Location*)argList[0]);
}

//------------------------------------------------------------------------------
static void* Track_stateAt_17const(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  return (State*) ((Track*)v)->stateAt(*(const State::Location*)argList[0]);
}

//------------------------------------------------------------------------------
static void* Track_chi2_18const(void* v)
//------------------------------------------------------------------------------
{
  static double ret;
  ret = ((Track*)v)->chi2();
  return &ret;
}

//------------------------------------------------------------------------------
static void Track_reset_19(void* v)
//------------------------------------------------------------------------------
{
  ((Track*)v)->reset();
}

//------------------------------------------------------------------------------
static void* Track_clone_20const(void* v)
//------------------------------------------------------------------------------
{
  return (Track*) ((Track*)v)->clone();
}

//------------------------------------------------------------------------------
static void Track_addToLhcbIDs_21(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((Track*)v)->addToLhcbIDs(*(const LHCbID*)argList[0]);
}

//------------------------------------------------------------------------------
static void Track_removeFromLhcbIDs_22(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((Track*)v)->removeFromLhcbIDs(*(const LHCbID*)argList[0]);
}

//------------------------------------------------------------------------------
static void* Track_producedByAlgo_23const(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  static bool ret;
  ret = ((Track*)v)->producedByAlgo(*(const Track::HistoryFlag*)argList[0]);
  return &ret;
}

//------------------------------------------------------------------------------
static void Track_setProducedByAlgo_24(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((Track*)v)->setProducedByAlgo(*(const Track::HistoryFlag*)argList[0]);
}


//------------------------------------------------------------------------------
static void* Track_chi2PerDoF_25(void* v)
//------------------------------------------------------------------------------
{
  static double ret;
  ret = ((Track*)v)->chi2PerDoF();
  return &ret;
}

//------------------------------------------------------------------------------
static void Track_setChi2PerDoF_26(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((Track*)v)->setChi2PerDoF(*(double*)argList[0]);
}

//------------------------------------------------------------------------------
static void* Track_nDoF_27(void* v)
//------------------------------------------------------------------------------
{
  static int ret;
  ret = ((Track*)v)->nDoF();
  return &ret;
}

//------------------------------------------------------------------------------
static void Track_setNDoF_28(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((Track*)v)->setNDoF(*(int*)argList[0]);
}

//------------------------------------------------------------------------------
static void Track_setType_29(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((Track*)v)->setType(*(Track::TrackType*)argList[0]);
}

//------------------------------------------------------------------------------
static void* Track_checkType_30(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  static bool ret;
  ret = ((Track*)v)->checkType(*(Track::TrackType*)argList[0]);
  return &ret;
}

//------------------------------------------------------------------------------
static void Track_setFlag_31(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((Track*)v)->setFlag(*(Track::Flag*)argList[0]);
}

//------------------------------------------------------------------------------
static void* Track_checkFlag_32(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  static bool ret;
  ret = ((Track*)v)->checkFlag(*(Track::Flag*)argList[0]);
  return &ret;
}

//------------------------------------------------------------------------------
static void* Track_flags_33(void* v)
//------------------------------------------------------------------------------
{
  static unsigned int ret;
  ret = ((Track*)v)->flags();
  return &ret;
}

//------------------------------------------------------------------------------
static void* Track_lhcbIDs_34(void* v)
//------------------------------------------------------------------------------
{
  const std::vector<LHCbID>& ret = ((Track*)v)->lhcbIDs();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void Track_setLhcbIDs_35(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((Track*)v)->setLhcbIDs(*(const std::vector<LHCbID>*)argList[0]);
}

//------------------------------------------------------------------------------
static void* Track_physicsState_36(void* v)
//------------------------------------------------------------------------------
{
  const State& ret = ((Track*)v)->physicsState();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void Track_setPhysicsState_37(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((Track*)v)->setPhysicsState(*(const State*)argList[0]);
}

//------------------------------------------------------------------------------
static void* Track_states_38(void* v)
//------------------------------------------------------------------------------
{
  const std::vector<State*>& ret = ((Track*)v)->states();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void Track_setStates_39(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((Track*)v)->setStates(*(const std::vector<State*>*)argList[0]);
}


//------------------------------------------------------------------------------
static void* Track_constructor_0(void* mem)
//------------------------------------------------------------------------------
{
  return new(mem) Track();
}


//------------------------------------------------------------------------------
static void Track_destructor(void* mem)
//------------------------------------------------------------------------------
{
  ((Track*)mem)->~Track();
}


//------------------------------------------------------------------------------
int Track_to_KeyedObjectLtintGt(void* mem)
//------------------------------------------------------------------------------
{
  return (int)(KeyedObject<int>*)(Track*)mem - (int)(Track*)mem;
}



//------------------------------------------------------------------------------
Track_dict::Track_dict()
//------------------------------------------------------------------------------
{
  ClassBuilder metaC("Track",
                     "Track is the base class for offline and online tracks.",
                     typeid(Track),
                     sizeof(Track),
                     std::vector<const std::type_info*>(),
                     false,
                     NOCONTAINER,
                     0,
                     PUBLIC | VIRTUAL);


  static char clid_txt[64];
  sprintf(clid_txt,"%08X-0000-0000-0000-000000000000",10010);
  metaC.addProperty("ClassID", (char*)clid_txt);

  metaC.addProperty("Author", "Jose Hernando, Eduardo Rodrigues");

  Track* cl = new Track();
  metaC.addSuperClass("KeyedObject<int>", 0, Track_to_KeyedObjectLtintGt);
  delete cl;


  metaC.addMethod("Track",
                  "default constructor",
                  (System::typeinfoName(typeid(Track))).c_str(),
                  Track_constructor_0,
                  PUBLIC);


  metaC.addMethod("~Track",
                  "Track destructor",
                  Track_destructor,
                  PUBLIC);


  metaC.addMethod("positionAndMomentum",
                  "Retrieve the position and momentum vectors and the corresponding 6D covariance matrix (pos:1->3,mom:4-6) of a track closest to the beam-line",
                  (System::typeinfoName(typeid(StatusCode))).c_str(),
                  (System::typeinfoName(typeid(HepPoint3D))+std::string("&")+";"+System::typeinfoName(typeid(HepVector3D))+std::string("&")+";"+System::typeinfoName(typeid(HepSymMatrix))+std::string("&")).c_str(),
                  Track_positionAndMomentum_0const,
                  PUBLIC | CONST);

  metaC.addMethod("position",
                  "Retrieve the 3D-position (+ errors) of a track closest to the beam-line",
                  (System::typeinfoName(typeid(StatusCode))).c_str(),
                  (System::typeinfoName(typeid(HepPoint3D))+std::string("&")+";"+System::typeinfoName(typeid(HepSymMatrix))+std::string("&")).c_str(),
                  Track_position_1const,
                  PUBLIC | CONST);

  metaC.addMethod("slopes",
                  "Retrieve the slopes (dx/dz,dy/dz,1) of a track closest to the beam-line",
                  (System::typeinfoName(typeid(StatusCode))).c_str(),
                  (System::typeinfoName(typeid(HepVector3D))+std::string("&")+";"+System::typeinfoName(typeid(HepSymMatrix))+std::string("&")).c_str(),
                  Track_slopes_2const,
                  PUBLIC | CONST);

  metaC.addMethod("p",
                  "Retrieve the momentum of a track closest to the beam-line",
                  (std::string("double")).c_str(),
                  Track_p_3const,
                  PUBLIC | CONST);

  metaC.addMethod("pt",
                  "Retrieve the transverse momentum of a track closest to the beam-line",
                  (std::string("double")).c_str(),
                  Track_pt_4const,
                  PUBLIC | CONST);

  metaC.addMethod("momentum",
                  "Retrieve the momentum vector (+ errors) of a track closest to the beam-line",
                  (System::typeinfoName(typeid(StatusCode))).c_str(),
                  (System::typeinfoName(typeid(HepVector3D))+std::string("&")+";"+System::typeinfoName(typeid(HepSymMatrix))+std::string("&")).c_str(),
                  Track_momentum_5const,
                  PUBLIC | CONST);

  metaC.addMethod("posMomCovariance",
                  "Retrieve the 6D covariance matrix (x,y,z,px,py,pz) of a track closest to the beam-line",
                  (System::typeinfoName(typeid(StatusCode))).c_str(),
                  (System::typeinfoName(typeid(HepSymMatrix))+std::string("&")).c_str(),
                  Track_posMomCovariance_6const,
                  PUBLIC | CONST);

  metaC.addMethod("charge",
                  "Retrieve the charge assigned to the track",
                  (std::string("int")).c_str(),
                  Track_charge_7const,
                  PUBLIC | CONST);

  metaC.addMethod("nStates",
                  "number of states on track",
                  (std::string("unsigned int")).c_str(),
                  Track_nStates_8const,
                  PUBLIC | CONST);

  metaC.addMethod("addToStates",
                  "Add a State to the list of states associated to the track",
                  (System::typeinfoName(typeid(State*))).c_str(),
                  Track_addToStates_9,
                  PUBLIC);

  metaC.addMethod("removeFromStates",
                  "Remove a State from the list of states associated to the track",
                  (System::typeinfoName(typeid(State*))).c_str(),
                  Track_removeFromStates_10,
                  PUBLIC);

  metaC.addMethod("closestState",
                  "Retrieve the pointer to the state closest to the given z-position",
                  (System::typeinfoName(typeid(State ))+std::string("&")).c_str(),
                  (std::string("double")).c_str(),
                  Track_closestState_11,
                  PUBLIC | VIRTUAL);

  metaC.addMethod("closestState",
                  "Retrieve the pointer to the state closest to the given z-position",
                  (System::typeinfoName(typeid(State ))+std::string("&")).c_str(),
                  (std::string("double")).c_str(),
                  Track_closestState_12const,
                  PUBLIC | CONST | VIRTUAL);

  metaC.addMethod("closestState",
                  "Retrieve the pointer to the state closest to the given plane",
                  (System::typeinfoName(typeid(State ))+std::string("&")).c_str(),
                  (System::typeinfoName(typeid(const HepPlane3D))+std::string("&")).c_str(),
                  Track_closestState_13,
                  PUBLIC | VIRTUAL);

  metaC.addMethod("closestState",
                  "Retrieve the pointer to the state closest to the given plane",
                  (System::typeinfoName(typeid(State ))+std::string("&")).c_str(),
                  (System::typeinfoName(typeid(const HepPlane3D))+std::string("&")).c_str(),
                  Track_closestState_14const,
                  PUBLIC | CONST | VIRTUAL);

  metaC.addMethod("hasStateAt",
                  "check the existence of a state at a certain predefined location. See State class.",
                  (std::string("bool")).c_str(),
                  (System::typeinfoName(typeid(const State::Location))+std::string("&")).c_str(),
                  Track_hasStateAt_15const,
                  PUBLIC | CONST | VIRTUAL);

  metaC.addMethod("stateAt",
                  "Retrieve the pointer to the state at a certain predefined location. See State class.",
                  (System::typeinfoName(typeid(State*))).c_str(),
                  (System::typeinfoName(typeid(const State::Location))+std::string("&")).c_str(),
                  Track_stateAt_16,
                  PUBLIC | VIRTUAL);

  metaC.addMethod("stateAt",
                  "Retrieve the pointer to the state at a certain predefined location. See State class.",
                  (System::typeinfoName(typeid(State*))).c_str(),
                  (System::typeinfoName(typeid(const State::Location))+std::string("&")).c_str(),
                  Track_stateAt_17const,
                  PUBLIC | CONST | VIRTUAL);

  metaC.addMethod("chi2",
                  "Retrieve the Chi^2 of the track",
                  (std::string("double")).c_str(),
                  Track_chi2_18const,
                  PUBLIC | CONST);

  metaC.addMethod("reset",
                  "Clear the track before re-use",
                  Track_reset_19,
                  PUBLIC | VIRTUAL);

  metaC.addMethod("clone",
                  "Clone the track",
                  (System::typeinfoName(typeid(Track*))).c_str(),
                  Track_clone_20const,
                  PUBLIC | CONST | VIRTUAL);

  metaC.addMethod("addToLhcbIDs",
                  "Add an LHCbID to the list of LHCbIDs associated to the track",
                  (System::typeinfoName(typeid(const LHCbID))+std::string("&")).c_str(),
                  Track_addToLhcbIDs_21,
                  PUBLIC);

  metaC.addMethod("removeFromLhcbIDs",
                  "Remove an LHCbID from the list of LHCbIDs associated to the track",
                  (System::typeinfoName(typeid(const LHCbID))+std::string("&")).c_str(),
                  Track_removeFromLhcbIDs_22,
                  PUBLIC);

  metaC.addMethod("producedByAlgo",
                  "Check whether the track was produced by a given algorithm",
                  (std::string("bool")).c_str(),
                  (System::typeinfoName(typeid(const Track::HistoryFlag))+std::string("&")).c_str(),
                  Track_producedByAlgo_23const,
                  PUBLIC | CONST | VIRTUAL);

  metaC.addMethod("setProducedByAlgo",
                  "Update the name of the algorithm that produced the track",
                  (System::typeinfoName(typeid(const Track::HistoryFlag))+std::string("&")).c_str(),
                  Track_setProducedByAlgo_24,
                  PUBLIC | VIRTUAL);

 
  metaC.addField("chi2PerDoF",
                 (std::string("double")).c_str(),
                 "chi^2 per degree of freedom of the track",
                 OffsetOf(Track, m_chi2PerDoF),
                 PRIVATE);

  metaC.addField("nDoF",
                 (std::string("int")).c_str(),
                 "number of degrees of freedom of the track",
                 OffsetOf(Track, m_nDoF),
                 PRIVATE);

  metaC.addField("flags",
                 (std::string("unsigned int")).c_str(),
                 "the variety of track flags",
                 OffsetOf(Track, m_flags),
                 PRIVATE);

  metaC.addField("lhcbIDs",
                 (std::string("std::vector<LHCbID>")).c_str(),
                 "container of LHCbIDs",
                 OffsetOf(Track, m_lhcbIDs),
                 PRIVATE);

  metaC.addField("physicsState",
                 (System::typeinfoName(typeid(State))).c_str(),
                 "State closest to the beam-line",
                 OffsetOf(Track, m_physicsState),
                 PRIVATE);

  metaC.addField("states",
                 (std::string("std::vector<State*>")).c_str(),
                 "container with pointers to all the states",
                 OffsetOf(Track, m_states),
                 PRIVATE);


  metaC.addMethod("chi2PerDoF",
                  "chi^2 per degree of freedom of the track",
                  (std::string("double")).c_str(),
                  Track_chi2PerDoF_25,
                  PUBLIC);

  metaC.addMethod("setChi2PerDoF",
                  "chi^2 per degree of freedom of the track",
                  (std::string("double")).c_str(),
                  Track_setChi2PerDoF_26,
                  PUBLIC);

  metaC.addMethod("nDoF",
                  "number of degrees of freedom of the track",
                  (std::string("int")).c_str(),
                  Track_nDoF_27,
                  PUBLIC);

  metaC.addMethod("setNDoF",
                  "number of degrees of freedom of the track",
                  (std::string("int")).c_str(),
                  Track_setNDoF_28,
                  PUBLIC);

  metaC.addMethod("setType",
                  "set track type",
                  (std::string("int")).c_str(),
                  Track_setType_29,
                  PUBLIC);

  metaC.addMethod("checkType",
                  "check track type",
                  (std::string("bool")).c_str(),
                  (std::string("int")).c_str(),
                  Track_checkType_30,
                  PUBLIC);

  metaC.addMethod("setFlag",
                  "set track flags",
                  (std::string("int")).c_str(),
                  Track_setFlag_31,
                  PUBLIC);

  metaC.addMethod("checkFlag",
                  "check track flags",
                  (std::string("bool")).c_str(),
                  (std::string("int")).c_str(),
                  Track_checkFlag_32,
                  PUBLIC);

  metaC.addMethod("flags",
                  "the variety of track flags",
                  (std::string("unsigned int")).c_str(),
                  Track_flags_33,
                  PUBLIC);

  metaC.addMethod("lhcbIDs",
                  "container of LHCbIDs",
                  (std::string("std::vector<LHCbID>&")).c_str(),
                  Track_lhcbIDs_34,
                  PUBLIC);

  metaC.addMethod("setLhcbIDs",
                  "container of LHCbIDs",
                  (std::string("const std::vector<LHCbID>&")).c_str(),
                  Track_setLhcbIDs_35,
                  PUBLIC);

  metaC.addMethod("physicsState",
                  "State closest to the beam-line",
                  (System::typeinfoName(typeid(State))+std::string("&")).c_str(),
                  Track_physicsState_36,
                  PUBLIC);

  metaC.addMethod("setPhysicsState",
                  "State closest to the beam-line",
                  (System::typeinfoName(typeid(const State))+std::string("&")).c_str(),
                  Track_setPhysicsState_37,
                  PUBLIC);

  metaC.addMethod("states",
                  "container with pointers to all the states",
                  (std::string("std::vector<State*>&")).c_str(),
                  Track_states_38,
                  PUBLIC);

  metaC.addMethod("setStates",
                  "container with pointers to all the states",
                  (std::string("const std::vector<State*>&")).c_str(),
                  Track_setStates_39,
                  PUBLIC);



  metaC.build();
}

namespace 
{
  struct _InitDict
  {
    _InitDict()
    {
      Track_dict();
      GaudiDict::KeyedObjectDict<int>();
      GaudiDict::SmartRefDict<Track>();
      GaudiDict::SmartRefVectorDict<Track>();
      GaudiDict::VectorDict<Track*>();
      GaudiDict::ObjectVectorDict<Track>();
      GaudiDict::KeyedContainerDict<Track>();
      Class::resolvePending();
    }
  };

  static _InitDict __init;
}

void* __init_InitDict(Track* /* dummy */ )
{
  return &__init;
}
