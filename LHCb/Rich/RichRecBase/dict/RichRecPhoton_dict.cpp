

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

#include "RichRecPhoton_dict.h"

#include "GaudiIntrospection/Introspection.h"


//------------------------------------------------------------------------------
class RichRecPhoton_dict
//------------------------------------------------------------------------------
{
public:
  RichRecPhoton_dict();
};

//------------------------------------------------------------------------------
static void RichRecPhoton_setExpPixelSignalPhots_0(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecPhoton*)v)->setExpPixelSignalPhots(*(Rich::ParticleIDType*)argList[0],
          *(float*)argList[1]);
}

//------------------------------------------------------------------------------
static void RichRecPhoton_setExpPixelScattPhots_1(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecPhoton*)v)->setExpPixelScattPhots(*(Rich::ParticleIDType*)argList[0],
          *(float*)argList[1]);
}

//------------------------------------------------------------------------------
static void* RichRecPhoton_expPixelSignalPhots_2const(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  static float ret;
  ret = ((RichRecPhoton*)v)->expPixelSignalPhots(*(Rich::ParticleIDType*)argList[0]);
  return &ret;
}

//------------------------------------------------------------------------------
static void* RichRecPhoton_expPixelScattPhots_3const(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  static float ret;
  ret = ((RichRecPhoton*)v)->expPixelScattPhots(*(Rich::ParticleIDType*)argList[0]);
  return &ret;
}

//------------------------------------------------------------------------------
static void* RichRecPhoton_geomPhoton_4(void* v)
//------------------------------------------------------------------------------
{
  const RichGeomPhoton& ret = ((RichRecPhoton*)v)->geomPhoton();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecPhoton_setGeomPhoton_5(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecPhoton*)v)->setGeomPhoton(*(RichGeomPhoton*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecPhoton_expPixelSignalPhots_6(void* v)
//------------------------------------------------------------------------------
{
  const std::vector<float>& ret = ((RichRecPhoton*)v)->expPixelSignalPhots();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecPhoton_setExpPixelSignalPhots_7(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecPhoton*)v)->setExpPixelSignalPhots(*(std::vector<float>*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecPhoton_expPixelScattPhots_8(void* v)
//------------------------------------------------------------------------------
{
  const std::vector<float>& ret = ((RichRecPhoton*)v)->expPixelScattPhots();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecPhoton_setExpPixelScattPhots_9(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecPhoton*)v)->setExpPixelScattPhots(*(std::vector<float>*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecPhoton_sharedRingFract_10(void* v)
//------------------------------------------------------------------------------
{
  static float ret;
  ret =  ((RichRecPhoton*)v)->sharedRingFract();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecPhoton_setSharedRingFract_11(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecPhoton*)v)->setSharedRingFract(*(float*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecPhoton_richRecSegment_12(void* v)
//------------------------------------------------------------------------------
{
  RichRecSegment* ret = ((RichRecPhoton*)v)->richRecSegment();
  return  (void*)ret;
}

//------------------------------------------------------------------------------
static void RichRecPhoton_setRichRecSegment_13(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecPhoton*)v)->setRichRecSegment((RichRecSegment*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecPhoton_richRecTrack_14(void* v)
//------------------------------------------------------------------------------
{
  RichRecTrack* ret = ((RichRecPhoton*)v)->richRecTrack();
  return  (void*)ret;
}

//------------------------------------------------------------------------------
static void RichRecPhoton_setRichRecTrack_15(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecPhoton*)v)->setRichRecTrack((RichRecTrack*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecPhoton_richRecPixel_16(void* v)
//------------------------------------------------------------------------------
{
  RichRecPixel* ret = ((RichRecPhoton*)v)->richRecPixel();
  return  (void*)ret;
}

//------------------------------------------------------------------------------
static void RichRecPhoton_setRichRecPixel_17(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecPhoton*)v)->setRichRecPixel((RichRecPixel*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecPhoton_constructor_0()
//------------------------------------------------------------------------------
{
  static RichRecPhoton* ret = new RichRecPhoton();
  return ret;
}

//------------------------------------------------------------------------------
static RichRecPhoton_dict instance;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
RichRecPhoton_dict::RichRecPhoton_dict()
//------------------------------------------------------------------------------
{
  std::vector<std::string> argTypes = std::vector<std::string>();
  MetaClass* metaC = new MetaClass("RichRecPhoton",
                "Rich reconstruction photon candidate. The result of an association between a RichRecTrack and a RichRecPixel.",
                0);

  RichRecPhoton* cl = new RichRecPhoton();
  metaC->addSuperClass("KeyedObject<RichRecPhotonKey>",
                (((int)cl)-((int)((KeyedObject<RichRecPhotonKey>*)cl))));
  delete cl;

  metaC->addConstructor("Default Constructor",
                RichRecPhoton_constructor_0);

  metaC->addField("geomPhoton",
                "RichGeomPhoton",
                "The result of the Cherenkov angles reconstruction",
                &((RichRecPhoton*)0)->m_geomPhoton,
                MetaModifier::setPrivate());

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addField("expPixelSignalPhots",
                "std::vector<float>",
                "Expected number of signal photons for each particle hypothesis",
                &((RichRecPhoton*)0)->m_expPixelSignalPhots,
                MetaModifier::setPrivate());

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addField("expPixelScattPhots",
                "std::vector<float>",
                "Expected number of scattered photons for each particle hypothesis",
                &((RichRecPhoton*)0)->m_expPixelScattPhots,
                MetaModifier::setPrivate());

  metaC->addField("sharedRingFract",
                "float",
                "The fraction of the segment path length associated with this photon",
                &((RichRecPhoton*)0)->m_sharedRingFract,
                MetaModifier::setPrivate());

  metaC->addField("richRecSegment",
                "SmartRef<RichRecSegment>",
                "Smart Reference to associated RichRecSegment",
                &((RichRecPhoton*)0)->m_richRecSegment,
                MetaModifier::setPrivate());

  metaC->addField("richRecTrack",
                "SmartRef<RichRecTrack>",
                "Smart Reference to associated RichRecTrack",
                &((RichRecPhoton*)0)->m_richRecTrack,
                MetaModifier::setPrivate());

  metaC->addField("richRecPixel",
                "SmartRef<RichRecPixel>",
                "Smart Reference to associated RichRecPixel",
                &((RichRecPhoton*)0)->m_richRecPixel,
                MetaModifier::setPrivate());

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  argTypes.push_back("float");
  metaC->addMethod("setExpPixelSignalPhots",
                "Set the expected number of signal photons for a given particle hypothesis",
                argTypes,
                RichRecPhoton_setExpPixelSignalPhots_0);

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  argTypes.push_back("float");
  metaC->addMethod("setExpPixelScattPhots",
                "Set the expected number of scattered photons for a given particle hypothesis",
                argTypes,
                RichRecPhoton_setExpPixelScattPhots_1);

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  metaC->addMethod("expPixelSignalPhots",
                "Returns the expected number of signal photons for a given particle type",
                "float",
                argTypes,
                RichRecPhoton_expPixelSignalPhots_2const);

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  metaC->addMethod("expPixelScattPhots",
                "Returns the expected number of scattered photons for a given particle type",
                "float",
                argTypes,
                RichRecPhoton_expPixelScattPhots_3const);

  metaC->addMethod("geomPhoton",
                "The result of the Cherenkov angles reconstruction",
                "RichGeomPhoton",
                RichRecPhoton_geomPhoton_4);

  argTypes.clear();
  argTypes.push_back("RichGeomPhoton");
  metaC->addMethod("setGeomPhoton",
                "The result of the Cherenkov angles reconstruction",
                argTypes,
                RichRecPhoton_setGeomPhoton_5);

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addMethod("expPixelSignalPhots",
                "Expected number of signal photons for each particle hypothesis",
                "std::vector<float>",
                RichRecPhoton_expPixelSignalPhots_6);

  argTypes.clear();
  argTypes.push_back("std::vector<float>");
  metaC->addMethod("setExpPixelSignalPhots",
                "Expected number of signal photons for each particle hypothesis",
                argTypes,
                RichRecPhoton_setExpPixelSignalPhots_7);

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addMethod("expPixelScattPhots",
                "Expected number of scattered photons for each particle hypothesis",
                "std::vector<float>",
                RichRecPhoton_expPixelScattPhots_8);

  argTypes.clear();
  argTypes.push_back("std::vector<float>");
  metaC->addMethod("setExpPixelScattPhots",
                "Expected number of scattered photons for each particle hypothesis",
                argTypes,
                RichRecPhoton_setExpPixelScattPhots_9);

  metaC->addMethod("sharedRingFract",
                "The fraction of the segment path length associated with this photon",
                "float",
                RichRecPhoton_sharedRingFract_10);

  argTypes.clear();
  argTypes.push_back("float");
  metaC->addMethod("setSharedRingFract",
                "The fraction of the segment path length associated with this photon",
                argTypes,
                RichRecPhoton_setSharedRingFract_11);

  metaC->addMethod("richRecSegment",
                "Smart Reference to associated RichRecSegment",
                "RichRecSegment",
                RichRecPhoton_richRecSegment_12);

  argTypes.clear();
  argTypes.push_back("RichRecSegment");
  metaC->addMethod("setRichRecSegment",
                "Smart Reference to associated RichRecSegment",
                argTypes,
                RichRecPhoton_setRichRecSegment_13);

  metaC->addMethod("richRecTrack",
                "Smart Reference to associated RichRecTrack",
                "RichRecTrack",
                RichRecPhoton_richRecTrack_14);

  argTypes.clear();
  argTypes.push_back("RichRecTrack");
  metaC->addMethod("setRichRecTrack",
                "Smart Reference to associated RichRecTrack",
                argTypes,
                RichRecPhoton_setRichRecTrack_15);

  metaC->addMethod("richRecPixel",
                "Smart Reference to associated RichRecPixel",
                "RichRecPixel",
                RichRecPhoton_richRecPixel_16);

  argTypes.clear();
  argTypes.push_back("RichRecPixel");
  metaC->addMethod("setRichRecPixel",
                "Smart Reference to associated RichRecPixel",
                argTypes,
                RichRecPhoton_setRichRecPixel_17);

  MetaPropertyList* pl = new MetaPropertyList();
  pl->setProperty("Author", "Chris Jones   (Christopher.Rob.Jones@cern.ch)");
  pl->setProperty("ClassID", "12424");
  metaC->setPropertyList(pl);
}


