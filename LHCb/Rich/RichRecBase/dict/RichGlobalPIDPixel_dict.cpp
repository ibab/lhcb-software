

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

#include "RichGlobalPIDPixel_dict.h"

#include "GaudiIntrospection/Introspection.h"


//------------------------------------------------------------------------------
class RichGlobalPIDPixel_dict
//------------------------------------------------------------------------------
{
public:
  RichGlobalPIDPixel_dict();
};

//------------------------------------------------------------------------------
static void* RichGlobalPIDPixel_backgroundEstimates_0(void* v)
//------------------------------------------------------------------------------
{
  const std::vector<float>& ret = ((RichGlobalPIDPixel*)v)->backgroundEstimates();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichGlobalPIDPixel_setBackgroundEstimates_1(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichGlobalPIDPixel*)v)->setBackgroundEstimates(*(std::vector<float>*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichGlobalPIDPixel_richRecPixel_2(void* v)
//------------------------------------------------------------------------------
{
  RichRecPixel* ret = ((RichGlobalPIDPixel*)v)->richRecPixel();
  return  (void*)ret;
}

//------------------------------------------------------------------------------
static void RichGlobalPIDPixel_setRichRecPixel_3(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichGlobalPIDPixel*)v)->setRichRecPixel((RichRecPixel*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichGlobalPIDPixel_constructor_1()
//------------------------------------------------------------------------------
{
  static RichGlobalPIDPixel* ret = new RichGlobalPIDPixel();
  return ret;
}

//------------------------------------------------------------------------------
static RichGlobalPIDPixel_dict instance;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
RichGlobalPIDPixel_dict::RichGlobalPIDPixel_dict()
//------------------------------------------------------------------------------
{
  std::vector<std::string> argTypes = std::vector<std::string>();
  MetaClass* metaC = new MetaClass("RichGlobalPIDPixel",
                "Working pixel object for Global PID",
                0);

  RichGlobalPIDPixel* cl = new RichGlobalPIDPixel();
  metaC->addSuperClass("KeyedObject<long>",
                (((int)cl)-((int)((KeyedObject<long>*)cl))));
  delete cl;

  metaC->addConstructor("default constructor",
                RichGlobalPIDPixel_constructor_1);

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addField("backgroundEstimates",
                "std::vector<float>",
                "Background estimate for this pixel, for each event iteration",
                &((RichGlobalPIDPixel*)0)->m_backgroundEstimates,
                MetaModifier::setPrivate());

  metaC->addField("richRecPixel",
                "SmartRef<RichRecPixel>",
                "Associated RichRecPixel",
                &((RichGlobalPIDPixel*)0)->m_richRecPixel,
                MetaModifier::setPrivate());

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addMethod("backgroundEstimates",
                "Background estimate for this pixel, for each event iteration",
                "std::vector<float>",
                RichGlobalPIDPixel_backgroundEstimates_0);

  argTypes.clear();
  argTypes.push_back("std::vector<float>");
  metaC->addMethod("setBackgroundEstimates",
                "Background estimate for this pixel, for each event iteration",
                argTypes,
                RichGlobalPIDPixel_setBackgroundEstimates_1);

  metaC->addMethod("richRecPixel",
                "Associated RichRecPixel",
                "RichRecPixel",
                RichGlobalPIDPixel_richRecPixel_2);

  argTypes.clear();
  argTypes.push_back("RichRecPixel");
  metaC->addMethod("setRichRecPixel",
                "Associated RichRecPixel",
                argTypes,
                RichGlobalPIDPixel_setRichRecPixel_3);

  MetaPropertyList* pl = new MetaPropertyList();
  pl->setProperty("Author", "Chris Jones    Christopher.Rob.Jones@cern.ch");
  pl->setProperty("ClassID", "12433");
  metaC->setPropertyList(pl);
}


