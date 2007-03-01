// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/PVSS/NamedIdentified.h,v 1.2 2007-03-01 20:08:55 frankb Exp $
//  ====================================================================
//  NamedIdentified.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef ONLINE_PVSS_NAMEDIDENTIFIED_H
#define ONLINE_PVSS_NAMEDIDENTIFIED_H

// Framework include files
#include "PVSS/Kernel.h"

// C++ include files
#include <string>

/*
 *   PVSS namespace declaration
 */
namespace PVSS {

  /** @class NamedIdentified   NamedIdentified.h  PVSS/NamedIdentified.h
    *
    *  PVSS base class for named and identified objects
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  class NamedIdentified   {

  protected:
    /// Attribute identifier
    int m_id;
    /// Attribute name
    std::string m_name;

    /// Empty constructor
    NamedIdentified() : m_id(~0x0)      {                  }
    /// Initializing constructor
    NamedIdentified(int i) 
    : m_id(i)                           {                  }
    /// Initializing constructor
    NamedIdentified(const std::string& n) 
    : m_id(~0x0), m_name(n)             {                  }
    /// Initializing constructor
    NamedIdentified(int i, const std::string& n) 
    : m_id(i), m_name(n)                {                  }
    /// Copy constructor
    NamedIdentified(const NamedIdentified& c) 
    : m_id(c.m_id), m_name(c.m_name)    {                  }
    /// Standard destructor
    virtual ~NamedIdentified()          {                  }
    /// Set identifier attribute
    void setId(int i)                   { m_id = i;        }
    /// Set name attribute
    void setName(const std::string& n)  { m_name = n;      }
  public:
    /// Access to the NamedIdentified's name
    const std::string& name()  const    { return m_name;   }
    /// Access variable type
    int id()  const                     { return m_id;     }
    /// Equivalence check
    virtual bool operator==(const NamedIdentified& c) const 
    { return m_id == c.m_id && m_name == c.m_name ;  }
  };
}      // End namespace PVSS
#endif // ONLINE_PVSS_NAMEDIDENTIFIED_H
