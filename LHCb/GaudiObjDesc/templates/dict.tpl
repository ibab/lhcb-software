
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
@@@includes@@@

#include <string>
#include <typeinfo>

#include "@@@classname@@@_dict.h"

using namespace seal::reflect;

//------------------------------------------------------------------------------
class @@@classname@@@_dict
//------------------------------------------------------------------------------
{
public:
  @@@classname@@@_dict();
};  

@@@methodStubs@@@
@@@getSetMethodStubs@@@
@@@constructorStubs@@@
@@@destructorStub@@@
@@@baseStubs@@@

//------------------------------------------------------------------------------
@@@classname@@@_dict::@@@classname@@@_dict()
//------------------------------------------------------------------------------
{
@@@classBuilder@@@
@@@classID@@@
@@@properties@@@
@@@bases@@@
@@@constructors@@@
@@@destructor@@@
@@@methods@@@ 
@@@fields@@@
@@@getSetMethods@@@

  metaC.build();
}

namespace 
{
  struct _InitDict
  {
    _InitDict()
    {
      @@@classname@@@_dict();
@@@initialisations@@@
      Class::resolvePending();
    }
  };

  static _InitDict __init;
}

void* __init_InitDict(@@@classname@@@* /* dummy */ )
{
  return &__init;
}
