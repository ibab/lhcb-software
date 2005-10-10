// $Id: lcgStl.h,v 1.3 2005-10-10 16:02:04 cattanem Exp $
#ifndef DAQEVENT_LCGSTL_H 
#define DAQEVENT_LCGSTL_H 1

// Additional classes to be added to automatically generated lcgdict

// Reflex include files
#include "Reflex/Builder/ReflexBuilder.h"

// begin include files
#include <vector>
#include <map>
#include "Event/L1Bank.h"
#include "Event/RawBank.h"
#include "Event/L1Buffer.h"
#include "Event/RawBuffer.h"
// end include files

namespace {
  struct _Instantiations {
    // begin instantiations
    std::map<int,std::vector<L1Bank> >  m_std_map_int_std_vector_L1Bank;
    std::map<int,std::vector<RawBank> > m_std_map_int_std_vector_RawBank;
    std::vector<L1Bank>                 m_std_vector_L1Bank;
    std::vector<RawBank>                m_std_vector_RawBank;
    BufferSerializer<RawBuffer>         m_BufferSerializer_RawBuffer;
    BufferSerializer<L1Buffer>          m_BufferSerializer_L1Buffer;    
    // end instantiations
  };
}
#endif // DAQEVENT_LCGSTL_H
