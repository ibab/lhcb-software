

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

#include "RichGlobalPID_dict.h"

#include "GaudiIntrospection/Introspection.h"


//------------------------------------------------------------------------------
class RichGlobalPID_dict
//------------------------------------------------------------------------------
{
public:
  RichGlobalPID_dict();
};

//------------------------------------------------------------------------------
static void* RichGlobalPID_constructor_0()
//------------------------------------------------------------------------------
{
  static RichGlobalPID* ret = new RichGlobalPID();
  return ret;
}

//------------------------------------------------------------------------------
static RichGlobalPID_dict instance;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
RichGlobalPID_dict::RichGlobalPID_dict()
//------------------------------------------------------------------------------
{
  MetaClass* metaC = new MetaClass("RichGlobalPID",
                "Rich particle identification object for the global track based algorithm",
                0);

  RichGlobalPID* cl = new RichGlobalPID();
  metaC->addSuperClass("RichPID",
                (((int)cl)-((int)((RichPID*)cl))));
  delete cl;

  metaC->addConstructor("Default constructor",
                RichGlobalPID_constructor_0);

  MetaPropertyList* pl = new MetaPropertyList();
  pl->setProperty("Author", "Chris Jones   (Christopher.Rob.Jones@cern.ch)");
  pl->setProperty("ClassID", "12434");
  metaC->setPropertyList(pl);
}


