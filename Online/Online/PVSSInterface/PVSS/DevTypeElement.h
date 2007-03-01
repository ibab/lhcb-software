// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/PVSS/DevTypeElement.h,v 1.1 2007-03-01 10:39:49 frankb Exp $
//  ====================================================================
//  DevTypeElement.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef ONLINE_PVSS_DEVTYPEELEMENT_H
#define ONLINE_PVSS_DEVTYPEELEMENT_H

// Framework include files
#include "PVSS/NamedIdentified.h"

// C++ include files
#include <vector>

/*
 *   PVSS namespace declaration
 */
namespace PVSS {

  // Forward declarations
  class DevTypeManager;
  class DevTypeElement;
  class DevType;
  class DPTime;
  class DPRef;


  /** @class DevTypeElement   DevTypeElement.h  PVSS/DevTypeElement.h
    *
    *  PVSS device type element.
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  class DevTypeElement : public NamedIdentified {
  public:
    friend struct DevManip<DevTypeElement>;
    enum Type {
      NOELEMENT = 0,
      RECORD,
      ARRAY,
      DYNCHAR,
      DYNUINT,
      DYNINT = 5,
      DYNFLOAT,
      DYNBIT,
      DYN32BIT,
      DYNTEXT,
      DYNTIME = 10,
      CHARARRAY,
      UINTARRAY,
      INTARRAY,
      FLOATARRAY,
      BITARRAY = 15,
      BIT32ARRAY,
      TEXTARRAY,
      TIMEARRAY,
      CHAR,
      UINT = 20,
      INT,
      FLOAT,
      BIT,
      BIT32,
      TEXT = 25,
      TIME,
      DPID,
      NOVALUE,
      DYNDPID,
      DYNCHARARRAY = 30,
      DYNUINTARRAY,
      DYNINTARRAY,
      DYNFLOATARRAY,
      DYNBITARRAY,
      DYN32BITARRAY = 35,
      DYNTEXTARRAY,
      DYNTIMEARRAY,
      DYNDPIDARRAY,
      DPIDARRAY,
      NOVALUEARRAY = 40,
      TYPEREFERENCE,
      LANGTEXT,
      LANGTEXTARRAY,
      DYNLANGTEXT,
      DYNLANGTEXTARRAY = 45,
      BLOB,
      BLOBARRAY,
      DYNBLOB,
      DYNBLOBARRAY,
      COUNT = 50   // Sentinel - must be last
    };

    /// Internal type definitions: device element container
    typedef std::vector<DevTypeElement*> Elements;

    template <class T>
    static Type lookup(const T&)                { return NO_ELEMENT; }
    static Type lookup(const bool&)             { return BIT; }
    static Type lookup(const char&)             { return CHAR; }
    static Type lookup(const unsigned char&)    { return CHAR; }
    static Type lookup(const short&)            { return INT; }
    static Type lookup(const unsigned short&)   { return INT; }
    static Type lookup(const long&)             { return UINT; }
    static Type lookup(const unsigned long&)    { return UINT; }
    static Type lookup(const float&)            { return FLOAT; }
    static Type lookup(const double&)           { return FLOAT; }
    static Type lookup(const DPTime&)           { return TIME; }
    static Type lookup(const DPRef&)            { return DPID; }

  protected:
    /// Access to device type
    DevType  *m_type;
    /// Element type
    Type      m_content;
    /// Son element container
    Elements  m_elements;

    /// Initializing constructor
    DevTypeElement(DevType* typ, int i, const std::string& nam, Type cont);
    /// Standard destructor
    virtual ~DevTypeElement();

    /// Add DevTypeElement to type
    void add(DevTypeElement* attr);


  public:

    /// Access device type manager
    DevTypeManager* manager() const;
    /// Access to device type
    DevType* type() const             { return m_type;     }
    /// Access to the element type
    Type     content() const          { return m_content;  }
    /// Client access to elements
    const Elements& elements()  const { return m_elements; }
    /// Is the element a primitive type ?
    bool isPrimitive() const;
    /// Type name as string
    std::string typeName()  const;
  };
}      // End namespace PVSS
#endif // ONLINE_PVSS_DEVTYPEELEMENT_H
