

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



//Include files
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"

#include <string>

#include "RichLocalPID_dict.h"

#include "GaudiIntrospection/Introspection.h"


//------------------------------------------------------------------------------
class RichLocalPID_dict
//------------------------------------------------------------------------------
{
public:
  RichLocalPID_dict();
};

//------------------------------------------------------------------------------
static void* RichLocalPID_localPIDType_0(void* v)
//------------------------------------------------------------------------------
{
  static unsigned long ret;
  ret =  ((RichLocalPID*)v)->localPIDType();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichLocalPID_setLocalPIDType_1(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichLocalPID*)v)->setLocalPIDType(*(unsigned long*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichLocalPID_gaussianSignal_2(void* v)
//------------------------------------------------------------------------------
{
  const std::vector<float>& ret = ((RichLocalPID*)v)->gaussianSignal();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichLocalPID_setGaussianSignal_3(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichLocalPID*)v)->setGaussianSignal(*(std::vector<float>*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichLocalPID_gaussianBackgrd_4(void* v)
//------------------------------------------------------------------------------
{
  const std::vector<float>& ret = ((RichLocalPID*)v)->gaussianBackgrd();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichLocalPID_setGaussianBackgrd_5(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichLocalPID*)v)->setGaussianBackgrd(*(std::vector<float>*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichLocalPID_signalCount_6(void* v)
//------------------------------------------------------------------------------
{
  const std::vector<int>& ret = ((RichLocalPID*)v)->signalCount();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichLocalPID_setSignalCount_7(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichLocalPID*)v)->setSignalCount(*(std::vector<int>*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichLocalPID_sideCount_8(void* v)
//------------------------------------------------------------------------------
{
  const std::vector<int>& ret = ((RichLocalPID*)v)->sideCount();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichLocalPID_setSideCount_9(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichLocalPID*)v)->setSideCount(*(std::vector<int>*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichLocalPID_signalProbability_10(void* v)
//------------------------------------------------------------------------------
{
  const std::vector<float>& ret = ((RichLocalPID*)v)->signalProbability();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichLocalPID_setSignalProbability_11(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichLocalPID*)v)->setSignalProbability(*(std::vector<float>*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichLocalPID_ringProbability_12(void* v)
//------------------------------------------------------------------------------
{
  const std::vector<float>& ret = ((RichLocalPID*)v)->ringProbability();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichLocalPID_setRingProbability_13(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichLocalPID*)v)->setRingProbability(*(std::vector<float>*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichLocalPID_relatedPIDs_14(void* v)
//------------------------------------------------------------------------------
{
  const SmartRefVector<RichLocalPID>& ret = ((RichLocalPID*)v)->relatedPIDs();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichLocalPID_setRelatedPIDs_15(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichLocalPID*)v)->setRelatedPIDs(*(SmartRefVector<RichLocalPID>*)argList[0]);
}

//------------------------------------------------------------------------------
static void RichLocalPID_clearRelatedPIDs_16(void* v)
//------------------------------------------------------------------------------
{
  ((RichLocalPID*)v)->clearRelatedPIDs();
}

//------------------------------------------------------------------------------
static void RichLocalPID_addToRelatedPIDs_17(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichLocalPID*)v)->addToRelatedPIDs((RichLocalPID*)argList[0]);
}

//------------------------------------------------------------------------------
static void RichLocalPID_removeFromRelatedPIDs_18(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichLocalPID*)v)->removeFromRelatedPIDs((RichLocalPID*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichLocalPID_constructor_0()
//------------------------------------------------------------------------------
{
  static RichLocalPID* ret = new RichLocalPID();
  return ret;
}

//------------------------------------------------------------------------------
static RichLocalPID_dict instance;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
RichLocalPID_dict::RichLocalPID_dict()
//------------------------------------------------------------------------------
{
  std::vector<std::string> argTypes = std::vector<std::string>();
  MetaClass* metaC = new MetaClass("RichLocalPID",
                "Rich particle identification object for the local track based algorithm",
                0);

  RichLocalPID* cl = new RichLocalPID();
  metaC->addSuperClass("RichPID",
                (((int)cl)-((int)((RichPID*)cl))));
  delete cl;

  metaC->addConstructor("Default constructor",
                RichLocalPID_constructor_0);

  metaC->addField("localPIDType",
                "unsigned long",
                "origin of the PID result",
                &((RichLocalPID*)0)->m_localPIDType,
                MetaModifier::setPrivate());

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addField("gaussianSignal",
                "std::vector<float>",
                "Deduced signal using Gaussian weighted sum for each mass hypothesis",
                &((RichLocalPID*)0)->m_gaussianSignal,
                MetaModifier::setPrivate());

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addField("gaussianBackgrd",
                "std::vector<float>",
                "Deduced background using Gaussian weighted sum for each mass hypothesis",
                &((RichLocalPID*)0)->m_gaussianBackgrd,
                MetaModifier::setPrivate());

  MetaClass::condCreateClass("std::vector<int>",
                "standard container of int",
                sizeof(std::vector<int>),
                0xC1,
                0);
  metaC->addField("signalCount",
                "std::vector<int>",
                " Number of photon candidates in signal region for each mass hypothesis",
                &((RichLocalPID*)0)->m_signalCount,
                MetaModifier::setPrivate());

  MetaClass::condCreateClass("std::vector<int>",
                "standard container of int",
                sizeof(std::vector<int>),
                0xC1,
                0);
  metaC->addField("sideCount",
                "std::vector<int>",
                " Number of photon candidates in signal +side-band regions for each mass hypothesis",
                &((RichLocalPID*)0)->m_sideCount,
                MetaModifier::setPrivate());

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addField("signalProbability",
                "std::vector<float>",
                "Probability values for each hypothesis based on observed signal",
                &((RichLocalPID*)0)->m_signalProbability,
                MetaModifier::setPrivate());

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addField("ringProbability",
                "std::vector<float>",
                "Probability values for each hypothesis based on a measure of the ring quality",
                &((RichLocalPID*)0)->m_ringProbability,
                MetaModifier::setPrivate());

  metaC->addField("relatedPIDs",
                "SmartRefVector<RichLocalPID>",
                "related local PIDs",
                &((RichLocalPID*)0)->m_relatedPIDs,
                MetaModifier::setPrivate());

  metaC->addMethod("localPIDType",
                "origin of the PID result",
                "unsigned long",
                RichLocalPID_localPIDType_0);

  argTypes.clear();
  argTypes.push_back("unsigned long");
  metaC->addMethod("setLocalPIDType",
                "origin of the PID result",
                argTypes,
                RichLocalPID_setLocalPIDType_1);

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addMethod("gaussianSignal",
                "Deduced signal using Gaussian weighted sum for each mass hypothesis",
                "std::vector<float>",
                RichLocalPID_gaussianSignal_2);

  argTypes.clear();
  argTypes.push_back("std::vector<float>");
  metaC->addMethod("setGaussianSignal",
                "Deduced signal using Gaussian weighted sum for each mass hypothesis",
                argTypes,
                RichLocalPID_setGaussianSignal_3);

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addMethod("gaussianBackgrd",
                "Deduced background using Gaussian weighted sum for each mass hypothesis",
                "std::vector<float>",
                RichLocalPID_gaussianBackgrd_4);

  argTypes.clear();
  argTypes.push_back("std::vector<float>");
  metaC->addMethod("setGaussianBackgrd",
                "Deduced background using Gaussian weighted sum for each mass hypothesis",
                argTypes,
                RichLocalPID_setGaussianBackgrd_5);

  MetaClass::condCreateClass("std::vector<int>",
                "standard container of int",
                sizeof(std::vector<int>),
                0xC1,
                0);
  metaC->addMethod("signalCount",
                " Number of photon candidates in signal region for each mass hypothesis",
                "std::vector<int>",
                RichLocalPID_signalCount_6);

  argTypes.clear();
  argTypes.push_back("std::vector<int>");
  metaC->addMethod("setSignalCount",
                " Number of photon candidates in signal region for each mass hypothesis",
                argTypes,
                RichLocalPID_setSignalCount_7);

  MetaClass::condCreateClass("std::vector<int>",
                "standard container of int",
                sizeof(std::vector<int>),
                0xC1,
                0);
  metaC->addMethod("sideCount",
                " Number of photon candidates in signal +side-band regions for each mass hypothesis",
                "std::vector<int>",
                RichLocalPID_sideCount_8);

  argTypes.clear();
  argTypes.push_back("std::vector<int>");
  metaC->addMethod("setSideCount",
                " Number of photon candidates in signal +side-band regions for each mass hypothesis",
                argTypes,
                RichLocalPID_setSideCount_9);

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addMethod("signalProbability",
                "Probability values for each hypothesis based on observed signal",
                "std::vector<float>",
                RichLocalPID_signalProbability_10);

  argTypes.clear();
  argTypes.push_back("std::vector<float>");
  metaC->addMethod("setSignalProbability",
                "Probability values for each hypothesis based on observed signal",
                argTypes,
                RichLocalPID_setSignalProbability_11);

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addMethod("ringProbability",
                "Probability values for each hypothesis based on a measure of the ring quality",
                "std::vector<float>",
                RichLocalPID_ringProbability_12);

  argTypes.clear();
  argTypes.push_back("std::vector<float>");
  metaC->addMethod("setRingProbability",
                "Probability values for each hypothesis based on a measure of the ring quality",
                argTypes,
                RichLocalPID_setRingProbability_13);

  metaC->addMethod("relatedPIDs",
                "related local PIDs",
                "SmartRefVector<RichLocalPID>",
                RichLocalPID_relatedPIDs_14);

  argTypes.clear();
  argTypes.push_back("RichLocalPID");
  metaC->addMethod("setRelatedPIDs",
                "related local PIDs",
                argTypes,
                RichLocalPID_setRelatedPIDs_15);

  metaC->addMethod("clearRelatedPIDs",
                "related local PIDs",
                RichLocalPID_clearRelatedPIDs_16);

  argTypes.clear();
  argTypes.push_back("RichLocalPID");
  metaC->addMethod("addToRelatedPIDs",
                "related local PIDs",
                argTypes,
                RichLocalPID_addToRelatedPIDs_17);

  argTypes.clear();
  argTypes.push_back("RichLocalPID");
  metaC->addMethod("removeFromRelatedPIDs",
                "related local PIDs",
                argTypes,
                RichLocalPID_removeFromRelatedPIDs_18);

  MetaPropertyList* pl = new MetaPropertyList();
  pl->setProperty("Author", "Chris Jones    (Christopher.Rob.Jones@cern.ch)");
  pl->setProperty("ClassID", "12441");
  metaC->setPropertyList(pl);
}


