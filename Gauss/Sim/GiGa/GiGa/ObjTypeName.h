#ifndef     GIGA_OBJTYPENAME_H
#define     GIGA_OBJTYPENAME_H 1 
/// STL
#include <string>
/// GaudiKernel
#include "GaudiKernel/System.h"

///
template <class TYPE>
inline const std::string ObjTypeName( TYPE obj )
{ return obj ? std::string(System::typeinfoName( typeid(*obj) ) ) : std::string("UNKNOWN_object") ; };
///

#endif  //  GIGA_OBJTYPENAME_H
