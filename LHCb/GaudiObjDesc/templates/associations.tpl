
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
#include "Relations/RelationsDict.h"
using namespace seal::reflect;

@@@includes@@@

namespace
{
  struct _InitDict
  {
    _InitDict()
    {
@@@associations@@@
    }
  };

  static _InitDict __init;
}

void* __init_InitDict_@@@packagename@@@Assoc()
{
  return &__init;
}
