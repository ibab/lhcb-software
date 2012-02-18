// $Id: lcgDict.h,v 1.2 2008-02-15 07:42:17 cattanem Exp $
#ifndef RECEVENT_LCGDICT_H 
#define RECEVENT_LCGDICT_H 1

// Additional classes to be added to automatically generated lcgdict

// begin include files
#include <vector>
#include "Event/RecVertex.h"
#include "Event/VertexBase.h"

// end include files

namespace 
{
  struct _Instantiations 
  {
    // begin instantiations
    std::vector<const LHCb::RecVertex*>  _i1;
    std::vector<const LHCb::VertexBase*> _i2;
    std::pair<int,float>                 _i3;
    std::vector<std::pair<int,float> >   _i4;
    // end instantiations
  };
}

#endif // RECEVENT_LCGDICT_H
