
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

#include "State_dict.h"

using namespace seal::reflect;

//------------------------------------------------------------------------------
class State_dict
//------------------------------------------------------------------------------
{
public:
  State_dict();
};  

//------------------------------------------------------------------------------
static void* State_nParameters_0const(void* v)
//------------------------------------------------------------------------------
{
  static unsigned int ret;
  ret = ((State*)v)->nParameters();
  return &ret;
}

//------------------------------------------------------------------------------
static void State_positionAndMomentum_1const(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((State*)v)->positionAndMomentum(*(HepPoint3D*)argList[0],
                                   *(HepVector3D*)argList[1],
                                   *(HepSymMatrix*)argList[2]);
}

//------------------------------------------------------------------------------
static void* State_position_2const(void* v)
//------------------------------------------------------------------------------
{
  static HepPoint3D ret = ((State*)v)->position();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void* State_x_3const(void* v)
//------------------------------------------------------------------------------
{
  static double ret;
  ret = ((State*)v)->x();
  return &ret;
}

//------------------------------------------------------------------------------
static void* State_y_4const(void* v)
//------------------------------------------------------------------------------
{
  static double ret;
  ret = ((State*)v)->y();
  return &ret;
}

//------------------------------------------------------------------------------
static void* State_z_5const(void* v)
//------------------------------------------------------------------------------
{
  static double ret;
  ret = ((State*)v)->z();
  return &ret;
}

//------------------------------------------------------------------------------
static void* State_slopes_6const(void* v)
//------------------------------------------------------------------------------
{
  static HepVector3D ret = ((State*)v)->slopes();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void* State_tx_7const(void* v)
//------------------------------------------------------------------------------
{
  static double ret;
  ret = ((State*)v)->tx();
  return &ret;
}

//------------------------------------------------------------------------------
static void* State_ty_8const(void* v)
//------------------------------------------------------------------------------
{
  static double ret;
  ret = ((State*)v)->ty();
  return &ret;
}

//------------------------------------------------------------------------------
static void* State_qOverP_9const(void* v)
//------------------------------------------------------------------------------
{
  static double ret;
  ret = ((State*)v)->qOverP();
  return &ret;
}

//------------------------------------------------------------------------------
static void* State_p_10const(void* v)
//------------------------------------------------------------------------------
{
  static double ret;
  ret = ((State*)v)->p();
  return &ret;
}

//------------------------------------------------------------------------------
static void* State_pt_11const(void* v)
//------------------------------------------------------------------------------
{
  static double ret;
  ret = ((State*)v)->pt();
  return &ret;
}

//------------------------------------------------------------------------------
static void* State_momentum_12const(void* v)
//------------------------------------------------------------------------------
{
  static HepVector3D ret = ((State*)v)->momentum();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void* State_qOverPperp_13const(void* v)
//------------------------------------------------------------------------------
{
  static double ret;
  ret = ((State*)v)->qOverPperp();
  return &ret;
}

//------------------------------------------------------------------------------
static void* State_posMomCovariance_14const(void* v)
//------------------------------------------------------------------------------
{
  static HepSymMatrix ret = ((State*)v)->posMomCovariance();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void* State_errPosition_15const(void* v)
//------------------------------------------------------------------------------
{
  static HepSymMatrix ret = ((State*)v)->errPosition();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void* State_errX2_16const(void* v)
//------------------------------------------------------------------------------
{
  static double ret;
  ret = ((State*)v)->errX2();
  return &ret;
}

//------------------------------------------------------------------------------
static void* State_errY2_17const(void* v)
//------------------------------------------------------------------------------
{
  static double ret;
  ret = ((State*)v)->errY2();
  return &ret;
}

//------------------------------------------------------------------------------
static void* State_errSlopes_18const(void* v)
//------------------------------------------------------------------------------
{
  static HepSymMatrix ret = ((State*)v)->errSlopes();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void* State_errTx2_19const(void* v)
//------------------------------------------------------------------------------
{
  static double ret;
  ret = ((State*)v)->errTx2();
  return &ret;
}

//------------------------------------------------------------------------------
static void* State_errTy2_20const(void* v)
//------------------------------------------------------------------------------
{
  static double ret;
  ret = ((State*)v)->errTy2();
  return &ret;
}

//------------------------------------------------------------------------------
static void* State_errQOverP2_21const(void* v)
//------------------------------------------------------------------------------
{
  static double ret;
  ret = ((State*)v)->errQOverP2();
  return &ret;
}

//------------------------------------------------------------------------------
static void* State_errP2_22const(void* v)
//------------------------------------------------------------------------------
{
  static double ret;
  ret = ((State*)v)->errP2();
  return &ret;
}

//------------------------------------------------------------------------------
static void* State_errMomentum_23const(void* v)
//------------------------------------------------------------------------------
{
  static HepSymMatrix ret = ((State*)v)->errMomentum();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void* State_errQOverPperp2_24const(void* v)
//------------------------------------------------------------------------------
{
  static double ret;
  ret = ((State*)v)->errQOverPperp2();
  return &ret;
}

//------------------------------------------------------------------------------
static void* State_clone_25const(void* v)
//------------------------------------------------------------------------------
{
  return (State*) ((State*)v)->clone();
}

//------------------------------------------------------------------------------
static void State_reset_26(void* v)
//------------------------------------------------------------------------------
{
  ((State*)v)->reset();
}

//------------------------------------------------------------------------------
static void State_setState_27(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((State*)v)->setState(*(const HepVector*)argList[0]);
}

//------------------------------------------------------------------------------
static void State_setState_28(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((State*)v)->setState(*(double*)argList[0],
                        *(double*)argList[1],
                        *(double*)argList[2],
                        *(double*)argList[3],
                        *(double*)argList[4],
                        *(double*)argList[5]);
}

//------------------------------------------------------------------------------
static void State_setState_29(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((State*)v)->setState(*(double*)argList[0],
                        *(double*)argList[1],
                        *(double*)argList[2],
                        *(double*)argList[3],
                        *(double*)argList[4]);
}

//------------------------------------------------------------------------------
static void State_setCovariance_30(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((State*)v)->setCovariance(*(const HepSymMatrix*)argList[0]);
}

//------------------------------------------------------------------------------
static void State_setX_31(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((State*)v)->setX(*(double*)argList[0]);
}

//------------------------------------------------------------------------------
static void State_setY_32(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((State*)v)->setY(*(double*)argList[0]);
}

//------------------------------------------------------------------------------
static void State_setZ_33(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((State*)v)->setZ(*(double*)argList[0]);
}

//------------------------------------------------------------------------------
static void State_setTx_34(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((State*)v)->setTx(*(double*)argList[0]);
}

//------------------------------------------------------------------------------
static void State_setTy_35(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((State*)v)->setTy(*(double*)argList[0]);
}

//------------------------------------------------------------------------------
static void State_setQOverP_36(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((State*)v)->setQOverP(*(double*)argList[0]);
}


//------------------------------------------------------------------------------
static void* State_checkType_37(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  static bool ret;
  ret = ((State*)v)->checkType(*(State::Type*)argList[0]);
  return &ret;
}

//------------------------------------------------------------------------------
static void State_setLocation_38(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  ((State*)v)->setLocation(*(State::Location*)argList[0]);
}

//------------------------------------------------------------------------------
static void* State_checkLocation_39(void* v, const std::vector<void*>& argList)
//------------------------------------------------------------------------------
{
  static bool ret;
  ret = ((State*)v)->checkLocation(*(State::Location*)argList[0]);
  return &ret;
}

//------------------------------------------------------------------------------
static void* State_flags_40(void* v)
//------------------------------------------------------------------------------
{
  static unsigned int ret;
  ret = ((State*)v)->flags();
  return &ret;
}

//------------------------------------------------------------------------------
static void* State_state_41(void* v)
//------------------------------------------------------------------------------
{
  const HepVector& ret = ((State*)v)->state();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void* State_covariance_42(void* v)
//------------------------------------------------------------------------------
{
  const HepSymMatrix& ret = ((State*)v)->covariance();
  return (void*)&ret;
}


//------------------------------------------------------------------------------
static void* State_constructor_1(void* mem)
//------------------------------------------------------------------------------
{
  return new(mem) State();
}


//------------------------------------------------------------------------------
static void State_destructor(void* mem)
//------------------------------------------------------------------------------
{
  ((State*)mem)->~State();
}



//------------------------------------------------------------------------------
State_dict::State_dict()
//------------------------------------------------------------------------------
{
  ClassBuilder metaC("State",
                     "State is the base class of offline and online track states.",
                     typeid(State),
                     sizeof(State),
                     std::vector<const std::type_info*>(),
                     false,
                     NOCONTAINER,
                     0,
                     PUBLIC | VIRTUAL);


  static char clid_txt[64];
  sprintf(clid_txt,"%08X-0000-0000-0000-000000000000",10011);
  metaC.addProperty("ClassID", (char*)clid_txt);

  metaC.addProperty("Author", "Jose Hernando, Eduardo Rodrigues");

  metaC.addMethod("State",
                  "Default constructor. State defined to be of type State::HasMomentum.",
                  (System::typeinfoName(typeid(State))).c_str(),
                  State_constructor_1,
                  PUBLIC);


  metaC.addMethod("~State",
                  "default destructor",
                  State_destructor,
                  PUBLIC | VIRTUAL);


  metaC.addMethod("nParameters",
                  "Retrieve the number of state parameters",
                  (std::string("unsigned int")).c_str(),
                  State_nParameters_0const,
                  PUBLIC | CONST);

  metaC.addMethod("positionAndMomentum",
                  "Retrieve the position and momentum vectors and the corresponding 6D covariance matrix (pos:1->3,mom:4-6) of the state",
                  (System::typeinfoName(typeid(HepPoint3D))+std::string("&")+";"+System::typeinfoName(typeid(HepVector3D))+std::string("&")+";"+System::typeinfoName(typeid(HepSymMatrix))+std::string("&")).c_str(),
                  State_positionAndMomentum_1const,
                  PUBLIC | CONST);

  metaC.addMethod("position",
                  "Retrieve the 3D-position vector of the state",
                  (System::typeinfoName(typeid(HepPoint3D))).c_str(),
                  State_position_2const,
                  PUBLIC | CONST);

  metaC.addMethod("x",
                  "Retrieve the x-position of the state",
                  (std::string("double")).c_str(),
                  State_x_3const,
                  PUBLIC | CONST);

  metaC.addMethod("y",
                  "Retrieve the y-position of the state",
                  (std::string("double")).c_str(),
                  State_y_4const,
                  PUBLIC | CONST);

  metaC.addMethod("z",
                  "Retrieve the z-position of the state",
                  (std::string("double")).c_str(),
                  State_z_5const,
                  PUBLIC | CONST);

  metaC.addMethod("slopes",
                  "Retrieve the slopes (Tx=dx/dz,Ty=dy/dz,1.) of the state",
                  (System::typeinfoName(typeid(HepVector3D))).c_str(),
                  State_slopes_6const,
                  PUBLIC | CONST);

  metaC.addMethod("tx",
                  "Retrieve the Tx=dx/dz slope of the state",
                  (std::string("double")).c_str(),
                  State_tx_7const,
                  PUBLIC | CONST);

  metaC.addMethod("ty",
                  "Retrieve the Ty=dy/dz slope of the state",
                  (std::string("double")).c_str(),
                  State_ty_8const,
                  PUBLIC | CONST);

  metaC.addMethod("qOverP",
                  "Retrieve the charge-over-momentum Q/P of the state",
                  (std::string("double")).c_str(),
                  State_qOverP_9const,
                  PUBLIC | CONST | VIRTUAL);

  metaC.addMethod("p",
                  "Retrieve the momentum of the state",
                  (std::string("double")).c_str(),
                  State_p_10const,
                  PUBLIC | CONST | VIRTUAL);

  metaC.addMethod("pt",
                  "Retrieve the transverse momentum of the state",
                  (std::string("double")).c_str(),
                  State_pt_11const,
                  PUBLIC | CONST | VIRTUAL);

  metaC.addMethod("momentum",
                  "Retrieve the momentum vector of the state",
                  (System::typeinfoName(typeid(HepVector3D))).c_str(),
                  State_momentum_12const,
                  PUBLIC | CONST);

  metaC.addMethod("qOverPperp",
                  "Retrieve the Q/Pperp (ratio of the charge to the component of the momentum transverse to the magnetic field) of the state",
                  (std::string("double")).c_str(),
                  State_qOverPperp_13const,
                  PUBLIC | CONST);

  metaC.addMethod("posMomCovariance",
                  "Retrieve the 6D covariance matrix (x,y,z,px,py,pz) of the state",
                  (System::typeinfoName(typeid(HepSymMatrix))).c_str(),
                  State_posMomCovariance_14const,
                  PUBLIC | CONST | VIRTUAL);

  metaC.addMethod("errPosition",
                  "Retrieve the errors on the 3D-position vector of the state",
                  (System::typeinfoName(typeid(HepSymMatrix))).c_str(),
                  State_errPosition_15const,
                  PUBLIC | CONST);

  metaC.addMethod("errX2",
                  "Retrieve the squared error on the x-position of the state",
                  (std::string("double")).c_str(),
                  State_errX2_16const,
                  PUBLIC | CONST);

  metaC.addMethod("errY2",
                  "Retrieve the squared error on the y-position of the state",
                  (std::string("double")).c_str(),
                  State_errY2_17const,
                  PUBLIC | CONST);

  metaC.addMethod("errSlopes",
                  "Retrieve the errors on the slopes of the state",
                  (System::typeinfoName(typeid(HepSymMatrix))).c_str(),
                  State_errSlopes_18const,
                  PUBLIC | CONST);

  metaC.addMethod("errTx2",
                  "Retrieve the squared error on the x-slope Tx=dx/dz of the state",
                  (std::string("double")).c_str(),
                  State_errTx2_19const,
                  PUBLIC | CONST);

  metaC.addMethod("errTy2",
                  "Retrieve the squared error on the y-slope Ty=dy/dz of the state",
                  (std::string("double")).c_str(),
                  State_errTy2_20const,
                  PUBLIC | CONST);

  metaC.addMethod("errQOverP2",
                  "Retrieve the squared error on the charge-over-momentum Q/P of the state",
                  (std::string("double")).c_str(),
                  State_errQOverP2_21const,
                  PUBLIC | CONST | VIRTUAL);

  metaC.addMethod("errP2",
                  "Retrieve the squared error on the momentum of the state",
                  (std::string("double")).c_str(),
                  State_errP2_22const,
                  PUBLIC | CONST | VIRTUAL);

  metaC.addMethod("errMomentum",
                  "Retrieve the errors on the momentum vector of the stateRetrieve the errors on the momentum vector of the state",
                  (System::typeinfoName(typeid(HepSymMatrix))).c_str(),
                  State_errMomentum_23const,
                  PUBLIC | CONST | VIRTUAL);

  metaC.addMethod("errQOverPperp2",
                  "Retrieve the squared error on the Q/Pperp of the state",
                  (std::string("double")).c_str(),
                  State_errQOverPperp2_24const,
                  PUBLIC | CONST | VIRTUAL);

  metaC.addMethod("clone",
                  "Clone the state",
                  (System::typeinfoName(typeid(State*))).c_str(),
                  State_clone_25const,
                  PUBLIC | CONST | VIRTUAL);

  metaC.addMethod("reset",
                  "Clear the state before re-using it",
                  State_reset_26,
                  PUBLIC | VIRTUAL);

  metaC.addMethod("setState",
                  "Update the state vector",
                  (System::typeinfoName(typeid(const HepVector))+std::string("&")).c_str(),
                  State_setState_27,
                  PUBLIC);

  metaC.addMethod("setState",
                  "Update the state vector (presumably of type State::HasMomentum)",
                  (std::string("double")+";"+std::string("double")+";"+std::string("double")+";"+std::string("double")+";"+std::string("double")+";"+std::string("double")).c_str(),
                  State_setState_28,
                  PUBLIC | VIRTUAL);

  metaC.addMethod("setState",
                  "Update the state vector (presumably of type State::StraightLine)",
                  (std::string("double")+";"+std::string("double")+";"+std::string("double")+";"+std::string("double")+";"+std::string("double")).c_str(),
                  State_setState_29,
                  PUBLIC);

  metaC.addMethod("setCovariance",
                  "Update the state covariance",
                  (System::typeinfoName(typeid(const HepSymMatrix))+std::string("&")).c_str(),
                  State_setCovariance_30,
                  PUBLIC);

  metaC.addMethod("setX",
                  "Update the x-position of the state",
                  (std::string("double")).c_str(),
                  State_setX_31,
                  PUBLIC);

  metaC.addMethod("setY",
                  "Update the y-position of the state",
                  (std::string("double")).c_str(),
                  State_setY_32,
                  PUBLIC);

  metaC.addMethod("setZ",
                  "Update the z-position of the state",
                  (std::string("double")).c_str(),
                  State_setZ_33,
                  PUBLIC);

  metaC.addMethod("setTx",
                  "Update the Tx slope of the state",
                  (std::string("double")).c_str(),
                  State_setTx_34,
                  PUBLIC);

  metaC.addMethod("setTy",
                  "Update the Ty slope of the state",
                  (std::string("double")).c_str(),
                  State_setTy_35,
                  PUBLIC);

  metaC.addMethod("setQOverP",
                  "Update the Q/P value of the state",
                  (std::string("double")).c_str(),
                  State_setQOverP_36,
                  PUBLIC | VIRTUAL);

 
  metaC.addField("flags",
                 (std::string("unsigned int")).c_str(),
                 "the variety of state flags",
                 OffsetOf(State, m_flags),
                 PROTECTED);

  metaC.addField("state",
                 (System::typeinfoName(typeid(HepVector))).c_str(),
                 "the state vector",
                 OffsetOf(State, m_state),
                 PRIVATE);

  metaC.addField("covariance",
                 (System::typeinfoName(typeid(HepSymMatrix))).c_str(),
                 "the state covariance matrix (indexes 1,...,5 for x, y, tx, ty, Q/p)",
                 OffsetOf(State, m_covariance),
                 PRIVATE);

  metaC.addField("z",
                 (std::string("double")).c_str(),
                 "the z-position of the state",
                 OffsetOf(State, m_z),
                 PRIVATE);


  metaC.addMethod("checkType",
                  "check state type",
                  (std::string("bool")).c_str(),
                  (std::string("int")).c_str(),
                  State_checkType_37,
                  PUBLIC);

  metaC.addMethod("setLocation",
                  "set state location",
                  (std::string("int")).c_str(),
                  State_setLocation_38,
                  PUBLIC);

  metaC.addMethod("checkLocation",
                  "check state location",
                  (std::string("bool")).c_str(),
                  (std::string("int")).c_str(),
                  State_checkLocation_39,
                  PUBLIC);

  metaC.addMethod("flags",
                  "the variety of state flags",
                  (std::string("unsigned int")).c_str(),
                  State_flags_40,
                  PUBLIC);

  metaC.addMethod("state",
                  "the state vector",
                  (System::typeinfoName(typeid(HepVector))+std::string("&")).c_str(),
                  State_state_41,
                  PUBLIC);

  metaC.addMethod("covariance",
                  "the state covariance matrix (indexes 1,...,5 for x, y, tx, ty, Q/p)",
                  (System::typeinfoName(typeid(HepSymMatrix))+std::string("&")).c_str(),
                  State_covariance_42,
                  PUBLIC);



  metaC.build();
}

namespace 
{
  struct _InitDict
  {
    _InitDict()
    {
      State_dict();
      GaudiDict::VectorDict<State*>();
      Class::resolvePending();
    }
  };

  static _InitDict __init;
}

void* __init_InitDict(State* /* dummy */ )
{
  return &__init;
}
