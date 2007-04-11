// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/src/DevTypeElement.cpp,v 1.5 2007-04-11 17:45:47 frankb Exp $
//  ====================================================================
//  DevTypeElement.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: DevTypeElement.cpp,v 1.5 2007-04-11 17:45:47 frankb Exp $

// Framework include files
#include "PVSS/DevTypeElement.h"
#include "PVSS/Internals.h"
#include "PVSS/DevType.h"

#include <algorithm>

namespace PVSS {
  template <> struct DevManip<DevTypeElement>  
  {  inline void operator()(DevTypeElement* e)    {  delete e;    }   };
}

using namespace PVSS;

/// Initializing constructor
DevTypeElement::DevTypeElement(DevType* typ, int i, const std::string& nam, int cont)
: NamedIdentified(i,nam), m_type(typ), m_content(cont)
{
}

/// Standard destructor
DevTypeElement::~DevTypeElement()   {
  std::for_each(m_elements.begin(),m_elements.end(),DevManip<DevTypeElement>());
}

/// Access device type manager
DevTypeManager* DevTypeElement::manager() const  {
  return m_type->manager();
}

/// Add elements to device type
void DevTypeElement::add(DevTypeElement* element)   {
  if ( element ) m_elements.push_back(element);
}

/// Type name
std::string DevTypeElement::typeName()  const  {
  return pvss_type_name(content());
}

bool DevTypeElement::isPrimitive()  const {
  switch(content())  {
#define _PRT(x)   case x:  return true;
     _PRT(ARRAY);
     _PRT(DYNCHAR);
     _PRT(DYNUINT);
     _PRT(DYNINT);
     _PRT(DYNFLOAT);
     _PRT(DYNBIT);
     _PRT(DYN32BIT);
     _PRT(DYNTEXT);
     _PRT(DYNTIME);
     _PRT(CHARARRAY);
     _PRT(UINTARRAY);
     _PRT(INTARRAY);
     _PRT(FLOATARRAY);
     _PRT(BITARRAY);
     _PRT(BIT32ARRAY);
     _PRT(TEXTARRAY);
     _PRT(TIMEARRAY);
     _PRT(CHAR);
     _PRT(UINT);
     _PRT(INT);
     _PRT(FLOAT);
     _PRT(BIT);
     _PRT(BIT32);
     _PRT(TEXT);
     _PRT(TIME);
     _PRT(DPID);
     _PRT(NOVALUE);
     _PRT(DYNDPID);
     _PRT(DYNCHARARRAY);
     _PRT(DYNUINTARRAY);
     _PRT(DYNINTARRAY);
     _PRT(DYNFLOATARRAY);
     _PRT(DYNBITARRAY);
     _PRT(DYN32BITARRAY);
     _PRT(DYNTEXTARRAY);
     _PRT(DYNTIMEARRAY);
     _PRT(DYNDPIDARRAY);
     _PRT(DPIDARRAY);
     _PRT(NOVALUEARRAY);
     _PRT(TYPEREFERENCE);
     _PRT(LANGTEXT);
     _PRT(LANGTEXTARRAY);
     _PRT(DYNLANGTEXT);
     _PRT(DYNLANGTEXTARRAY);
     _PRT(BLOB);
     _PRT(BLOBARRAY);
     _PRT(DYNBLOB);
     _PRT(DYNBLOBARRAY);
#undef _PRT
  default:
    return false;
  }
  return false;
}

