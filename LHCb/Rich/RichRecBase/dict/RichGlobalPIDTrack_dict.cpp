

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

#include "RichGlobalPIDTrack_dict.h"

#include "GaudiIntrospection/Introspection.h"


//------------------------------------------------------------------------------
class RichGlobalPIDTrack_dict
//------------------------------------------------------------------------------
{
public:
  RichGlobalPIDTrack_dict();
};

//------------------------------------------------------------------------------
static void* RichGlobalPIDTrack_trQuality_0(void* v)
//------------------------------------------------------------------------------
{
  const Rich::GlobalPID::TkQuality& ret = ((RichGlobalPIDTrack*)v)->trQuality();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichGlobalPIDTrack_setTrQuality_1(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichGlobalPIDTrack*)v)->setTrQuality(*(Rich::GlobalPID::TkQuality*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichGlobalPIDTrack_richRecTrack_2(void* v)
//------------------------------------------------------------------------------
{
  RichRecTrack* ret = ((RichGlobalPIDTrack*)v)->richRecTrack();
  return  (void*)ret;
}

//------------------------------------------------------------------------------
static void RichGlobalPIDTrack_setRichRecTrack_3(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichGlobalPIDTrack*)v)->setRichRecTrack((RichRecTrack*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichGlobalPIDTrack_globalPID_4(void* v)
//------------------------------------------------------------------------------
{
  RichGlobalPID* ret = ((RichGlobalPIDTrack*)v)->globalPID();
  return  (void*)ret;
}

//------------------------------------------------------------------------------
static void RichGlobalPIDTrack_setGlobalPID_5(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichGlobalPIDTrack*)v)->setGlobalPID((RichGlobalPID*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichGlobalPIDTrack_constructor_0()
//------------------------------------------------------------------------------
{
  static RichGlobalPIDTrack* ret = new RichGlobalPIDTrack();
  return ret;
}

//------------------------------------------------------------------------------
static RichGlobalPIDTrack_dict instance;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
RichGlobalPIDTrack_dict::RichGlobalPIDTrack_dict()
//------------------------------------------------------------------------------
{
  std::vector<std::string> argTypes = std::vector<std::string>();
  MetaClass* metaC = new MetaClass("RichGlobalPIDTrack",
                "Working track object for Rich Global PID",
                0);

  RichGlobalPIDTrack* cl = new RichGlobalPIDTrack();
  metaC->addSuperClass("KeyedObject<long>",
                (((int)cl)-((int)((KeyedObject<long>*)cl))));
  delete cl;

  metaC->addConstructor("Default Constructor",
                RichGlobalPIDTrack_constructor_0);

  metaC->addField("trQuality",
                "Rich::GlobalPID::TkQuality",
                "Integer code describing the quality of this track for the Global likelihood method",
                &((RichGlobalPIDTrack*)0)->m_trQuality,
                MetaModifier::setPrivate());

  metaC->addField("richRecTrack",
                "SmartRef<RichRecTrack>",
                "SmartRef to RichRecTrack",
                &((RichGlobalPIDTrack*)0)->m_richRecTrack,
                MetaModifier::setPrivate());

  metaC->addField("globalPID",
                "SmartRef<RichGlobalPID>",
                "Associated GlobalPID object",
                &((RichGlobalPIDTrack*)0)->m_globalPID,
                MetaModifier::setPrivate());

  metaC->addMethod("trQuality",
                "Integer code describing the quality of this track for the Global likelihood method",
                "Rich::GlobalPID::TkQuality",
                RichGlobalPIDTrack_trQuality_0);

  argTypes.clear();
  argTypes.push_back("Rich::GlobalPID::TkQuality");
  metaC->addMethod("setTrQuality",
                "Integer code describing the quality of this track for the Global likelihood method",
                argTypes,
                RichGlobalPIDTrack_setTrQuality_1);

  metaC->addMethod("richRecTrack",
                "SmartRef to RichRecTrack",
                "RichRecTrack",
                RichGlobalPIDTrack_richRecTrack_2);

  argTypes.clear();
  argTypes.push_back("RichRecTrack");
  metaC->addMethod("setRichRecTrack",
                "SmartRef to RichRecTrack",
                argTypes,
                RichGlobalPIDTrack_setRichRecTrack_3);

  metaC->addMethod("globalPID",
                "Associated GlobalPID object",
                "RichGlobalPID",
                RichGlobalPIDTrack_globalPID_4);

  argTypes.clear();
  argTypes.push_back("RichGlobalPID");
  metaC->addMethod("setGlobalPID",
                "Associated GlobalPID object",
                argTypes,
                RichGlobalPIDTrack_setGlobalPID_5);

  MetaPropertyList* pl = new MetaPropertyList();
  pl->setProperty("Author", "Chris Jones     Christopher.Rob.Jones@cern.ch");
  pl->setProperty("ClassID", "12432");
  metaC->setPropertyList(pl);
}


