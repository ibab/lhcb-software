

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

#include "RichRecTrack_dict.h"

#include "GaudiIntrospection/Introspection.h"


//------------------------------------------------------------------------------
class RichRecTrack_dict
//------------------------------------------------------------------------------
{
public:
  RichRecTrack_dict();
};

//------------------------------------------------------------------------------
static void RichRecTrack_setNEmittedPhotons_0(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecTrack*)v)->setNEmittedPhotons(*(Rich::ParticleIDType*)argList[0],
          *(float*)argList[1]);
}

//------------------------------------------------------------------------------
static void RichRecTrack_setNDetectablePhotons_1(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecTrack*)v)->setNDetectablePhotons(*(Rich::ParticleIDType*)argList[0],
          *(float*)argList[1]);
}

//------------------------------------------------------------------------------
static void RichRecTrack_setNSignalPhotons_2(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecTrack*)v)->setNSignalPhotons(*(Rich::ParticleIDType*)argList[0],
          *(float*)argList[1]);
}

//------------------------------------------------------------------------------
static void RichRecTrack_setNObservableSignalPhotons_3(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecTrack*)v)->setNObservableSignalPhotons(*(Rich::ParticleIDType*)argList[0],
          *(float*)argList[1]);
}

//------------------------------------------------------------------------------
static void RichRecTrack_setNScatteredPhotons_4(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecTrack*)v)->setNScatteredPhotons(*(Rich::ParticleIDType*)argList[0],
          *(float*)argList[1]);
}

//------------------------------------------------------------------------------
static void RichRecTrack_setNObservableScatteredPhotons_5(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecTrack*)v)->setNObservableScatteredPhotons(*(Rich::ParticleIDType*)argList[0],
          *(float*)argList[1]);
}

//------------------------------------------------------------------------------
static void* RichRecTrack_nEmittedPhotons_6const(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  static float ret;
  ret = ((RichRecTrack*)v)->nEmittedPhotons(*(Rich::ParticleIDType*)argList[0]);
  return &ret;
}

//------------------------------------------------------------------------------
static void* RichRecTrack_nDetectablePhotons_7const(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  static float ret;
  ret = ((RichRecTrack*)v)->nDetectablePhotons(*(Rich::ParticleIDType*)argList[0]);
  return &ret;
}

//------------------------------------------------------------------------------
static void* RichRecTrack_nSignalPhotons_8const(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  static float ret;
  ret = ((RichRecTrack*)v)->nSignalPhotons(*(Rich::ParticleIDType*)argList[0]);
  return &ret;
}

//------------------------------------------------------------------------------
static void* RichRecTrack_nObservableSignalPhotons_9const(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  static float ret;
  ret = ((RichRecTrack*)v)->nObservableSignalPhotons(*(Rich::ParticleIDType*)argList[0]);
  return &ret;
}

//------------------------------------------------------------------------------
static void* RichRecTrack_nScatteredPhotons_10const(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  static float ret;
  ret = ((RichRecTrack*)v)->nScatteredPhotons(*(Rich::ParticleIDType*)argList[0]);
  return &ret;
}

//------------------------------------------------------------------------------
static void* RichRecTrack_nObservableScatteredPhotons_11const(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  static float ret;
  ret = ((RichRecTrack*)v)->nObservableScatteredPhotons(*(Rich::ParticleIDType*)argList[0]);
  return &ret;
}

//------------------------------------------------------------------------------
static void* RichRecTrack_nTotalObservablePhotons_12const(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  static float ret;
  ret = ((RichRecTrack*)v)->nTotalObservablePhotons(*(Rich::ParticleIDType*)argList[0]);
  return &ret;
}

//------------------------------------------------------------------------------
static void* RichRecTrack_vertexMomentum_13(void* v)
//------------------------------------------------------------------------------
{
  static float ret;
  ret =  ((RichRecTrack*)v)->vertexMomentum();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecTrack_setVertexMomentum_14(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecTrack*)v)->setVertexMomentum(*(float*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecTrack_parentType_15(void* v)
//------------------------------------------------------------------------------
{
  const Rich::RecTrack::ParentType& ret = ((RichRecTrack*)v)->parentType();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecTrack_setParentType_16(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecTrack*)v)->setParentType(*(Rich::RecTrack::ParentType*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecTrack_nEmittedPhotons_17(void* v)
//------------------------------------------------------------------------------
{
  const std::vector<float>& ret = ((RichRecTrack*)v)->nEmittedPhotons();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecTrack_setNEmittedPhotons_18(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecTrack*)v)->setNEmittedPhotons(*(std::vector<float>*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecTrack_nDetectablePhotons_19(void* v)
//------------------------------------------------------------------------------
{
  const std::vector<float>& ret = ((RichRecTrack*)v)->nDetectablePhotons();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecTrack_setNDetectablePhotons_20(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecTrack*)v)->setNDetectablePhotons(*(std::vector<float>*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecTrack_nSignalPhotons_21(void* v)
//------------------------------------------------------------------------------
{
  const std::vector<float>& ret = ((RichRecTrack*)v)->nSignalPhotons();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecTrack_setNSignalPhotons_22(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecTrack*)v)->setNSignalPhotons(*(std::vector<float>*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecTrack_nObservableSignalPhotons_23(void* v)
//------------------------------------------------------------------------------
{
  const std::vector<float>& ret = ((RichRecTrack*)v)->nObservableSignalPhotons();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecTrack_setNObservableSignalPhotons_24(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecTrack*)v)->setNObservableSignalPhotons(*(std::vector<float>*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecTrack_nScatteredPhotons_25(void* v)
//------------------------------------------------------------------------------
{
  const std::vector<float>& ret = ((RichRecTrack*)v)->nScatteredPhotons();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecTrack_setNScatteredPhotons_26(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecTrack*)v)->setNScatteredPhotons(*(std::vector<float>*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecTrack_nObservableScatteredPhotons_27(void* v)
//------------------------------------------------------------------------------
{
  const std::vector<float>& ret = ((RichRecTrack*)v)->nObservableScatteredPhotons();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecTrack_setNObservableScatteredPhotons_28(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecTrack*)v)->setNObservableScatteredPhotons(*(std::vector<float>*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecTrack_currentHypothesis_29(void* v)
//------------------------------------------------------------------------------
{
  const Rich::ParticleIDType& ret = ((RichRecTrack*)v)->currentHypothesis();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecTrack_setCurrentHypothesis_30(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecTrack*)v)->setCurrentHypothesis(*(Rich::ParticleIDType*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecTrack_inUse_31(void* v)
//------------------------------------------------------------------------------
{
  static bool ret;
  ret =  ((RichRecTrack*)v)->inUse();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecTrack_setInUse_32(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecTrack*)v)->setInUse(*(bool*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecTrack_trackType_33(void* v)
//------------------------------------------------------------------------------
{
  const Rich::Track::Type& ret = ((RichRecTrack*)v)->trackType();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecTrack_setTrackType_34(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecTrack*)v)->setTrackType(*(Rich::Track::Type*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecTrack_parentTrack_35(void* v)
//------------------------------------------------------------------------------
{
  ContainedObject* ret = ((RichRecTrack*)v)->parentTrack();
  return  (void*)ret;
}

//------------------------------------------------------------------------------
static void RichRecTrack_setParentTrack_36(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecTrack*)v)->setParentTrack((ContainedObject*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecTrack_richRecSegments_37(void* v)
//------------------------------------------------------------------------------
{
  const SmartRefVector<RichRecSegment>& ret = ((RichRecTrack*)v)->richRecSegments();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecTrack_setRichRecSegments_38(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecTrack*)v)->setRichRecSegments(*(SmartRefVector<RichRecSegment>*)argList[0]);
}

//------------------------------------------------------------------------------
static void RichRecTrack_clearRichRecSegments_39(void* v)
//------------------------------------------------------------------------------
{
  ((RichRecTrack*)v)->clearRichRecSegments();
}

//------------------------------------------------------------------------------
static void RichRecTrack_addToRichRecSegments_40(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecTrack*)v)->addToRichRecSegments((RichRecSegment*)argList[0]);
}

//------------------------------------------------------------------------------
static void RichRecTrack_removeFromRichRecSegments_41(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecTrack*)v)->removeFromRichRecSegments((RichRecSegment*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecTrack_richRecPixels_42(void* v)
//------------------------------------------------------------------------------
{
  const SmartRefVector<RichRecPixel>& ret = ((RichRecTrack*)v)->richRecPixels();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecTrack_setRichRecPixels_43(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecTrack*)v)->setRichRecPixels(*(SmartRefVector<RichRecPixel>*)argList[0]);
}

//------------------------------------------------------------------------------
static void RichRecTrack_clearRichRecPixels_44(void* v)
//------------------------------------------------------------------------------
{
  ((RichRecTrack*)v)->clearRichRecPixels();
}

//------------------------------------------------------------------------------
static void RichRecTrack_addToRichRecPixels_45(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecTrack*)v)->addToRichRecPixels((RichRecPixel*)argList[0]);
}

//------------------------------------------------------------------------------
static void RichRecTrack_removeFromRichRecPixels_46(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecTrack*)v)->removeFromRichRecPixels((RichRecPixel*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecTrack_richRecPhotons_47(void* v)
//------------------------------------------------------------------------------
{
  const SmartRefVector<RichRecPhoton>& ret = ((RichRecTrack*)v)->richRecPhotons();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecTrack_setRichRecPhotons_48(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecTrack*)v)->setRichRecPhotons(*(SmartRefVector<RichRecPhoton>*)argList[0]);
}

//------------------------------------------------------------------------------
static void RichRecTrack_clearRichRecPhotons_49(void* v)
//------------------------------------------------------------------------------
{
  ((RichRecTrack*)v)->clearRichRecPhotons();
}

//------------------------------------------------------------------------------
static void RichRecTrack_addToRichRecPhotons_50(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecTrack*)v)->addToRichRecPhotons((RichRecPhoton*)argList[0]);
}

//------------------------------------------------------------------------------
static void RichRecTrack_removeFromRichRecPhotons_51(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecTrack*)v)->removeFromRichRecPhotons((RichRecPhoton*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecTrack_constructor_0()
//------------------------------------------------------------------------------
{
  static RichRecTrack* ret = new RichRecTrack();
  return ret;
}

//------------------------------------------------------------------------------
static RichRecTrack_dict instance;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
RichRecTrack_dict::RichRecTrack_dict()
//------------------------------------------------------------------------------
{
  std::vector<std::string> argTypes = std::vector<std::string>();
  MetaClass* metaC = new MetaClass("RichRecTrack",
                "Rich reconstruction working track object.",
                0);

  RichRecTrack* cl = new RichRecTrack();
  metaC->addSuperClass("KeyedObject<int>",
                (((int)cl)-((int)((KeyedObject<int>*)cl))));
  delete cl;

  metaC->addConstructor("Default constructor",
                RichRecTrack_constructor_0);

  metaC->addField("vertexMomentum",
                "float",
                "Reconstructed momentum at vertex",
                &((RichRecTrack*)0)->m_vertexMomentum,
                MetaModifier::setPrivate());

  metaC->addField("parentType",
                "Rich::RecTrack::ParentType",
                "Enumeration to Parent type",
                &((RichRecTrack*)0)->m_parentType,
                MetaModifier::setPrivate());

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addField("nEmittedPhotons",
                "std::vector<float>",
                "The number of emitted photons expected for each particle hypothesis",
                &((RichRecTrack*)0)->m_nEmittedPhotons,
                MetaModifier::setPrivate());

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addField("nDetectablePhotons",
                "std::vector<float>",
                "The number of detectable emitted photons expected for each particle hypothesis",
                &((RichRecTrack*)0)->m_nDetectablePhotons,
                MetaModifier::setPrivate());

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addField("nSignalPhotons",
                "std::vector<float>",
                "The number of signal photons expected for each particle hypothesis",
                &((RichRecTrack*)0)->m_nSignalPhotons,
                MetaModifier::setPrivate());

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addField("nObservableSignalPhotons",
                "std::vector<float>",
                "The number of observable signal photons expected for each particle hypothesis",
                &((RichRecTrack*)0)->m_nObservableSignalPhotons,
                MetaModifier::setPrivate());

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addField("nScatteredPhotons",
                "std::vector<float>",
                "The number of scattered photons expected for each particle hypothesis",
                &((RichRecTrack*)0)->m_nScatteredPhotons,
                MetaModifier::setPrivate());

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addField("nObservableScatteredPhotons",
                "std::vector<float>",
                "The number of observable scattered photons expected for each particle hypothesis",
                &((RichRecTrack*)0)->m_nObservableScatteredPhotons,
                MetaModifier::setPrivate());

  metaC->addField("currentHypothesis",
                "Rich::ParticleIDType",
                "The current working particle hypothesis for this track. Used by specific algorithms",
                &((RichRecTrack*)0)->m_currentHypothesis,
                MetaModifier::setPrivate());

  metaC->addField("inUse",
                "bool",
                "Flag for use by algorithms to tag in this track is in use or not",
                &((RichRecTrack*)0)->m_inUse,
                MetaModifier::setPrivate());

  metaC->addField("trackType",
                "Rich::Track::Type",
                "Track Type. Used to define resolutions etc.",
                &((RichRecTrack*)0)->m_trackType,
                MetaModifier::setPrivate());

  metaC->addField("parentTrack",
                "SmartRef<ContainedObject>",
                "Parent Object this track was produced from",
                &((RichRecTrack*)0)->m_parentTrack,
                MetaModifier::setPrivate());

  metaC->addField("richRecSegments",
                "SmartRefVector<RichRecSegment>",
                "Smart Reference vector of radiator segments for this track",
                &((RichRecTrack*)0)->m_richRecSegments,
                MetaModifier::setPrivate());

  metaC->addField("richRecPixels",
                "SmartRefVector<RichRecPixel>",
                "SmartRefVector of RichRecPixels for which this track has valid photon associations",
                &((RichRecTrack*)0)->m_richRecPixels,
                MetaModifier::setPrivate());

  metaC->addField("richRecPhotons",
                "SmartRefVector<RichRecPhoton>",
                "SmartRefVector of associated RichRecPhotons",
                &((RichRecTrack*)0)->m_richRecPhotons,
                MetaModifier::setPrivate());

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  argTypes.push_back("float");
  metaC->addMethod("setNEmittedPhotons",
                "Set the number of emitted photons for a given particle type",
                argTypes,
                RichRecTrack_setNEmittedPhotons_0);

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  argTypes.push_back("float");
  metaC->addMethod("setNDetectablePhotons",
                "Set the number of detectable photons for a given particle type",
                argTypes,
                RichRecTrack_setNDetectablePhotons_1);

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  argTypes.push_back("float");
  metaC->addMethod("setNSignalPhotons",
                "Set the number of signal photons for a given particle type",
                argTypes,
                RichRecTrack_setNSignalPhotons_2);

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  argTypes.push_back("float");
  metaC->addMethod("setNObservableSignalPhotons",
                "Set the number of observable signal photons for a given particle type",
                argTypes,
                RichRecTrack_setNObservableSignalPhotons_3);

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  argTypes.push_back("float");
  metaC->addMethod("setNScatteredPhotons",
                "Set the number of scattered photons for a given particle type",
                argTypes,
                RichRecTrack_setNScatteredPhotons_4);

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  argTypes.push_back("float");
  metaC->addMethod("setNObservableScatteredPhotons",
                "Set the number of observable scattered photons for a given particle type",
                argTypes,
                RichRecTrack_setNObservableScatteredPhotons_5);

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  metaC->addMethod("nEmittedPhotons",
                "Returns the number of emitted photons for a given particle type",
                "float",
                argTypes,
                RichRecTrack_nEmittedPhotons_6const);

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  metaC->addMethod("nDetectablePhotons",
                "Returns the number of detectable photons for a given particle type",
                "float",
                argTypes,
                RichRecTrack_nDetectablePhotons_7const);

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  metaC->addMethod("nSignalPhotons",
                "Returns the number of signal photons for a given particle type",
                "float",
                argTypes,
                RichRecTrack_nSignalPhotons_8const);

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  metaC->addMethod("nObservableSignalPhotons",
                "Returns the number of observable signal photons for a given particle type",
                "float",
                argTypes,
                RichRecTrack_nObservableSignalPhotons_9const);

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  metaC->addMethod("nScatteredPhotons",
                "Returns the number of scattered photons for a given particle type",
                "float",
                argTypes,
                RichRecTrack_nScatteredPhotons_10const);

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  metaC->addMethod("nObservableScatteredPhotons",
                "Returns the number of observable scattered photons for a given particle type",
                "float",
                argTypes,
                RichRecTrack_nObservableScatteredPhotons_11const);

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  metaC->addMethod("nTotalObservablePhotons",
                "Returns the number of photons for a given particle type",
                "float",
                argTypes,
                RichRecTrack_nTotalObservablePhotons_12const);

  metaC->addMethod("vertexMomentum",
                "Reconstructed momentum at vertex",
                "float",
                RichRecTrack_vertexMomentum_13);

  argTypes.clear();
  argTypes.push_back("float");
  metaC->addMethod("setVertexMomentum",
                "Reconstructed momentum at vertex",
                argTypes,
                RichRecTrack_setVertexMomentum_14);

  metaC->addMethod("parentType",
                "Enumeration to Parent type",
                "Rich::RecTrack::ParentType",
                RichRecTrack_parentType_15);

  argTypes.clear();
  argTypes.push_back("Rich::RecTrack::ParentType");
  metaC->addMethod("setParentType",
                "Enumeration to Parent type",
                argTypes,
                RichRecTrack_setParentType_16);

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addMethod("nEmittedPhotons",
                "The number of emitted photons expected for each particle hypothesis",
                "std::vector<float>",
                RichRecTrack_nEmittedPhotons_17);

  argTypes.clear();
  argTypes.push_back("std::vector<float>");
  metaC->addMethod("setNEmittedPhotons",
                "The number of emitted photons expected for each particle hypothesis",
                argTypes,
                RichRecTrack_setNEmittedPhotons_18);

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addMethod("nDetectablePhotons",
                "The number of detectable emitted photons expected for each particle hypothesis",
                "std::vector<float>",
                RichRecTrack_nDetectablePhotons_19);

  argTypes.clear();
  argTypes.push_back("std::vector<float>");
  metaC->addMethod("setNDetectablePhotons",
                "The number of detectable emitted photons expected for each particle hypothesis",
                argTypes,
                RichRecTrack_setNDetectablePhotons_20);

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addMethod("nSignalPhotons",
                "The number of signal photons expected for each particle hypothesis",
                "std::vector<float>",
                RichRecTrack_nSignalPhotons_21);

  argTypes.clear();
  argTypes.push_back("std::vector<float>");
  metaC->addMethod("setNSignalPhotons",
                "The number of signal photons expected for each particle hypothesis",
                argTypes,
                RichRecTrack_setNSignalPhotons_22);

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addMethod("nObservableSignalPhotons",
                "The number of observable signal photons expected for each particle hypothesis",
                "std::vector<float>",
                RichRecTrack_nObservableSignalPhotons_23);

  argTypes.clear();
  argTypes.push_back("std::vector<float>");
  metaC->addMethod("setNObservableSignalPhotons",
                "The number of observable signal photons expected for each particle hypothesis",
                argTypes,
                RichRecTrack_setNObservableSignalPhotons_24);

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addMethod("nScatteredPhotons",
                "The number of scattered photons expected for each particle hypothesis",
                "std::vector<float>",
                RichRecTrack_nScatteredPhotons_25);

  argTypes.clear();
  argTypes.push_back("std::vector<float>");
  metaC->addMethod("setNScatteredPhotons",
                "The number of scattered photons expected for each particle hypothesis",
                argTypes,
                RichRecTrack_setNScatteredPhotons_26);

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addMethod("nObservableScatteredPhotons",
                "The number of observable scattered photons expected for each particle hypothesis",
                "std::vector<float>",
                RichRecTrack_nObservableScatteredPhotons_27);

  argTypes.clear();
  argTypes.push_back("std::vector<float>");
  metaC->addMethod("setNObservableScatteredPhotons",
                "The number of observable scattered photons expected for each particle hypothesis",
                argTypes,
                RichRecTrack_setNObservableScatteredPhotons_28);

  metaC->addMethod("currentHypothesis",
                "The current working particle hypothesis for this track. Used by specific algorithms",
                "Rich::ParticleIDType",
                RichRecTrack_currentHypothesis_29);

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  metaC->addMethod("setCurrentHypothesis",
                "The current working particle hypothesis for this track. Used by specific algorithms",
                argTypes,
                RichRecTrack_setCurrentHypothesis_30);

  metaC->addMethod("inUse",
                "Flag for use by algorithms to tag in this track is in use or not",
                "bool",
                RichRecTrack_inUse_31);

  argTypes.clear();
  argTypes.push_back("bool");
  metaC->addMethod("setInUse",
                "Flag for use by algorithms to tag in this track is in use or not",
                argTypes,
                RichRecTrack_setInUse_32);

  metaC->addMethod("trackType",
                "Track Type. Used to define resolutions etc.",
                "Rich::Track::Type",
                RichRecTrack_trackType_33);

  argTypes.clear();
  argTypes.push_back("Rich::Track::Type");
  metaC->addMethod("setTrackType",
                "Track Type. Used to define resolutions etc.",
                argTypes,
                RichRecTrack_setTrackType_34);

  metaC->addMethod("parentTrack",
                "Parent Object this track was produced from",
                "ContainedObject",
                RichRecTrack_parentTrack_35);

  argTypes.clear();
  argTypes.push_back("ContainedObject");
  metaC->addMethod("setParentTrack",
                "Parent Object this track was produced from",
                argTypes,
                RichRecTrack_setParentTrack_36);

  metaC->addMethod("richRecSegments",
                "Smart Reference vector of radiator segments for this track",
                "SmartRefVector<RichRecSegment>",
                RichRecTrack_richRecSegments_37);

  argTypes.clear();
  argTypes.push_back("RichRecSegment");
  metaC->addMethod("setRichRecSegments",
                "Smart Reference vector of radiator segments for this track",
                argTypes,
                RichRecTrack_setRichRecSegments_38);

  metaC->addMethod("clearRichRecSegments",
                "Smart Reference vector of radiator segments for this track",
                RichRecTrack_clearRichRecSegments_39);

  argTypes.clear();
  argTypes.push_back("RichRecSegment");
  metaC->addMethod("addToRichRecSegments",
                "Smart Reference vector of radiator segments for this track",
                argTypes,
                RichRecTrack_addToRichRecSegments_40);

  argTypes.clear();
  argTypes.push_back("RichRecSegment");
  metaC->addMethod("removeFromRichRecSegments",
                "Smart Reference vector of radiator segments for this track",
                argTypes,
                RichRecTrack_removeFromRichRecSegments_41);

  metaC->addMethod("richRecPixels",
                "SmartRefVector of RichRecPixels for which this track has valid photon associations",
                "SmartRefVector<RichRecPixel>",
                RichRecTrack_richRecPixels_42);

  argTypes.clear();
  argTypes.push_back("RichRecPixel");
  metaC->addMethod("setRichRecPixels",
                "SmartRefVector of RichRecPixels for which this track has valid photon associations",
                argTypes,
                RichRecTrack_setRichRecPixels_43);

  metaC->addMethod("clearRichRecPixels",
                "SmartRefVector of RichRecPixels for which this track has valid photon associations",
                RichRecTrack_clearRichRecPixels_44);

  argTypes.clear();
  argTypes.push_back("RichRecPixel");
  metaC->addMethod("addToRichRecPixels",
                "SmartRefVector of RichRecPixels for which this track has valid photon associations",
                argTypes,
                RichRecTrack_addToRichRecPixels_45);

  argTypes.clear();
  argTypes.push_back("RichRecPixel");
  metaC->addMethod("removeFromRichRecPixels",
                "SmartRefVector of RichRecPixels for which this track has valid photon associations",
                argTypes,
                RichRecTrack_removeFromRichRecPixels_46);

  metaC->addMethod("richRecPhotons",
                "SmartRefVector of associated RichRecPhotons",
                "SmartRefVector<RichRecPhoton>",
                RichRecTrack_richRecPhotons_47);

  argTypes.clear();
  argTypes.push_back("RichRecPhoton");
  metaC->addMethod("setRichRecPhotons",
                "SmartRefVector of associated RichRecPhotons",
                argTypes,
                RichRecTrack_setRichRecPhotons_48);

  metaC->addMethod("clearRichRecPhotons",
                "SmartRefVector of associated RichRecPhotons",
                RichRecTrack_clearRichRecPhotons_49);

  argTypes.clear();
  argTypes.push_back("RichRecPhoton");
  metaC->addMethod("addToRichRecPhotons",
                "SmartRefVector of associated RichRecPhotons",
                argTypes,
                RichRecTrack_addToRichRecPhotons_50);

  argTypes.clear();
  argTypes.push_back("RichRecPhoton");
  metaC->addMethod("removeFromRichRecPhotons",
                "SmartRefVector of associated RichRecPhotons",
                argTypes,
                RichRecTrack_removeFromRichRecPhotons_51);

  MetaPropertyList* pl = new MetaPropertyList();
  pl->setProperty("Author", "Chris Jones   (Christopher.Rob.Jones@cern.ch)");
  pl->setProperty("ClassID", "12421");
  metaC->setPropertyList(pl);
}


