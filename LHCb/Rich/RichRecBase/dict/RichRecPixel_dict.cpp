

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

#include "RichRecPixel_dict.h"

#include "GaudiIntrospection/Introspection.h"


//------------------------------------------------------------------------------
class RichRecPixel_dict
//------------------------------------------------------------------------------
{
public:
  RichRecPixel_dict();
};

//------------------------------------------------------------------------------
static void* RichRecPixel_detector_0const(void* v)
//------------------------------------------------------------------------------
{
  static Rich::DetectorType ret;
  ret = ((RichRecPixel*)v)->detector();
  return &ret;
}

//------------------------------------------------------------------------------
static void* RichRecPixel_hpd_1const(void* v)
//------------------------------------------------------------------------------
{
  static int ret;
  ret = ((RichRecPixel*)v)->hpd();
  return &ret;
}

//------------------------------------------------------------------------------
static void* RichRecPixel_panel_2const(void* v)
//------------------------------------------------------------------------------
{
  static int ret;
  ret = ((RichRecPixel*)v)->panel();
  return &ret;
}

//------------------------------------------------------------------------------
static void* RichRecPixel_smartID_3(void* v)
//------------------------------------------------------------------------------
{
  const RichSmartID& ret = ((RichRecPixel*)v)->smartID();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecPixel_setSmartID_4(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecPixel*)v)->setSmartID(*(RichSmartID*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecPixel_globalPosition_5(void* v)
//------------------------------------------------------------------------------
{
  const HepPoint3D& ret = ((RichRecPixel*)v)->globalPosition();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecPixel_setGlobalPosition_6(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecPixel*)v)->setGlobalPosition(*(HepPoint3D*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecPixel_localPosition_7(void* v)
//------------------------------------------------------------------------------
{
  const HepPoint3D& ret = ((RichRecPixel*)v)->localPosition();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecPixel_setLocalPosition_8(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecPixel*)v)->setLocalPosition(*(HepPoint3D*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecPixel_currentBackground_9(void* v)
//------------------------------------------------------------------------------
{
  static float ret;
  ret =  ((RichRecPixel*)v)->currentBackground();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecPixel_setCurrentBackground_10(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecPixel*)v)->setCurrentBackground(*(float*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecPixel_parentType_11(void* v)
//------------------------------------------------------------------------------
{
  const Rich::RecPixel::ParentType& ret = ((RichRecPixel*)v)->parentType();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecPixel_setParentType_12(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecPixel*)v)->setParentType(*(Rich::RecPixel::ParentType*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecPixel_richRecTracks_13(void* v)
//------------------------------------------------------------------------------
{
  const SmartRefVector<RichRecTrack>& ret = ((RichRecPixel*)v)->richRecTracks();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecPixel_setRichRecTracks_14(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecPixel*)v)->setRichRecTracks(*(SmartRefVector<RichRecTrack>*)argList[0]);
}

//------------------------------------------------------------------------------
static void RichRecPixel_clearRichRecTracks_15(void* v)
//------------------------------------------------------------------------------
{
  ((RichRecPixel*)v)->clearRichRecTracks();
}

//------------------------------------------------------------------------------
static void RichRecPixel_addToRichRecTracks_16(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecPixel*)v)->addToRichRecTracks((RichRecTrack*)argList[0]);
}

//------------------------------------------------------------------------------
static void RichRecPixel_removeFromRichRecTracks_17(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecPixel*)v)->removeFromRichRecTracks((RichRecTrack*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecPixel_parentPixel_18(void* v)
//------------------------------------------------------------------------------
{
  ContainedObject* ret = ((RichRecPixel*)v)->parentPixel();
  return  (void*)ret;
}

//------------------------------------------------------------------------------
static void RichRecPixel_setParentPixel_19(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecPixel*)v)->setParentPixel((ContainedObject*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecPixel_richRecPhotons_20(void* v)
//------------------------------------------------------------------------------
{
  const SmartRefVector<RichRecPhoton>& ret = ((RichRecPixel*)v)->richRecPhotons();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecPixel_setRichRecPhotons_21(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecPixel*)v)->setRichRecPhotons(*(SmartRefVector<RichRecPhoton>*)argList[0]);
}

//------------------------------------------------------------------------------
static void RichRecPixel_clearRichRecPhotons_22(void* v)
//------------------------------------------------------------------------------
{
  ((RichRecPixel*)v)->clearRichRecPhotons();
}

//------------------------------------------------------------------------------
static void RichRecPixel_addToRichRecPhotons_23(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecPixel*)v)->addToRichRecPhotons((RichRecPhoton*)argList[0]);
}

//------------------------------------------------------------------------------
static void RichRecPixel_removeFromRichRecPhotons_24(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecPixel*)v)->removeFromRichRecPhotons((RichRecPhoton*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecPixel_constructor_0()
//------------------------------------------------------------------------------
{
  static RichRecPixel* ret = new RichRecPixel();
  return ret;
}

//------------------------------------------------------------------------------
static RichRecPixel_dict instance;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
RichRecPixel_dict::RichRecPixel_dict()
//------------------------------------------------------------------------------
{
  std::vector<std::string> argTypes = std::vector<std::string>();
  MetaClass* metaC = new MetaClass("RichRecPixel",
                "Rich reconstruction working pixel object",
                0);

  RichRecPixel* cl = new RichRecPixel();
  metaC->addSuperClass("KeyedObject<int>",
                (((int)cl)-((int)((KeyedObject<int>*)cl))));
  delete cl;

  metaC->addConstructor("Default constructor",
                RichRecPixel_constructor_0);

  metaC->addField("smartID",
                "RichSmartID",
                "RichSmartID",
                &((RichRecPixel*)0)->m_smartID,
                MetaModifier::setPrivate());

  metaC->addField("globalPosition",
                "HepPoint3D",
                "Position in global coordinates",
                &((RichRecPixel*)0)->m_globalPosition,
                MetaModifier::setPrivate());

  metaC->addField("localPosition",
                "HepPoint3D",
                "Position in local coordinates",
                &((RichRecPixel*)0)->m_localPosition,
                MetaModifier::setPrivate());

  metaC->addField("currentBackground",
                "float",
                "Current value for a working estimate of the background in this pixel",
                &((RichRecPixel*)0)->m_currentBackground,
                MetaModifier::setPrivate());

  metaC->addField("parentType",
                "Rich::RecPixel::ParentType",
                "Enumeration to Parent type",
                &((RichRecPixel*)0)->m_parentType,
                MetaModifier::setPrivate());

  metaC->addField("richRecTracks",
                "SmartRefVector<RichRecTrack>",
                "SmartRefVector of RichRecTracks for which this pixel has valid photon associations",
                &((RichRecPixel*)0)->m_richRecTracks,
                MetaModifier::setPrivate());

  metaC->addField("parentPixel",
                "SmartRef<ContainedObject>",
                "Parent Object this pixel was produced from",
                &((RichRecPixel*)0)->m_parentPixel,
                MetaModifier::setPrivate());

  metaC->addField("richRecPhotons",
                "SmartRefVector<RichRecPhoton>",
                "SmartRefVector to associated RichRecPhotons",
                &((RichRecPixel*)0)->m_richRecPhotons,
                MetaModifier::setPrivate());

  argTypes.clear();
  metaC->addMethod("detector",
                "Returns Rich Detector",
                "Rich::DetectorType",
                RichRecPixel_detector_0const);

  argTypes.clear();
  metaC->addMethod("hpd",
                "Returns unique integer hpd identifier",
                "int",
                RichRecPixel_hpd_1const);

  argTypes.clear();
  metaC->addMethod("panel",
                "Returns unique integer Rich panel identifier",
                "int",
                RichRecPixel_panel_2const);

  metaC->addMethod("smartID",
                "RichSmartID",
                "RichSmartID",
                RichRecPixel_smartID_3);

  argTypes.clear();
  argTypes.push_back("RichSmartID");
  metaC->addMethod("setSmartID",
                "RichSmartID",
                argTypes,
                RichRecPixel_setSmartID_4);

  metaC->addMethod("globalPosition",
                "Position in global coordinates",
                "HepPoint3D",
                RichRecPixel_globalPosition_5);

  argTypes.clear();
  argTypes.push_back("HepPoint3D");
  metaC->addMethod("setGlobalPosition",
                "Position in global coordinates",
                argTypes,
                RichRecPixel_setGlobalPosition_6);

  metaC->addMethod("localPosition",
                "Position in local coordinates",
                "HepPoint3D",
                RichRecPixel_localPosition_7);

  argTypes.clear();
  argTypes.push_back("HepPoint3D");
  metaC->addMethod("setLocalPosition",
                "Position in local coordinates",
                argTypes,
                RichRecPixel_setLocalPosition_8);

  metaC->addMethod("currentBackground",
                "Current value for a working estimate of the background in this pixel",
                "float",
                RichRecPixel_currentBackground_9);

  argTypes.clear();
  argTypes.push_back("float");
  metaC->addMethod("setCurrentBackground",
                "Current value for a working estimate of the background in this pixel",
                argTypes,
                RichRecPixel_setCurrentBackground_10);

  metaC->addMethod("parentType",
                "Enumeration to Parent type",
                "Rich::RecPixel::ParentType",
                RichRecPixel_parentType_11);

  argTypes.clear();
  argTypes.push_back("Rich::RecPixel::ParentType");
  metaC->addMethod("setParentType",
                "Enumeration to Parent type",
                argTypes,
                RichRecPixel_setParentType_12);

  metaC->addMethod("richRecTracks",
                "SmartRefVector of RichRecTracks for which this pixel has valid photon associations",
                "SmartRefVector<RichRecTrack>",
                RichRecPixel_richRecTracks_13);

  argTypes.clear();
  argTypes.push_back("RichRecTrack");
  metaC->addMethod("setRichRecTracks",
                "SmartRefVector of RichRecTracks for which this pixel has valid photon associations",
                argTypes,
                RichRecPixel_setRichRecTracks_14);

  metaC->addMethod("clearRichRecTracks",
                "SmartRefVector of RichRecTracks for which this pixel has valid photon associations",
                RichRecPixel_clearRichRecTracks_15);

  argTypes.clear();
  argTypes.push_back("RichRecTrack");
  metaC->addMethod("addToRichRecTracks",
                "SmartRefVector of RichRecTracks for which this pixel has valid photon associations",
                argTypes,
                RichRecPixel_addToRichRecTracks_16);

  argTypes.clear();
  argTypes.push_back("RichRecTrack");
  metaC->addMethod("removeFromRichRecTracks",
                "SmartRefVector of RichRecTracks for which this pixel has valid photon associations",
                argTypes,
                RichRecPixel_removeFromRichRecTracks_17);

  metaC->addMethod("parentPixel",
                "Parent Object this pixel was produced from",
                "ContainedObject",
                RichRecPixel_parentPixel_18);

  argTypes.clear();
  argTypes.push_back("ContainedObject");
  metaC->addMethod("setParentPixel",
                "Parent Object this pixel was produced from",
                argTypes,
                RichRecPixel_setParentPixel_19);

  metaC->addMethod("richRecPhotons",
                "SmartRefVector to associated RichRecPhotons",
                "SmartRefVector<RichRecPhoton>",
                RichRecPixel_richRecPhotons_20);

  argTypes.clear();
  argTypes.push_back("RichRecPhoton");
  metaC->addMethod("setRichRecPhotons",
                "SmartRefVector to associated RichRecPhotons",
                argTypes,
                RichRecPixel_setRichRecPhotons_21);

  metaC->addMethod("clearRichRecPhotons",
                "SmartRefVector to associated RichRecPhotons",
                RichRecPixel_clearRichRecPhotons_22);

  argTypes.clear();
  argTypes.push_back("RichRecPhoton");
  metaC->addMethod("addToRichRecPhotons",
                "SmartRefVector to associated RichRecPhotons",
                argTypes,
                RichRecPixel_addToRichRecPhotons_23);

  argTypes.clear();
  argTypes.push_back("RichRecPhoton");
  metaC->addMethod("removeFromRichRecPhotons",
                "SmartRefVector to associated RichRecPhotons",
                argTypes,
                RichRecPixel_removeFromRichRecPhotons_24);

  MetaPropertyList* pl = new MetaPropertyList();
  pl->setProperty("Author", "Chris Jones   (Christopher.Rob.Jones@cern.ch)");
  pl->setProperty("ClassID", "12422");
  metaC->setPropertyList(pl);
}


